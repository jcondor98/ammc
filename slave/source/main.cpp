// AVR Multi Motor Control
/*! \file slave/source/main.c
 * Slave Main routine
 *
 * \author Paolo Lucchesi
 */
#include <avr/io.h>
#include <avr/power.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "twi_address.h"
#include "sleep_util.h"
#include "command.h"
#include "dcmotor.h"
#include "twi.h"


// DC Motor handled by this slave controller
DCMotor motor;


static inline void executeMasterCommand(MasterCommand &cmd) {
  if (!cmd.isValid()) return;

  uint8_t newTWIOwnAddress;
  dc_rpm_t speed;

  switch (cmd.getId()) {
    case MasterCommandId::getSpeed:
      speed = motor.getActualSpeed();
      cmd.respond(&speed, sizeof(dc_rpm_t));
      break;
    case MasterCommandId::setSpeed:
      speed = *(static_cast<dc_rpm_t*>(cmd.getArgument()));
      motor.setTargetSpeed(speed );
      break;
    case MasterCommandId::applySpeed:
      motor.applyTargetSpeed();
      break;
    case MasterCommandId::ping:
      cmd.respond(cmd.getArgument(), cmd.getArgumentSize());
      break;
    case MasterCommandId::setAddress:
      newTWIOwnAddress = *(static_cast<uint8_t*>(cmd.getArgument()));
      TWI::setAddress(newTWIOwnAddress);
      TWI::saveAddress();
      break;
    default: break; // Unknown command, simply ignore
  }
}

static inline void setupPowerSaving(void) {
  power_spi_disable();
  power_usart0_disable();
}

static inline void setup(void) {
  setupPowerSaving();
  sei();

  TWI::fetchAddress();
  TWI::initialize(TWI::getAddress());

  motor.startPid();
}


int main(void) {
  setup();

  MasterCommand command;
  while (1) {
    command.receive();
    executeMasterCommand(command);
  }
}
