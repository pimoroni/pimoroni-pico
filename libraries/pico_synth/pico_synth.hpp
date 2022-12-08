#pragma once

//#include <string>
//#include <array>
//#include <cstdint>
//#include <algorithm>
//#include <vector>
//#include <functional>

#include "common/pimoroni_common.hpp"

namespace pimoroni {

  // The duration a note is played is determined by the amount of attack,
  // decay, and release, combined with the length of the note as defined by
  // the user.
  //
  // - Attack:  number of milliseconds it takes for a note to hit full volume
  // - Decay:   number of milliseconds it takes for a note to settle to sustain volume
  // - Sustain: percentage of full volume that the note sustains at (duration implied by other factors)
  // - Release: number of milliseconds it takes for a note to reduce to zero volume after it has ended
  //
  // Attack (750ms) - Decay (500ms) -------- Sustain ----- Release (250ms)
  //
  //                +         +                                  +    +
  //                |         |                                  |    |
  //                |         |                                  |    |
  //                |         |                                  |    |
  //                v         v                                  v    v
  // 0ms               1000ms              2000ms              3000ms              4000ms
  //
  // |              XXXX |                   |                   |                   |
  // |             X    X|XX                 |                   |                   |
  // |            X      |  XXX              |                   |                   |
  // |           X       |     XXXXXXXXXXXXXX|XXXXXXXXXXXXXXXXXXX|                   |
  // |          X        |                   |                   |X                  |
  // |         X         |                   |                   |X                  |
  // |        X          |                   |                   | X                 |
  // |       X           |                   |                   | X                 |
  // |      X            |                   |                   |  X                |
  // |     X             |                   |                   |  X                |
  // |    X              |                   |                   |   X               |
  // |   X               |                   |                   |   X               |
  // |  X +    +    +    |    +    +    +    |    +    +    +    |    +    +    +    |    +
  // | X  |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
  // |X   |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
  // +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+--->

  enum Waveform {
    NOISE     = 128,
    SQUARE    = 64,
    SAW       = 32,
    TRIANGLE  = 16,
    SINE      = 8,
    WAVE      = 1
  };

  enum class ADSRPhase : uint8_t {
    ATTACK,
    DECAY,
    SUSTAIN,
    RELEASE,
    OFF
  };

  const int16_t sine_waveform[256] = {-32768,-32758,-32729,-32679,-32610,-32522,-32413,-32286,-32138,-31972,-31786,-31581,-31357,-31114,-30853,-30572,-30274,-29957,-29622,-29269,-28899,-28511,-28106,-27684,-27246,-26791,-26320,-25833,-25330,-24812,-24279,-23732,-23170,-22595,-22006,-21403,-20788,-20160,-19520,-18868,-18205,-17531,-16846,-16151,-15447,-14733,-14010,-13279,-12540,-11793,-11039,-10279,-9512,-8740,-7962,-7180,-6393,-5602,-4808,-4011,-3212,-2411,-1608,-804,0,804,1608,2411,3212,4011,4808,5602,6393,7180,7962,8740,9512,10279,11039,11793,12540,13279,14010,14733,15447,16151,16846,17531,18205,18868,19520,20160,20788,21403,22006,22595,23170,23732,24279,24812,25330,25833,26320,26791,27246,27684,28106,28511,28899,29269,29622,29957,30274,30572,30853,31114,31357,31581,31786,31972,32138,32286,32413,32522,32610,32679,32729,32758,32767,32758,32729,32679,32610,32522,32413,32286,32138,31972,31786,31581,31357,31114,30853,30572,30274,29957,29622,29269,28899,28511,28106,27684,27246,26791,26320,25833,25330,24812,24279,23732,23170,22595,22006,21403,20788,20160,19520,18868,18205,17531,16846,16151,15447,14733,14010,13279,12540,11793,11039,10279,9512,8740,7962,7180,6393,5602,4808,4011,3212,2411,1608,804,0,-804,-1608,-2411,-3212,-4011,-4808,-5602,-6393,-7180,-7962,-8740,-9512,-10279,-11039,-11793,-12540,-13279,-14010,-14733,-15447,-16151,-16846,-17531,-18205,-18868,-19520,-20160,-20788,-21403,-22006,-22595,-23170,-23732,-24279,-24812,-25330,-25833,-26320,-26791,-27246,-27684,-28106,-28511,-28899,-29269,-29622,-29957,-30274,-30572,-30853,-31114,-31357,-31581,-31786,-31972,-32138,-32286,-32413,-32522,-32610,-32679,-32729,-32758};
  const uint32_t sample_rate = 22050;

  struct AudioChannel {
    uint8_t   waveforms     = 0;      // bitmask for enabled waveforms (see Waveform enum for values)
    uint16_t  frequency     = 660;    // frequency of the voice (Hz)
    uint16_t  volume        = UINT16_MAX; // channel volume

    uint16_t  attack_ms     = 2;      // attack period (cannot be zero)
    uint16_t  decay_ms      = 6;      // decay period (cannot be zero)
    uint16_t  sustain       = UINT16_MAX; // sustain volume
    uint16_t  release_ms    = 1;      // release period
    uint16_t  pulse_width   = 0x7fff; // duty cycle of square wave (default 50%)
    int16_t   noise         = 0;      // current noise value

    uint32_t  waveform_offset  = 0;   // voice offset (Q8)

    int32_t   filter_last_sample = 0;
    bool      filter_enable = false;
    uint16_t  filter_cutoff_frequency = 0;

    uint32_t  adsr_frame     = 0;     // number of frames into the current ADSR phase
    uint32_t  adsr_end_frame = 0;     // frame target at which the ADSR changes to the next phase
    uint32_t  adsr_level     = 0;     // the output level at the current frame of the ADSR phase
    int32_t   adsr_step      = 0;     // the amount to increment the level with each frame
    ADSRPhase adsr_phase     = ADSRPhase::OFF;

    uint8_t   wave_buf_pos  = 0;      //
    int16_t   wave_buffer[64];        // buffer for arbitrary waveforms. small as it's filled by user callback

    void *user_data = nullptr;
    void (*wave_buffer_callback)(AudioChannel &channel);

    void trigger_attack();
    void trigger_decay();
    void trigger_sustain();
    void trigger_release();
    void off();
    void restore();
  };

  class PicoSynth {
  public:
    uint16_t volume = 0x2fff;

    static const uint CHANNEL_COUNT = 8;
    AudioChannel channels[CHANNEL_COUNT];

    int16_t get_audio_frame();
    bool is_audio_playing();
  };

  constexpr float pi = 3.14159265358979323846f;

}
