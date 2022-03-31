#pragma once

#include <string.h>
#include <string>
#include "pico/stdlib.h"
#include "spi_drv.hpp"
#include "ip_address.hpp"

#define WL_FW_VER_LENGTH 6

#define WIFI_SPI_ACK        1
#define WIFI_SPI_ERR        0xFF

// Maxmium number of socket
#define	WIFI_MAX_SOCK_NUM	10
// Socket not available constant
#define SOCK_NOT_AVAIL  255
// Default state value for Wifi state field
#define NA_STATE -1
//Maximum number of attempts to establish wifi connection
#define WL_MAX_ATTEMPT_CONNECTION	10

enum wl_ping_result_t {
  WL_PING_DEST_UNREACHABLE = -1,
  WL_PING_TIMEOUT = -2,
  WL_PING_UNKNOWN_HOST = -3,
  WL_PING_ERROR = -4
};

enum wl_status_t {
	WL_NO_SHIELD = 255,
  WL_NO_MODULE = WL_NO_SHIELD,
  WL_IDLE_STATUS = 0,
  WL_NO_SSID_AVAIL,
  WL_SCAN_COMPLETED,
  WL_CONNECTED,
  WL_CONNECT_FAILED,
  WL_CONNECTION_LOST,
  WL_DISCONNECTED,
  WL_AP_LISTENING,
  WL_AP_CONNECTED,
  WL_AP_FAILED
};


enum wl_tcp_state {
  CLOSED      = 0,
  LISTEN      = 1,
  SYN_SENT    = 2,
  SYN_RCVD    = 3,
  ESTABLISHED = 4,
  FIN_WAIT_1  = 5,
  FIN_WAIT_2  = 6,
  CLOSE_WAIT  = 7,
  CLOSING     = 8,
  LAST_ACK    = 9,
  TIME_WAIT   = 10
};

enum wl_enc_type {  /* Values map to 802.11 encryption suites... */
    ENC_TYPE_WEP  = 5,
    ENC_TYPE_TKIP = 2,
    ENC_TYPE_CCMP = 4,
    /* ... except these two, 7 and 8 are reserved in 802.11-2007 */
    ENC_TYPE_NONE = 7,
    ENC_TYPE_AUTO = 8,

    ENC_TYPE_UNKNOWN = 255
};

enum wl_error_code_t {
    WL_FAILURE = -1,
    WL_SUCCESS = 1,
};

enum sv_protocol_mode {
    TCP_MODE,
    UDP_MODE,
    TLS_MODE,
    UDP_MULTICAST_MODE,
    TLS_BEARSSL_MODE
};


#define KEY_IDX_LEN     1



namespace pimoroni {

  class Esp32Spi {
    //--------------------------------------------------
    // Constants
    //--------------------------------------------------
  public:
    static const uint8_t INPUT = 0;
    static const uint8_t OUTPUT = 1;
    static const uint8_t INPUT_PULLUP = 2;


    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:
    SpiDrv driver;

    char network_ssid[WL_NETWORKS_LIST_MAXNUM][WL_SSID_MAX_LENGTH];

  	// Firmware version string in the format a.b.c
	  char fw_version[WL_FW_VER_LENGTH];

	  // Settings of current selected network
	  char ssid[WL_SSID_MAX_LENGTH];
	  uint8_t bssid[WL_MAC_ADDR_LENGTH];
	  uint8_t mac[WL_MAC_ADDR_LENGTH];
	  uint8_t local_ip[WL_IPV4_LENGTH];
	  uint8_t subnet_mask[WL_IPV4_LENGTH];
	  uint8_t gateway_ip[WL_IPV4_LENGTH];


    //--------------------------------------------------
    // Methods
    //--------------------------------------------------
  public:
    virtual bool init();

    //--------------------------------------------------
    //From https://github.com/adafruit/WiFiNINA/blob/master/src/utility/wifi_drv.cpp
    //--------------------------------------------------

    bool get_network_data(uint8_t *ip_out, uint8_t *mask_out, uint8_t *gwip_out);
    bool get_remote_data(uint8_t sock, uint8_t *ip_out, uint8_t *port_out);

    int8_t wifi_set_network(const std::string ssid);
    int8_t wifi_set_passphrase(const std::string ssid, const std::string passphrase);
    int8_t wifi_set_key(const std::string ssid, uint8_t key_idx, const std::string key);

    void config(uint8_t valid_params, uint32_t local_ip, uint32_t gateway, uint32_t subnet);

    void set_dns(uint8_t valid_params, uint32_t dns_server1, uint32_t dns_server2);
    void set_hostname(std::string hostname);
    int8_t disconnect();

    uint8_t get_connection_status();
    uint8_t* get_mac_address();

    bool get_ip_address(IPAddress &ip_out);
    bool get_subnet_mask(IPAddress &mask_out);
    bool get_gateway_ip(IPAddress &ip_out);

    std::string get_current_ssid();
    uint8_t* get_current_bssid();
    int32_t get_current_rssi();
    uint8_t get_current_encryption_type();

    int8_t start_scan_networks();
    uint8_t get_scan_networks();
    const char* get_ssid_networks(uint8_t network_item);

    wl_enc_type get_enc_type_networks(uint8_t network_item);
    uint8_t* get_bssid_networks(uint8_t network_item, uint8_t* bssid_out);
    uint8_t get_channel_networks(uint8_t network_item);
    int32_t get_rssi_networks(uint8_t network_item);

    bool req_host_by_name(const std::string hostname);
    bool get_host_by_name(IPAddress& ip_out);
    bool get_host_by_name(const std::string hostname, IPAddress& ip_out);

    const char* get_fw_version();
    uint32_t get_time();
    void set_power_mode(uint8_t mode);

    int8_t wifi_set_ap_network(const std::string ssid, uint8_t channel);
    int8_t wifi_set_ap_passphrase(const std::string ssid, const std::string passphrase, uint8_t channel);

    int16_t ping(uint32_t ip_address, uint8_t ttl);

    void debug(uint8_t on);
    float get_temperature();
    void pin_mode(uint8_t pin, uint8_t mode);

    void digital_write(uint8_t pin, uint8_t value);
    void analog_write(uint8_t pin, uint8_t value);
    bool digital_read(uint8_t pin);
    uint16_t analog_read(uint8_t pin, uint8_t atten = 3);

    //--------------------------------------------------
    //From https://github.com/adafruit/WiFiNINA/blob/master/src/utility/server_drv.cpp
    //--------------------------------------------------

    void start_server(uint16_t port, uint8_t sock, uint8_t protocol_mode = TCP_MODE);
    void start_server(uint32_t ip_address, uint16_t port, uint8_t sock, uint8_t protocol_mode = TCP_MODE);
    void start_client(uint32_t ip_address, uint16_t port, uint8_t sock, uint8_t protocol_mode = TCP_MODE);
    void start_client(const std::string host, uint32_t ip_address, uint16_t port, uint8_t sock, uint8_t protocol_mode = TCP_MODE);
    void stop_client(uint8_t sock);
    
    uint8_t get_server_state(uint8_t sock);
    uint8_t get_client_state(uint8_t sock);
    uint16_t avail_data(uint8_t sock);
    uint8_t avail_server(uint8_t sock); // a weird copy of avail_data that truncates to uint8_t and returns 255 if unavailable...
    // see: https://github.com/arduino-libraries/WiFiNINA/blob/e74d115d252bac24267e4b1a504c033f399924f2/src/utility/server_drv.cpp#L228-L288
    // and: https://github.com/adafruit/nina-fw/blob/d73fe315cc7f9148a0918490d3b75430c8444bf7/main/CommandHandler.cpp#L437-L498

    bool get_data(uint8_t sock, uint8_t *data_out, bool peek);
    bool get_data_buf(uint8_t sock, uint8_t *data_out, uint16_t *data_len_out);
    bool insert_data_buf(uint8_t sock, const uint8_t *data_in, uint16_t len);
    bool send_udp_data(uint8_t sock);

    uint16_t send_data(uint8_t sock, const uint8_t *data_in, uint16_t len);
    uint8_t check_data_sent(uint8_t sock);
    uint8_t get_socket();

    //--------------------------------------------------

    void wifi_set_ent_identity(const std::string identity);
    void wifi_set_ent_username(const std::string username);
    void wifi_set_ent_password(const std::string password);
    void wifi_set_ent_enable();

    void sleep_set_wake_pin(uint8_t wake_pin);
    void sleep_light();
    void sleep_deep(uint8_t time);
  };

}