// AVR Temperature Monitor -- Paolo Lucchesi
// Test Framework - Source file
#include "test-framework.h"

// Color escape sequence (to start printing colored stuff)
#define COLOR_ESCAPE_SEQ_FMT "\x1b[%dm"


// Register passed and failed tests
static unsigned test_passed;
static unsigned test_failed;


test_result_t test_expr(int expr, const char *desc, ...) {
  // Pass or fail test
  if (expr) {
    printf_colored(C_BRIGHT_GREEN, "[PASSED] ");
    ++test_passed;
  } else {
    printf_colored(C_BRIGHT_RED, "[FAILED] ");
    ++test_failed;
  }

  // Print test message
  va_list args;
  va_start(args, desc);
  vprintf(desc, args);
  va_end(args);

  putc('\n', OUT_STREAM);
  return expr ? 1 : 0;
}

unsigned test_count_passed(void) { return test_passed; }
unsigned test_count_failed(void) { return test_failed; }
unsigned test_count(void) { return test_passed + test_failed; }

void test_summary(void) {
  printf_colored(C_BRIGHT_BLUE, "\n\n| TEST SUMMARY\n\\___________________\n");
  printf("TOTAL:  %u\nPASSED: %u\nFAILED: %u\n\n",
      test_passed + test_failed, test_passed, test_failed);
}

static inline int is_valid_color(ansi_color_t color) {
  return (color < 31 || (color > 37 && color < 90) || color > 97) ? 0 : 1;
}

static inline void set_color(FILE *out, ansi_color_t color) {
  if (color != C_NONE)
    fprintf(out, COLOR_ESCAPE_SEQ_FMT, color);
}

static inline void clear_color(FILE *out, ansi_color_t color) {
  if (color != C_NONE)
    fprintf(out, COLOR_ESCAPE_SEQ_FMT, 0);  // i.e. color reset sequence
}

int vfprintf_colored(FILE *out, ansi_color_t color,
    const char *format, va_list args) {
  if (!format || !is_valid_color(color))
    return 0;

  set_color(out, color);
  int ret = vfprintf(out, format, args); // Print format
  clear_color(out, color);

  return ret;
}

int fprintf_colored(FILE *out, ansi_color_t color, const char *format, ...) {
  va_list args;
  va_start(args, format);

  int ret = vfprintf_colored(out, color, format, args);

  va_end(args);
  return ret;
}
