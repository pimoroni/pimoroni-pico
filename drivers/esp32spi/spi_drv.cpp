#include "spi_drv.hpp"

namespace pimoroni {

  void SpiDrv::init() {
    spi_init(spi, 8000000);
    gpio_set_function(miso, GPIO_FUNC_SPI);
    gpio_set_function(sck, GPIO_FUNC_SPI);
    gpio_set_function(mosi, GPIO_FUNC_SPI);

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(cs);
    gpio_set_dir(cs, GPIO_OUT);
    gpio_put(cs, true);

    gpio_init(gpio0);
    gpio_set_dir(gpio0, GPIO_OUT);

    gpio_init(resetn);
    gpio_set_dir(resetn, GPIO_OUT);
  }

  void SpiDrv::reset() {
    gpio_put(gpio0, true);
    gpio_put(cs, true);
    gpio_put(resetn, false);
    sleep_ms(10);
    gpio_put(resetn, true);
    sleep_ms(750);
  }

  bool SpiDrv::available() {
    return gpio_get(gpio0);
  }

  void SpiDrv::esp_select() {
    gpio_put(cs, false);
  }
    
  void SpiDrv::esp_deselect() {
    gpio_put(cs, true);
  }

  bool SpiDrv::get_esp_ready() {
    return !gpio_get(ack);
  }

  bool SpiDrv::get_esp_ack() {
    return gpio_get(ack);
  }

  bool SpiDrv::wait_for_esp_ack(uint32_t timeout_ms) {
    absolute_time_t timeout = make_timeout_time_ms(timeout_ms);
	  while(!get_esp_ack()) {
      tight_loop_contents();
      if (absolute_time_diff_us(get_absolute_time(),  timeout) <= 0) {
        return false;
      }
    }
    return true;
  }

  bool SpiDrv::wait_for_esp_ready(uint32_t timeout_ms) {
    absolute_time_t timeout = make_timeout_time_ms(timeout_ms);
    while(!get_esp_ready()) {
      tight_loop_contents();
      if (absolute_time_diff_us(get_absolute_time(),  timeout) <= 0) {
        return false;
      }
    }
    return true;
  }

  bool SpiDrv::wait_for_esp_select(uint32_t timeout_ms) {
    if(!wait_for_esp_ready(timeout_ms)) {
      return false;
    }
    esp_select();
    if(!wait_for_esp_ack(timeout_ms)) {
      esp_deselect();
      return false;
    }
    return true;
  }

  int SpiDrv::wait_for_byte(uint8_t wait_byte) {
    int timeout = BYTE_TIMEOUT;
    uint8_t byte_read = 0;
    do{
      byte_read = read_byte(); //get data byte
      if (byte_read == ERR_CMD) {
        WARN("Err cmd received\n");
        return -1;
      }
    } while((timeout-- > 0) && (byte_read != wait_byte));
    return (byte_read == wait_byte);
  }
    
  bool SpiDrv::read_and_check_byte(uint8_t check_byte, uint8_t *byte_out) {
    get_param(byte_out);
    return (*byte_out == check_byte);
  }

  uint8_t SpiDrv::read_byte() {
    uint8_t byte_read = 0;
    get_param(&byte_read);
    return byte_read;
  }

  bool SpiDrv::wait_response_params(uint8_t cmd, uint8_t num_param, outParam *params_out) {
    uint8_t data = 0;
    int i = 0;

    IF_CHECK_START_CMD() {
      CHECK_DATA(cmd | REPLY_FLAG, data){};

      uint8_t num_param_read = read_byte();
      if(num_param_read != 0) {        
        for(i = 0; i < num_param_read; ++i) {
          params_out[i].param_len = read_param_len8();
          spi_read_blocking(spi, DUMMY_DATA, params_out[i].param, params_out[i].param_len);
        }
      }
      else {
        WARN("Error num_param == 0\n");
        return false;
      }

      if(num_param != num_param_read) {
        WARN("Mismatch num_param\n");
        return false;
      }

      read_and_check_byte(END_CMD, &data);
    }         
    return true;
  }
    
  bool SpiDrv::wait_response_cmd(uint8_t cmd, uint8_t num_param, uint8_t *param_out, uint16_t *param_len_out) {
    uint8_t data = 0;
    int ii = 0;

    IF_CHECK_START_CMD() {
      CHECK_DATA(cmd | REPLY_FLAG, data){};

      CHECK_DATA(num_param, data) {
        read_param_len8(param_len_out);
        for(ii = 0; ii < (*param_len_out); ++ii) {
          get_param(&param_out[ii]);
        } 
      }

      read_and_check_byte(END_CMD, &data);
    }
    
    return true;
  }

  bool SpiDrv::wait_response_data8(uint8_t cmd, uint8_t *param_out, uint16_t *param_len_out) {
    uint8_t data = 0;

    IF_CHECK_START_CMD() {
      CHECK_DATA(cmd | REPLY_FLAG, data){};

      uint8_t num_param_read = read_byte();
      if(num_param_read != 0) {        
        read_param_len8(param_len_out);
        spi_read_blocking(spi, DUMMY_DATA, param_out, *param_len_out);
      }

      read_and_check_byte(END_CMD, &data);
    }     
    
    return true;    
  }
     
  bool SpiDrv::wait_response_data16(uint8_t cmd, uint8_t* param_out, uint16_t *param_len_out) {
    uint8_t data = 0;

    IF_CHECK_START_CMD() {
      CHECK_DATA(cmd | REPLY_FLAG, data){};

      uint8_t num_param_read = read_byte();
      if(num_param_read != 0) {
        read_param_len16(param_len_out);
        spi_read_blocking(spi, DUMMY_DATA, param_out, *param_len_out);
      }

      read_and_check_byte(END_CMD, &data);
    }     
    
    return false;
  }

  bool SpiDrv::wait_response(uint8_t cmd, uint16_t *num_param_out, uint8_t **params_out, uint8_t max_num_params) {
    uint8_t data = 0;
    int i = 0;

    uint8_t* index[WL_SSID_MAX_LENGTH];

    for(i = 0 ; i < WL_NETWORKS_LIST_MAXNUM; i++)
      index[i] = (uint8_t*)params_out + (WL_SSID_MAX_LENGTH * i);

    IF_CHECK_START_CMD() {
      CHECK_DATA(cmd | REPLY_FLAG, data){};

      uint8_t num_param_read = read_byte();

      if(num_param_read > max_num_params) {
        num_param_read = max_num_params;
      }
      *num_param_out = num_param_read;
      if(num_param_read != 0) {
        for(i = 0; i < num_param_read; ++i) {
          uint8_t param_len = read_param_len8();
          spi_read_blocking(spi, DUMMY_DATA, index[i], param_len);
          index[i][param_len] = 0;
        }
      }
      else {
        WARN("Error numParams == 0\n");
        read_and_check_byte(END_CMD, &data);
        return false;
      }
      read_and_check_byte(END_CMD, &data);
    }
    return true;
  }

  void SpiDrv::send_param(const uint8_t *param, uint8_t param_len) {
    send_param_len8(param_len);

    spi_write_blocking(spi, param, param_len);
    command_length += param_len;
  }

  void SpiDrv::send_param_len8(uint8_t param_len) {
    spi_write_blocking(spi, &param_len, 1);
    command_length += 1;
  }

  void SpiDrv::send_param_len16(uint16_t param_len) {
    uint8_t buf[2];
    buf[0] = (uint8_t)((param_len & 0xff00) >> 8);
    buf[1] = (uint8_t)(param_len & 0xff);
    spi_write_blocking(spi, buf, 2);
    command_length += 2;
  }

  uint8_t SpiDrv::read_param_len8(uint16_t *param_len_out) {
    uint8_t param_len;
    get_param(&param_len);
    if(param_len_out != nullptr) {
      *param_len_out = param_len;
    }
    return param_len;
  }

  uint16_t SpiDrv::read_param_len16(uint16_t *param_len_out) {
    uint8_t buf[2];
    spi_read_blocking(spi, DUMMY_DATA, buf, 2);
    uint16_t param_len = (buf[0] << 8) | (buf[1] & 0xff);
    if(param_len_out != nullptr) {
      *param_len_out = param_len;
    }
    return param_len;
  }

  void SpiDrv::send_buffer(const uint8_t* param, uint16_t param_len) {
    send_param_len16(param_len);

    spi_write_blocking(spi, param, param_len);
    command_length += param_len;
  }
    
  void SpiDrv::start_cmd(uint8_t cmd, uint8_t num_param) {
    uint8_t buf[3];
    buf[0] = START_CMD;
    buf[1] = cmd & ~(REPLY_FLAG);
    buf[2] = num_param;
    spi_write_blocking(spi, buf, 3);

    command_length = 3;
  }

  void SpiDrv::end_cmd() {
    uint8_t buf = END_CMD;
    spi_write_blocking(spi, &buf, 1);
    command_length += 1;
    WARN("Command len: %ld\n", command_length);
    pad_to_multiple_of_4(command_length);
    command_length = 0;
  }

  void SpiDrv::pad_to_multiple_of_4(int command_size) {
    while(command_size % 4) {
      read_byte();
      command_size++;
    }
  }

  void SpiDrv::get_param(uint8_t* param_out) {
    spi_read_blocking(spi, DUMMY_DATA, param_out, 1);
  }

  bool SpiDrv::send_command(uint8_t command, const SpiDrv::inParam *params_in, uint8_t num_in, uint8_t *data, uint16_t *data_len, cmd_response_type response_type) {
    if (!wait_for_esp_select()) {
      // Timeout waiting for ESP select
      // This could be a transport error, or a sleeping EPS32
      return false;
    }

    WARN("\n%s %d\n", commands[command], num_in);

    // Send Command
    start_cmd(command, num_in);

    // Send params
    for(uint8_t i = 0; i < num_in; i++) {
      SpiDrv::inParam param = params_in[i];
      switch(param.type) {
        case PARAM_NORMAL:
          WARN("param %d\n", param.len);
          send_param(param.addr, param.len);   // uint8_t length
          break;
        case PARAM_BUFFER:
          WARN("buffer %d\n", param.len);
          send_buffer(param.addr, param.len);  // uint16_t length
          break;
        case PARAM_DUMMY:
          WARN("dummy\n");
          uint8_t dummy = DUMMY_DATA;
          send_param(&dummy, 1);
          break;
      }
    }
    end_cmd();
    esp_deselect();
  
    // Wait for reply
    // START_SCAN_NETWORKS is a no-op, and SCAN_NETWORKS will block while the scan is performed
    wait_for_esp_select(command == 0x27 ? 30000 : 10000);
    *data = -1;
    bool status = false;
    switch(response_type) {
      case RESPONSE_TYPE_NORMAL:
        WARN("wait_response\n");
        // Currently SCAN_NETWORKS is the only command using "wait_response" so its max_num_params value is hard-coded
        status = wait_response(command, data_len, (uint8_t**)data, WL_NETWORKS_LIST_MAXNUM);
        break;
      case RESPONSE_TYPE_CMD:
        WARN("wait_response_cmd\n");
        status = wait_response_cmd(command, SpiDrv::PARAM_NUMS_1, data, data_len);
        break;
      case RESPONSE_TYPE_DATA8:
        WARN("wait_response_data8\n");
        status = wait_response_data8(command, data, data_len);
        break;
      case RESPONSE_TYPE_DATA16:
        WARN("wait_response_data16\n");
        status = wait_response_data16(command, data, data_len);
        break;
    }
    esp_deselect();

    if(status) {
      // Any successful command should reset sleep status to AWAKE
      // a sleeping ESP32 wont respond to commands!
      sleep_state = AWAKE;
    }
    return status;
  }

  bool SpiDrv::send_command(uint8_t command, SpiDrv::outParam *params_out, SpiDrv::numParams num_out) {
    if (!wait_for_esp_select()) {
      // Timeout waiting for ESP select!
      return false;
    }

    start_cmd(command, SpiDrv::PARAM_NUMS_1);
    uint8_t dummy = DUMMY_DATA;
    send_param(&dummy, 1);
    end_cmd();
    esp_deselect();

    wait_for_esp_select();
    bool status = wait_response_params(command, num_out, params_out);
    esp_deselect();

    if(status) {
      // Any successful command should reset sleep status to AWAKE
      // a sleeping ESP32 wont respond to commands!
      sleep_state = AWAKE;
    }
    return status;
  }
}