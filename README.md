# STM32-UART-PC-to-MCU-LED-Control

This project implements a register-level UART control system on an STM32F103 microcontroller
A PC sends 2-byte commands through USART1 to control 8 LEDs connected to the MCU. The MCU also sends back the current LED status as an 8-bit status byte

## Overview

The project focuses on low-level embedded programming using direct peripheral register configuration instead of high-level libraries such as HAL.

Main features:

- Control 8 LEDs from a PC through UART commands
- Receive UART data using USART1 RXNE interrupt
- Configure GPIO, USART1, EXTI, NVIC, system clock, and Flash latency at register level
- Support individual LED control and all-LED control
- Send error messages for invalid commands
- Send current LED status back to the PC
- Use an external interrupt input to request LED status transmission

## Hardware

- STM32F103 microcontroller
- 8 LEDs, Transistor, Resistor
- UART - TTL for PC communication

## Tools

- Visual Studio Code
- Hercules
- Logic Analyzer
- Keil uVision
- STM32CubeIDE

## Command Format

| Command | Description |
|---|---|
| `1i` | Turn on LED 1 |
| `1o` | Turn off LED 1 |
| `8i` | Turn on LED 8 |
| `8o` | Turn off LED 8 |
| `ai` | Turn on all LEDs |
| `ao` | Turn off all LEDs |

## UART Feedback

The MCU sends the current LED state as an 8-bit status byte

- `0xFF`: all LEDs ON
- `0x00`: all LEDs OFF

Since the status is transmitted as a raw byte, Hercules may display the response in HEX format

## What I Learned

- Register-level STM32 peripheral configuration
- USART communication
- UART RX interrupt handling
- External interrupt handling
- GPIO control using BSRR
- Bitwise operations for LED state management
- Simple command protocol design
- Comparing the actual speed between using registers and HAL
