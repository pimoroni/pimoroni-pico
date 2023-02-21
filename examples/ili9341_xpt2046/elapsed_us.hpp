// class for geting execution times

#pragma once


class ElapsedUs {
  public:
    ElapsedUs()
    {
      last_time = time_us_64();
    }

    uint64_t elapsed(bool reset = true)
    {
      uint64_t time_now = time_us_64();
      uint64_t elapsed = time_now - last_time;
      if(reset) {
        last_time = time_now;
      }
      return elapsed;
    }

    void reset()
    {
      uint64_t time_now = time_us_64();
      last_time = time_now;
    }

  private:
    uint64_t last_time;
};

