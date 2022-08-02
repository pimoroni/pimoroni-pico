
.syntax unified
.cpu cortex-m0plus
.thumb

#include "pico/asm_helper.S"

// This macro tells the pico runtime to call __wakeup_gpio_latch very early in boot
__pre_init __wakeup_gpio_latch, 00001

.section .data.wakeup_gpio_latch
.global wakeup_gpio_state
.align 4
wakeup_gpio_state:
.word 0x00000000

.section .text
.thumb_func
__wakeup_gpio_latch:
   // Read GPIO state for front buttons and store
   movs r3, 0xd0                // Load 0xd0 into r3
   lsls r3, r3, 24              // Shift left 24 to get 0xd0000000
   ldr r1, [r3, 4]              // Load GPIO state (0xd0000004) into r1
   ldr r2, =wakeup_gpio_state   // Load output var addr into r2
   str r1, [r2]                 // Store r1 to r2
   bx lr                        // Return
