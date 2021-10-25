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

### Features

* Text-based client application for POSIX environments
* Master and Slave(s) controller firmware
* Fully binary Client-Master communication protocol on top of the serial interface
* Fully binary Master-Slave communication protocol on top of the I2C interface
* Up to 126 DC motors (limited by 7-bit I2C Slave addressing, `0x00` is reserved)
* Ability to get and set the DC motors speed, individually
* Software defined PID controller embedded in each Slave controller

### Documentation

An overview of the project is given in this Markdown document.

Documentation generated with doxygen for the source code and the programming API
can be found [here](https://jcondor98.github.io/ammc/).

The client is documented by the man page `ammc(1)`.

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

A list of _make_ recipes is given below:
```
# Make client executables and master and slave .elf binaries
make

# Compile and link the client-side executable
make client

# Install the -ALREADY COMPILED- client-side executable
sudo make install

# Generate man page (requires pandoc)
make docs

# Install man page (pregenerated in the repo)
sudo make install-docs

# Compile and link master .elf binary
make master

# Compile and link slave .elf binary
make slave

# Encode the master .elf binary into a .hex file and flash it into the AVR
make master-flash

# Encode the slave .elf binary into a .hex file and flash it into the AVR
make slave-flash
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
`COM_TYPE_NULL`      | `0x00` | Reserved, never use
`COM_TYPE_HND`       | `0x01` | Handshake
`COM_TYPE_ACK`       | `0x02` | Acknowledgement
`COM_TYPE_NAK`       | `0x03` | Communication error
`COM_TYPE_ECHO`      | `0x04` | [DEBUG] Echo between Client and Master
`COM_TYPE_TWI_ECHO`  | `0x05` | [DEBUG] Echo a single char to the first Slave via TWI
`COM_TYPE_GET_SPEED` | `0x06` | Get the current speed for a DC motor
`COM_TYPE_SET_SPEED` | `0x07` | Set (and apply) the speed for a DC motor
`COM_TYPE_APPLY`     | `0x08` | Tell all the slaves to apply the previously set speeds
`COM_TYPE_DAT`       | `0x09` | Primarily used for responses from the AVR device
`COM_TYPE_LIMIT`     | `0x0A` | Used for sanity checks - Must have highest value

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

The master controller offers an upper-level, command-based API to handle the DC
motors:

Client command | Description
:-:|---
`get-speed <motor_id>` | Get the speed of a DC motor
`set-speed <motor_id>=<speed>` | Set the speed of a DC motor
`apply` | Apply the previously set speeds to all the DC motors

Further informations can be found in the man page or by issuing the `help`
command to the client shell.

**NOTE**: The `sample` command has been removed from the specification, since
the effective speed is periodically sampled by each Slave, as needed by the PID
embedded controller.

### Master-Slave communication

Master and slaves communicate to each other using the TWI/I2C protocol. The
implementation had been realized from scratch and also offers broadcasting
capabilities from master to slaves, based on the `0x00` built-in broadcasting
address.

The communication layer on top of the I2C protocol is completely binary.

#### Communication frame

The I2C communication frame is composed of a leading byte representing the
code of the command to be executed by the slave controller, and a variable
size trailing argument. The commands are listed below:

Command | Code | Description
:-:|:-:|---
`DC\_MOTOR\_CMD\_GET`   | `0x00` | Get the sampled motor speed
`DC\_MOTOR\_CMD\_SET`   | `0x01` | Set a new target speed
`DC\_MOTOR\_CMD\_APPLY` | `0x02` | Apply the previously set speed
`TWI\_CMD\_ECHO`        | `0x03` | Echo a byte back to master (debug)
`TWI\_CMD\_SET\_ADDR`   | `0x04` | Change the current I2C address (i.e. motor id)


## Notes

The doxygen custom CSS files is taken from _jothepro_, [here](https://github.com/jothepro/doxygen-awesome-css/blob/v1.6.0/LICENSE).
It is released under the MIT license.
