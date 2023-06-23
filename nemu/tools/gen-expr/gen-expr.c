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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
/* wuyc */
/* #include <errno.h> */
#define BUFLEN 65536
/* wuyc */

// this should be enough
static char buf[BUFLEN] = {};
static char code_buf[BUFLEN + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

/* wuyc */
#define ARRLEN(a) (sizeof(a) / sizeof(a[0]))
#define GEN_VALUE(format, value) \
  do { \
    int str_len = 0; \
    int rest_len = BUFLEN - buf_len; \
    str_len = snprintf(buf+buf_len, BUFLEN - buf_len, format, value); \
    if (str_len >= rest_len) { printf("buf = %s\n", buf); } \
    assert(str_len < rest_len); \
    buf_len += str_len; \
  } while(0) 

#define PROB 0.1

static int buf_len = 0;
static char *op_str[] = {"+", "-", "*", "/"};

static void gen_space();

static int choose(int i) {return rand() % i;}
static void gen_num()
{
  /* int str_len = 0; */
  /* str_len = snprintf(buf+buf_len, BUFLEN - buf_len, "%d", choose(65536)); */
  /* printf("buf_len = %d\n", buf_len); */
  GEN_VALUE("%d", choose(65536));
  gen_space();
}
void gen(char c)
{
  /* buf_len += sprintf(buf+buf_len, "%c", c); */
  GEN_VALUE("%c", c);
}
void gen_rand_op()
{
  int i = choose(ARRLEN(op_str));
  GEN_VALUE("%s", op_str[i]);
  gen_space();
  /* buf_len += sprintf(buf+buf_len, "%s", op_str[i]); */
}
static void gen_space()
{
  float p = rand() / (float)RAND_MAX;
  while(p < PROB) 
  {
    p = rand() / (float)RAND_MAX;
    GEN_VALUE("%c", ' ');
  }
}
/* wuyc */

static void gen_rand_expr() {
  /* gen_space(); */
  switch(choose(3))
  {
    case 0: gen_num(); break;
    case 1: gen('('); gen_rand_expr(); gen(')'); break;
    default: gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break;
  }
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    buf_len = 0;
    gen_rand_expr();
    /* printf("%d\n", choose(3)); */

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -Wno-overflow -Werror -Wall -o /tmp/.expr");
    printf("ret = %d\n", ret);
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
