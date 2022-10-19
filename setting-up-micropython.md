# Installing MicroPython

We provide pre-built MicroPython images which include all the drivers that include all the libraries and drivers you'll need to use our [supported products](https://github.com/pimoroni/pimoroni-pico#supported-products) To install MicroPython, you'll need to **copy the appropriate .uf2 file from the releases page to your device while it's in DFU/bootloader mode.**

## Which file do I need to download?

On the releases page you'll find a bunch of different .uf2 files for use on different RP2040 boards (if you can't see them, click on the arrow next to 'assets' to expand the list).

- [Releases page](https://github.com/pimoroni/pimoroni-pico/releases)

| Board                                                        | What uf2 file to use                                         | Notes                                                        |
| ------------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| Raspberry Pi Pico and most other RP2040 boards without wireless (Plasma 2040, Interstate 75, Servo 2040, Motor 2040, Tiny 2040 2MB) | **pimoroni-pico**                                            |                                                              |
| Raspberry Pi Pico W and Pico W Aboard products (Automation 2040 W, Inventor 2040 W, Inky Frame, Plasma Stick) | **pimoroni-picow**                                           |                                                              |
| Badger 2040                                                  | **pimoroni-badger2040** or **pimoroni-badger2040-without-badger-os** | Download **pimoroni-badger2040** for built in examples!      |
| Tufty 2040                                                   | **pimoroni-tufty2040**                                       |                                                              |
| Pimoroni Pico LiPo                                           | **pimoroni-picolipo_4mb** or **pimoroni-picolipo_8mb**       | Includes support for the increased flash memory on these boards |
| Tiny 2040 8MB                                                | **pimoroni-tiny2040**                                        | Includes support for the increased flash memory on this board |

## Entering DFU/bootloader mode

### If you have a board with a reset button:

Lucky you! With your board plugged into your computer, just **hold down the BOOTSEL button and tap RESET**. A  new drive should pop up on your computer called 'RPI_RP2'. 

### If you have a board without a reset button:

- **Disconnect the USB cable from your board.**
- **Press and hold the 'BOOTSEL'  button.**
- **Plug the USB cable back into your board (making sure to keep 'BOOTSEL' pressed).** A new drive should pop up on your computer called 'RPI_RP2'. 

Here's what the 'RPI-RP2' drive looks like in Windows:

![](dfu_mode.png)

## Copying the firmware to your board

**Copy the downloaded firmware image across to the 'RPI-RP2' drive**. Once it has finished uploading (which takes a few seconds) the Pico will restart itself.

⚠ Note that once your board restarts running MicroPython it will no longer show up as a drive. To program it and to transfer files to and from it you'll need to use an interpreter, such as Thonny or Mu.

- [Download Thonny](https://thonny.org/)
- [Download Mu](https://codewith.mu/)

You can also transfer files to boards running MicroPython using command line tools, like `mpremote`.

- https://docs.micropython.org/en/latest/reference/mpremote.html

## Where are the examples?

Note that most of our MicroPython images don't include examples, so you'll need to copy across the ones you want using Thonny. You can find all our MicroPython examples at the link below.

- [MicroPython examples](https://github.com/pimoroni/pimoroni-pico/tree/main/micropython/examples)
