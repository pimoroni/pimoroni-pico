// Derived from Aleksander Alekseev (afiskon) in https://github.com/afiskon/stm32-ssd1306
// That code was based on 4ilo/ssd1306-stm32HAL library developed by Olivier Van den Eede ( @4ilo ) in 2016.
// Some functionality (selecting the Arduino board) removed, and code converted to C++.
// Ported to Raspberry Pico, to drive Pimorone 1.12 OLED monochrome display on I2C.
// SPI code has not been ported or used.
// Port by Simon Reap https://github.com/simon3270
// MIT License

#include "sh1107.hpp"
#include <math.h>
#include <stdlib.h>
#include <string.h>  // For memcpy

namespace pimoroni {

  /***** Device registers and masks here *****/

  bool SH1107::init() {
    bool succeeded = false;

    i2c_init(i2c, 400000);

    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_pull_up(sda);
    gpio_set_function(scl, GPIO_FUNC_I2C);
    gpio_pull_up(scl);

    if(interrupt != PIN_UNUSED) {
      gpio_set_function(interrupt, GPIO_FUNC_SIO);
      gpio_set_dir(interrupt, GPIO_IN);
      gpio_pull_up(interrupt);
    }

    Screen_Init();

    /***** Replace if(true) with any operations needed to initialise the device *****/
    if(true) {
      succeeded = true;
    }

    return succeeded;
  }

#if defined(SH1107_USE_I2C)

  void SH1107::Reset(void) {
      /* for I2C - do nothing */
  }

  // Send a byte to the command register
  void SH1107::WriteCommand(uint8_t byte) {
      //HAL_I2C_Mem_Write(&SH1107_I2C_PORT, SH1107_I2C_ADDR, 0x00, 1, &byte, 1, HAL_MAX_DELAY);
      uint8_t buffer[2] = {0x00, byte};
      uint8_t len = 1;
      i2c_write_blocking(i2c, address, buffer, len + 1, false);
  }

  // Send data
  void SH1107::WriteData(uint8_t* buffer, size_t buff_size) {
      //HAL_I2C_Mem_Write(&SH1107_I2C_PORT, SH1107_I2C_ADDR, 0x40, 1, buffer, buff_size, HAL_MAX_DELAY);
      SH1107_drawBuffer[0] = 0x40;
      memcpy(SH1107_drawBuffer+1, buffer, buff_size);
      i2c_write_blocking(i2c, address, SH1107_drawBuffer, buff_size + 1, false);
      //SAR uint8_t reg = 0x40;
      //SAR i2c_write_blocking(i2c, address, &reg, 0 + 1, false);
      //SAR i2c_write_blocking(i2c, address, buffer, buff_size, false);
  }

#elif defined(SH1107_USE_SPI)

  void SH1107::Reset(void) {
      // CS = High (not selected)
      GPIO_WritePin(SH1107_CS_Port, SH1107_CS_Pin, GPIO_PIN_SET);

      // Reset the OLED
      GPIO_WritePin(SH1107_Reset_Port, SH1107_Reset_Pin, GPIO_PIN_RESET);
      Delay(10);
      GPIO_WritePin(SH1107_Reset_Port, SH1107_Reset_Pin, GPIO_PIN_SET);
      sleep_ms(10);
  }

  // Send a byte to the command register
  void SH1107::WriteCommand(uint8_t byte) {
      GPIO_WritePin(SH1107_CS_Port, SH1107_CS_Pin, GPIO_PIN_RESET); // select OLED
      GPIO_WritePin(SH1107_DC_Port, SH1107_DC_Pin, GPIO_PIN_RESET); // command
      SPI_Transmit(&SH1107_SPI_PORT, (uint8_t *) &byte, 1, HAL_MAX_DELAY);
      GPIO_WritePin(SH1107_CS_Port, SH1107_CS_Pin, GPIO_PIN_SET); // un-select OLED
  }

  // Send data
  void SH1107::WriteData(uint8_t* buffer, size_t buff_size) {
      GPIO_WritePin(SH1107_CS_Port, SH1107_CS_Pin, GPIO_PIN_RESET); // select OLED
      GPIO_WritePin(SH1107_DC_Port, SH1107_DC_Pin, GPIO_PIN_SET); // data
      SPI_Transmit(&SH1107_SPI_PORT, buffer, buff_size, HAL_MAX_DELAY);
      GPIO_WritePin(SH1107_CS_Port, SH1107_CS_Pin, GPIO_PIN_SET); // un-select OLED
  }

#else
#error "You should define SH1107_USE_SPI or SH1107_USE_I2C macro"
#endif

  /* Fills the Screenbuffer with values from a given buffer of a fixed length */
  SH1107_Error_t SH1107::FillBuffer(uint8_t* buf, uint32_t len) {
      SH1107_Error_t ret = SH1107_ERR;
      if (len <= SH1107_BUFFER_SIZE) {
          memcpy(SH1107_Buffer,buf,len);
          ret = SH1107_OK;
      }
      return ret;
  }

  // Initialize the oled screen
  void SH1107::Screen_Init(void) {
      // Reset OLED
      Reset();

      // Wait for the screen to boot
      sleep_ms(100);

      // Init OLED
      SetDisplayOn(0); //display off

      WriteCommand(0x20); //Set Memory Addressing Mode
      WriteCommand(0x00); // 00b,Horizontal Addressing Mode; 01b,Vertical Addressing Mode;
                                  // 10b,Page Addressing Mode (RESET); 11b,Invalid

      WriteCommand(0xB0); //Set Page Start Address for Page Addressing Mode,0-7

#ifdef SH1107_MIRROR_VERT
      WriteCommand(0xC0); // Mirror vertically
#else
      WriteCommand(0xC8); //Set COM Output Scan Direction
#endif

      WriteCommand(0x00); //---set low column address
      WriteCommand(0x10); //---set high column address

      WriteCommand(0x40); //--set start line address - CHECK

      SetContrast(0xFF);

#ifdef SH1107_MIRROR_HORIZ
      WriteCommand(0xA0); // Mirror horizontally
#else
      WriteCommand(0xA1); //--set segment re-map 0 to 127 - CHECK
#endif

#ifdef SH1107_INVERSE_COLOR
      WriteCommand(0xA7); //--set inverse color
#else
      WriteCommand(0xA6); //--set normal color
#endif

  // Set multiplex ratio.
#if (SH1107_HEIGHT == 128)
      // Found in the Luma Python lib for SH1106.
      WriteCommand(0xFF);
#else
      WriteCommand(0xA8); //--set multiplex ratio(1 to 64) - CHECK
#endif

#if (SH1107_HEIGHT == 32)
      WriteCommand(0x1F); //
#elif (SH1107_HEIGHT == 64)
      WriteCommand(0x3F); //
#elif (SH1107_HEIGHT == 128)
      WriteCommand(0x3F); // Seems to work for 128px high displays too.
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif

      WriteCommand(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content

      WriteCommand(0xD3); //-set display offset - CHECK
      WriteCommand(0x00); //-not offset

      WriteCommand(0xD5); //--set display clock divide ratio/oscillator frequency
      WriteCommand(0xF0); //--set divide ratio

      WriteCommand(0xD9); //--set pre-charge period
      WriteCommand(0x22); //

      WriteCommand(0xDA); //--set com pins hardware configuration - CHECK
#if (SH1107_HEIGHT == 32)
      WriteCommand(0x02);
#elif (SH1107_HEIGHT == 64)
      WriteCommand(0x12);
#elif (SH1107_HEIGHT == 128)
      WriteCommand(0x12);
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif

      WriteCommand(0xDB); //--set vcomh
      WriteCommand(0x20); //0x20,0.77xVcc

      WriteCommand(0x8D); //--set DC-DC enable
      WriteCommand(0x14); //
      SetDisplayOn(1); //--turn on SH1107 panel

      // Clear screen
      Fill(Black);
      
      // Flush buffer to screen
      UpdateScreen();
      
      // Set default values for screen object
      SH1107_State.CurrentX = 0;
      SH1107_State.CurrentY = 0;
      
      SH1107_State.Initialized = 1;
  }

  // Fill the whole screen with the given color
  void SH1107::Fill(SH1107_COLOR color) {
      /* Set memory */
      memset(SH1107_Buffer, (color == Black) ? 0x00 : 0xFF, SH1107_BUFFER_SIZE);
  }

  // Write the screenbuffer with changed to the screen
  void SH1107::UpdateScreen(void) {
      // Write data to each page of RAM. Number of pages
      // depends on the screen height:
      //
      //  * 32px   ==  4 pages
      //  * 64px   ==  8 pages
      //  * 128px  ==  16 pages
      for(uint8_t i = 0; i < SH1107_HEIGHT/8; i++) {
          WriteCommand(0xB0 + i); // Set the current RAM page address.
          WriteCommand(0x00);
          WriteCommand(0x10);
          WriteData(SH1107_Buffer+SH1107_WIDTH*i,SH1107_WIDTH);
      }
  }

  //    Draw one pixel in the screenbuffer
  //    X => X Coordinate
  //    Y => Y Coordinate
  //    color => Pixel color
  void SH1107::DrawPixel(uint8_t x, uint8_t y, SH1107_COLOR color) {
      if(x >= SH1107_WIDTH || y >= SH1107_HEIGHT) {
          // Don't write outside the buffer
          return;
      }
      
      // Check if pixel should be inverted
      if(SH1107_State.Inverted) {
          color = (SH1107_COLOR)!color;
      }
      
      // Draw in the right color
      if(color == White) {
          SH1107_Buffer[x + (y / 8) * SH1107_WIDTH] |= 1 << (y % 8);
      } else { 
          SH1107_Buffer[x + (y / 8) * SH1107_WIDTH] &= ~(1 << (y % 8));
      }
  }

  // Draw 1 char to the screen buffer
  // ch       => char om weg te schrijven
  // Font     => Font waarmee we gaan schrijven
  // color    => Black or White
  char SH1107::WriteChar(char ch, FontDef Font, SH1107_COLOR color) {
      uint32_t i, b, j;
      
      // Check if character is valid
      if (ch < 32 || ch > 126)
          return 0;
      
      // Check remaining space on current line
      if (SH1107_WIDTH < (SH1107_State.CurrentX + Font.FontWidth) ||
          SH1107_HEIGHT < (SH1107_State.CurrentY + Font.FontHeight))
      {
          // Not enough space on current line
          return 0;
      }
      
      // Use the font to write
      for(i = 0; i < Font.FontHeight; i++) {
          b = Font.data[(ch - 32) * Font.FontHeight + i];
          for(j = 0; j < Font.FontWidth; j++) {
              if((b << j) & 0x8000)  {
                  DrawPixel(SH1107_State.CurrentX + j, (SH1107_State.CurrentY + i), (SH1107_COLOR) color);
              } else {
                  DrawPixel(SH1107_State.CurrentX + j, (SH1107_State.CurrentY + i), (SH1107_COLOR)!color);
              }
          }
      }
      
      // The current space is now taken
      SH1107_State.CurrentX += Font.FontWidth;
      
      // Return written char for validation
      return ch;
  }

  // Write full string to screenbuffer
  char SH1107::WriteString(char* str, FontDef Font, SH1107_COLOR color) {
      // Write until null-byte
      while (*str) {
          if (WriteChar(*str, Font, color) != *str) {
              // Char could not be written
              return *str;
          }
          
          // Next char
          str++;
      }
      
      // Everything ok
      return *str;
  }

  // Position the cursor
  void SH1107::SetCursor(uint8_t x, uint8_t y) {
      SH1107_State.CurrentX = x;
      SH1107_State.CurrentY = y;
  }

  // Draw line by Bresenhem's algorithm
  void SH1107::Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SH1107_COLOR color) {
    int32_t deltaX = abs(x2 - x1);
    int32_t deltaY = abs(y2 - y1);
    int32_t signX = ((x1 < x2) ? 1 : -1);
    int32_t signY = ((y1 < y2) ? 1 : -1);
    int32_t error = deltaX - deltaY;
    int32_t error2;
      
    DrawPixel(x2, y2, color);
      while((x1 != x2) || (y1 != y2))
      {
      DrawPixel(x1, y1, color);
      error2 = error * 2;
      if(error2 > -deltaY)
      {
        error -= deltaY;
        x1 += signX;
      }
      else
      {
      /*nothing to do*/
      }
          
      if(error2 < deltaX)
      {
        error += deltaX;
        y1 += signY;
      }
      else
      {
      /*nothing to do*/
      }
    }
    return;
  }
  //Draw polyline
  void SH1107::Polyline(const SH1107_VERTEX *par_vertex, uint16_t par_size, SH1107_COLOR color) {
    uint16_t i;
    if(par_vertex != 0){
      for(i = 1; i < par_size; i++){
        Line(par_vertex[i - 1].x, par_vertex[i - 1].y, par_vertex[i].x, par_vertex[i].y, color);
      }
    }
    else
    {
      /*nothing to do*/
    }
    return;
  }
  /*Convert Degrees to Radians*/
  float SH1107::DegToRad(float par_deg) {
      return par_deg * 3.14 / 180.0;
  }
  /*Normalize degree to [0;360]*/
  uint16_t SH1107::NormalizeTo0_360(uint16_t par_deg) {
    uint16_t loc_angle;
    if(par_deg <= 360)
    {
      loc_angle = par_deg;
    }
    else
    {
      loc_angle = par_deg % 360;
      loc_angle = ((par_deg != 0)?par_deg:360);
    }
    return loc_angle;
  }
  /*DrawArc. Draw angle is beginning from 4 quart of trigonometric circle (3pi/2)
   * start_angle in degree
   * sweep in degree
   */
  void SH1107::DrawArc(uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep, SH1107_COLOR color) {
      #define CIRCLE_APPROXIMATION_SEGMENTS 36
      float approx_degree;
      uint32_t approx_segments;
      uint8_t xp1,xp2;
      uint8_t yp1,yp2;
      uint32_t count = 0;
      uint32_t loc_sweep = 0;
      float rad;
      
      loc_sweep = NormalizeTo0_360(sweep);
      
      count = (NormalizeTo0_360(start_angle) * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
      approx_segments = (loc_sweep * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
      approx_degree = loc_sweep / (float)approx_segments;
      while(count < approx_segments)
      {
          rad = DegToRad(count*approx_degree);
          xp1 = x + (int8_t)(sin(rad)*radius);
          yp1 = y + (int8_t)(cos(rad)*radius);    
          count++;
          if(count != approx_segments)
          {
              rad = DegToRad(count*approx_degree);
          }
          else
          {            
              rad = DegToRad(loc_sweep);
          }
          xp2 = x + (int8_t)(sin(rad)*radius);
          yp2 = y + (int8_t)(cos(rad)*radius);    
          Line(xp1,yp1,xp2,yp2,color);
      }
      
      return;
  }
  //Draw circle by Bresenhem's algorithm
  void SH1107::DrawCircle(uint8_t par_x,uint8_t par_y,uint8_t par_r,SH1107_COLOR par_color) {
    int32_t x = -par_r;
    int32_t y = 0;
    int32_t err = 2 - 2 * par_r;
    int32_t e2;

    if (par_x >= SH1107_WIDTH || par_y >= SH1107_HEIGHT) {
      return;
    }

      do {
        DrawPixel(par_x - x, par_y + y, par_color);
        DrawPixel(par_x + x, par_y + y, par_color);
        DrawPixel(par_x + x, par_y - y, par_color);
        DrawPixel(par_x - x, par_y - y, par_color);
          e2 = err;
          if (e2 <= y) {
              y++;
              err = err + (y * 2 + 1);
              if(-x == y && e2 <= x) {
                e2 = 0;
              }
              else
              {
                /*nothing to do*/
              }
          }
          else
          {
            /*nothing to do*/
          }
          if(e2 > x) {
            x++;
            err = err + (x * 2 + 1);
          }
          else
          {
            /*nothing to do*/
          }
      } while(x <= 0);

      return;
  }

  //Draw rectangle
  void SH1107::DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SH1107_COLOR color) {
    Line(x1,y1,x2,y1,color);
    Line(x2,y1,x2,y2,color);
    Line(x2,y2,x1,y2,color);
    Line(x1,y2,x1,y1,color);

    return;
  }

  void SH1107::SetContrast(const uint8_t value) {
      const uint8_t kSetContrastControlRegister = 0x81;
      WriteCommand(kSetContrastControlRegister);
      WriteCommand(value);
  }

  void SH1107::SetDisplayOn(const uint8_t on) {
      uint8_t value;
      if (on) {
          value = 0xAF;   // Display on
          SH1107_State.DisplayOn = 1;
      } else {
          value = 0xAE;   // Display off
          SH1107_State.DisplayOn = 0;
      }
      WriteCommand(value);
  }

  uint8_t SH1107::GetDisplayOn() {
      return SH1107_State.DisplayOn;
  }

}
