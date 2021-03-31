#pragma once

#include <cstdint>

namespace synth {

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

  #define CHANNEL_COUNT 8

  constexpr float pi = 3.14159265358979323846f;

  const uint32_t sample_rate = 44100;
  extern uint16_t volume;

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

  struct AudioChannel {
    uint8_t   waveforms     = 0;      // bitmask for enabled waveforms (see AudioWaveform enum for values)
    uint16_t  frequency     = 660;    // frequency of the voice (Hz)
    uint16_t  volume        = 0xffff; // channel volume (default 50%)

    uint16_t  attack_ms     = 2;      // attack period
    uint16_t  decay_ms      = 6;      // decay period
    uint16_t  sustain       = 0xffff; // sustain volume
    uint16_t  release_ms    = 1;      // release period
    uint16_t  pulse_width   = 0x7fff; // duty cycle of square wave (default 50%)
    int16_t   noise         = 0;      // current noise value

    uint32_t  waveform_offset  = 0;   // voice offset (Q8)

    int32_t   filter_last_sample = 0;
    bool      filter_enable = false;
    uint16_t  filter_cutoff_frequency = 0;

    uint32_t  adsr_frame    = 0;      // number of frames into the current ADSR phase
    uint32_t  adsr_end_frame = 0;     // frame target at which the ADSR changes to the next phase
    uint32_t  adsr          = 0;
    int32_t   adsr_step     = 0;
    ADSRPhase adsr_phase    = ADSRPhase::OFF;

    uint8_t   wave_buf_pos  = 0;      //
    int16_t   wave_buffer[64];        // buffer for arbitrary waveforms. small as it's filled by user callback

    void *user_data = nullptr;
    void (*wave_buffer_callback)(AudioChannel &channel);

    void trigger_attack()  {
      adsr_frame = 0;
      adsr_phase = ADSRPhase::ATTACK;
      adsr_end_frame = (attack_ms * sample_rate) / 1000;
      adsr_step = (int32_t(0xffffff) - int32_t(adsr)) / int32_t(adsr_end_frame);
    }
    void trigger_decay() {
      adsr_frame = 0;
      adsr_phase = ADSRPhase::DECAY;
      adsr_end_frame = (decay_ms * sample_rate) / 1000;
      adsr_step = (int32_t(sustain << 8) - int32_t(adsr)) / int32_t(adsr_end_frame);
    }
    void trigger_sustain() {
      adsr_frame = 0;
      adsr_phase = ADSRPhase::SUSTAIN;
      adsr_end_frame = 0;
      adsr_step = 0;
    }
    void trigger_release() {
      adsr_frame = 0;
      adsr_phase = ADSRPhase::RELEASE;
      adsr_end_frame = (release_ms * sample_rate) / 1000;
      adsr_step = (int32_t(0) - int32_t(adsr)) / int32_t(adsr_end_frame);
    }
    void off() {
      adsr_frame = 0;
      adsr_phase = ADSRPhase::OFF;
      adsr_step = 0;
    }
  };

  extern AudioChannel channels[CHANNEL_COUNT];

  int16_t get_audio_frame();
  bool is_audio_playing();

}
