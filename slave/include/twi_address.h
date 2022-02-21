// AVR Multi Motor Control
/*! \file slave/include/twi_address.h
 * TWI persistent address module
 *
 * \author Paolo Lucchesi
 */
#pragma once
#include <stdint.h>

#define TW_DEFAULT_ADDRESS 0x01

namespace TWI {
  //! Get the current TWI address
  uint8_t getAddress(void);

  //! Set the current TWI address
  void setAddress(uint8_t newAddress);

  //! Fetch the address from the EEPROM and set it as the current address
  void fetchAddress(void);

  //! Save the current TWI address to the EEPROM
  void saveAddress(void);
}
