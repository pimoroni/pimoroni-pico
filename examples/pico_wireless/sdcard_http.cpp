#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico_wireless.hpp"
#include "secrets.h"
#include "ff.h"

#include <vector>
#include <algorithm>

#define HTTP_PORT 80
#define HTTP_REQUEST_BUF_SIZE 2048
#define HTTP_RESPONSE_BUF_SIZE 1024 * 10
#define HTTP_RESPONSE_CHUNK_SIZE 1024u

#define DNS_CLOUDFLARE IPAddress(1, 1, 1, 1)
#define DNS_GOOGLE IPAddress(8, 8, 8, 8)
#define USE_DNS DNS_CLOUDFLARE

using namespace pimoroni;

FATFS fs;
FIL fil;
FRESULT fr;

PicoWireless wireless;
uint8_t r, g, b;
uint8_t request_buf[HTTP_REQUEST_BUF_SIZE];
uint8_t response_buf[HTTP_RESPONSE_BUF_SIZE];
const std::string response_501 = "HTTP/1.1 501 Not Implemented\nContent-Length: 19\n\n501 Not Implemented";

enum http_request_method_t {
  GET,
  POST
};

enum http_content_type_t {
  TEXT_HTML,
  IMAGE_SVG,
};

struct http_response_t {
  std::string_view response_body;
  int response_code;
  http_content_type_t content_type = TEXT_HTML;
};

typedef http_response_t(*http_request_handler)(http_request_method_t method, std::string_view path, std::vector<std::string_view> request_head, std::vector<std::string_view> request_body);

std::vector<std::string_view> split(std::string_view str, std::string delim="\r\n") {
  std::vector<std::string_view> result;
  size_t offset = 0;
  while (offset < str.size()) {
    const auto pos = str.find_first_of(delim, offset);
    // Emit an empty view even if two adjacent delimiters are found
    // this ensurs the HTTP "blank line" start of content is found
    result.emplace_back(str.substr(offset, pos - offset));
    if (pos == std::string_view::npos) break;
    offset = pos + delim.length();
  }
  return result;
}

uint32_t millis() {
  return to_us_since_boot(get_absolute_time()) / 1000;
}

bool wifi_connect(std::string network, std::string password, IPAddress dns_server, uint32_t timeout=10000) {
  printf("Connecting to %s...\n", network.c_str());
  wireless.wifi_set_passphrase(network, password);

  uint32_t t_start = millis();

  while(millis() - t_start < timeout) {
    if(wireless.get_connection_status() == WL_CONNECTED) {
      printf("Connected!\n");
      wireless.set_dns(1, dns_server, 0);
      return true;
    }
    wireless.set_led(255, 0, 0);
    sleep_ms(500);
    wireless.set_led(0, 0, 0);
    sleep_ms(500);
    printf("...\n");
  }

  return false;
}

int socket_accept(int8_t server_sock) {
  uint8_t new_sock = wireless.avail_server(server_sock);
  if(new_sock != server_sock
  && new_sock != 255) {
     return new_sock;
  }
  return -1;
}

bool has_suffix(std::string_view path, std::string_view suffix) {
  return path.length() >= suffix.length() && path.compare(path.length() - suffix.length(), suffix.length(), suffix) == 0;
}

uint8_t start_server(uint16_t http_port) {
  printf("Starting server...\n", NETWORK);
  // Get a socket for our server
  uint8_t server_sock = wireless.get_socket();
  wireless.start_server(http_port, server_sock);
  while(1){
    uint8_t state = wireless.get_server_state(server_sock);
    if(state == LISTEN) break;
  }
  IPAddress ip;
  wireless.get_ip_address(ip);
  printf("Server listening on %s:%i\n", ip.to_string().c_str(), http_port);
  return server_sock;
}

bool poll_http_requests(uint8_t server_sock, http_request_handler request_handler, uint32_t timeout = 1000) {
  uint16_t request_length = 0;
  uint16_t avail_length = 0;
  uint32_t t_start = millis();

  // Clear the request buffer
  memset(request_buf, 0, HTTP_REQUEST_BUF_SIZE);

  // Try to accept an incoming connection
  int client_sock = socket_accept(server_sock);
  if(client_sock == -1) return false;

  printf("Got client: %d\n", client_sock);

  // Any data? This *usually* seems to be available immediately,
  // so we can dispense with any ongoing polling and hope for the best.
  avail_length = wireless.avail_data(client_sock);
  if(avail_length == 0) return false;

  printf("Got request: %i bytes\n",  avail_length);

  // Read the full response
  // Sometimes the bytes read is less than the bytes we request, so loop until we get the data we expect
  while(request_length < avail_length) {
    uint16_t read_length = avail_length; // Request the full buffer
    wireless.get_data_buf(client_sock, request_buf + request_length, &read_length);
    request_length += read_length; // Increment the response_length by the amount we actually read

    // Also check for timeouts here, too
    if(millis() - t_start >= timeout) break;
  }

  // Bail if we timed out.
  if(millis() - t_start >= timeout) {
    wireless.stop_client(client_sock);
  };

  if(request_length > 0) {
    std::vector<std::string_view> request = split(std::string_view((char *)request_buf, request_length));
    std::vector<std::string_view> request_body;
    std::vector<std::string_view> request_detail = split(request[0], " ");
    http_request_method_t method;

    // Bail early on an invalid HTTP request
    if(request_detail[0].compare("POST") == 0) {
      method = POST;
    } else if (request_detail[0].compare("GET") == 0) {
      method = GET;
    } else { // PUT, DELETE, HEAD?
      // Return a 501 unimplemented
      wireless.send_data(client_sock, (const uint8_t *)response_501.data(), response_501.length());
      wireless.stop_client(client_sock);
      return false;
    }

    // Get the request path so we can handle routes
    std::string_view path = request_detail[1];

    // Scan for the blank line indicating content start for form post data
    auto body_start = std::find(request.begin(), request.end(), "");

    // Split the body from the head (ow!)
    if(body_start != request.end()) {
      request_body = std::vector<std::string_view>(body_start + 1, request.end());
      request = std::vector<std::string_view>(request.begin(), body_start);
    }

    std::string response_head;
    std::string content_type = "text/html";

    http_response_t response = request_handler(method, path, request, request_body);

    switch(response.content_type) {
      case TEXT_HTML:
        content_type = "text/html";
        break;
      case IMAGE_SVG:
        content_type = "image/svg+xml";
        break;
    }

    if(response.response_code == 200) {
      response_head = "HTTP/1.1 200 OK\nContent-Length: " + std::to_string(response.response_body.length()) + "\nContent-Type: " + content_type + "\n\n";
    } else { // Assume 404
      response_head = "HTTP/1.1 404 File Not Found\nContent-Length: 22\nContent-Type: " + content_type + "\n\n";
    }

    wireless.send_data(client_sock, (const uint8_t *)response_head.data(), response_head.length());

    uint16_t chunks = (response.response_body.length() / HTTP_RESPONSE_CHUNK_SIZE) + 1;
    for(auto chunk = 0u; chunk < chunks; chunk++) {
      uint16_t offset = chunk * HTTP_RESPONSE_CHUNK_SIZE;
      uint16_t length = std::min(response.response_body.length() - offset, HTTP_RESPONSE_CHUNK_SIZE);
      if(length == 0) break;
      wireless.send_data(client_sock, (const uint8_t *)response.response_body.data() + offset, length);
      sleep_ms(1);
    }

    wireless.stop_client(client_sock);

    return true;
  }

  return false;
}

int main() {
  stdio_init_all();

  fr = f_mount(&fs, "", 1);
  if (fr != FR_OK) {
      printf("Failed to mount SD card, error: %d\n", fr);
      return 0;
  }

  FILINFO file;
  auto dir = new DIR();
  printf("Listing /\n");
  f_opendir(dir, "/");
  while(f_readdir(dir, &file) == FR_OK && file.fname[0]) {
      printf("%s %d\n", file.fname, file.fsize);
  }
  f_closedir(dir);

  wireless.init();
  sleep_ms(500);

  printf("Firmware version Nina %s\n", wireless.get_fw_version());

  if(!wifi_connect(NETWORK, PASSWORD, USE_DNS)) {
    return 0;
  }

  uint8_t server_sock = start_server(HTTP_PORT);

  g = 255;
  wireless.set_led(r, g, b);

  while(1) {
    // Handle any incoming HTTP requests
    poll_http_requests(server_sock, [](
      http_request_method_t request_method,
      std::string_view request_path,
      std::vector<std::string_view> request_head,
      std::vector<std::string_view> request_body) -> http_response_t {

      http_response_t response;
      response.response_body = "Nothing to see here...";
      response.response_code = 404;

      printf("Request path: \"%s\"\n", std::string(request_path).c_str());

      // Only serve HTML files!
      if (has_suffix(request_path, ".html")) {
        fr = f_open(&fil, std::string(request_path).c_str(), FA_READ);
        if(fr == FR_OK) {
          size_t read = 0;
          f_read(&fil, response_buf, HTTP_RESPONSE_BUF_SIZE, &read);
          f_close(&fil);
          response.response_body = std::string_view((char*)response_buf, read);
          response.response_code = 200;
        }
      // And, maybe SVG!?
      } else if (has_suffix(request_path, ".svg")) {
        fr = f_open(&fil, std::string(request_path).c_str(), FA_READ);
        if(fr == FR_OK) {
          size_t read = 0;
          f_read(&fil, response_buf, HTTP_RESPONSE_BUF_SIZE, &read);
          f_close(&fil);
          response.response_body = std::string_view((char*)response_buf, read);
          response.response_code = 200;
          response.content_type = IMAGE_SVG;
        }
      /// Oooh! Directory listings...
      } else if (has_suffix(request_path, "/")) {
        FILINFO file;
        auto dir = new DIR();
        printf("Listing directory: %s\n", std::string(request_path).c_str());
        f_opendir(dir, std::string(request_path).c_str());
        uint16_t offset = 0u;
        while(f_readdir(dir, &file) == FR_OK && file.fname[0]) {
          std::string filename = std::string(file.fname);
          if(!has_suffix(filename, ".svg")  && !has_suffix(filename, ".html")) continue;
          snprintf((char *)response_buf + offset, (size_t)HTTP_RESPONSE_BUF_SIZE - offset, "<li><a href=\"%s%s\">%s</a>", std::string(request_path).c_str(), file.fname, file.fname);
          offset += filename.length() * 2 + 20 + request_path.length(); // Ugh
        }
        f_closedir(dir);
        response.response_body = std::string_view((char*)response_buf, offset);
        response.response_code = 200;
      }

      return response;


    });
    sleep_ms(10);
  }

  return 0;
}
