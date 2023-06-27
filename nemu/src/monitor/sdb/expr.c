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

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

/* wuyc */
/* #include "local-include/reg.h" */
/* wuyc */

enum {
  TK_NOTYPE = 256, TK_EQ, TK_NUM, TK_HEX, TK_NEG,
  TK_REG,

  /* TODO: Add more token types */

};
/* wuyc */

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},        // equal
/* wuyc */
  {"\\b(0|[1-9][0-9]*)\\b", TK_NUM},
  {"\\b0x(0|[1-9a-fA-F][0-9a-fA-F]*)\\b", TK_HEX},
  {"\\$[0-9a-zA-Z$]+\\b", TK_REG},
  {"\\*", '*'},
  {"-", '-'},
  {"/", '/'},
  {"\\(", '('},
  {"\\)", ')'},
/* wuyc */

};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[65536] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

#define IS_UNARY(op_index) \
  (op_index == 0 || !(tokens[op_index-1].type == TK_NUM || tokens[op_index-1].type == ')'))

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

/* wuyc */
        switch (rules[i].token_type) {
          case TK_NOTYPE:
            break;
          case '+':
          case '-':
          case '*':
          case '/':
          case '(':
          case ')':
          case TK_EQ:
            /* if (is_unary(nr_token)) */
              /* tokens[nr_token].type = TK_NEG; */
            /* else */
              /* tokens[nr_token].type = '-'; */
            tokens[nr_token].type = rules[i].token_type;
            nr_token++;
            break;
          case TK_REG:
            substr_len--;
            substr_start++;
          /* if the token is TK_NUM, storge the SUBSTR. */
          case TK_NUM:
          case TK_HEX:
            if (substr_len >= 32)
            {
              printf(ANSI_FMT("The token of number is too long.\n", ANSI_FG_RED));
              return false;
            }
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            /* printf("%s\n", tokens[nr_token].str); */
            tokens[nr_token].str[substr_len] = '\0';
            tokens[nr_token].type = rules[i].token_type;
            nr_token++;
            break;
          default: //TODO();
            break;
        }
/* wuyc */

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}


/* wuyc */
static bool check_parentheses(int p, int q, bool *success)
{
  int level = 0;
  int tag = 0;
  if (tokens[p].type != '(' || tokens[q].type != ')')
    return false;
  for(int i = p; i <= q; i++)
  {
    if (tokens[i].type == '(') level++;
    else if (tokens[i].type == ')') level--;

    if (level == 0 && i != q)
      tag = 1;
    else if (level < 0)
    {
      *success = false;
      return false;
    }
  }

  if (tag == 0 && level == 0)
    return true;
  else if (level > 0)
    *success = false;

  return false;

}

enum { EQ_NE = 1, PL_MI, MU_DI, NEG};

static unsigned int priority(int p)
{
  switch(tokens[p].type)
  {
    case TK_EQ:
      return EQ_NE;
    case '+':
    case '-':
      return PL_MI;
    case '*':
    case '/':
      return MU_DI;
    case TK_NEG:
      return NEG;
  }
  return 0;

}

#define IS_VAL(i) (tokens[i].type != TK_NUM && tokens[i].type != TK_HEX && tokens[i].type != TK_REG)

static int dominant_operator(int p, int q)
{
  int level = 0;
  unsigned int pri = 0, cur_pri;
  int dom = -1;
  for(int i = p; i <= q; i++)
  {
    if (tokens[i].type == '(')
      level++;
    else if (tokens[i].type == ')')
      level--;
    else if (level == 0)
    {
      if (IS_VAL(i))
      {
        cur_pri = priority(i);
        /* printf("%d\n", cur_pri); */
        if (cur_pri == 0)
        {
          Log("Please set token[%d]'s priority", tokens[i].type);
          /* *success = false; */
          return -1;
        }
        if (pri == 0 || cur_pri <= pri)
        {
          pri = cur_pri;
          dom = i;
        }
      }
    }
  }
  return dom;
}

static word_t eval(int p, int q, bool *success)
{
  if (p > q)
  {
    /* Bad expression */
    *success = false;
    printf("Bad expression\n");
  }
  else if (p == q)
  {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
    int num = 0;
    if (*success == false)
      return 0;

    switch(tokens[p].type)
    {
      case TK_NUM:
        num = atoi(tokens[p].str);
        return num;
      case TK_HEX:
        sscanf(tokens[p].str, "%x", &num);
        return num;
      case TK_REG:
        num = isa_reg_str2val(tokens[p].str, success);
        if (*success == false)
        {
          printf("The register is wrong\n");
          return 0;
        }
        return num;
      default:
        *success = false;
    }
  }
  else if (check_parentheses(p, q, success) == true)
  {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1, success);
  }
  /* else if (tokens[p].type == TK_NEG && tokens[q].type == TK_NUM) */
  /* { */
  /*   return -eval(p+1, q, success); */
  /* } */
  else
  {
    /* If the CHECK_PARENTHESES is not successful, finish the EVAL. */
    if (*success == false)
    {
      /* Assert(*success, "a"); */
      return 0;
    }

    /* op = the position of major operator in the token expression; */
    int op = dominant_operator(p, q);
    /* printf("%d\n", op); */
    if (op == -1)
    {
      *success = false;
      printf("Can not find dominant operator\n");
      return 0;
    }
    if (tokens[op].type == TK_NEG)
    {
      return -eval(p+1, q, success);
    }
    int val1 = eval(p, op - 1, success);
    int val2 = eval(op + 1, q, success);
    switch (tokens[op].type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return val1 / val2;
      case TK_EQ: return val1 == val2;
      default: assert(0);
    }
  }
  return 0;
}
/* wuyc */

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    /* Assert(*success, "all"); */
    /* panic("a"); */
    return 0;
  }

  for (int i = 0; i < nr_token; i++)
  {
    /* distinguish the TK_NEG and '-'. */
    if (tokens[i].type == '-' && IS_UNARY(i))
      tokens[i].type = TK_NEG;
  }

  /* TODO: Insert codes to evaluate the expression. */
  /* TODO(); */
  /* for(int i = 0; i < nr_token; i++) */
  /*   printf("%d\n", tokens[i].type); */
  int result = eval(0, nr_token-1, success);
  if (*success == true)
    return result;
  else
    return 0;
}
