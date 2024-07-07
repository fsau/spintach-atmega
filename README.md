# Simple tachometer with ATtiny2313A

## Description

A simple tachometer that shows the frequency (in RPM) of a input signal on an alphanumeric LCD display. Also has a PWM output which can be toggled and adjusted using the buttons interface. Range from about 50 RPM to 20 kRPM seems to be working well.

## Build

Just running `make` with the avr toolchain installed should generate the `.hex` file required to program the microcontroller. Also can use `make prog` for automatically programming the µC, just specify the programmer to be used on the `Makefile`.

## TODOs

1. Schematics (µC pinout is specified on source code)
2. Closed loop PWM/RPM control (will probably require a µC with larger memory, like a 4313A or ATmega)