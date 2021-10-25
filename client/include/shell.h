// AVR Multi Motor Control
/*! \file client/include/shell.h
 * Program shell
 *
 * \author Paolo Lucchesi
 */
#ifndef __SHELL_MODULE_H
#define __SHELL_MODULE_H

/*!
 * Different types of commands, in order of descending priority
 *
 * \attention CMD_TYPE_NONE must be the last value!
 */
typedef enum COMMAND_TYPE_T {
  CMD_TYPE_ALL,       //!< Matches any command
  CMD_TYPE_BUILTIN,   //!< Built-in command
  CMD_TYPE_EXTERNAL,  //!< External command
  CMD_TYPE_NONE       //!< Unknown command type
} command_type_t;

/*!
 * Type definition for a shell command executor
 * A shell command can have a return value (0 on success), and argv[0] is the
 * name of the command itself, like command-line programs in the POSIX standard
 * 'env' can be the shell storage in case of external commands and the entire
 * shell context for built-ins, but the choice is up to the programmer
 */
typedef int (*shell_command_f)(int argc, char *argv[], void *env);

//! Type definition for a shell command
typedef struct _shell_command_s {
  char *name;
  char *help;  // Usage and brief description of the command
  shell_command_f exec;
} shell_command_t;

//! Shell flags
typedef enum SHELL_FLAG_E {
  SH_SIG_EXIT = 1 << 0,     //!< Exit at the next shell iteration
  SH_SCRIPT_MODE = 1 << 1,  //!< Non-interactive (i.e. script) mode
  SH_EXIT_ON_ERR = 1 << 2   //!< Exit if a command fails
} shell_flag_t;


//! Type definition for a shell
typedef struct _shell_s {
  char *prompt; // Printed at every non-script shell iteration

  // Commands
  shell_command_t *builtins;
  shell_command_t *commands;
  size_t builtins_count;
  size_t commands_count;

  void *storage; // Implementation dependent

  // Operation table for command operations
  // Other operations shall be added on any further needing
  struct {
    int (*compare)(const void *cmd1, const void *cmd2);
    shell_command_t* (*get)(const struct _shell_s *shell,
        const char *name, unsigned char type);
  } command_ops;

  unsigned char flags;
} shell_t;


/*!
 * Create a new shell object
 *
 * @param prompt          The input shell prompt to display at each iteration
 * @param commands        An array containing the external shell commands
 * @param commands_count  Number of items in the 'commands' array
 * @param storage         Buffer to store the shell storage
 * @returns A pointer to the new shell object on success, NULL on failure
 * \note The function will recreate a new commands array, so the one passed can
 * be deallocated without any wanted effect. Same thing will be done for
 * 'prompt'
 */
shell_t *shell_new(const char *prompt, const shell_command_t *commands,
    size_t commands_count, void *storage);

/*!
 * Delete a shell (along with all the memory objects it uses)
 *
 * @param shell Pointer to the shell to destroy
 */
void shell_delete(shell_t *shell);

/*!
 * Launch a shell - Blocks until the user exits
 *
 * @param shell Pointer to the shell to launch
 * @param input Pointer to the file or stream to use as user input
 */
void shell_loop(shell_t *shell, FILE *input);

/*!
 * Process and execute a line
 *
 * @param shell Pointer to the shell to use
 * @param line  Input line to execute
 */
int shell_exec(shell_t *shell, const char *line);

/*!
 * Execute a shell command, argv-style
 *
 * @param shell Pointer to the shell to use
 * @param argv  Input line to execute, argv-style tokenized and NULL-terminated
 */
int shell_execv(shell_t *shell, char *argv[]);

/*!
 * Get the value of a shell flag
 *
 * @param sh Pointer to the reference shell
 * @param flag Flag identifier
 */
#define shell_flag_get(sh,flag) (((sh)->flags & (flag)) ? 1 : 0)

/*!
 * Set a shell flag (to 1)
 *
 * @param sh Pointer to the reference shell
 * @param flag Flag identifier
 */
#define shell_flag_set(sh,flag) do { (sh)->flags |= (flag); } while (0)

/*!
 * Clear a shell flag (i.e. set to 0)
 *
 * @param sh Pointer to the reference shell
 * @param flag Flag identifier
 */
#define shell_flag_clr(sh,flag) do { (sh)->flags &= ~(flag); } while (0)

/*!
 * Toggle a shell flag
 *
 * @param sh Pointer to the reference shell
 * @param flag Flag identifier
 */
#define shell_flag_tog(sh,flag) do { (sh)->flags ^= (flag); } while (0)


/*!
 * Exit with an error from a shell command
 *
 * @param err_ret Error value to return (must be a byte-sized integer)
 * @param err_fmt Printf-style format for the error message to print
 * \warning Use only within a shell command
 */
#define sh_error(err_ret, err_fmt, ...) do {\
    fprintf(stderr, "%s: ", argv[0]);\
    fprintf(stderr, err_fmt __VA_OPT__(,) __VA_ARGS__);\
    fputc('\n', stderr);\
    return err_ret;\
} while (0)

/*!
 * Exit with an error from a shell command if 'expr' evaluates to true
 *
 * @param expr Condition to evaluate
 * @param err_ret Error value to return (must be a byte-sized integer)
 * @param err_fmt Printf-style format for the error message to print
 * \warning Use only within a shell command
 */
#define sh_error_on(expr, err_ret, err_fmt, ...) do {\
  if (expr) {\
    fprintf(stderr, "%s: ", argv[0]);\
    fprintf(stderr, err_fmt __VA_OPT__(,) __VA_ARGS__);\
    fputc('\n', stderr);\
    return err_ret;\
  }\
} while (0)


/*!
 * Print informations about a given shell
 *
 * @param shell Pointer to the shell to print
 */
void shell_print(const shell_t *shell);

#endif  // __SHELL_MODULE_H
