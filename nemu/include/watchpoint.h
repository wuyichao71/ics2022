#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

/* #include <common.h> */

#define NR_WP 32
#define EXPR_LEN 1024

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  /* wuyc */
  char expr[EXPR_LEN];
  /* bool used; */
  word_t old_val;
  /* wuyc */

  /* TODO: Add more members if necessary */

} WP;

void add_wp(char *expr);
void del_wp(int no);
int get_head_no();
char *get_head_expr();
void watchpoints_display();
#endif
