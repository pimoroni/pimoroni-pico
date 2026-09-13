#include <stdio.h>
#include <string.h>
#include <time.h>

#include "badger2040w.hpp"
#include "common/pimoroni_common.hpp"
#include "lwip/dns.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

#define NTP_SERVER "pool.ntp.org"

using namespace pimoroni;

Badger2040W badger;

typedef void (*ntp_callback_t)(datetime_t *datetime, void *arg);
static volatile bool ntp_time_set = false;

typedef struct NTP_T_ {
    ip_addr_t ntp_server_address;
    struct udp_pcb *ntp_pcb;
    alarm_id_t ntp_failure_alarm;
    ntp_callback_t ntp_callback;
    void *ntp_callback_arg;
} NTP_T;

#define NTP_MSG_LEN 48
#define NTP_PORT 123
#define NTP_DELTA 2208988800  // seconds between 1 Jan 1900 and 1 Jan 1970
#define NTP_FAILURE_TIME (5 * 1000)

static void ntp_convert_epoch(const time_t *epoch, datetime_t *datetime) {
    struct tm *timeinfo = localtime(epoch);
    datetime->year = timeinfo->tm_year + 1900;
    datetime->month = timeinfo->tm_mon + 1;
    datetime->day = timeinfo->tm_mday;
    datetime->dotw = timeinfo->tm_wday;
    datetime->hour = timeinfo->tm_hour;
    datetime->min = timeinfo->tm_min;
    datetime->sec = timeinfo->tm_sec;
}

// Called with results of operation
static void ntp_result(NTP_T *state, int status, time_t *result) {
    if (state->ntp_failure_alarm > 0) {
        cancel_alarm(state->ntp_failure_alarm);
        state->ntp_failure_alarm = 0;
    }

    if (status == 0 && result) {
        datetime_t datetime;
        ntp_convert_epoch(result, &datetime);
        if (state->ntp_callback) {
            state->ntp_callback(&datetime, state->ntp_callback_arg);
        }
    } else {
        if (state->ntp_callback) {
            state->ntp_callback(NULL, state->ntp_callback_arg);
        }
    }
    free(state);
}

// Make an NTP request
static void ntp_request(NTP_T *state) {
    cyw43_arch_lwip_begin();
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, NTP_MSG_LEN, PBUF_RAM);
    uint8_t *req = (uint8_t *)p->payload;
    memset(req, 0, NTP_MSG_LEN);
    req[0] = 0x1b;
    udp_sendto(state->ntp_pcb, p, &state->ntp_server_address, NTP_PORT);
    pbuf_free(p);
    cyw43_arch_lwip_end();
}

static int64_t ntp_failed_handler(alarm_id_t id, void *user_data) {
    NTP_T *state = (NTP_T *)user_data;
    printf("ntp request failed\n");
    ntp_result(state, -1, NULL);
    return 0;
}

// Call back with a DNS result
static void ntp_dns_found(const char *hostname, const ip_addr_t *ipaddr, void *arg) {
    NTP_T *state = (NTP_T *)arg;
    if (ipaddr) {
        state->ntp_server_address = *ipaddr;
        printf("ntp address %s\n", ipaddr_ntoa(ipaddr));
        ntp_request(state);
    } else {
        printf("ntp dns request failed\n");
        ntp_result(state, -1, NULL);
    }
}

// NTP data received
static void ntp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port) {
    NTP_T *state = (NTP_T *)arg;
    uint8_t mode = pbuf_get_at(p, 0) & 0x7;
    uint8_t stratum = pbuf_get_at(p, 1);

    // Check the result
    if (ip_addr_cmp(addr, &state->ntp_server_address) && port == NTP_PORT && p->tot_len == NTP_MSG_LEN &&
        mode == 0x4 && stratum != 0) {
        uint8_t seconds_buf[4] = {0};
        pbuf_copy_partial(p, seconds_buf, sizeof(seconds_buf), 40);
        uint32_t seconds_since_1900 = seconds_buf[0] << 24 | seconds_buf[1] << 16 | seconds_buf[2] << 8 | seconds_buf[3];
        uint32_t seconds_since_1970 = seconds_since_1900 - NTP_DELTA;
        time_t epoch = seconds_since_1970;
        ntp_result(state, 0, &epoch);
    } else {
        printf("invalid ntp response\n");
        ntp_result(state, -1, NULL);
    }
    pbuf_free(p);
}

// Perform initialisation
static NTP_T *ntp_init(ntp_callback_t callback, void *arg) {
    NTP_T *state = (NTP_T *)calloc(1, sizeof(NTP_T));
    if (!state) {
        printf("failed to allocate state\n");
        return NULL;
    }
    state->ntp_pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
    if (!state->ntp_pcb) {
        printf("failed to create pcb\n");
        free(state);
        return NULL;
    }
    state->ntp_callback = callback;
    state->ntp_callback_arg = arg;
    udp_recv(state->ntp_pcb, ntp_recv, state);
    return state;
}

void ntp_get_time(ntp_callback_t callback, void *arg) {
    NTP_T *state = ntp_init(callback, arg);
    if (!state)
        return;

    // Set alarm in case udp request is lost
    state->ntp_failure_alarm = add_alarm_in_ms(NTP_FAILURE_TIME, ntp_failed_handler, state, true);

    cyw43_arch_lwip_begin();
    int err = dns_gethostbyname(NTP_SERVER, &state->ntp_server_address, ntp_dns_found, state);
    cyw43_arch_lwip_end();

    if (err == ERR_OK) {  // dns record retrieved from cache
        ntp_request(state);
    } else if (err != ERR_INPROGRESS) {  // ERR_INPROGRESS means the dns callback is pending
        printf("dns request failed\n");
        ntp_result(state, -1, NULL);
    }
}

void ntp_callback(datetime_t *datetime, void *arg) {
    if (datetime == NULL) {
        ntp_time_set = true;
        return;
    }
    printf("Got datetime from NTP\n");
    badger.pcf85063a->set_datetime(datetime);
    rtc_set_datetime(datetime);

    // Flag that NTP callback has concluded
    ntp_time_set = true;
}

void retrieve_time(bool from_ntp) {
    ntp_time_set = false;
    // Check if RTC has been initialised previously, if not get the internet time via NTP
    if (from_ntp) {
        printf("Retrieving time from NTP\n");
        ntp_get_time(ntp_callback, NULL);
        // Block until the callback sets datetime
        while (!ntp_time_set) {
            tight_loop_contents();
        }
    } else {
        printf("Retrieving time from RTC\n");
        // Retrieve stored time from external RTC
        datetime_t datetime = badger.pcf85063a->get_datetime();
        // Store time in internal RTC
        rtc_set_datetime(&datetime);
    }
}

int main() {
    badger.init();
    rtc_init();
    stdio_init_all();
    printf("\n\n=======\nbadger2040 starting up\n\n");

    badger.graphics->set_font("bitmap8");
    badger.graphics->set_thickness(2);

    cyw43_arch_init();
    cyw43_arch_enable_sta_mode();
    if (cyw43_arch_wifi_connect_blocking(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK)) {
        printf("Failed to connect to wifi, halting\n");
        badger.halt();
    }

    // External RTC has 1 free byte, we can use this to determine whether the time has been set previously
    if (!badger.pcf85063a->get_byte()) {
        retrieve_time(true);

        // Set the external RTC free byte so we can later determine if it has been initalized
        badger.pcf85063a->set_byte(1);
    } else {
        retrieve_time(false);
    }

    char time_str[32];
    datetime_t datetime;
    while (true) {
        badger.graphics->set_pen(15);
        badger.graphics->clear();

        badger.graphics->set_pen(0);
        rtc_get_datetime(&datetime);
        sprintf(time_str, "%02d/%02d/%02d %02d:%02d\n", datetime.day, datetime.month, datetime.year, datetime.hour, datetime.min);
        int32_t time_x_centered = (badger.DISPLAY_WIDTH - badger.graphics->measure_text(time_str, 2.0f)) / 2;
        badger.graphics->text("Time (GMT+0)", Point(5, 10), badger.DISPLAY_WIDTH, 2.0f);
        badger.graphics->text(time_str, Point(time_x_centered, badger.DISPLAY_HEIGHT / 2 - 8), badger.DISPLAY_WIDTH, 2.0f);

        badger.update();

        // Wait for next minute before redrawing
        do {
          rtc_get_datetime(&datetime);
        } while(datetime.sec != 0);
    }
}
