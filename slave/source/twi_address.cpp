// AVR Multi Motor Control
/*! \file slave/include/twi_address.h
 * TWI persistent address module
 *
 * \author Paolo Lucchesi
 */
#include <avr/io.h>
#include <avr/eeprom.h>
#include "twi_address.h"

namespace TWI {
  static uint8_t EEMEM twiAddressEeprom = TW_DEFAULT_ADDRESS;

  //! Get the current TWI address
  uint8_t getAddress(void) {
    return TWAR >> 1;
  }

  //! Set the current TWI address
  void setAddress(uint8_t newAddress) {
    if (newAddress & (1 << 7))
      TWAR |= newAddress << 1;
  }

  //! Fetch the address from the EEPROM and set it as the current address
  void fetchAddress(void) {
    TWAR = eeprom_read_byte(&twiAddressEeprom);
  }

  //! Save the current TWI address to the EEPROM
  void saveAddress(void) {
    eeprom_write_byte(&twiAddressEeprom, TWAR);
  }
}
