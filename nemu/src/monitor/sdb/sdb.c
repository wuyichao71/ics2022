/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <cpu/cpu.h>
/* wuyc */
#include <memory/vaddr.h>
/* wuyc */
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

/* wuyc */
static int cmd_x(char *args)
{
  vaddr_t incr = sizeof(word_t);
  char *N_str = strtok(NULL, " ");
  char *vaddr_str = strtok(NULL, " ");
  if (N_str == NULL || vaddr_str == NULL)
  {
    printf("Missing arguments\n");
    return 0;
  }
  int N = atoi(N_str);
  vaddr_t vaddr;
  word_t inst;
  sscanf(vaddr_str, "%x", &vaddr);
  for(int i = 0; i < N; i++)
  {
    inst = vaddr_read(vaddr, incr);
    /* printf("%d\n", incr); */
    printf(FMT_WORD": "FMT_WORD"\n", vaddr, inst);
    vaddr += incr;
  }
  return 0;
}

static int cmd_info(char *args)
{
  char *arg = NULL;
  if (args == NULL)
  {
    printf("Missing subcommand\n");
    return 0;
  }
  arg = strtok(NULL, " ");
  if (strcmp(arg, "r") == 0)
  {
    isa_reg_display();
  }
  else if (strcmp(arg, "w") == 0)
  {

  }
  else
  {
    printf("Unknown subcommand '%s'\n", arg);
  }
  return 0;
}

static int cmd_si(char *args) {
  int N = 1;
  char *arg = strtok(NULL, " "); 
  if (arg != NULL)
    N = atoi(arg);
  cpu_exec(N);
  /* printf("%d\n", nemu_state.state); */
  return 0;
}

static int cmd_p(char *args) {
  if (args == NULL)
  {
    printf("Missing expression\n");
    return 0;
  }

  bool success_val = true;
  word_t result = expr(args, &success_val);
  if (success_val)
  {
    printf("%d\n", result);
  }
  else
  {
    printf("The expression is incorrect\n");
  }
  return 0;
}
/* wuyc */

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  /* wuyc */
  { "si", "Execution N step commands", cmd_si },
  { "info", "Generic command for showing things about the program being debugged.", cmd_info },
  { "x", "Print [N] memory.", cmd_x},
  { "p", "Evaluate the expression.", cmd_p},

  /* TODO: Add more commands */
  /* wuyc */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

/* wuyc */
#define RST_LEN 128
#define BUF_LEN 65536
#define STR_LEN (BUF_LEN + RST_LEN)
#define CHK_NAME "./tools/gen-expr/build/input"
void check_expr()
{
  return;
  char str[STR_LEN];
  FILE *fp = fopen(CHK_NAME, "r");
  char *result_p, *expr_p;
  int result, expr_result;
  bool success_val = true;
  if (!fp) {Assert(0, "Something wrong when open input file\n");}

  int i = 1;
  while(fgets(str, STR_LEN, fp) != NULL)
  {
    str[strlen(str)-1] = '\0';
    result_p = strtok(str, " ");
    result = atoi(result_p);
    expr_p = str + strlen(result_p) + 1;
    expr_result = expr(expr_p, &success_val);
    if (success_val == false)
      printf("incorrect\n");
    if (result != expr_result)
      Assert(0, "expr[%d] make difference[%d, %d]\n", i, result, expr_result);
    i++;
  }
}
/* wuyc */

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }
  /* wuyc */
  /* check_expr(); */
  /* wuyc */

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
