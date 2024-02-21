#include <proc.h>
/* wuyc */
#include <loader.h>
/* wuyc */

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;
static PCB *fg_pcb = NULL;
static int fg_pcb_index = -1;

/* wuyc */
#define ARRLEN(a) (sizeof(a) / sizeof(a[0]))
typedef struct {
  const char *filename;
  char **argv;
  char **envp;
} task_t;

#define CMD(f) f(hello, 1) f(bird, 1000) f(nslider, 1000) f(pal, 1000, "--skip")
#define DEFINE_ARGV(name, logtime, ...) static char *name##_argv[] = {"/bin/"#name, ##__VA_ARGS__, NULL};
#define DEFINE_TASK(name, logtime, ...) {.filename = "/bin/"#name, .argv = name##_argv, .envp = NULL},
#define DEFINE_LOGTIME(name, logtime, ...) logtime,

CMD(DEFINE_ARGV)
/* DEFINE_ARGV(hello) */
/* static char *hello_argv[] = {"/bin/hello", NULL}; */
/* static char *dummy_argv[] = {"/bin/dummy", NULL}; */
/* static char *nterm_argv[] = {"/bin/nterm", NULL}; */
/* static char *nterm_argv[] = {"/bin/bird", NULL}; */
/* static char *nslider_argv[] = {"/bin/nslider", NULL}; */
/* static char *pal_argv[] = {"/bin/pal", "--skip", NULL}; */
/* static char *menu_argv[] = {"/bin/menu", NULL}; */
static task_t utask_table[] = {
  CMD(DEFINE_TASK)
  /* {.filename = "/bin/dummy", .argv = dummy_argv, .envp = NULL}, */
  /* {.filename = "/bin/hello", .argv = hello_argv, .envp = NULL}, */
  /* {.filename = "/bin/bird", .argv = hello_argv, .envp = NULL}, */
  /* {.filename = "/bin/dummy", .argv = dummy_argv, .envp = NULL}, */
  /* {.filename = "/bin/pal",   .argv = pal_argv,   .envp = NULL}, */
  /* {.filename = "/bin/hello", .argv = hello_argv, .envp = NULL}, */
  /* {.filename = "/bin/nterm", .argv = nterm_argv, .envp = NULL}, */
  /* {.filename = "/bin/menu", .argv = menu_argv, .envp = NULL}, */
};
static int utask_log_time[] = {
  CMD(DEFINE_LOGTIME)
};
/* wuyc */

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    /* if (j % 1000 == 0) */
      Log("Hello World from Nanos-lite with arg '%s' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
  }
}

void init_proc() {
  /* context_kload(&pcb[1], hello_fun, "pbc 1"); */
  /* context_uload(&pcb[0], "/bin/hello"); */
  /* char *argv[3] = {NULL}; */
  /* char *envp[2] = {NULL}; */
  /* argv[0] = "/bin/nterm"; */
  /* argv[0] = "/bin/hello"; */
  /* argv[0] = "/bin/dummy"; */
  /* argv[0] = "/bin/pal"; */
  /* argv[1] = "--skip"; */
  /* envp[0] = "PATH=/bin"; */
  /* printf("ARRLEN(utask_table) = %d\n", ARRLEN(utask_table)); */
  for (int i = 0; i < MAX_NR_PROC; i++)
    context_uload(&pcb[i], utask_table[i].filename, utask_table[i].argv, utask_table[i].envp);
  /* context_uload(&pcb[0], argv[0], argv, envp); */
  /* context_kload(&pcb[1], hello_fun, "pcb 0"); */
  /* context_kload(&pcb[1], hello_fun, "pbc 1"); */
  fg_pcb_index = 1;
  fg_pcb = &pcb[fg_pcb_index];
  switch_boot_pcb();
  /* printf("pcb_boot.as.ptr = 0x%08x\n", pcb_boot.as.ptr); */
  /* printf("pcb[0].as.ptr = 0x%08x\n", pcb[0].as.ptr); */
  /* printf("pcb[1].as.ptr = 0x%08x\n", pcb[1].as.ptr); */

  Log("Initializing processes...");
  /* wuyc */
  /* naive_uload(NULL, "/bin/file-test"); */
  /* naive_uload(NULL, "/bin/timer-test"); */
  /* naive_uload(NULL, "/bin/event-test"); */
  /* naive_uload(NULL, "/bin/bmp-test"); */
  /* naive_uload(NULL, "/bin/nslider"); */
  /* naive_uload(NULL, "/bin/menu"); */
  /* naive_uload(NULL, "/bin/nterm"); */
  /* naive_uload(NULL, "/bin/typing-game"); */
  /* naive_uload(NULL, "/bin/dhrystone"); */
  /* naive_uload(NULL, "/bin/nplayer"); */
  /* naive_uload(NULL, "/bin/cpp-test"); */
  /* naive_uload(NULL, "/bin/pal"); */
  /* naive_uload(NULL, "/bin/coremark"); */
  /* naive_uload(NULL, "/bin/hello"); */
  /* wuyc */

  // load program here

}

Context* schedule(Context *prev) {
  int log_time[] = {utask_log_time[0], utask_log_time[fg_pcb_index]};
  static int time = 1;
  static int index = -1;
  current->cp = prev;
  if (current == &pcb_boot)
  {
    index = 0;
  }
  else if (time < log_time[index])
  {
    time++;
  }
  else
  {
    time = 1;
    index = (index + 1) % ARRLEN(log_time);
  }
  current = &pcb[index];
  /* current = &pcb[0]; */
  /* if (time < log_time[index]) */
  /* { */
  /*   time++; */
  /* } */
  /* else */
  /* { */
  /*   time = 0; */
  /*   index = (index + 1) % 2; */
  /* } */

  /* current = (current == &pcb[0] ? fg_pcb : &pcb[0]); */
  /* printf("mepc = 0x%08x\n", current->cp->mepc); */
  /* printf("mcause = 0x%08x\n", current->cp->mcause); */
  return current->cp;
  /* return NULL; */
}

void set_fg_pcb(int index)
{
  assert(index >= 0 && index < MAX_NR_PROC);
  fg_pcb_index = index;
  fg_pcb = &pcb[fg_pcb_index];
}
