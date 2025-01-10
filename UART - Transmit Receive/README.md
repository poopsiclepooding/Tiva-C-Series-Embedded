## Question

1. Program your micro-controller to transmit the 8-bit value "0xF0" if SW1 is pressed and "0xAA" if SW2 is pressed over UART with baud rate 9600 and odd parity. Read the relevant sections of the datasheet and board manual.
2. Sketch the expected waveforms for both cases with indicative timings.
3. Connect the Scope to the TX pin and verify that the captured signals match what you have drawn in part2.
4. Add code to your program to also listen for incoming data on the UART with the same baud and parity config. If "0xAA" is received, turn LED should light up GREEN. If "0xF0" is received, the LED should be BLUE and if any error is detected LED should be RED. Test this by communicating with your neighboring group. Remember to connect RX of one board to TX of the other, and make sure to connect the board grounds together.

## Solution

