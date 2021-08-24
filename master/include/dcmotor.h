// AVR Multi Motor Control -- Paolo Lucchesi
// DC Motor Master Module - Head file
#ifndef __DC_MOTOR_MODULE_H
#define __DC_MOTOR_MODULE_H
#include <stdint.h>

// Maximum speed of a motor in RPM
// TODO: Set this to something meaningful
#define DC_MOTOR_MAX_SPEED 200

// Handle DC motor number
#define DC_MOTOR_NUM 4
#define DC_MOTOR_SEL_ALL (0xFF << (8-DC_MOTOR_NUM) >> (8-DC_MOTOR_NUM))

// Type to properly store a rounds per minute value
typedef uint8_t dc_rpm_t;

// Master -> Slave commands
typedef enum _DC_MOTOR_CMD_E {
  DC_MOTOR_CMD_GET,
  DC_MOTOR_CMD_SET,
  DC_MOTOR_CMD_APPLY,
  TWI_CMD_ECHO
} dc_motor_cmd_t;

// Get the speed of a DC motor in RPM
dc_rpm_t dcmotor_get(uint8_t slave_addr);

// Set the speed of a DC motor in RPM
// Returns 1 on success, 0 otherwise
uint8_t dcmotor_set(uint8_t slave_addr, dc_rpm_t rpm);

// Tell the DC motor controllers to apply their speed
void dcmotor_apply(void);

#endif	// __DC_MOTOR_MODULE_H
