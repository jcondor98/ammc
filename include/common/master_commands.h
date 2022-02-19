// AVR Multi Motor Control
/*! \file include/common/master_commands.h
 * DC Motor Master Command common definitions
 *
 * \author Paolo Lucchesi
 */
#ifndef __MASTER_COMMANDS_MODULE_COMMON_H
#define __MASTER_COMMANDS_MODULE_COMMON_H
#include <stdint.h>

#define COMMAND_MAX_SIZE 16
#define COMMAND_ARG_MAX_SIZE (COMMAND_MAX_SIZE-1)

/*!
 * \enum _DC_MOTOR_CMD_E
 * \brief Master -> Slave commands
 */
enum class MasterCommandId : uint8_t {
  none,       //!< Null command id
  getSpeed,   //!< Get the dc motor speed
  setSpeed,   //!< Set a new speed for the dc motor
  applySpeed, //!< Apply the previously set speed
  ping,       //!< Ping a slave controller
  setAddress  //!< Change the slave current address
};


//! Data type for a master command
class RawMasterCommand {
 public:
  MasterCommandId id;
  uint8_t argument[COMMAND_ARG_MAX_SIZE];
};

#endif	// __MASTER_COMMANDS_MODULE_COMMON_H
