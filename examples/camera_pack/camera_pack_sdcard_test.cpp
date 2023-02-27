#include <cstdio>
#include <math.h>

#include <stdio.h>
#include <stdio.h>
#include <string>
#include "pico/stdlib.h"
#include "hardware/pio.h"
//#include "drivers/sdcard/sdcard.h"

#define SDCARD_SPI_BUS spi0
#define SDCARD_PIN_SPI0_CS     26
#define SDCARD_PIN_SPI0_SCK    18
#define SDCARD_PIN_SPI0_MOSI   19
#define SDCARD_PIN_SPI0_MISO   20
#include "drivers/fatfs/ff.h"

FATFS fs;
FRESULT fr;

const void* buff = "hello world";


int main() {
    stdio_init_all();
    sleep_ms(2000);
    gpio_init(17);
    gpio_set_function(17, GPIO_FUNC_SIO);
    gpio_init(SDCARD_PIN_SPI0_CS);
    gpio_pull_up(SDCARD_PIN_SPI0_CS);
    gpio_set_function(SDCARD_PIN_SPI0_CS, GPIO_FUNC_SIO);
    gpio_set_dir(17 , true);
    gpio_put(17, 1);
    int count =0 ; 
while (1)
{
    /* code */


    printf("mounting sd card.. \n");

    printf(" cs pin %d ", SDCARD_PIN_SPI0_CS);
    printf(" count %d \n", count);
    count ++;
    fr = f_mount(&fs, "", 1);
    if (fr != FR_OK) {
        printf("Failed to mount SD card, error: %d\n", fr);
    
    }
    else 
    {
        printf("done!\n");
    }

    printf("Listing sd card contents..\n");
    FILINFO file;
    auto dir = new DIR();
    f_opendir(dir, "/");
    while(f_readdir(dir, &file) == FR_OK && file.fname[0]) {
        printf("- %s %lld\n", file.fname, file.fsize);
    }
    const TCHAR* filename = "test1.txt";
    FIL *fil = new FIL;
    uint bytes_written;

    if(f_open(fil, filename, FA_CREATE_NEW)){
        f_write (fil,			/* Open file to be written */
	            buff,	/* Data to be written */
	            11,			/* Number of bytes to write */
	            &bytes_written			/* Number of bytes written */
);
printf("bytes written %d", bytes_written);
f_close(fil);
    }
    else {
        printf("no write \n");
    }
  f_closedir(dir);
  printf("done!\n");

sleep_ms(2000);
} 
    


    return 0;
}
