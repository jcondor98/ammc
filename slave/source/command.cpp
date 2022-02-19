// AVR Multi Motor Control
/*! \file slave/source/command.c
 * Master->Slave ammc commands
 *
 * \author Paolo Lucchesi
 */
#include "command.h"
#include "dcmotor.h"
#include "twi.h"

MasterCommandId MasterCommand::getId(void) {
  return data.id;
}

uint8_t MasterCommand::getArgumentSize(void) {
  return argumentSize;
}

void *MasterCommand::getArgument() {
  return argumentSize == 0 ? nullptr : static_cast<void*>(data.argument);
}

void MasterCommand::receive(void) {
  uint8_t received = TWI::recv(&data, TW_RX_MAX_LEN);
  argumentSize = received ? received - sizeof(MasterCommandId) : 0;
}

uint8_t MasterCommand::respond(const void *response, uint8_t size) {
  return TWI::send(response, size);
}

bool MasterCommand::isValid(void) {
  uint8_t new_addr;
  switch (data.id) {
    case MasterCommandId::none:
      return true;
    case MasterCommandId::getSpeed:
    case MasterCommandId::applySpeed:
      return argumentSize == 0;
    case MasterCommandId::setSpeed:
      return argumentSize == sizeof(dc_rpm_t);
    case MasterCommandId::ping:
      return argumentSize == 1;
    case MasterCommandId::setAddress:
      new_addr = *(static_cast<uint8_t*>(getArgument()));
      return (argumentSize == 1 && new_addr != 0 && new_addr < 127);
    default: return false; // Unknown command
  }
}
