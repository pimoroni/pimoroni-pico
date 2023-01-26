#include <iomanip>
#include <sstream>
#include "pico_explorer.hpp"
#include "pico/stdlib.h"
#include "encoder.hpp"
#include "quadrature_out.pio.h"
#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "button.hpp"

/*
An interactive demo of how rotary encoders work.

Connect up an encoder (be it rotary or magnetic) as detailed below
and see the resulting signals and stats on the Pico Explorer's display.

Connections:
- A to GP0
- B to GP1
- C (if present) to GP2
- Switch (if present) to GP3

Buttons
- A is 'Zoom Out'
- X is 'Zoom In'
- B is 'Motor 1 Forward'
- Y is 'Motor 1 Reverse'
- Switch is 'Zero the Count'

If you do not have an encoder and wish to try out
this example, simulated A and B encoder signals can
be used by jumping GP0 to GP6 and GP1 to GP7.
*/

using namespace pimoroni;
using namespace encoder;
using namespace motor;

//--------------------------------------------------
// Constants
//--------------------------------------------------

// The pins used by the encoder
static const pin_pair ENCODER_PINS = {0, 1};
static const uint ENCODER_COMMON_PIN = 2;
static const uint ENCODER_SWITCH_PIN = 3;

// The counts per revolution of the encoder's output shaft
static constexpr float COUNTS_PER_REV = encoder::ROTARY_CPR;

// Set to true if using a motor with a magnetic encoder
static const bool COUNT_MICROSTEPS = false;

// Increase this to deal with switch bounce. 250 Gives a 1ms debounce
static const uint16_t FREQ_DIVIDER = 1;

// Time between each sample, in microseconds
static const int32_t TIME_BETWEEN_SAMPLES_US = 100;

// The full time window that will be stored
static const int32_t WINDOW_DURATION_US = 1000000;

static const int32_t READINGS_SIZE = WINDOW_DURATION_US / TIME_BETWEEN_SAMPLES_US;
static const int32_t SCRATCH_SIZE = READINGS_SIZE / 10;   // A smaller value, for temporarily storing readings during screen drawing

// Whether to output a synthetic quadrature signal
static const bool QUADRATURE_OUT_ENABLED = true;

// The frequency the quadrature output will run at (note that counting microsteps will show 4x this value)
static constexpr float QUADRATURE_OUT_FREQ = 800;

// Which first pin to output the quadrature signal to (e.g. GP6 and GP7)
static const float QUADRATURE_OUT_1ST_PIN = 6;

// How long there should be in microseconds between each screen refresh
static const uint64_t MAIN_LOOP_TIME_US = 50000;

// The zoom level beyond which edge alignment will be enabled to make viewing encoder patterns look nice
static const uint16_t EDGE_ALIGN_ABOVE_ZOOM = 4;



//--------------------------------------------------
// Enums
//--------------------------------------------------
enum DrawState {
  DRAW_LOW = 0,
  DRAW_HIGH,
  DRAW_TRANSITION,
};



//--------------------------------------------------
// Variables
//--------------------------------------------------
ST7789 st7789(PicoExplorer::WIDTH, PicoExplorer::HEIGHT, ROTATE_0, false, get_spi_pins(BG_SPI_FRONT));
PicoGraphics_PenRGB332 graphics(st7789.width, st7789.height, nullptr);

Button button_a(PicoExplorer::A);
Button button_b(PicoExplorer::B);
Button button_x(PicoExplorer::X);
Button button_y(PicoExplorer::Y);

Motor motor1(PicoExplorer::MOTOR1_PINS);

Encoder enc(pio0, 0, ENCODER_PINS, ENCODER_COMMON_PIN, NORMAL_DIR, COUNTS_PER_REV, COUNT_MICROSTEPS, FREQ_DIVIDER);

volatile bool enc_a_readings[READINGS_SIZE];
volatile bool enc_b_readings[READINGS_SIZE];
volatile bool enc_a_scratch[SCRATCH_SIZE];
volatile bool enc_b_scratch[SCRATCH_SIZE];
volatile uint32_t next_reading_index = 0;
volatile uint32_t next_scratch_index = 0;
volatile bool drawing_to_screen = false;
uint16_t current_zoom_level = 1;



////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t draw_plot(Point p1, Point p2, volatile bool (&readings)[READINGS_SIZE], uint32_t reading_pos, bool edge_align) {
  uint32_t reading_window = READINGS_SIZE / current_zoom_level;
  uint32_t start_index_no_modulus = (reading_pos + (READINGS_SIZE - reading_window));
  uint32_t start_index = start_index_no_modulus % READINGS_SIZE;
  int32_t screen_window = std::min(p2.x, (int32_t)PicoExplorer::WIDTH) - p1.x;

  bool last_reading = readings[start_index % READINGS_SIZE];

  uint32_t alignment_offset = 0;
  if(edge_align) {
    // Perform edge alignment by first seeing if there is a window of readings available (will be at anything other than x1 zoom)
    uint32_t align_window = (start_index_no_modulus - reading_pos);

    // Then go backwards through that window
    for(uint32_t i = 1; i < align_window; i++) {
      uint32_t align_index = (start_index + (READINGS_SIZE - i)) % READINGS_SIZE;
      bool align_reading = readings[align_index];

      // Has a transition from high to low been detected?
      if(!align_reading && align_reading != last_reading) {
        // Set the new start index from which to draw from and break out of the search
        start_index = align_index;
        alignment_offset = i;
        break;
      }
      last_reading = align_reading;
    }

    last_reading = readings[start_index % READINGS_SIZE];
  }

  // Go through each X pixel within the screen window
  uint32_t reading_window_start = 0;
  for(int32_t x = 0; x < screen_window; x++) {
    uint32_t reading_window_end = ((x + 1) * reading_window) / screen_window;

    // Set the draw state to be whatever the last reading was
    DrawState draw_state = last_reading ? DRAW_HIGH : DRAW_LOW;

    // Go through the readings in this window to see if a transition from low to high or high to low occurs
    if(reading_window_end > reading_window_start) {
      for(uint32_t i = reading_window_start; i < reading_window_end; i++) {
        bool reading = readings[(i + start_index) % READINGS_SIZE];
        if(reading != last_reading) {
          draw_state = DRAW_TRANSITION;
          break;  // A transition occurred, so no need to continue checking readings
        }
        last_reading = reading;
      }
      last_reading = readings[((reading_window_end - 1) + start_index) % READINGS_SIZE];
    }
    reading_window_start = reading_window_end;

    // Draw a pixel in a high or low position, or a line between the two if a transition
    switch(draw_state) {
      case DRAW_TRANSITION:
        for(uint8_t y = p1.y; y < p2.y; y++)
          graphics.pixel(Point(x + p1.x, y));
        break;
      case DRAW_HIGH:
        graphics.pixel(Point(x + p1.x, p1.y));
        break;
      case DRAW_LOW:
        graphics.pixel(Point(x + p1.x, p2.y - 1));
        break;
    }
  }

  // Return the alignment offset so subsequent encoder channel plots can share the alignment
  return alignment_offset;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool repeating_timer_callback(struct repeating_timer *t) {
  bool_pair state = enc.state();
  if(drawing_to_screen && next_scratch_index < SCRATCH_SIZE) {
    enc_a_scratch[next_scratch_index] = state.a;
    enc_b_scratch[next_scratch_index] = state.b;
    next_scratch_index++;
  }
  else {
    enc_a_readings[next_reading_index] = state.a;
    enc_b_readings[next_reading_index] = state.b;

    next_reading_index++;
    if(next_reading_index >= READINGS_SIZE) {
      next_reading_index = 0;
    }
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  stdio_init_all();

#ifdef PICO_DEFAULT_LED_PIN
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
#endif

  if(ENCODER_SWITCH_PIN != PIN_UNUSED) {
    gpio_init(ENCODER_SWITCH_PIN);
    gpio_set_dir(ENCODER_SWITCH_PIN, GPIO_IN);
    gpio_pull_down(ENCODER_SWITCH_PIN);
  }

  motor1.init();
  enc.init();

  bool_pair state = enc.state();
  for(uint i = 0; i < READINGS_SIZE; i++) {
    enc_a_readings[i] = state.a;
    enc_b_readings[i] = state.b;
  }

  if(QUADRATURE_OUT_ENABLED) {
    // Set up the quadrature encoder output
    PIO pio = pio1;
    uint offset = pio_add_program(pio, &quadrature_out_program);
    uint sm = pio_claim_unused_sm(pio, true);
    quadrature_out_program_init(pio, sm, offset, QUADRATURE_OUT_1ST_PIN, QUADRATURE_OUT_FREQ);
  }
}



////////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////////////////////////////////////////////////
int main() {
  Pen WHITE = graphics.create_pen(255, 255, 255);

  // Perform the main setup for the demo
  setup();

  // Begin the timer that will take readings of the coder at regular intervals
  struct repeating_timer timer;
  add_repeating_timer_us(-TIME_BETWEEN_SAMPLES_US, repeating_timer_callback, NULL, &timer);

  bool button_latch_a = false;
  bool button_latch_x = false;
  uint64_t last_time = time_us_64();

  while(true) {

    // Has enough time elapsed since we last refreshed the screen?
    uint64_t current_time = time_us_64();
    if(current_time > last_time + MAIN_LOOP_TIME_US) {
      last_time = current_time;

#ifdef PICO_DEFAULT_LED_PIN
      gpio_put(PICO_DEFAULT_LED_PIN, true);    // Show the screen refresh has stated
#endif

      // If the user has wired up their encoder switch, and it is pressed, set the encoder count to zero
      if(ENCODER_SWITCH_PIN != PIN_UNUSED && gpio_get(ENCODER_SWITCH_PIN)) {
        enc.zero();
      }

      // Capture the encoder state
      Encoder::Capture capture = enc.capture();

      // Spin Motor 1 either clockwise or counterclockwise depending on if B or Y are pressed
      if(button_b.read() && !button_y.read()) {
        motor1.speed(1.0f); // Full forward
      }
      else if(button_y.read() && !button_b.read()) {
        motor1.speed(-0.2f); // Reverse
      }
      else {
        motor1.speed(0.0f); // Stop
      }

      // If A has been pressed, zoom the view out to a min of x1
      if(button_a.read()) {
        if(!button_latch_a) {
          button_latch_a = true;
          current_zoom_level = std::max(current_zoom_level / 2, 1);
        }
      }
      else {
        button_latch_a = false;
      }

      // If X has been pressed, zoom the view in to the max of x512
      if(button_x.read()) {
        if(!button_latch_x) {
          button_latch_x = true;
          current_zoom_level = std::min(current_zoom_level * 2, 512);
        }
      }
      else {
        button_latch_x = false;
      }

      //--------------------------------------------------            
      // Draw the encoder readings to the screen as a signal plot

      graphics.set_pen(graphics.create_pen(0, 0, 0));
      graphics.clear();

      drawing_to_screen = true;

      graphics.set_pen(graphics.create_pen(255, 255, 0));
      uint32_t local_pos = next_reading_index;
      uint32_t alignment_offset = draw_plot(Point(0, 10), Point(PicoExplorer::WIDTH, 10 + 50), enc_a_readings, local_pos, current_zoom_level > EDGE_ALIGN_ABOVE_ZOOM);

      graphics.set_pen(graphics.create_pen(0, 255, 255));
      draw_plot(Point(0, 80), Point(PicoExplorer::WIDTH, 80 + 50), enc_b_readings, (local_pos + (READINGS_SIZE - alignment_offset)) % READINGS_SIZE, false);

      // Copy values that may have been stored in the scratch buffers, back into the main buffers
      for(uint16_t i = 0; i < next_scratch_index; i++) {
        enc_a_readings[next_reading_index] = enc_a_scratch[i];
        enc_b_readings[next_reading_index] = enc_b_scratch[i];

        next_reading_index++;
        if(next_reading_index >= READINGS_SIZE)
          next_reading_index = 0;
      }

      drawing_to_screen = false;
      next_scratch_index = 0;

      graphics.set_pen(WHITE);
      graphics.character('A', Point(5, 10 + 15), 3);
      graphics.character('B', Point(5, 80 + 15), 3);

      if(current_zoom_level < 10)
        graphics.text("x" + std::to_string(current_zoom_level), Point(220, 62), 200, 2);
      else if(current_zoom_level < 100)
        graphics.text("x" + std::to_string(current_zoom_level), Point(210, 62), 200, 2);
      else
        graphics.text("x" + std::to_string(current_zoom_level), Point(200, 62), 200, 2);


      //--------------------------------------------------            
      // Write out the count, frequency and rpm of the encoder

      graphics.set_pen(graphics.create_pen(8, 8, 8));
      graphics.rectangle(Rect(0, 140, PicoExplorer::WIDTH, PicoExplorer::HEIGHT - 140));

      graphics.set_pen(graphics.create_pen(64, 64, 64));
      graphics.rectangle(Rect(0, 140, PicoExplorer::WIDTH, 2));

      {
        std::stringstream sstream;
        sstream << capture.count();
        graphics.set_pen(WHITE);                               graphics.text("Count:",      Point(10, 150),  200, 3);
        graphics.set_pen(graphics.create_pen(255, 128, 255));  graphics.text(sstream.str(), Point(110, 150), 200, 3);
      }

      {
        std::stringstream sstream;
        sstream << std::fixed << std::setprecision(1) << capture.frequency() << "hz";
        graphics.set_pen(WHITE);                               graphics.text("Freq: ",      Point(10, 180), 220, 3);
        graphics.set_pen(graphics.create_pen(128, 255, 255));  graphics.text(sstream.str(), Point(90, 180), 220, 3);
      }

      {
        std::stringstream sstream;
        sstream << std::fixed << std::setprecision(1) << capture.revolutions_per_minute();
        graphics.set_pen(WHITE);                               graphics.text("RPM: ",       Point(10, 210), 220, 3);
        graphics.set_pen(graphics.create_pen(255, 255, 128));  graphics.text(sstream.str(), Point(80, 210), 220, 3);
      }

      st7789.update(&graphics);                 // Refresh the screen

#ifdef PICO_DEFAULT_LED_PIN
      gpio_put(PICO_DEFAULT_LED_PIN, false);  // Show the screen refresh has ended
#endif
    }
  }
}