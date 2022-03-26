
.syntax unified
.cpu cortex-m0plus
.thumb

#include "pico/asm_helper.S"

// This macro tells the pico runtime to call __badger_init very early in boot
__pre_init __badger_init, 00003

.section .data.badger_init
.global badger_buttons_on_wake
.align 4
badger_buttons_on_wake:
.word 0x00000000

.section .text
.thumb_func
__badger_init:
   // Read GPIO state for front buttons and store
   movs r3, 0xd0
   lsls r3, r3, 24
   ldr r1, [r3, 4]      // Read all GPIOs
   movs r2, 0b11111     // Mask the front buttons (pins 15-11)
   lsls r2, 11
   ands r1, r1, r2
   ldr r2, =badger_buttons_on_wake
   str r1, [r2]

   // Enable 3v3 pin on the badger
   ldr r1, =0x40014054  // GPIO control register 10
   movs r2, 5           // SIO function
   str r2, [r1]         // Set Enable 3v3 to SIO
   str r2, [r1, 120]    // Also set LED (25) to SIO
   ldr r2, =0x02000400  // Pins 25 and 10
   str r2, [r3, 36]     // Enable pins out
   str r2, [r3, 20]     // Set pins high
   bx lr
