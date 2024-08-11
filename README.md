# Pimoroni Pico Libraries and Examples <!-- omit in toc -->

Welcome to the brave new world of Pico!

This repository contains the C/C++ and MicroPython libraries for our range of RP2040-based boards, Raspberry Pi Pico addons & [supported Breakout Garden sensors](#breakouts).

[![CMake Build Status](https://img.shields.io/github/actions/workflow/status/pimoroni/pimoroni-pico/cmake.yml?branch=main&label=CMake)](https://github.com/pimoroni/pimoroni-pico/actions/workflows/cmake.yml)
[![MicroPython Build Status](https://img.shields.io/github/actions/workflow/status/pimoroni/pimoroni-pico/micropython.yml?branch=main&label=MicroPython)](https://github.com/pimoroni/pimoroni-pico/actions/workflows/micropython.yml)
[![MicroPython PicoW Build Status](https://img.shields.io/github/actions/workflow/status/pimoroni/pimoroni-pico/micropython-picow.yml?branch=main&label=MicroPython%20PicoW)](https://github.com/pimoroni/pimoroni-pico/actions/workflows/micropython-picow.yml)
[![GitHub release (latest by date)](https://img.shields.io/github/v/release/pimoroni/pimoroni-pico)](https://github.com/pimoroni/pimoroni-pico/releases/latest/)


- [MicroPython](#micropython)
  - [MicroPython Examples](#micropython-examples)
- [C/C++](#cc)
  - [C++ Examples](#c-examples)
  - [Boilerplate for C++ Projects](#boilerplate-for-c-projects)
- [Supported Products](#supported-products)
  - [Packs and Bases](#packs-and-bases)
  - [SHIMs](#shims)
  - [RP2040 Boards](#rp2040-boards)
  - [Pico W Aboard](#pico-w-aboard)
  - [Breakouts](#breakouts)
  - [Kits](#kits)
- [Tutorials and Guides](#tutorials-and-guides)

# MicroPython

The easiest way to get started. If you're new to Pico, we recommend you read our [getting started with Pico](https://learn.pimoroni.com/article/getting-started-with-pico) tutorial.

:warning: All of our MicroPython libraries are baked into a batteries-included, custom version of MicroPython which you can grab from releases: [https://github.com/pimoroni/pimoroni-pico/releases/latest/](https://github.com/pimoroni/pimoroni-pico/releases/latest/)

New releases are issued regularly with new libraries, bug fixes to our existing libraries and new features inherited from MicroPython upstream. Be sure to check back!

* :link: [Learn: Getting started with Pico](https://learn.pimoroni.com/article/getting-started-with-pico)
* [Readme: Instructions for installing MicroPython](setting-up-micropython.md)
* [Readme: Frequently Asked Questions](faqs-micropython.md)
* [Pimoroni Pico MicroPython + Drivers Releases](https://github.com/pimoroni/pimoroni-pico/releases)
* [Readme: PicoGraphics](micropython/modules/picographics)

## MicroPython Examples

You can find MicroPython examples for supported sensors, packs and bases in the "micropython/examples" directory:

* [MicroPython Examples](micropython/examples)

# C/C++

Advanced users that want to unleash the full power of Pico can use our C++ libraries. If you know what you're doing and want to build your own Pimoroni Pico project then start with the [Pimoroni Pico SDK Boilerplate](https://github.com/pimoroni/pico-boilerplate).

* :link: [Learn: Pico C++ Development on Windows](https://learn.pimoroni.com/article/pico-development-using-wsl)
* [Readme: Instructions for setting up the C/C++ SDK](setting-up-the-pico-sdk.md)

## C++ Examples

You can find examples for supported sensors, packs and bases in the "examples" directory:

* [C++ Examples](examples)


## Boilerplate for C++ Projects

We also maintain a C++/CMake boilerplate with GitHub workflows configured for testing and a basic skeleton project.

* [Pimoroni Pico SDK Boilerplate](https://github.com/pimoroni/pico-boilerplate)


# Supported Products

## Packs and Bases

* Pico Display Pack - https://shop.pimoroni.com/products/pico-display-pack
* Pico Scroll Pack - https://shop.pimoroni.com/products/pico-scroll-pack
* Pico Explorer Base - https://shop.pimoroni.com/products/pico-explorer-base
* Pico RGB Keypad Base - https://shop.pimoroni.com/products/pico-rgb-keypad-base
* Pico Unicorn Pack - https://shop.pimoroni.com/products/pico-unicorn-pack
* Pico Audio Pack (C++ only) - https://shop.pimoroni.com/products/pico-audio-pack
* Pico Wireless Pack - https://shop.pimoroni.com/products/pico-wireless-pack
* Pico Display 2.0 - https://shop.pimoroni.com/products/pico-display-pack-2-0
* Pico Enviro+ Pack - https://shop.pimoroni.com/products/pico-enviro-pack
* Pico Inky Pack - https://shop.pimoroni.com/products/pico-inky-pack
* Pico GFX Pack - https://shop.pimoroni.com/products/pico-gfx-pack

## SHIMs

* LiPo SHIM for Pico - https://shop.pimoroni.com/products/pico-lipo-shim
* Motor SHIM for Pico - https://shop.pimoroni.com/products/motor-shim-for-pico

## RP2040 Boards

* Plasma 2040 (LED strip driver) - https://shop.pimoroni.com/products/plasma-2040
* Interstate 75 (HUB75 matrix driver) - https://shop.pimoroni.com/products/interstate-75
* Badger 2040 (E Ink badge) - https://shop.pimoroni.com/products/badger-2040
* Servo 2040 (18 Channel Servo Controller) - https://shop.pimoroni.com/products/servo-2040
* Motor 2040 (Quad Motor+Encoder Controller) - https://shop.pimoroni.com/products/motor-2040
* Tufty 2040 (LCD badge) - https://shop.pimoroni.com/products/tufty-2040

## Pico W Aboard

* Automation 2040 W (inputs, outputs and relays, 6-40V compatible) - https://shop.pimoroni.com/products/automation-2040-w
* Inventor 2040 W (motors, servos, noise) - https://shop.pimoroni.com/products/inventor-2040-w
* Inky Frame 5.7" (7-colour E Ink) - https://shop.pimoroni.com/products/inky-frame-5-7
* Automation 2040 W Mini (inputs, outputs and a relay, 6-40V compatible) - https://shop.pimoroni.com/products/automation-2040-w-mini
* Plasma Stick 2040 W (bijou LED strip controller) - https://shop.pimoroni.com/products/plasma-stick-2040-w
* Galactic Unicorn (53 x 11 LED matrix) - https://shop.pimoroni.com/products/galactic-unicorn
* Interstate 75 W (HUB75 matrix driver) - https://shop.pimoroni.com/products/interstate-75-w
* Inky Frame 4.0" (7-colour E Ink) - https://shop.pimoroni.com/products/inky-frame-4
* Badger 2040 W (E Ink badge) - https://shop.pimoroni.com/products/badger-2040-w
* Cosmic Unicorn (32 x 32 LED matrix) - https://shop.pimoroni.com/products/cosmic-unicorn
* Inky Frame 7.3" (7-colour E Ink) - https://shop.pimoroni.com/products/inky-frame-7-3

## Breakouts

* AS7262 - 6-channel Spectral Sensor - https://shop.pimoroni.com/products/as7262-6-channel-spectral-sensor-spectrometer-breakout
* MSA301 - 3DoF Motion Sensor - https://shop.pimoroni.com/products/msa301-3dof-motion-sensor-breakout
* MICS6814 - Gas Sensor - https://shop.pimoroni.com/products/mics6814-gas-sensor-breakout
* RGB Potentiometer - https://shop.pimoroni.com/products/rgb-potentiometer-breakout
* RGB Encoder - https://shop.pimoroni.com/products/rgb-encoder-breakout
* RGB Encoder Wheel - https://shop.pimoroni.com/products/rgb-encoder-wheel-breakout
* IO Expander - https://shop.pimoroni.com/products/io-expander
* RV3028 - Real-Time Clock (RTC) - https://shop.pimoroni.com/products/rv3028-real-time-clock-rtc-breakout
* ST7735 - 0.96" LCD - https://shop.pimoroni.com/products/0-96-spi-colour-lcd-160x80-breakout
* IS31FL3730 - dual LTP-305 LED matrices - https://shop.pimoroni.com/products/led-dot-matrix-breakout
* LTR559 - Proximity/Presence/Light Sensor - https://shop.pimoroni.com/products/ltr-559-light-proximity-sensor-breakout
* IS31FL3731 - 11x7 and 5x5 matrix displays 
  - https://shop.pimoroni.com/products/11x7-led-matrix-breakout 
  - https://shop.pimoroni.com/products/5x5-rgb-matrix-breakout
* Trackball - https://shop.pimoroni.com/products/trackball-breakout
* SGP30 - Air Quality Sensor - https://shop.pimoroni.com/products/sgp30-air-quality-sensor-breakout
* ST7789 - 1.3" LCD, 1.54" LCD and 1.3" round LCD
  - https://shop.pimoroni.com/products/1-3-spi-colour-lcd-240x240-breakout
  - https://shop.pimoroni.com/products/1-3-spi-colour-round-lcd-240x240-breakout
  - https://shop.pimoroni.com/products/1-54-spi-colour-square-lcd-240x240-breakout
* BME680 - Temperature, Pressure, Humidity & Gas Sensor - https://shop.pimoroni.com/products/bme680-breakout
* BME688 - Temperature, Pressure, Humidity & Gas Sensor - https://shop.pimoroni.com/products/bme688-breakout
* BH1745 - Luminance & Colour Sensor - https://shop.pimoroni.com/products/bh1745-luminance-and-colour-sensor-breakout
* BME280 - Temperature, Pressure & Humidity Sensor - https://shop.pimoroni.com/products/bme280-breakout
* BMP280 - Temperature & Pressure Sensor - https://shop.pimoroni.com/products/bmp280-breakout-temperature-pressure-altitude-sensor
* PWM3901/PAA5100JE - Near Optical Flow Sensor - https://shop.pimoroni.com/products/paa5100je-optical-tracking-spi-breakout
* ICP10125 - High Accuracy Pressure / Altitude / Temperature Sensor - https://shop.pimoroni.com/products/icp10125-air-pressure-breakout
* SCD41 CO2 Sensor (Carbon Dioxide / Temperature / Humidity) - https://shop.pimoroni.com/products/scd41-co2-sensor-breakout
* VL53L5CX 8x8 Time of Flight Array Sensor - https://shop.pimoroni.com/products/vl53l5cx-time-of-flight-tof-sensor-breakout
* RGB Encoder Wheel - https://shop.pimoroni.com/products/rgb-encoder-wheel-breakout

## Kits

* Wireless Plasma Kit (Plasma Stick + LED wire + bottle!) - https://shop.pimoroni.com/products/wireless-plasma-kit

# Tutorials and Guides

- :link: [Getting Started with (MicroPython on) Pico](https://learn.pimoroni.com/article/getting-started-with-pico)
- :link: [Pico C/C++ Development on Windows / WSL](https://learn.pimoroni.com/article/pico-development-using-wsl)
- :link: [Getting Started with Interstate 75 (and W)](https://learn.pimoroni.com/article/getting-started-with-interstate-75)
- :link: [Getting Started with Plasma 2040](https://learn.pimoroni.com/article/plasma-2040)
- :link: [Assembling Keybow 2040](https://learn.pimoroni.com/article/assembling-keybow-2040)
- :link: [Getting Started with Badger 2040](https://learn.pimoroni.com/article/getting-started-with-badger-2040)
- :link: [MicroPython and VL53L5CX](https://learn.pimoroni.com/article/micropython-and-vl53l5cx)
- :link: [Getting Started with Tufty 2040](https://learn.pimoroni.com/article/getting-started-with-tufty-2040)
- :link: [Getting Started with Inky Frame](https://learn.pimoroni.com/article/getting-started-with-inky-frame)
- :link: [Getting Started with Automation 2040 W (and Mini)](https://learn.pimoroni.com/article/getting-started-with-automation-2040-w)
- :link: [Assembling Wireless Plasma Kit](https://learn.pimoroni.com/article/assembling-wireless-plasma-kit)
- :link: [Getting Started with Badger 2040 W](https://learn.pimoroni.com/article/getting-started-with-badger-2040-w)
