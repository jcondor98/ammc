// AVR Multi Motor Control
/*! \file client/source/shell.c
 * Shell module - Source file
 *
 * \author Paolo Lucchesi
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "shell.h"
#include "debug.h"


// Maximum length for a shell input line
#define SHELL_LINE_MAX_LEN 2046

// Shell default prompt
static const char *default_prompt = "> ";

// [AUX] Comparator for commands name
static int cmdcmp(const void *c1, const void *c2);

// [AUX] Retrieve a shell command given its name
static shell_command_t *shell_command_get(const shell_t *shell,
    const char *name, unsigned char type);

// Shell built-in commands
static shell_command_t _shell_builtins[];
static size_t _shell_builtins_count;
static unsigned char _shell_builtins_sorted;


// Create a new shell object, given a prompt and and a command array
// Returns a pointer to the new shell object on success, NULL otherwise
// The function will recreate a new commands array, so the one passed can be
// deallocated without any unwanted effect. Same thing will be done for 'prompt'
shell_t *shell_new(const char *prompt, const shell_command_t *commands,
    size_t commands_count, void *storage) {
  if (!commands || !commands_count)
    return NULL;
  shell_t *shell;

  do { // Break on error
    // Create a new shell memory object
    if (! (shell = malloc(sizeof(shell_t))))
      break;
    *shell = (shell_t) { 0 };

    // Allocate the commands and builtins array
    shell->commands = malloc(commands_count * sizeof(shell_command_t));
    if (!shell->commands) break;

    // Take care of the command prompt
    shell->prompt = strdup(prompt ? prompt : default_prompt);
    if (!shell->prompt) break;

    shell->commands_count = commands_count;
    shell->builtins_count = _shell_builtins_count;
    shell->storage = storage;
    shell->flags = 0;
    shell->command_ops.compare = cmdcmp;
    shell->command_ops.get = shell_command_get;

    // Sort the commands array
    memcpy(shell->commands, commands, commands_count * sizeof(shell_command_t));
    qsort(shell->commands, commands_count, sizeof(shell_command_t),
        shell->command_ops.compare);

    // Assign and, if required, sort the builtins array
    shell->builtins = _shell_builtins;
    if (!_shell_builtins_sorted) {
      qsort(shell->builtins, shell->builtins_count, sizeof(shell_command_t),
          shell->command_ops.compare);
      _shell_builtins_sorted = 1;
    }

    return shell;
  } while (0);

  // Error handler
  if (shell) {
    if (shell->prompt)   free(shell->prompt);
    if (shell->commands) free(shell->commands);
    free(shell);
  }

  return NULL;
}


void shell_delete(shell_t *shell) {
  if (!shell) return;
  free(shell->commands);
  free(shell->prompt);
  free(shell);
}


// Execute a fully parsed input line
static int _execute(shell_t *shell, int argc, char *argv[]) {
  shell_command_t *cmd;
  int ret;

  if ((cmd = shell->command_ops.get(shell, argv[0], CMD_TYPE_BUILTIN)) != NULL)
    ret = cmd->exec(argc, argv, shell);
  else if ((cmd = shell->command_ops.get(shell, argv[0], CMD_TYPE_EXTERNAL)) != NULL)
    ret = cmd->exec(argc, argv, shell->storage);
  else { // Command does not exist
    printf("Command not found: %s\n", argv[0]);
    return 127; // Behave like POSIX shell
  }

  if (ret == 1 && cmd->help) puts(cmd->help);  // i.e. command syntax error
  return ret;
}


static inline void tokenize_line(char *line, int *argc, char *argv[]) {
  *argc = 0;
  argv[0] = strtok(line, " \t\n");
  if (!argv[0])
    return;
  for (*argc = 1; (argv[*argc] = strtok(NULL, " \t\n")) != NULL; ++(*argc))
    ;
  argv[*argc] = NULL;
}

// Process an input line
// Returns the code returned by the executed command, or 127 if the command
// does not exist
static int _shell_exec(shell_t *shell, char *line) {
  if (!shell->command_ops.get) return 0xFF;

  int argc;
  char *argv[SHELL_LINE_MAX_LEN / 2 + 1];
  tokenize_line(line, &argc, argv);

  if (argc == 0)
    return 0;
  return _execute(shell, argc, argv);
}


int shell_exec(shell_t *shell, const char *line) {
  if (!line) return 0;

  char _line[SHELL_LINE_MAX_LEN];
  strncpy(_line, line, SHELL_LINE_MAX_LEN);
  _line[SHELL_LINE_MAX_LEN-1] = '\0';

  return _shell_exec(shell, _line);
}


// Execute a shell command, argv-style
int shell_execv(shell_t *shell, char *argv[]) {
  if (!shell || !argv) return 0xFF;
  int argc = 0;
  while (argv[argc])
    ++argc;
  return _execute(shell, argc, argv);
}


// Print the shell prompt
static inline void _prompt(const shell_t *shell) {
  if (!shell_flag_get(shell, SH_SCRIPT_MODE))
    fputs(shell->prompt, stdout);
}

// Launch a shell - Blocks until the user exits
void shell_loop(shell_t *shell, FILE *input) {
  if (!shell || !input) return;

  // Input line entered by the user
  char line[SHELL_LINE_MAX_LEN + 2]; // Consider newline plus null terminator

  // Main shell loop
  _prompt(shell); // Print first prompt
  while (1) {
    // Get a new input line
    if (!fgets(line, SHELL_LINE_MAX_LEN + 2, input)) { // No line was read
      if (feof(input))
        break;
      else if (ferror(input) && errno != EINTR)
        perror(__func__);
      continue;
    }

    // Attempt to execute command
    int ret = _shell_exec(shell, line);
    debug if (ret != 0) printf("Command returned error code %d\n", ret);
    if (ret && shell_flag_get(shell, SH_EXIT_ON_ERR))
      break;

    // Catch eventual exit signal
    if (shell_flag_get(shell, SH_SIG_EXIT))
      break;

    _prompt(shell); // Print the shell prompt again for the next input line
  }

  // User exited, print the exit message and return
  if (!shell_flag_get(shell, SH_SCRIPT_MODE))
    puts("\nGood bye!");
}



// [AUX] Comparator for commands name to sort the commands array of the shell
static int cmdcmp(const void *c1, const void *c2) {
  if (!c1 && !c2) return 0;
  if (!c1) return -1;
  if (!c2) return  1;
  const shell_command_t *_c1 = c1, *_c2 = c2;
  return strcmp(_c1->name, _c2->name);
}


// [AUX] Retrieve a shell command given its name
static shell_command_t *shell_command_get(const shell_t *shell,
    const char *name, unsigned char type) {
  // Bad cast, I know, but 'name' is not going to be touched
  const shell_command_t cmd_dummy = { .name = (char*) name };

  switch (type) {
    case CMD_TYPE_ALL:
      while (++type < CMD_TYPE_NONE) {
        shell_command_t *cmd = shell_command_get(shell, name, type);
        if (cmd) return cmd;
      }
      return NULL;

    case CMD_TYPE_BUILTIN:
      return bsearch(&cmd_dummy, shell->builtins, shell->builtins_count,
          sizeof(shell_command_t), shell->command_ops.compare);

    case CMD_TYPE_EXTERNAL:
      return bsearch(&cmd_dummy, shell->commands, shell->commands_count,
          sizeof(shell_command_t), shell->command_ops.compare);

    default: return NULL; // Unknown type
  }
}


// Print informations about a given shell
void shell_print(const shell_t *shell) {
  puts("\nPrinting shell");
  if (!shell) {
    printf("Shell pointer is NULL");
    return;
  }

  printf("Prompt: %s\n", shell->prompt);
  printf("Storage: %s\n", shell->storage ? "Present" : "Absent");
  printf("Commands number: %zu\n", shell->commands_count);

  if (shell->commands_count) {
    puts("Commands:\n");
    for (size_t i=0; i < shell->commands_count; ++i)
      printf("  - %s\n", shell->commands[i].name);
  }

  putchar('\n');
}



// Built-in shell commands

static inline void _print_command_list(const shell_t *sh) {
  printf("\nBuilt-in commands: %zu\n", sh->builtins_count);
  for (size_t i=0; i < sh->builtins_count; ++i)
    printf(" - %s\n", sh->builtins[i].name);
  printf("\nExternal commands: %zu\n", sh->commands_count);
  for (size_t i=0; i < sh->commands_count; ++i)
    printf(" - %s\n", sh->commands[i].name);
  putchar('\n');
}

static inline void _print_command_specific_help(const shell_command_t *cmd) {
  if (cmd->help) puts(cmd->help);
  else printf("No help for command %s\n", cmd->name);
}

// CMD: help
// Usage: help [command]
// Display the entire command set, or a description of a specific command
int _builtin_help(int argc, char *argv[], void *env) {
  shell_t *sh = env;

  if (argc == 1) _print_command_list(sh);
  else if (argc == 2) {
    shell_command_t *cmd = sh->command_ops.get(sh, argv[1], CMD_TYPE_ALL);
    if (cmd)
      _print_command_specific_help(cmd);
    else {
      printf("Unknown command: %s\n", argv[1]);
      return 2;
    }
  }
  else return 1; // Unaccepted syntax
  return 0; // Success
}


// CMD: exit
// Exit from the shell
int _builtin_exit(int argc, char *argv[], void *env) {
  shell_t *sh = env;
  if (argc != 1)
    return 1;
  shell_flag_set(sh, SH_SIG_EXIT);
  return 0;
}


// CMD: echo
// Usage: echo [arg1 arg2 ...]
// Print back the arguments given
int _builtin_echo(int argc, char *argv[], void *env) {
  for (int i=1; i < argc; ++i) {
    fputs(argv[i], stdout);
    putchar(i == argc-1 ? '\n' : ' ');
  }
  return 0;
}


// Set of the builtin shell commands
static shell_command_t _shell_builtins[] = {
  (shell_command_t) { // CMD: echo
    .name = "echo",
    .help = "Usage: echo [arg1 arg2 ...]\n"
      "Print back the arguments given",
    .exec = _builtin_echo
  },

  (shell_command_t) { // CMD: help
    .name = "help",
    .help = "Usage: help [command]\n"
      "Display the entire command set, or a description of a specific command",
    .exec = _builtin_help
  },

  (shell_command_t) { // CMD: exit
    .name = "exit",
    .help = "Exit from the shell",
    .exec = _builtin_exit
  }
};

static size_t _shell_builtins_count = sizeof(_shell_builtins) / sizeof(shell_command_t);
