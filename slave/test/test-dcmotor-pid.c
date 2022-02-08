// AVR Multi Motor Control
/*! \file test/dcmotor_pid.c
 * DC Motor Proportional-Integral-Derivative Slave Test Unit
 *
 * \author Paolo Lucchesi
 */
#include <stdlib.h>
#include "framework/test-framework.h"
#include "dcmotor_pid.h"

#define PID_KP 0.9
#define PID_KI 0.0
#define PID_KD 0.1
#define PID_SAMPLE_INT 500

#define PID_TEST_ITERATIONS 1000
#define PID_FINAL_SPEED_TOLERANCE 10
#define PID_DERIVATIVE_ABS_CONSTANT 20

static dc_rpm_t speed_actual, speed_target;
extern float err_int, err_prev;

static inline void initialize_test(
    dc_rpm_t actual, dc_rpm_t target, float pid_der) {
  dcmotor_pid_init(PID_KP, PID_KI, PID_KD, PID_SAMPLE_INT);
  speed_actual = actual;
  speed_target = target;
}

static int test_dynamic_behaviour(void) {
  for (unsigned i=0; i < PID_TEST_ITERATIONS; ++i) {
    speed_target = dcmotor_pid_iterate(speed_actual, speed_target);
    speed_actual = (speed_target + speed_actual) / 2;
  }
  test_expr(abs(speed_actual - speed_target) < PID_FINAL_SPEED_TOLERANCE,
    "On normal behaviour should correct the speed"
    "under a certain tolerance");
}

static int test_still_motor_behaviour(void) {
  initialize_test(0, 0);
  unsigned times_speed_changed = 0;

  for (unsigned i=0; i < PID_TEST_ITERATIONS; ++i) {
    speed_target = dcmotor_pid_iterate(speed_actual, speed_target);
    speed_actual = (speed_target + speed_actual) / 2;
    if (speed_target != speed_actual || speed_target != 0)
      ++times_speed_changed;
  }

  test_expr(times_speed_changed == 0,
      "With a still motor and zero target speed should not correct");
}


static void with_positive_derivative(void) {
  err_prev = PID_DERIVATIVE_ABS_CONSTANT;
}

static void with_negative_derivative(void) {
  err_prev = -PID_DERIVATIVE_ABS_CONSTANT;
}

static void with_null_derivative(void) {
  err_prev = 0;
}

static void when_sampled_lt_target(void) {
  speed_actual = 100;
  speed_target = 110;
}

static void when_sampled_eq_target(void) {
  speed_actual = 100;
  speed_target = 100;
}

static void when_sampled_gt_target(void) {
  speed_actual = 110;
  speed_target = 100;
}


int test_case_1(void) {
  with_positive_derivative();
  when_sampled_lt_target();
  dc_rpm_t corrected = dcmotor_pid_iterate(speed_actual, speed_target);
  return corrected > speed_target; // ???
}

int test_case_2(void) {
  with_positive_derivative();
  when_sampled_eq_target();
  dc_rpm_t corrected = dcmotor_pid_iterate(speed_actual, speed_target);
  return corrected < speed_target;
}

int test_case_3(void) {
  with_positive_derivative();
  when_sampled_gt_target();
  dc_rpm_t corrected = dcmotor_pid_iterate(speed_actual, speed_target);
  return corrected < speed_target;
}

int test_case_4(void) {
  with_negative_derivative();
  when_sampled_lt_target();
  dc_rpm_t corrected = dcmotor_pid_iterate(speed_actual, speed_target);
  return corrected > speed_target;
}

int test_case_5(void) {
  with_negative_derivative();
  when_sampled_eq_target();
  dc_rpm_t corrected = dcmotor_pid_iterate(speed_actual, speed_target);
  return corrected > speed_target;
}

int test_case_6(void) {
  with_negative_derivative();
  when_sampled_gt_target();
  dc_rpm_t corrected = dcmotor_pid_iterate(speed_actual, speed_target);
  return corrected < speed_target; // ???
}

int test_case_7(void) {
  with_null_derivative();
  when_sampled_lt_target();
  dc_rpm_t corrected = dcmotor_pid_iterate(speed_actual, speed_target);
  return corrected < speed_target;
}

int test_case_8(void) {
  with_null_derivative();
  when_sampled_eq_target();
  dc_rpm_t corrected = dcmotor_pid_iterate(speed_actual, speed_target);
  return corrected = speed_target;
}

int test_case_9(void) {
  with_null_derivative();
  when_sampled_gt_target();
  dc_rpm_t corrected = dcmotor_pid_iterate(speed_actual, speed_target);
  return corrected > speed_target;
}


int main(int argc, const char *argv[]) {
  test_expr(test_case_1(),
      "With positive derivative, "
      "when sampled lt target "
      "corrected should be > target");
  test_expr(test_case_2(),
      "With positive derivative, "
      "when sampled eq target "
      "corrected should be < target");
  test_expr(test_case_3(),
      "With positive derivative, "
      "when sampled gt target "
      "corrected should be < target");
  test_expr(test_case_4(),
      "With negative derivative, "
      "when sampled lt target "
      "corrected should be > target");
  test_expr(test_case_5(),
      "With negative derivative, "
      "when sampled eq target "
      "corrected should be > target");
  test_expr(test_case_6(),
      "With negative derivative, "
      "when sampled gt target "
      "corrected should be < target");
  test_expr(test_case_7(),
      "With null derivative, "
      "when sampled lt target "
      "corrected should be < target");
  test_expr(test_case_8(),
      "With null derivative, "
      "when sampled eq target "
      "corrected should be = target");
  test_expr(test_case_9(),
      "With null derivative, "
      "when sampled gt target "
      "corrected should be > target");

  test_summary();
  return 0;
}
