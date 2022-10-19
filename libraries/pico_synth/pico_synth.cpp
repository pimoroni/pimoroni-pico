#include "pico_synth.hpp"

namespace pimoroni {

  uint32_t prng_xorshift_state = 0x32B71700;

  uint32_t prng_xorshift_next() {
    uint32_t x = prng_xorshift_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    prng_xorshift_state = x;
    return x;
  }

  int32_t prng_normal() {
    // rough approximation of a normal distribution
    uint32_t r0 = prng_xorshift_next();
    uint32_t r1 = prng_xorshift_next();
    uint32_t n = ((r0 & 0xffff) + (r1 & 0xffff) + (r0 >> 16) + (r1 >> 16)) / 2;
    return n - 0xffff;
  }


  void AudioChannel::trigger_attack() {
    adsr_frame = 0;
    adsr_phase = ADSRPhase::ATTACK;
    adsr_end_frame = (attack_ms * sample_rate) / 1000;
    adsr_step = (int32_t(0xffffff) - int32_t(adsr_level)) / int32_t(adsr_end_frame);
  }

  void AudioChannel::trigger_decay() {
    adsr_frame = 0;
    adsr_phase = ADSRPhase::DECAY;
    adsr_end_frame = (decay_ms * sample_rate) / 1000;
    adsr_step = (int32_t(sustain << 8) - int32_t(adsr_level)) / int32_t(adsr_end_frame);
  }

  void AudioChannel::trigger_sustain() {
    adsr_frame = 0;
    adsr_phase = ADSRPhase::SUSTAIN;
    adsr_end_frame = 0;
    adsr_step = 0;
  }

  void AudioChannel::trigger_release() {
    adsr_frame = 0;
    adsr_phase = ADSRPhase::RELEASE;
    adsr_end_frame = (release_ms * sample_rate) / 1000;
    adsr_step = (int32_t(0) - int32_t(adsr_level)) / int32_t(adsr_end_frame);
  }

  void AudioChannel::off() {
    adsr_frame = 0;
    adsr_phase = ADSRPhase::OFF;
    adsr_step = 0;
  }

  void AudioChannel::restore() {
    // Put all the parameters back to their initial values
    waveforms     = 0;
    frequency     = 660;
    volume        = 0xffff;

    attack_ms     = 2;
    decay_ms      = 6;
    sustain       = 0xffff;
    release_ms    = 1;
    pulse_width   = 0x7fff;
    noise         = 0;
  }

  bool PicoSynth::is_audio_playing() {
    if(volume == 0) {
      return false;
    }

    bool any_channel_playing = false;
    for(uint c = 0; c < CHANNEL_COUNT; c++) {
      if(channels[c].volume > 0 && channels[c].adsr_phase != ADSRPhase::OFF) {
        any_channel_playing = true;
      }
    }

    return any_channel_playing;
  }

  int16_t PicoSynth::get_audio_frame() {
    int32_t sample = 0;  // used to combine channel output

    for(uint c = 0; c < CHANNEL_COUNT; c++) {

      auto &channel = channels[c];

      // increment the waveform position counter. this provides an
      // Q16 fixed point value representing how far through
      // the current waveform we are
      channel.waveform_offset += ((channel.frequency * 256) << 8) / sample_rate;

      if(channel.adsr_phase == ADSRPhase::OFF) {
        continue;
      }

      if((channel.adsr_frame >= channel.adsr_end_frame) && (channel.adsr_phase != ADSRPhase::SUSTAIN)) {
        switch (channel.adsr_phase) {
          case ADSRPhase::ATTACK:
            channel.trigger_decay();
            break;
          case ADSRPhase::DECAY:
            channel.trigger_sustain();
            break;
          case ADSRPhase::RELEASE:
            channel.off();
            break;
          default:
            break;
        }
      }

      channel.adsr_level += channel.adsr_step;
      channel.adsr_frame++;

      if(channel.waveform_offset & 0x10000) {
        // if the waveform offset overflows then generate a new
        // random noise sample
        channel.noise = prng_normal();
      }

      channel.waveform_offset &= 0xffff;

      // check if any waveforms are active for this channel
      if(channel.waveforms) {
        uint8_t waveform_count = 0;
        int32_t channel_sample = 0;

        if(channel.waveforms & Waveform::NOISE) {
          channel_sample += channel.noise;
          waveform_count++;
        }

        if(channel.waveforms & Waveform::SAW) {
          channel_sample += (int32_t)channel.waveform_offset - 0x7fff;
          waveform_count++;
        }

        // creates a triangle wave of ^
        if(channel.waveforms & Waveform::TRIANGLE) {
          if(channel.waveform_offset < 0x7fff) { // initial quarter up slope
            channel_sample += int32_t(channel.waveform_offset * 2) - int32_t(0x7fff);
          }
          else { // final quarter up slope
            channel_sample += int32_t(0x7fff) - ((int32_t(channel.waveform_offset) - int32_t(0x7fff)) * 2);
          }
          waveform_count++;
        }

        if(channel.waveforms & Waveform::SQUARE) {
          channel_sample += (channel.waveform_offset < channel.pulse_width) ? 0x7fff : -0x7fff;
          waveform_count++;
        }

        if(channel.waveforms & Waveform::SINE) {
          // the sine_waveform sample contains 256 samples in
          // total so we'll just use the most significant bits
          // of the current waveform position to index into it
          channel_sample += sine_waveform[channel.waveform_offset >> 8];
          waveform_count++;
        }

        if(channel.waveforms & Waveform::WAVE) {
          channel_sample += channel.wave_buffer[channel.wave_buf_pos];
          if(++channel.wave_buf_pos == 64) {
            channel.wave_buf_pos = 0;
            if(channel.wave_buffer_callback)
              channel.wave_buffer_callback(channel);
          }
          waveform_count++;
        }

        channel_sample = channel_sample / waveform_count;

        channel_sample = (int64_t(channel_sample) * int32_t(channel.adsr_level >> 8)) >> 16;

        // apply channel volume
        channel_sample = (int64_t(channel_sample) * int32_t(channel.volume)) >> 16;

        // apply channel filter
        //if (channel.filter_enable) {
          //float filter_epow = 1 - expf(-(1.0f / 22050.0f) * 2.0f * pi * int32_t(channel.filter_cutoff_frequency));
          //channel_sample += (channel_sample - channel.filter_last_sample) * filter_epow;
        //}

        //channel.filter_last_sample = channel_sample;

        // combine channel sample into the final sample
        sample += channel_sample;
      }
    }

    sample = (int64_t(sample) * int32_t(volume)) >> 16;

    // clip result to 16-bit
    sample = sample <= -0x8000 ? -0x8000 : (sample > 0x7fff ? 0x7fff : sample);
    return sample;
  }

}
