#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>

char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
  sh_printf("sh> ");
}

static void sh_handle_cmd(const char *cmd) {
  char cmd_tmp[256];
  strcpy(cmd_tmp, cmd);
  char *cmd_0 = strtok(cmd_tmp, " \n");
  /* printf("%d\n", cmd_0); */
  /* printf("%d\n", cmd_0[0]); */
  if (cmd_0 == NULL)
    return;
  if (strcmp(cmd_0, "echo") == 0)
  {
    char *cmd_1 = strtok(NULL, "\n");
    if (cmd_1 == NULL)
      sh_printf("\n");
    else
      sh_printf("%s\n", cmd_1);
  }
  else
  {
    const char *exec_argv[16];
    exec_argv[0] = cmd_0;
    int argc =1;
    for (; (exec_argv[argc] = strtok(NULL, " \n")) != NULL; argc++);
    /* execve(cmd_0, (char **)exec_argv, NULL); */
    execvp(cmd_0, (char **)exec_argv);
  }
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();

  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}
