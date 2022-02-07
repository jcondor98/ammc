// AVR Multi Motor Control
/*! \file slave/source/dcmotor_phy_params.c
 * DC Motor Slave HAL hardware bindings and parameters, ATMega2560
 *
 * Used I/O pins are:
 * D53 (PORTB0/PCINT0) -> Encoder Phase A
 * D52 (PORTB1/PCINT1) -> Encoder Phase B
 * D13 (PORTB7/OCR0A)  -> PWM output
 * D12 (PORTB6/OCR1A)  -> Direction switch 1
 * D11 (PORTB5/OCR1B)  -> Direction switch 2
 *
 * \author Paolo Lucchesi
 */
#ifndef __DCMOTOR_HAL_PARAMETERS_H
#define __DCMOTOR_HAL_PARAMETERS_H

// Register used to set the speed in RPM
#define SPEED_REGISTER OCR0A

// PWM Parameters
#define PWM_DDR         DDRB
#define PWM_DDR_MASK    (1 << 7)
#define PWM_TCCRA       TCCR0A
#define PWM_TCCRA_VALUE ((1 << WGM01) | (1 << WGM00) | (1 << COM0A1))
#define PWM_TCCRB       TCCR0B
#define PWM_TCCRB_VALUE ((1 << CS00) | (1 << CS01))

// Direction parameters
#define DIRECTION_IN1_DDR       DDRB
#define DIRECTION_IN1_DDR_MASK  (1 << 6)
#define DIRECTION_IN2_DDR       DDRB
#define DIRECTION_IN2_DDR_MASK  (1 << 5)
#define DIRECTION_IN1_PORT      PORTB
#define DIRECTION_IN1_PORT_MASK DIRECTION_IN1_DDR_MASK
#define DIRECTION_IN2_PORT      PORTB
#define DIRECTION_IN2_PORT_MASK DIRECTION_IN2_DDR_MASK

// Encoder parameters
#define DCMOTOR_PHY_ENCODER_ISR PCINT0_vect
#define ENCODER_A_DDR       DDRB
#define ENCODER_A_DDR_MASK  (1 << 0)
#define ENCODER_A_PORT      PORTB
#define ENCODER_A_PORT_MASK ENCODER_A_DDR_MASK
#define ENCODER_A_PIN       PINB
#define ENCODER_A_PIN_MASK  ENCODER_A_DDR_MASK
#define ENCODER_B_DDR       DDRB
#define ENCODER_B_DDR_MASK  (1 << 1)
#define ENCODER_B_PORT      PORTB
#define ENCODER_B_PORT_MASK ENCODER_B_DDR_MASK
#define ENCODER_B_PIN       PINB
#define ENCODER_B_PIN_MASK  ENCODER_B_DDR_MASK
#define ENCODER_PCICR       PCICR
#define ENCODER_PCICR_MASK  (1 << PCIE0)
#define ENCODER_PCMSK       PCMSK0
#define ENCODER_PCMSK_MASK  (1 << PCINT0)

// PID controller parameters
#define DCMOTOR_PHY_PID_ISR TIMER1_COMPA_vect
#define PID_TCCRA           TCCR1A
#define PID_TCCRA_VALUE     0
#define PID_TCCRB           TCCR1B
#define PID_TCCRB_VALUE     ((1 << CS10) | (1 << CS12)) // Prescaled by 1024
#define PID_TIMSK           TIMSK1
#define PID_TIMSK_MASK      (1 << OCIE1A)
#define PID_TCNT            TCNT1
#define PID_OCRH            OCR1AH
#define PID_OCRL            OCR1AL

#endif  // __DCMOTOR_HAL_PARAMETERS_H
