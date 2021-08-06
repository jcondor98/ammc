# AVR Multi Motor Control

This repo contains the entire codebase and documentation for the bachelor degree
apprenticeship, which is supervised by Prof. Giorgio Grisetti and
Drs. Barbara Bazzana.

## Overview

The project consists in a multi-motor control system. Each DC motor is
individually handled by a dedicated standalone AVR controller. An Atmel AT2560
is used as master controller.

The master controller must:

* Communicate with the slaves (i.e. controllers handling individual motors) via the I2C protocol
* Communicate with a PC via the serial (over USB) protocol

The end user will interface just the master controller, using a TUI program which
runs under POSIX-compliant OSes.

### Professor Reference

Quoting from [Prof. Grisetti's repository](https://gitlab.com/grisetti/sistemi_operativi_2020_21/-/blob/master/projects_preliminary/readme.md):

> [...]
>
> 1. **Arduino TWI communication** ---> `[x2]` `[C98]`
>    Use two (or more arduino), one configured as master the other ones as slaves
>    Implement an interrupt based interaction  protocol on i2c [http://www.chrisherring.net/all/tutorial-interrupt-driven-twi-interface-for-avr-part1/.
>    The protocol should contain the following messages:
>
>    Direction | Command | Description
>    ---|:-:|---
>    `master -> all`               | `sample` | the slaves sample all digital inputs and puts them in a struct
>    `master -> slave[i] ->master` | `get`    | the slave number _i_ sends the sampled digital inputs to the master
>    `master -> slave[i]`          | `set`    | the master sends to the slave _i_ the desired output pin configuration in an internal struct, "without" applying it
>    `master -> all`               | `apply`  | the slaves apply the saved pin status to the outputs
>
>    The slaves react only to interrupts. they are in halt status.
>    The master offers an interface to the host (PC) program to control the slaves, via UART.
>    All protocols are binary, packet based
>
> 2. **Arduino motor controller** `[x1]` `[C98]`
>    If you have a DC motor with encoder, implement a closed loop motor servo using a PID controller
>    The arduino reads the encoder, and issues a PWM, so that the speed of the motor
>    measured by the encoder matches a desired speed set by the user.
>    The Host program allows to set a speed via uart, and the program on the arduino periodically
>    sends back the status (pwm, encoder position, desired encoder speed and measured encoder speed)
>
> 3. **Arduino multi motor control** `[x4]` `[C98]`
>    integrate project 1 and project 2. Each slave controls a motor and communicates via i2c to the master
>    that provides a unified interface. The event loop on the slaves is synchronized with the "apply" command.
>
> [...]


## Installation

The entire project uses the _GNU Make_ build system. Client, Master and Slave
programs' codebases are separated from each others.

### Client

A list of _make_ recipes is given below:
```
# Compile and link the client-side executable
make

# Install the -ALREADY COMPILED- client-side executable
sudo make install

# Generate (requires pandoc) man page
make docs

# Install man page (pregenerated in the repo)
sudo make install-docs
```

### Master and Slave

A list of _make_ recipes is given below:
```
# Compile and link the avr-side .elf executable
make

# Encode the .elf executable into a .hex file and flash it into the AVR
make flash
```


## Specification

### Client-Master communication

The Client-Master communication protocol is binary, packet-based and built on
top of the Serial-over-USB protocol offered by the AT2560 board. Each packet
has a variable length and its integrity is checked with a trailing CRC-8
checksum.

#### Packet Header

The header is composed of the following fields:

Field | Size (bits) | Description
:-:|:-:|---
`id`        | 8 | Packet ID
`type`      | 8 | Packet type
`selector`  | 8 | Selector for DC motors. Also used to store error codes in NAK packets
`size`      | 8 | Total packet size, including header and checksum

`type` can have the following values:

Type | Actual value | Description
:-:|:-:|---
`NUL`       | `0x00` | Reserved (not used)
`HND`       | `0x01` | Handshake (sent by PC, ACKed by Master)
`ACK`       | `0x02` | Acknowledgement
`NAK`       | `0x03` | Communication error
`ECHO`      | `0x04` | String echoing, used for debug
`GET_SPEED` | `0x05` | Get DC motor(s) speed value(s) in m/s
`SET_SPEED` | `0x06` | Set DC motor(s) speed value(s) in m/s
`DAT`       | `0x07` | Data (sent from Master to Client)

#### Acknowledgements

A communication endpoint must wait for an acknowledgement message from the
counterpart once it sent a packet in order to send a new one. ACK and NAK
packets do not bring any data.

When a packet arrives, it is checked for integrity and sanity. If it is sane,
then an ACK packet is sent; if not, then a NAK packet is sent.
ACK and NAK packets are simply discarded if corrupted in some way.

A NAK packet uses the 'selector' field to send to the other endpoint the error
code describing what happened on its side. Error codes can be found in
`communication.h`.

#### Controlling DC motors' speed

**TODO**

### Master-Slave communication

**TODO**
