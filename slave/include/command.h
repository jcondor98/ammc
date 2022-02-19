// AVR Multi Motor Control
/*! \file slave/include/command.h
 * Master->Slave ammc commands
 *
 * \author Paolo Lucchesi
 */
#pragma once
#include "common/master_commands.h"
#include "twi.h"

class MasterCommand {
 public:
  MasterCommand();
  MasterCommand(MasterCommandId id);
  MasterCommand(MasterCommandId id, const void *arg, uint8_t arg_size);

  //! Get the master command id
  MasterCommandId getId(void);

  //! Get the master command argument size
  uint8_t getArgumentSize(void);

  //! Get the argument as an opaque pointer
  void *getArgument(void);

  //! Receive a command, storing it in 'this' object
  void receive(void);

  /*!
   * Respond to any received command
   *
   * @param response The response to the command
   * @param size The response size
   * @returns E_SUCCESS on success, another error value otherwise
   */
  static uint8_t respond(const void *response, uint8_t size);

  //! @returns true if the command is valid, false otherwise
  bool isValid(void);

 private:
  RawMasterCommand data;
  uint8_t argumentSize;
};
