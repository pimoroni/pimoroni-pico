#pragma once

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

// Maximum size of a SSID
#define WL_SSID_MAX_LENGTH 32
// Length of passphrase. Valid lengths are 8-63.
#define WL_WPA_KEY_MAX_LENGTH 63
// Length of key in bytes. Valid values are 5 and 13.
#define WL_WEP_KEY_MAX_LENGTH 13
// Size of a MAC-address or BSSID
#define WL_MAC_ADDR_LENGTH 6
// Size of a MAC-address or BSSID
#define WL_IPV4_LENGTH 4
// Maximum size of a SSID list
#define WL_NETWORKS_LIST_MAXNUM	10


#define IF_CHECK_START_CMD()              \
if(!wait_for_byte(START_CMD)) {           \
    printf("Error waiting START_CMD\n");  \
    return false;                         \
}                                         \
else                                      \


#define CHECK_DATA(check, x)              \
if(!read_and_check_byte(check, &x)) {     \
    printf("Reply error\n");              \
    return false;                         \
}                                         \
else                                      \

namespace pimoroni {

  class SpiDrv {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t DEFAULT_CS_PIN     = 7;
    static const uint8_t DEFAULT_SCK_PIN    = 18;
    static const uint8_t DEFAULT_MOSI_PIN   = 19;
    static const uint8_t DEFAULT_MISO_PIN   = 16;
    static const uint8_t DEFAULT_RESETN_PIN = 11;
    static const uint8_t DEFAULT_GPIO0_PIN  = 2;
    static const uint8_t DEFAULT_ACK_PIN    = 10;

    static const uint8_t DUMMY_DATA = 0xFF;

  private:
    static const uint8_t START_CMD  = 0xE0;
    static const uint8_t END_CMD    = 0xEE;
    static const uint8_t ERR_CMD    = 0xEF;

    static const uint8_t CMD_FLAG   = 0;
    static const uint8_t REPLY_FLAG = 1 << 7;
    static const uint8_t DATA_FLAG  = 0x40;

    static const uint8_t CMD_POS        = 1;    // Position of Command OpCode on SPI stream
    static const uint8_t PARAM_LEN_POS  = 2;    // Position of Param len on SPI stream

    static const int BYTE_TIMEOUT = 1000;


    //--------------------------------------------------
    // Enums
    //--------------------------------------------------
  public:
    enum numParams : uint8_t {
      PARAM_NUMS_0 = 0,
      PARAM_NUMS_1,
      PARAM_NUMS_2,
      PARAM_NUMS_3,
      PARAM_NUMS_4,
      PARAM_NUMS_5
    };

    enum lastParam : bool {
      NO_LAST_PARAM = false,
      LAST_PARAM = true,
    };


    //--------------------------------------------------
    // Substructures
    //--------------------------------------------------
  public:
    struct tParam {
      uint8_t param_len;
	    uint8_t* param;
    };


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    spi_inst_t *spi = spi0;

    int8_t cs     = DEFAULT_CS_PIN;
    int8_t sck    = DEFAULT_SCK_PIN;
    int8_t mosi   = DEFAULT_MOSI_PIN;
    int8_t miso   = DEFAULT_MISO_PIN;
    int8_t resetn = DEFAULT_RESETN_PIN;
    int8_t gpio0  = DEFAULT_GPIO0_PIN;
    int8_t ack    = DEFAULT_ACK_PIN;


    //--------------------------------------------------
    // Constructors/Destructor
    //--------------------------------------------------
  public:
    SpiDrv() {}

    SpiDrv(spi_inst_t *spi,
           uint8_t cs, uint8_t sck, uint8_t mosi, uint8_t miso,
           uint8_t resetn, uint8_t gpio0, uint8_t ack) :
      spi(spi), cs(cs), sck(sck), mosi(mosi), miso(miso),
      resetn(resetn), gpio0(gpio0), ack(ack) {}


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    void init();
    void reset();
    
    bool available();

    void esp_select();        
    void esp_deselect();

    bool get_esp_ready();
    bool get_esp_ack();

    void wait_for_esp_ack();
    void wait_for_esp_ready();
    void wait_for_esp_select();
    int wait_for_byte(uint8_t wait_byte);
        
    bool read_and_check_byte(uint8_t check_byte, uint8_t *byte_out);
    uint8_t read_byte();

    bool wait_response_params(uint8_t cmd, uint8_t num_param, tParam *params_out);
    bool wait_response_cmd(uint8_t cmd, uint8_t num_param, uint8_t *param_out, uint8_t *param_len_out);
    bool wait_response_data8(uint8_t cmd, uint8_t *param_out, uint8_t *param_len_out);    
    bool wait_response_data16(uint8_t cmd, uint8_t *param_out, uint16_t *param_len_out);
    bool wait_response(uint8_t cmd, uint8_t *num_param_out, uint8_t **params_out, uint8_t max_num_params);

    void send_param(const uint8_t* param, uint8_t param_len, lastParam last_param = NO_LAST_PARAM);
    void send_param_len8(uint8_t param_len);
    void send_param_len16(uint16_t param_len);

    uint8_t read_param_len8(uint8_t *param_len_out = nullptr);
    uint16_t read_param_len16(uint16_t *param_len_out = nullptr);

    void send_buffer(const uint8_t *param, uint16_t param_len, lastParam last_param = NO_LAST_PARAM);
    void send_param(uint16_t param, lastParam last_param = NO_LAST_PARAM);
    void send_byte_param(uint8_t param, lastParam last_param = NO_LAST_PARAM);
    void send_cmd(uint8_t cmd, uint8_t num_param);

    void pad_to_multiple_of_4(int command_size);

  private:
    void get_param(uint8_t *param_out);
  };
}