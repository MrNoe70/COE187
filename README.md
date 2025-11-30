# LED Sequence Controller for MAX78000 FTHR Board

## Project Overview
This project implements an interrupt-driven LED sequence controller on the MAX78000 FTHR board. The system features two distinct LED patterns that can be toggled instantly via a push button with proper debouncing.

## Features
- **Two LED Patterns**: Morse code "C O E" and rotating LED pattern
- **Instant Mode Switching**: Interrupt-driven button response
- **Hardware Debouncing**: 50ms debounce delay for reliable button presses
- **Low Power Operation**: CPU sleeps between interrupts using `__WFI()`
- **Timer-Driven Updates**: Precise 100ms timing via hardware timer
- **Multi-Port LED Support**: Controls LEDs across GPIO0, GPIO1, GPIO2, and GPIO3

## Hardware Requirements
- MAX78000 FTHR Evaluation Board
- 8 LEDs connected to specified GPIO pins
- Push button (SW1 on FTHR board)

## File Structure

| File             | Description                                        |
|------------------|----------------------------------------------------|
| `main.c`         | Main program: initializes LEDs, button, timer.     |
| `ledblink.c`     | LED control logic (Morse & shift patterns).        |
| `ledblink.h`     | Header file for LED functions.                     |
| `Makefile`       | Build configuration for MAX78000 SDK.              |

## LED Pin Assignments
| LED | GPIO Port | Pin    |
|-----|-----------|--------|
| 0   | GPIO0     | PIN_17 |
| 1   | GPIO0     | PIN_16 |
| 2   | GPIO3     | PIN_1  |
| 3   | GPIO0     | PIN_19 |
| 4   | GPIO0     | PIN_11 |
| 5   | GPIO0     | PIN_8  |
| 6   | GPIO2     | PIN_3  |
| 7   | GPIO1     | PIN_1  |

## Pattern Descriptions

### Morse Code Pattern (Mode 0)
Displays "C O E" in Morse code using all 8 LEDs:
- **C** = `-.-.` (Dash Dot Dash Dot)
- **O** = `---` (Dash Dash Dash) 
- **E** = `.` (Dot)

**Timing** (100ms per step):
- Dot = 100ms ON
- Dash = 300ms ON (3 consecutive ON steps)
- Element gap = 100ms OFF
- Letter gap = 100ms OFF
- Word gap = 300ms OFF

### Rotating Pattern (Mode 1)
Creates a "moving dark spot" effect where one LED is off while all others are on:

11111110 → 11111101 → 11111011 → 11110111 →
11101111 → 11011111 → 10111111 → 01111111 →
(Repeat)

- Each pattern displays for 100ms
- Creates smooth rotating animation

## System Architecture

### Interrupt Structure
1. **GPIO Interrupt** - Button press detection
2. **Timer Interrupt** - 100ms pattern updates

### Key Components
- **State Machine**: Pattern progression via array indexing
- **Debounce Logic**: 50ms time-based debouncing
- **Multi-Port GPIO**: Simultaneous control of LEDs across different ports
- **Low-Power Design**: CPU sleeps between interrupts

## Building and Flashing

### Prerequisites
- Maxim SDK for MAX78000
- ARM GCC toolchain
- OpenOCD or other flashing utility

### Build Commands 
make clean
make TARGET=MAX78000 BOARD=FTHR_RevA

### Flash to Board
make flash

## Troubleshooting

### Common Issues
1. LEDs not lighting: Verify GPIO pin assignments match hardware
2. Button not responding: Check SW1 connection and pull-up configuration
3. Multiple mode toggles: Increase debounce delay if switch bouncing occurs
4. Erratic timing: Verify timer configuration and clock settings

### Debugging
- Use serial output to verify system state
- Check interrupt flags in debugger
- Verify GPIO configurations match hardware

## License
This project is provided as example code for the MAX78000 FTHR board.

## References
- MAX78000 Datasheet
- FTHR Board User Guide
- Maxim SDK Documentation