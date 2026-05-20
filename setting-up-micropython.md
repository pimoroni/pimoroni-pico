# Installing MicroPython  <!-- omit in toc -->

- [What firmware files are in this repo?](#what-firmware-files-are-in-this-repo)
- [What firmware files are **not** in this repo?](#what-firmware-files-are-not-in-this-repo)
- [Entering DFU/bootloader mode](#entering-dfubootloader-mode)
  - [If you have a board with a reset button:](#if-you-have-a-board-with-a-reset-button)
  - [If you have a board without a reset button:](#if-you-have-a-board-without-a-reset-button)
- [Copying the firmware to your board](#copying-the-firmware-to-your-board)
- [Where are the examples?](#where-are-the-examples)
- [Troubleshooting](#troubleshooting)

We provide pre-built MicroPython images which include all the drivers that include all the libraries and drivers you'll need to use our [supported products](https://github.com/pimoroni/pimoroni-pico#supported-products) To install MicroPython, you'll need to **copy the appropriate .uf2 file from the releases page to your device while it's in DFU/bootloader mode.**

## What firmware files are in this repo?

On the releases page of this Github repository you'll find a bunch of different .uf2 files for use on **Pico, Pico W and other RP2040 boards** (if you can't see them, click on the arrow next to 'assets' to expand the list).

- [Releases page](https://github.com/pimoroni/pimoroni-pico/releases)

| Board                                                        | What uf2 file to use                                         | Notes                                                        |
| ------------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| Raspberry Pi Pico and other RP2040 boards without wireless (Servo 2040, Motor 2040, Tiny 2040 2MB) | **pico-vx.x.x-pimoroni-micropython.uf2**                     |                                                              |
| Raspberry Pi Pico W and Pico W Aboard products (Automation 2040 W) | **picow-vx.x.x-pimoroni-micropython.uf2**                    |                                                              |
| Tufty 2040                                                   | **tufty2040-vx.x.x-pimoroni-micropython.uf2**                |                                                              |
| Tiny 2040 8MB                                                | **tiny2040_8mb-vx.x.x-pimoroni-micropython.uf2**                 | Includes support for the increased flash memory on this board |
| Enviro Urban, Indoor, Weather & Grow                         | **enviro-vx.x.x-pimoroni-micropython.uf2**             | For a .uf2 with examples built in, go to the [Enviro releases page](https://github.com/pimoroni/enviro/releases)! |
| Pico + USB | **pico_usb-vx.x.x-pimoroni-micropython.uf2** | :warning: Will erase your files when switching to/from a regular Pico build

## What firmware files are **not** in this repo?

 If you have a **Pico 2, Pico 2 W or other RP2350 board** you'll find MicroPython builds from the releases page of our [pimoroni-pico-rp2350 repo](https://github.com/pimoroni/pimoroni-pico-rp2350/releases).

Many of our own RP2040 and RP2350 boards now have their own repos where you can find the firmware and examples:

- [Badger 2040 / W](https://github.com/pimoroni/badger2040/)
- [Inky Frame](https://github.com/pimoroni/inky-frame)
- [Interstate 75 / W](https://github.com/pimoroni/interstate75)
- [Inventor 2040 W / 2350 W](https://github.com/pimoroni/inventor/)
- [Pico / Pico 2 W Unicorn (Stellar, Galactic, Cosmic)](https://github.com/pimoroni/unicorn)
- [Pimoroni Pico LiPo / 2 / XL W](https://github.com/pimoroni/pico-lipo)
- [Plasma 2040 / Plasma Stick / Plasma 2350 / W](https://github.com/pimoroni/plasma)

## Entering DFU/bootloader mode

### If you have a board with a reset button:

Lucky you! With your board plugged into your computer, just **hold down the BOOTSEL button and tap RESET**. A  new drive should pop up on your computer called 'RPI_RP2'. 

If your board has a power on/off button instead of a reset button, you'll need to hold down BOOTSEL and press the power button twice to cut the power, and then turn it on again.

### If you have a board without a reset button:

- **Disconnect the USB cable from your board.**
- **Press and hold the 'BOOTSEL'  button.**
- **Plug the USB cable back into your board (making sure to keep 'BOOTSEL' pressed).** A new drive should pop up on your computer called 'RPI_RP2'. 

Here's what the 'RPI-RP2' drive looks like in Windows:

![Screenshot showing the RPI-RP2 drive](dfu_mode.png)

## Copying the firmware to your board

**Copy the downloaded firmware image across to the 'RPI-RP2' drive**. Once it has finished uploading (which takes a few seconds) the board will restart itself.

⚠ Note that once your board restarts running MicroPython it will no longer show up as a drive. To program it and to transfer files to and from it you'll need to use an interpreter, such as Thonny.

- [Download Thonny](https://thonny.org/)

You can also transfer files to boards running MicroPython using command line tools, like `mpremote`.

- https://docs.micropython.org/en/latest/reference/mpremote.html

## Where are the examples?

Note that most of our MicroPython images don't include examples, so you'll need to copy across the ones you want using Thonny. You can find all our MicroPython examples at the link below.

- [MicroPython examples](https://github.com/pimoroni/pimoroni-pico/tree/main/micropython/examples)

## Troubleshooting

Having trouble getting started? Check out the link below:

- [MicroPython FAQs](faqs-micropython.md)

