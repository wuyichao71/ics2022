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

#include "sdb.h"

/* wuyc */
#include <watchpoint.h>
/* wuyc */


static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
/* wuyc */
WP *new_wp()
{
  WP *new;

  if (free_ == NULL)
    assert(0);

  new = free_;
  free_ = new->next;
  return new;
}

void free_wp(WP *wp)
{
  wp->next = free_;
  free_ = wp;
}

WP *add_wp(char *expr_str)
{
  bool success_val = true;
  WP *new = new_wp();
  new->next = head;
  head = new;
  strcpy(new->expr, expr_str);
  /* new->used = false; */
  new->old_val = expr(expr_str, &success_val);
  if (!success_val) 
  {
    head = new->next;
    free_wp(new);
    return NULL;
  }
  return new;
}

/* int get_head_no() {return head->NO;} */
/* char *get_head_expr() {return head->expr;} */

void del_wp(int no)
{
  WP *p = NULL;
  WP *q = head;

  while(q != NULL && q->NO != no)
  {
    p = q; q = p->next;
  }

  /* if the WP is head */
  if (q != NULL && p == NULL)
  {
    p = q->next;
    free_wp(q);
    head = p;
  }
  /* if the WP is not head */
  else if (q != NULL && p != NULL)
  {
    p->next = q->next;
    free_wp(q);
  }
  /* not found */
  else
    printf("No watchpint number %d.\n", no);

}

void watchpoints_display()
{
  WP *p = head;
  if (p == NULL)
    printf("No watchpoints.\n");
  else
  {
    printf("%-8s%s\n", "Num", "What");
    while (p != NULL)
    {
      printf("%-8d%s\n", p->NO, p->expr);
      p = p->next;
    }
  }

}

bool check_watchpoint(vaddr_t pc)
{
  WP *p = head;
  word_t val;
  bool changed = false, success_val = true;
  while (p != NULL)
  {
    val = expr(p->expr, &success_val);
    if (val != p->old_val)
    {
      if (!changed)
      {
        printf(FMT_WORD "\n", pc);
        changed = true;
      }
      printf("Watchpoint %d: %s\n", p->NO, p->expr);
      printf("Old value = " FMT_WORD "\n", p->old_val);
      printf("New value = " FMT_WORD "\n", val);
      putchar('\n');
      p->old_val = val;
    }
    p = p->next;
  }
  return changed;
}

/* wuyc */
