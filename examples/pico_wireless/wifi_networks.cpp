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

using namespace pimoroni;

PicoWireless wireless;


int main() {
    stdio_init_all(); 
    wireless.init();
    sleep_ms(500);

    printf("firmware version Nina %s\n", wireless.get_fw_version());
    uint8_t* mac = wireless.get_mac_address();
    printf("mac address ");
    for(uint i = 0; i < WL_MAC_ADDR_LENGTH; i++) {
      printf("%d:", mac[i]);
    }
    printf("\n");

    while (true) {
        int networks = wireless.get_scan_networks();
        printf("Found %d network(s)\n", networks);

        for(auto network = 0; network < networks; network++) {
            std::string ssid = wireless.get_ssid_networks(network);
            wl_enc_type enc_type = wireless.get_enc_type_networks(network);
            switch(enc_type) {
                case ENC_TYPE_WEP:
                    printf("%s (WEP)\n", ssid.c_str());
                    break;
                case ENC_TYPE_TKIP:
                    printf("%s (TKIP)\n", ssid.c_str());
                    break;
                case ENC_TYPE_CCMP:
                    printf("%s (CCMP)\n", ssid.c_str());
                    break;
                case ENC_TYPE_UNKNOWN:
                    printf("%s (UNKNOWN)\n", ssid.c_str());
                    break;
                default:
                    printf("%s\n", ssid.c_str());
                    break;
            }
        }

        sleep_ms(5000);
    }
    return 0;
}