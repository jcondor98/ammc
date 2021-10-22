% AMMC(1) | AVR Multi Motor Control

NAME
====

**ammc** —  AVR Temperature Monitor client

SYNOPSIS
========

**ammc** \[**-s** _\[script]_] \[**-c** _device-file_]  
**ammc** -h

DESCRIPTION
===========

**ammc** is a multi-motor control device realized with an AVR microcontroller;
this program is an ad-hoc, tui client to interface it.  
The ammc device is designed to work with DC motors with an embedded or external
encoder.

Options
-------

-c _device-file_
:   Connect to the device identified by _device-file_ (e.g. /dev/ttyACM0)

-s _\[script]_
:   Execute in script mode (do not print prompt, exit on error etc...), being
:   _script_ a file containing a command each line. If _script_ is not given,
:   standard input is used

-h
:   Display a help message and exit

Commands
--------

**help** \[_command_]
:   Show help, also for a specific command if an argument is given

**connect** _device\_path_
:   Connect to an ammc Master MCU given its device file (usually under /dev)

**disconnect**
:   Close an existing connection - Has no effect on the device

**dev-echo** \<_arg_> \[_arg2_ _arg3_ _..._]
:   Send a string to the device, which should send it back

**get-speed** \<_motor-id_>
:   Get the speed of a DC motor in RPM

**set-speed** \<_motor-id_>=\<_value_>
:   Set the speed of a DC motor in RPM

**set-slave-addr** \<_actual-addr_> \<_new-addr_>
:   Set a new TWI address for a slave controller

AUTHOR
======

Paolo Lucchesi <paololucchesi@protonmail.com>

https://www.github.com/jcondor98/ammc
