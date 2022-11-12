# 746-compact-compatible-board
 An open source compatible board for the 746 compact

## The idea
 Idea came out when my automatic sliding gate stopped working.
 After some analisis I found out that the controlling board was broken.
 Now it is impossible to find a new replacement of this old board, which FAAC discontinued, so I decided to make one myself.
 
## I/O information
 - The high voltage part has been projected preserving functionality and safety, from the original board;
 - The low voltage/contols part of the board has been completely reworked to use an Arduino Nano as controller;
 - For the inputs I used some optoisolators to adjust the 24V of the sensor to the 5V of Arduino.
 - The motor is controlled by three relays to:
   - open the gate;
   - close the gate;
   - brake the gate in position;
 - The relays are driven by an integrated circuit, to adjust the 5V of arduno to the 24V of relays.
 
## Differences with the original
 - You can connect the radio control board, directly to the main board;
   - the original manufacturer let this tiny board float into the case;
 - There are more screw connectors, than the original, to connect easily the board with the sensors and the motor with its capacitor;
 - Now the board is bigger, but still fits my case;
 - If the gate is taking too much time to open/close completely, before a configured time, the board stop the movement, to preserve the motor;
 - There are some jumpers for future software expansion, now one of them make you choose if the gate should close by himself;
 - There is a software program to check if infrared photocell is wotking correctly, if not the board disable the opening/closing of the gate.
 
## Future expansions
 - Create a program to measure automatically the time to open/close completely the gate, using one of the configurable jumpers.

## Liense
 This entire project is shared as it is, so I am not responsible for any damage to people, animals, things or anything else.
 So use it at your danger.
