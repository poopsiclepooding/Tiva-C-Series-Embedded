## Overview

This repository demonstrates an embedded implementation of printf() and a trace function for debugging and logging, designed to work with the Tiva C Series TM4C123G LaunchPad. The project uses the UART module in interrupt mode and a ring buffer to manage data efficiently during UART transmissions.

## Features

1. UART Module in interrupt mode is used to transmit and receive data.

2. Ring Buffer is implemented for managing data whilst UART does its job.

3. printf() implementation is used for [this](https://github.com/mpaland/printf) github

## File Structure

```plaintext
.
├── code/
│   ├── main.c              # Main application code
│   ├── uart.c              # UART driver implementation
│   ├── uart.h              # UART driver header
│   ├── ring_buffer.c       # Ring buffer implementation
│   ├── ring_buffer.h       # Ring buffer header
│   ├── printf.c            # Custom printf() implementation
│   ├── printf.h            # Header for printf() functionality
│   ├── printf_config.h     # Config File for printf() implementation
│   ├── trace.h             # Trace header 
│   └── trace.c             # Trace/logging implementation
└── README.md               # Project documentation
```
