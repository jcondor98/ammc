// AVR Multi Motor Control
/*! \file include/common/communication.h
 * Communication layer common definitions
 *
 * \author Paolo Lucchesi
 */
#ifndef __COMMUNICATION_COMMON_H
#define __COMMUNICATION_COMMON_H

//! Precise error state codes for receiving/sending errors
typedef enum _COM_ERROR_E {
  E_SUCCESS,            // Communication ended successfully
  E_ID_MISMATCH,        // Packet ID mismatch
  E_CORRUPTED_CHECKSUM, // Checksum mismatch, packet is corrupted
  E_WRONG_TYPE,         // Packet type is not valid
  E_TOO_BIG             // Packet is too big
} com_error_t;


#endif	// __COMMUNICATION_COMMON_H
