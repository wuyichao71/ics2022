#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

/* wuyc */
void __libc_init_array(void);
/* wuyc */

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  /* char *empty[] =  {NULL }; */
  /* environ = empty; */
  int *argc_p = (int *)args;
  int argc = *(int *)argc_p;
  char **argv = (char **)(argc_p + 1);
  char **envp = argv + argc + 1;
  environ = envp;
  /* wuyc */
  __libc_init_array();
  /* wuyc */
  exit(main(argc, argv, envp));
  assert(0);
}
