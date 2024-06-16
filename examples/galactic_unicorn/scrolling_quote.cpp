/*
 * scrolling_quote.cpp - a Galactic Unicorn example.
 *
 * This is an extension of the original 'scrolling_text.cpp' example, with 
 * additional code to fetch a quote to display from the Web.
 *
 * The quote is simply pulled via HTTP from http://quotes.rest/ to demonstrate
 * basic networking on the PicoW.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/apps/http_client.h"

#include "libraries/pico_graphics/pico_graphics.hpp"
#include "galactic_unicorn.hpp"
#include "okcolor.hpp"
#include "font8_data.hpp"

using namespace pimoroni;

#define QUOTE_BUFSIZE 1024

PicoGraphics_PenRGB888 graphics(53, 11, nullptr);
GalacticUnicorn galactic_unicorn;
char quote[QUOTE_BUFSIZE];
bool fetched_quote;


/* Handy utility function for sending debug messages straight to the Unicorn! */

void unicorn_message(const char *message) {
  graphics.set_pen(0, 0, 0);
  graphics.clear();
  graphics.set_pen(255, 255, 255);
  graphics.text(message, Point(1, 2), -1, 0.55);
  galactic_unicorn.update(&graphics);

  return;
}


/*
 * Callback functions used by lwIP for HTTP client requests. 
 */

/*
 * httpc_result - called once, when the complete HTTP request is complete.
 *                This is the point that we can extract the actual quote from
 *                our JSON, and flag that we've finished.
 */

void httpc_result(void *arg, httpc_result_t httpc_result,
                  u32_t rx_content_len, u32_t srv_res, err_t err) {

  char *quoteptr;

  /* Need to look for the "quote": element in the response. */
  quoteptr = strstr(quote, "\"quote\":" );
  if(quoteptr != nullptr) {
    /* Now, look for the next double quote after that semicolon. */
    quoteptr = strchr(quoteptr+7, '"');
    if(quoteptr != nullptr) {
      /* Copy everything beyond that double quote, and we have our string. */
      memmove(quote, quoteptr+1, strlen(quoteptr));

      /* Lastly, we need to find the end double quote, and terminate there. */
      quoteptr = strchr(quote, '"');
      if(quoteptr != nullptr) {
        *quoteptr = '\0';
      }
    }
  }

  /* And lastly, set a flag to indicate that the quote has been fetched fully. */
  fetched_quote = true;
  return;
}

/*
 * httpc_callback - called whenever data is received; the data is potentially
 *                  fragmented, so we may be called multiple times and need to
 *                  assemble this into one complete buffer.
 */

err_t httpc_callback(void *arg, struct altcp_pcb *conn, struct pbuf *p, err_t err) {

  static int recv_count = 0;
  struct pbuf *q;

  /*
   * We will potentially be called multiple times, so we build up our buffer
   * as we go. Largely cloned from the way internal lwIP routines work.
   */
  for(q = p; q != nullptr; q = q->next) {
    /* Sanity check that we won't blow our buffer size. */
    if((recv_count+q->len)>=QUOTE_BUFSIZE) {
      break;
    }

    /* And just copy it. */
    memcpy(&quote[recv_count], q->payload, q->len);
    recv_count += q->len;
  }

  /* Once we're done, we should free up the pbuf we worked with. */
  altcp_recved(conn, p->tot_len);
  pbuf_free(p);

  /* All good then! */
  return ERR_OK;
}

/*
 * fetch_quote - this function contains all the logic for connecting to WiFi,
 *               sending an HTTP request to the server and making sense of it.
 */

std::string fetch_quote( void ) {

  httpc_connection_t httpc_settings;

  /*
   * WiFi on the PicoW is handled by the CYW43 library (that being the chip
   * providing WiFi and BT services)
   */

  /* Initialise the library. */
  cyw43_arch_init();

  /* Enable 'station' mode - this indicates we want to connect to an AP */
  cyw43_arch_enable_sta_mode();

  /*
   * Lastly, we connect to the WiFi, using credentials supplied at compile time.
   * Note that we use a 'blocking' connection for simplicity - there are also
   * async versions of this call that allow you to carry on other processing
   * while you wait for the WiFi to come up.
   */
  switch(cyw43_arch_wifi_connect_blocking(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK)) {
    case PICO_OK:
      break;
    case PICO_ERROR_BADAUTH:
      return "WiFi Authentication failed";
    case PICO_ERROR_CONNECT_FAILED:
      return "WiFi connection failed";
    default:
      return "Unknown WiFi Error";
  }

  /* A return of PICO_OK indicates that WiFi is up, and TCP/IP available. */

  /*
   * Network comms are handled by lwIP, rather than CYW43. Happily, it includes
   * an HTTP client library, so our request is relatively simple.
   */
  fetched_quote = false;

  /* Need to provide settings; make sure it's empty apart from our results cb */
  memset(&httpc_settings, 0, sizeof(httpc_connection_t));
  httpc_settings.result_fn = httpc_result;

  /*
   * This function starts the request; httpc_callback will be called (possibly
   * multiple times) with the received data, before finally httpc_result is
   * called to indicate the transfer is complete.
   */
  auto response = httpc_get_file_dns(
    "quotes.rest", 80, "/qod?language=en", 
    &httpc_settings, httpc_callback, nullptr, nullptr
  );

  /* If this failed, we were unable to successfully send the request. */
  if(response != ERR_OK) {
    return "HTTPC failed to send request";
  }

  /*
   * The response is handled by the callback functions, so we will just wait
   * until our message buffer has a message in it. 
   */
  while(!fetched_quote) {
    sleep_ms(100);
  }

  /* Ok, we've finished all our work so shut down WiFi and return. */
  cyw43_arch_deinit();
  return quote;
}


/* main - entry point to the program. */

int main() {

  stdio_init_all();

  galactic_unicorn.init();
  graphics.set_font(&font8);
  sleep_ms(10);

  float scroll = -53.0f;

  /*
   * Before we try to fetch a quote, just display a message to reassure
   * folk that we're alive!
   */
  unicorn_message("Starting...");

  std::string message = fetch_quote();

  while(true) {

    if(galactic_unicorn.is_pressed(galactic_unicorn.SWITCH_BRIGHTNESS_UP)) {
      galactic_unicorn.adjust_brightness(+0.01);
    }
    if(galactic_unicorn.is_pressed(galactic_unicorn.SWITCH_BRIGHTNESS_DOWN)) {
      galactic_unicorn.adjust_brightness(-0.01);
    }

    int width = graphics.measure_text(message, 1);
    scroll += 0.25f;

    if(scroll > width) {
      scroll = -53.0f;
    }

    graphics.set_pen(0, 0, 0);
    graphics.clear();

    ok_color::HSL hsl{scroll / 100.0f, 1.0f, 0.5f};
    ok_color::RGB rgb = ok_color::okhsl_to_srgb(hsl);
    graphics.set_pen(rgb.r * 255, rgb.g * 255, rgb.b * 255);
    graphics.text(message, Point(0 - scroll, 2), -1, 0.55);

    galactic_unicorn.update(&graphics);

    sleep_ms(10);
  }

  return 0;
}
