#include <cstdio>
#include <math.h>

#include <stdio.h>
#include <stdio.h>
#include <string>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "drivers/fatfs/ff.h"

FATFS fs;
FRESULT fr;
char data[] = "ouch this is going to hurt";




void print_error(FRESULT error){
    switch (error)
    {
    case FR_DISK_ERR:
        printf("FR_DISK_ERROR\n");
        break;
    case FR_NOT_READY:
        printf("FR_NOT_READY\n");
        break;
    case FR_EXIST:
        printf("FR_EXIST\n");
        break;
    case FR_NOT_ENABLED:
        printf("FR_NOT_ENABLED\n");
        break;
    default:
        printf("unknown error %d\n", error);
        break;
    }
}

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
    const void* buff = &data[0];
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
    f_closedir(dir);
    const TCHAR* filename = "test1.txt";
    FIL *fil = new FIL;

    uint count = 0;
    uint bytes_written;
    while (1){
    fr = f_open(fil, filename, FA_OPEN_APPEND | FA_WRITE);
    if ( fr == FR_OK) {
    //fr = f_lseek(fil, f_size(fil));
        
        f_printf(fil, "hello world %d \n", count);

        f_write(fil, buff, 12, &bytes_written);
        printf("I have written %d\n", bytes_written);


        fr = f_close(fil);
        if (fr != FR_OK){
            print_error(fr);
        }
        printf("linewritten\n");
    }
    else {
        printf("no write \n");
        print_error(fr);
    }
  sleep_ms(2000);
  count++;
    }
  printf("done!\n");

sleep_ms(2000);
} 
    


    return 0;
}
