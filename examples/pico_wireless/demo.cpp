#include <stdio.h>
#include <string.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico_wireless.hpp"
#include <chrono>
#include <sstream>
#include <iomanip>
#include "secrets.h"

using namespace pimoroni;

#define UART_ID uart1 //uart0
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 8 //0
#define UART_RX_PIN 9 //1



// HSV Conversion expects float inputs in the range of 0.00-1.00 for each channel
// Outputs are rgb in the range 0-255 for each channel
void from_hsv(float h, float s, float v, uint8_t &r, uint8_t &g, uint8_t &b) {
  float i = floor(h * 6.0f);
  float f = h * 6.0f - i;
  v *= 255.0f;
  uint8_t p = v * (1.0f - s);
  uint8_t q = v * (1.0f - f * s);
  uint8_t t = v * (1.0f - (1.0f - f) * s);

  switch (int(i) % 6) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    case 5: r = v; g = p; b = q; break;
  }
}

PicoWireless wireless;

#define PICO_LED        25

int main() {
    stdio_init_all();
    //stdio_set_translate_crlf(&stdio_usb, false);

    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    uart_init(UART_ID, BAUD_RATE);

    gpio_put(PICO_LED, true);

    printf("Initialised\n");    

    wireless.init();

    sleep_ms(1000);

    uint8_t r, g, b;
    uint8_t a = 0;
    while(!wireless.is_pressed(PicoWireless::A)) {
        from_hsv((float)a/256.0f, 1, 1, r, g, b);
        
        wireless.set_led(0, 0, b);
        sleep_ms(10);
        a++;
    }
    wireless.set_led(16, 16, 0);

    printf("firmware version Nina %s\n", wireless.get_fw_version());
    uint8_t* mac = wireless.get_mac_address();
    printf("mac address ", wireless.get_mac_address()[0]);
    for(uint i =0; i < WL_MAC_ADDR_LENGTH; i++) {
      printf("%d:", mac[i]);
    }
    printf("\n");

    printf("starting connection\n");

    bool connected = wireless.wifi_set_passphrase(NETWORK, sizeof(NETWORK), PASSWORD, sizeof(PASSWORD));

    printf("waiting to establish connection status\n");
    while(wireless.get_connection_status() != 3) {
      sleep_ms(1000);
      printf("still waiting\n");
    }

    IPAddress ip;
    wireless.get_ip_address(ip);
    printf("ip address: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);

    IPAddress gateway;
    wireless.get_gateway_ip(gateway);
    printf("gateway address: %d.%d.%d.%d\n", gateway[0], gateway[1], gateway[2], gateway[3]);

    printf("SSID = %s\n", wireless.get_current_ssid());
    printf("RSSI = %d\n", wireless.get_current_rssi());
    
    uint8_t t = 0;
    while (true) {      
      from_hsv((float)t/256.0f, 1, 1, r, g, b);
      wireless.set_led(r, g, b);
      sleep_ms(10);
      t++;
      if(t == 0) {
        //printf("time: %d, temp: %f\n", wireless.get_time(), wireless.get_temperature());
        std::uint32_t time_date_stamp = wireless.get_time();
        std::time_t temp = time_date_stamp;
        std::tm* t = std::gmtime(&temp);
        std::stringstream ss; // or if you're going to print, just input directly into the output stream
        ss << std::put_time(t, "%Y-%m-%d %I:%M:%S %p\n");
        std::string output = ss.str();
        printf(output.c_str());
      }
    }
    return 0;
}