#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>

// Some fucking legency compilers don't support NAN and INFINITY,
// create it ourselves.
#ifndef NAN
#define NAN (0.0 / 0.0)
#endif

#ifndef INFINITY
#define INFINITY (1.0 / 0.0)
#endif

#include <ctype.h>
#include <errno.h>
#include <limits.h>

#include "exprpr.h"

typedef double (*pr_func2)(double, double);
#define ARGC(TYPE) ((((TYPE)&PR_FUNCTION0) != 0) ? (TYPE & 0x3) : 0)
#define FUNC(...) ((double (*)(__VA_ARGS__))expr->func)
#define EVAL()

//////////////////////////////////////////////////////////
// Error handling.                                      //
//////////////////////////////////////////////////////////
int __prprprpr_error = 0;
const char *__prprprpr_error_text = NULL;
#define ERR_UNKNOWN_FUNCTION "PARSER: unknown function"
#define ERR_UNKNOWN_SYMBOL "PARSER: unknown symbol"
#define ERR_ARGC_MISMATCH "PARSER: two few arguments"
#define ERR_DIVISION_BY_ZERO "Division by zero"
#define SET_ERR_TEXT(TEXT)                                           \
  do {                                                               \
    if (__prprprpr_error_text == NULL) __prprprpr_error_text = TEXT; \
  } while (0)

void pr_clear_error(void) { __prprprpr_error_text = NULL; }

int pr_error(void) { return __prprprpr_error != 0; }

int pr_error_code(void) { return __prprprpr_error; }

const char *pr_error_text(void) {
  if (__prprprpr_error == 0) return "No error.";
  if (__prprprpr_error_text == NULL) return "Be more careful.";
  return __prprprpr_error_text;
}

//////////////////////////////////////////////////////////
// Expressions.                                         //
//////////////////////////////////////////////////////////
#define NEW_EXPR(TYPE, ...) new_expr((TYPE), (const pr_expr *[]){__VA_ARGS__})

static pr_expr *new_expr(const int type, const pr_expr *arguments[]) {
  int argc = ARGC(type);
  int psize = sizeof(void *) * argc;
  int size = (sizeof(pr_expr) - sizeof(void *)) + psize;
  pr_expr *expr = malloc(size);
  memset(expr, 0, size);
  if (argc && arguments) {
    memcpy(expr->arguments, arguments, psize);
  }
  expr->type = type;
  expr->refer = 0;
  return expr;
}

void pr_free_arguments(pr_expr *expr) {
  if (expr == NULL) return;
  switch (expr->type) {
    case PR_FUNCTION3:
      pr_free(expr->arguments[2]);  // fall-through
    case PR_FUNCTION2:
      pr_free(expr->arguments[1]);  // fall-through
    case PR_FUNCTION1:
      pr_free(expr->arguments[0]);
  }
  return;
}

void pr_free(pr_expr *expr) {
  if (expr == NULL) return;
  pr_free_arguments(expr);
  free(expr);
  return;
}

//////////////////////////////////////////////////////////
// Tokens.                                              //
//////////////////////////////////////////////////////////
enum {
  TOK_NUL = 8,
  TOK_ERR,
  TOK_END,
  TOK_SEP,
  TOK_OPN,
  TOK_CLO,
  TOK_NUM,
  TOK_VAR,
  TOK_IFX
};

typedef struct __state {
  const char *start;
  const char *next;
  int type;
  union {
    double value;
    const double *refer;
    const void *func;
  };

  // User defined variable table
  const pr_var *var_table;
  int var_count;
} state;

static double pi(void) { return acos(-1.0); }

static double e(void) { return exp(1); }

static double __exit(void) {
  exit(0);
  return NAN;
}

static const pr_var __built_in_var_table[] = {
    {"abs", fabs, PR_FUNCTION1},    {"acos", acos, PR_FUNCTION1},
    {"asin", asin, PR_FUNCTION1},   {"atan", atan, PR_FUNCTION1},
    {"atan2", atan2, PR_FUNCTION1}, {"ceil", ceil, PR_FUNCTION1},
    {"cos", cos, PR_FUNCTION1},     {"cosh", cosh, PR_FUNCTION1},
    {"e", e, PR_FUNCTION0},         {"exit", __exit, PR_FUNCTION0},
    {"exp", exp, PR_FUNCTION1},     {"floor", floor, PR_FUNCTION1},
    {"lg", log10, PR_FUNCTION1},    {"ln", log, PR_FUNCTION1},
    {"log", log, PR_FUNCTION1},     {"pi", pi, PR_FUNCTION0},
    {"pow", pow, PR_FUNCTION2},     {"sin", sin, PR_FUNCTION1},
    {"sqrt", sqrt, PR_FUNCTION1},   {"tan", tan, PR_FUNCTION1},
};

// Naive linear search. Use binary search to get a better performance,
static const pr_var *get_builtin_token(const char *name, int len) {
  int i;
  const pr_var *var;

  for (var = __built_in_var_table,
      i = sizeof(__built_in_var_table) / sizeof(pr_var) - 1;
       i > 0; var++, i--) {
    if (strncmp(name, var->name, len) == 0 && var->name[len] == '\0') {
      return var;
    }
  }
  return 0;
}

static const pr_var *get_external_token(const state *s, const char *name,
                                        int len) {
  int i;
  const pr_var *var;
  if (s->var_table == NULL) return 0;

  for (var = s->var_table, i = s->var_count; i > 0; var++, i--) {
    if (strncmp(name, var->name, len) == 0 && var->name[len] == '\0') {
      return var;
    }
  }
  return 0;
}

// "Magic method"
static double __add__(double a, double b) { return a + b; }

static double __sub__(double a, double b) { return a - b; }

static double __mul__(double a, double b) { return a * b; }

static double __div__(double a, double b) {
  if (b == 0) {
    __prprprpr_error = -2;
    SET_ERR_TEXT(ERR_DIVISION_BY_ZERO);
  }
  return a / b;
}

static double __neg__(double a) { return -a; }

static double __comma__(double a, double b) { return b; }

//////////////////////////////////////////////////////////
// Tokenizer.                                           //
//////////////////////////////////////////////////////////
void next_token(state *s) {
  s->type = TOK_NUL;
  while (s->type == TOK_NUL) {
    if (s->next[0] == '\0') {
      s->type = TOK_END;
      return;
    }
    // Read a number
    if (isdigit(s->next[0]) || s->next[0] == '.') {
      s->value = strtod(s->next, (char **)&s->next);
      s->type = TOK_NUM;
    } else if (isalpha(s->next[0])) {
      const char *start;
      start = s->next;
      while (isalpha(s->next[0]) || isdigit(s->next[0]) || (s->next[0] == '_'))
        s->next++;
      const pr_var *temp = get_external_token(s, start, s->next - start);
      if (temp == NULL) temp = get_builtin_token(start, s->next - start);

      if (temp == NULL) {
        s->type = TOK_ERR;
        SET_ERR_TEXT(ERR_UNKNOWN_FUNCTION);
      } else {
        switch (temp->type) {
          case PR_VARIABLE:
            s->type = TOK_VAR;
            s->refer = (double *)temp->address;
            break;
          case PR_FUNCTION0:
          case PR_FUNCTION1:
          case PR_FUNCTION2:
          case PR_FUNCTION3:
            s->type = temp->type;
            s->func = temp->address;
            break;
        }
      }
    } else {
      switch (s->next[0]) {
        case ' ':
        case '\t':
        case '\n':
          s->type = TOK_NUL;
          break;
        case '+':
          s->type = TOK_IFX;
          s->func = __add__;
          break;
        case '-':
          s->type = TOK_IFX;
          s->func = __sub__;
          break;
        case '*':
          s->type = TOK_IFX;
          s->func = __mul__;
          break;
        case '/':
          s->type = TOK_IFX;
          s->func = __div__;
          break;
        case '^':
          s->type = TOK_IFX;
          s->func = pow;
          break;
        case '%':
          s->type = TOK_IFX;
          s->func = fmod;
          break;
        case '(':
          s->type = TOK_OPN;
          break;
        case ')':
          s->type = TOK_CLO;
          break;
        case ',':
          s->type = TOK_SEP;
          break;
        default:
          s->type = TOK_ERR;
          SET_ERR_TEXT(ERR_UNKNOWN_SYMBOL);
          break;
      }
      s->next++;
    }
  }
}

//////////////////////////////////////////////////////////
// Recusive decent parser.                              //
//////////////////////////////////////////////////////////
static pr_expr *val(state *s);
static pr_expr *po(state *s);
static pr_expr *fact(state *s);
static pr_expr *term(state *s);
static pr_expr *expr(state *s);

// args -> expr | expr,expr
// for handling function call
static pr_expr *args(state *s) {
  pr_expr *ret = expr(s);

  while (s->type == TOK_SEP) {
    puts("S -> E, E");
    next_token(s);
    ret = NEW_EXPR(PR_FUNCTION2, ret, expr(s));
    ret->func = __comma__;
  }
  puts("S -> E");
  return ret;
}

// expr -> term + term | term - term | term
static pr_expr *expr(state *s) {
  pr_expr *ret = term(s);

  while (s->type == TOK_IFX && (s->func == __add__ || s->func == __sub__)) {
    puts(s->func == __add__? "E -> T + T" : "E-> T - T");
    pr_func2 temp = s->func;
    next_token(s);
    ret = NEW_EXPR(PR_FUNCTION2, ret, term(s));
    ret->func = temp;
  }
  puts("E -> T");
  return ret;
}

// term -> fact * fact | fact / fact | fact % fact | fact
static pr_expr *term(state *s) {
  pr_expr *ret = fact(s);

  while (s->type == TOK_IFX &&
         (s->func == __mul__ || s->func == __div__ || s->func == fmod)) {
    if(s->func == __mul__)
        puts("T -> F * F");
    else if(s->func == __div__)
        puts("T -> F / F");
    else
        puts("T -> F % F");
    pr_func2 temp = s->func;
    next_token(s);
    ret = NEW_EXPR(PR_FUNCTION2, ret, fact(s));
    ret->func = temp;
  }
  puts("T -> F");
  return ret;
}

// fact -> po^po | po
static pr_expr *fact(state *s) {
  pr_expr *ret = po(s);

  while (s->type == TOK_IFX && s->func == pow) {
    puts("F -> P^P");
    pr_func2 temp = s->func;
    next_token(s);
    ret = NEW_EXPR(PR_FUNCTION2, ret, po(s));
    ret->func = temp;
  }
  puts("F -> P");
  return ret;
}

// po -> +val | -val | val
static pr_expr *po(state *s) {
  pr_expr *ret;

  int flag = 0;  // Meaning '+'
  while (s->type == TOK_IFX && (s->func == __add__ || s->func == __sub__)) {
    if (s->func == __sub__) {
        puts("P -> -V");
        flag = !flag;
    } else
        puts("P -> +V");
    next_token(s);
  }

  if (flag == 0) {
    ret = val(s);
  } else {
    ret = NEW_EXPR(PR_FUNCTION1, val(s));
    ret->func = __neg__;
  }
  puts("P -> V");
  return ret;
}

// the begin of everything
static pr_expr *val(state *s) {
  pr_expr *ret;
  int argc;

  switch (s->type) {
    case TOK_NUM:
      puts("V -> num");
      ret = new_expr(PR_CONSTANT, 0);
      ret->value = s->value;
      next_token(s);
      break;

    case TOK_VAR:
      puts("V -> var");
      ret = new_expr(PR_VARIABLE, 0);
      ret->refer = s->refer;
      next_token(s);
      break;

    case PR_FUNCTION0:
      puts("V -> FUNC(S)");
      ret = new_expr(s->type, 0);
      ret->func = s->func;
      next_token(s);
      if (s->type == TOK_OPN) {
        next_token(s);
        if (s->type != TOK_CLO) {
          s->type = TOK_ERR;
        } else {
          next_token(s);
        }
      }
      break;

    case PR_FUNCTION1:
      ret = new_expr(s->type, 0);
      ret->func = s->func;
      next_token(s);
      ret->arguments[0] = po(s);
      break;

    case PR_FUNCTION2:
    case PR_FUNCTION3:
      argc = ARGC(s->type);

      ret = new_expr(s->type, 0);
      ret->func = s->func;
      next_token(s);

      if (s->type != TOK_OPN) {
        s->type = TOK_ERR;
      } else {
        int i;
        for (i = 0; i < argc; i++) {
          next_token(s);
          ret->arguments[i] = expr(s);
          if (s->type != TOK_SEP) {
            break;
          }
        }
        if (s->type != TOK_CLO) {
          s->type = TOK_ERR;
        } else if (i != argc - 1) {
          s->type = TOK_ERR;
          SET_ERR_TEXT(ERR_ARGC_MISMATCH);
        } else {
          next_token(s);
        }
      }

      break;

    case TOK_OPN:
      puts("V -> (S)");
      next_token(s);
      ret = args(s);
      if (s->type != TOK_CLO) {
        s->type = TOK_ERR;
      } else {
        next_token(s);
      }
      break;

    default:
      ret = new_expr(0, 0);
      s->type = TOK_ERR;
      ret->value = NAN;
      break;
  }

  return ret;
}

pr_expr *pr_parse(const char *c_expr, const pr_var *var_table, int var_count) {
  state s;
  s.start = s.next = c_expr;
  s.var_table = var_table;
  s.var_count = var_count;

  next_token(&s);
  pr_expr *root = expr(&s);

  if (s.type != TOK_END) {
    pr_free(root);
    __prprprpr_error = (s.next - s.start);
    if (__prprprpr_error == 0) __prprprpr_error = -1;  // Error at position 0.
    return NULL;
  } else {
    __prprprpr_error = 0;
    return root;
  }
}

//////////////////////////////////////////////////////////
// Evaluator.                                           //
//////////////////////////////////////////////////////////
double __pr_eval(const pr_expr *expr) {
  if (expr == NULL) {
    return NAN;
  }

  switch (expr->type) {
    case PR_CONSTANT:
      return expr->value;
    case PR_VARIABLE:
      return *expr->refer;

    case PR_FUNCTION0:
    case PR_FUNCTION1:
    case PR_FUNCTION2:
    case PR_FUNCTION3:
      switch (ARGC(expr->type)) {
        case 0:
          return FUNC(void)();
        case 1:
          return FUNC(double)(pr_eval(expr->arguments[0]));
        case 2:
          return FUNC(double, double)(pr_eval(expr->arguments[0]),
                                      pr_eval(expr->arguments[1]));
        case 3:
          return FUNC(double, double, double)(pr_eval(expr->arguments[0]),
                                              pr_eval(expr->arguments[1]),
                                              pr_eval(expr->arguments[2]));
        default:
          return NAN;
      }
    default:
      return NAN;
  }
}

double pr_eval(const pr_expr *expr) {
  errno = 0;
  double ret = __pr_eval(expr);
  if (errno != 0) {
    __prprprpr_error = -2;
    SET_ERR_TEXT(strerror(errno));
  }
  return ret;
}
