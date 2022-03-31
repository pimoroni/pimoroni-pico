#pragma once

#include <stdio.h>
#include <string>
#include "pico/stdlib.h"
#include "hardware/spi.h"

//#define WARN(...) {printf(__VA_ARGS__);}
#define WARN(...) {}

#define PARAM_COUNT(params) sizeof(params) / sizeof(SpiDrv::inParam)

// Maximum size of a SSID
#define WL_SSID_MAX_LENGTH 32
// Length of passphrase. Valid lengths are 8-63.
#define WL_WPA_KEY_MAX_LENGTH 63
// Length of key in bytes. Valid values are 5 and 13.
#define WL_WEP_KEY_MAX_LENGTH 13
// Size of a MAC-address or BSSID
#define WL_MAC_ADDR_LENGTH 6
// Size of a IP address
#define WL_IPV4_LENGTH 4
// Maximum size of a SSID list
#define WL_NETWORKS_LIST_MAXNUM	10


#define IF_CHECK_START_CMD()              \
if(!wait_for_byte(START_CMD)) {           \
    WARN("Error waiting START_CMD\n");    \
    return false;                         \
}                                         \
else                                      \


#define CHECK_DATA(check, x)              \
if(!read_and_check_byte(check, &x)) {     \
    WARN("Reply error\n");                \
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

    static const int BYTE_TIMEOUT = 5000;
    static const int SELECT_ACK_TIMEOUT = 5000;


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

    enum p_type : uint8_t {
        PARAM_NORMAL,
        PARAM_BUFFER,
        PARAM_DUMMY,
    };

    enum cmd_response_type : uint8_t {
        RESPONSE_TYPE_NORMAL,
        RESPONSE_TYPE_CMD,
        RESPONSE_TYPE_DATA8,
        RESPONSE_TYPE_DATA16,
    };

    enum s_sleep_state : uint8_t {
        AWAKE,
        LIGHT_SLEEP,
        DEEP_SLEEP
    };

    s_sleep_state sleep_state = AWAKE;

    const char* commands[88] = {
      // 0x00 -> 0x0f
      "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL",

      // 0x10 -> 0x1f
      "setNet", "setPassPhrase", "setKey", "NULL", "setIPconfig", "setDNSconfig", "setHostname", "setPowerMode", "setApNet", "setApPassPhrase", "setDebug", "getTemperature", "NULL", "NULL", "NULL", "NULL",

      // 0x20 -> 0x2f
      "getConnStatus", "getIPaddr", "getMACaddr", "getCurrSSID", "getCurrBSSID", "getCurrRSSI", "getCurrEnct", "scanNetworks", "startServerTcp", "getStateTcp", "dataSentTcp", "availDataTcp", "getDataTcp", "startClientTcp", "stopClientTcp", "getClientStateTcp",

      // 0x30 -> 0x3f
      "disconnect", "NULL", "getIdxRSSI", "getIdxEnct", "reqHostByName", "getHostByName", "startScanNetworks", "getFwVersion", "NULL", "sendUDPdata", "getRemoteData", "getTime", "getIdxBSSID", "getIdxChannel", "ping", "getSocket",

      // 0x40 -> 0x4f
      "setClientCert", "setCertKey", "NULL", "NULL", "sendDataTcp", "getDataBufTcp", "insertDataBuf", "NULL", "NULL", "NULL", "wpa2EntSetIdentity", "wpa2EntSetUsername", "wpa2EntSetPassword", "wpa2EntSetCACert", "wpa2EntSetCertKey", "wpa2EntEnable",

      // 0x50 -> 0x5f
      "setPinMode", "setDigitalWrite", "setAnalogWrite", "setDigitalRead", "setAnalogRead", "setWakePin", "setLightSleep", "setDeepSleep"
    };

    //--------------------------------------------------
    // Substructures
    //--------------------------------------------------
  public:
    struct outParam {
      uint8_t param_len;
	    uint8_t* param;
    };

    struct inParam {
      const uint8_t *addr = nullptr;
      const uint16_t len = 0;
      const p_type type = PARAM_NORMAL;
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
    uint32_t command_length = 0;
  
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

    bool wait_for_esp_ack(uint32_t timeout_ms=SELECT_ACK_TIMEOUT);
    bool wait_for_esp_ready(uint32_t timeout_ms=SELECT_ACK_TIMEOUT);
    bool wait_for_esp_select(uint32_t timeout_ms=SELECT_ACK_TIMEOUT);
    int wait_for_byte(uint8_t wait_byte);
        
    bool read_and_check_byte(uint8_t check_byte, uint8_t *byte_out);
    uint8_t read_byte();

    bool wait_response_params(uint8_t cmd, uint8_t num_param, outParam *params_out);
    bool wait_response_cmd(uint8_t cmd, uint8_t num_param, uint8_t *param_out, uint16_t *param_len_out);
    bool wait_response_data8(uint8_t cmd, uint8_t *param_out, uint16_t *param_len_out);    
    bool wait_response_data16(uint8_t cmd, uint8_t *param_out, uint16_t *param_len_out);
    bool wait_response(uint8_t cmd, uint16_t *num_param_out, uint8_t **params_out, uint8_t max_num_params);

    void send_param(const uint8_t* param, uint8_t param_len);
    void send_param_len8(uint8_t param_len);
    void send_param_len16(uint16_t param_len);

    uint8_t read_param_len8(uint16_t *param_len_out = nullptr);
    uint16_t read_param_len16(uint16_t *param_len_out = nullptr);

    void send_buffer(const uint8_t *param, uint16_t param_len);
    void start_cmd(uint8_t cmd, uint8_t num_param);
    void end_cmd();

    void pad_to_multiple_of_4(int command_size);

    static inParam build_param(const std::string *param) {
      return inParam{.addr = (const uint8_t *)param->data(), .len = (uint16_t)param->length(), .type = PARAM_NORMAL};
    };
    static inParam build_param(uint32_t *param) {
      // This type is basically just for IP Addresses AFAIK, probably byteswap any uint32_t you want to survive...
      return inParam{.addr = (uint8_t *)param, .len = 4, .type = PARAM_NORMAL};
    };
    static inParam build_param(uint16_t *param) {
      // Beware ye who doth not pass a byteswapped uint16_t to this function
      return inParam{.addr = (uint8_t *)param, .len = 2, .type = PARAM_NORMAL};
    };
    static inParam build_param(uint8_t *param) {
      // Single bytes are easy, they can't get weird
      return inParam{.addr = param, .len = 1, .type = PARAM_NORMAL};
    };
    static inParam build_param(const uint8_t *buffer, uint16_t len) {
      // Normal params are good up to len 255
      return inParam{.addr = buffer, .len = len, .type = PARAM_NORMAL};
    };
    static inParam build_param_buffer(const uint8_t *buffer, uint16_t len) {
      // Buffer-type params are for length  >255 (usually sending data) but <= 65535 because a uint32_t is too costly for an ESP32 :/
      return inParam{.addr = buffer, .len = len, .type = PARAM_BUFFER};
    };
    static inParam build_param_dummy() {
      return inParam{.type = PARAM_DUMMY};
    };
    bool send_command(uint8_t command, const inParam *params_in, uint8_t num_in, uint8_t *data, uint16_t *data_len, cmd_response_type response_type=RESPONSE_TYPE_CMD);
    bool send_command(uint8_t command, SpiDrv::outParam *params_out, SpiDrv::numParams num_out);
  private:
    void get_param(uint8_t *param_out);
  };
}