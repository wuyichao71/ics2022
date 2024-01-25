#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

/* wuyc */
void __libc_init_array(void);
/* wuyc */

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  char *empty[] =  {NULL };
  environ = empty;
  /* wuyc */
  __libc_init_array();
  /* wuyc */
  exit(main(0, empty, empty));
  assert(0);
}
