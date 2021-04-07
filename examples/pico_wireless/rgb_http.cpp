#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico_wireless.hpp"
#include "secrets.h"

#include <vector>
#include <algorithm>

#define HTTP_PORT 80
#define HTTP_REQUEST_BUF_SIZE 1024

using namespace pimoroni;

PicoWireless wireless;
uint8_t r, g, b;
std::vector<uint8_t> client_pool;
std::string body;
uint8_t buf[HTTP_REQUEST_BUF_SIZE];

std::vector<std::string_view> split(std::string_view str, const char delim='\n') {
    std::vector<std::string_view> result;
    size_t offset = 0;
    while (offset < str.size()) {
        const auto pos = str.find_first_of(std::basic_string_view(std::addressof(delim), 1), offset);
        if(pos != offset) result.emplace_back(str.substr(offset, pos - offset));
        if (pos == std::string_view::npos) break;
        offset = pos + 1;
    }
    return result;
}

void socket_accept(int8_t server_sock) {
  uint8_t new_sock = wireless.avail_server(server_sock);
  if(new_sock != server_sock
  && new_sock != 255
  && std::find(client_pool.begin(), client_pool.end(), new_sock) == client_pool.end()) {
    printf("New connection on socket: %i\n", new_sock);
    client_pool.push_back(new_sock);
  }
}

int main() {
    stdio_init_all(); 

    wireless.init();
    sleep_ms(500);

    printf("Firmware version Nina %s\n", wireless.get_fw_version());

    printf("Connecting to %s...\n", NETWORK);
    wireless.wifi_set_passphrase(NETWORK, PASSWORD);
    while(wireless.get_connection_status() != WL_CONNECTED) {
      wireless.set_led(255, 0, 0);
      sleep_ms(500);
      wireless.set_led(0, 0, 0);
      sleep_ms(500);
      printf("...\n");
    }
    printf("Connected!\n");

    printf("Starting server...\n", NETWORK);
    // Get a socket for our server
    uint8_t server_sock = wireless.get_socket();
    wireless.start_server(HTTP_PORT, server_sock);
    while(1){
      uint8_t state = wireless.get_server_state(server_sock);
      if(state == LISTEN) break;
    }
    IPAddress ip;
    wireless.get_ip_address(ip);
    printf("Server listening on %s:%i\n", ip.to_string().c_str(), HTTP_PORT);

    g = 255;
    wireless.set_led(r, g, b);

    while(1) {
      socket_accept(server_sock);

      for(auto &client_sock : client_pool) {
        uint16_t length = wireless.avail_data(client_sock);
        if(length > 0) {
          memset(buf, 0, HTTP_REQUEST_BUF_SIZE);

          if(wireless.get_data_buf(client_sock, buf, &length) && length > 0) {
            printf("%s\n", buf);
            std::vector<std::string_view> request = split(std::string_view((char *)buf, length));

            if(request[0].compare(0, 6, "POST /") == 0) {
              int i;
              for(i = 0; i < request.size(); i++) {
                if(request[i].length() == 1) {
                  printf("Found blank line at %i\n", i);
                  i++;
                  break;
                }
              }
              std::vector<std::string_view> post_data = split(request[i], '&');
              for(auto &data : post_data) {
                if(data.length() < 3) continue;
                if(data.compare(1, 1, "=") != 0) continue;
                switch(data[0]) {
                  case 'r':
                    r = atoi((const char *)(data.data() + 2));
                    printf("Got R: %i\n", r);
                    break;
                  case 'g':
                    g = atoi((const char *)(data.data() + 2));
                    printf("Got G: %i\n", g);
                    break;
                  case 'b':
                    b = atoi((const char *)(data.data() + 2));
                    printf("Got B: %i\n", b);
                    break;
                }
                wireless.set_led(r, g, b);
              }
            }
            body = "\
            <form method=\"post\" action=\"/\">\
              <input name=\"r\" type=\"number\" value=\"" + std::to_string(r) + "\" />\
              <input name=\"g\" type=\"number\" value=\"" + std::to_string(g) + "\"  />\
              <input name=\"b\" type=\"number\" value=\"" + std::to_string(b) + "\"  />\
              <input type=\"submit\" value=\"Set LED\" />\
            </form>";

            std::string response = "HTTP/1.1 200 OK\nContent-Length: " + std::to_string(body.length()) + "\nContent-Type: text/html\n\n" + body;
            wireless.send_data(client_sock, (const uint8_t *)response.data(), response.length());
          }
        }
      }
      sleep_ms(10);
    }

    return 0;
}