#ifndef __PRPRPRPR_H
#define __PRPRPRPR_H

enum {
  PR_VARIABLE = 0,
  PR_CONSTANT,
  PR_FUNCTION0 = 4,
  PR_FUNCTION1,
  PR_FUNCTION2,
  PR_FUNCTION3,
};

typedef struct __expression {
  int type;
  union {
    double value;
    const double *refer;
    const void *func;
  };
  void *arguments[1];
} pr_expr;

typedef struct __variable {
  const char *name;
  const void *address;
  int type;
} pr_var;

/**
 * Parse the expression into an AST.
 *
 * @expr        the expression string
 * @var_table   external variable table(we treat functions as variable)
 * @var_count   length of var_table
 * @return      the AST
 */
pr_expr *pr_parse(const char *expr, const pr_var *var_table, int var_count);

/**
 * Evaluate the AST.
 *
 * @expr    the AST.
 * @return  returns result if the AST exists else return NaN
 */
double pr_eval(const pr_expr *expr);

/**
 * Checks wether the procedure produced the error.
 *
 * @return returns 0 if no error occured
 */
int pr_error(void);

/**
 * Clear the error.
 */
void pr_clear_error(void);

/**
 * Get the error code.
 *
 * @return Returns the error code. When the error is
 * in expression, the error code points out where the
 * error was caught. Or we will return -1 representing
 * a unknown error. When we return -2, it means a
 * arithmetic error, usually was caused by a devision-by-zero.
 */
int pr_error_code(void);

/**
 * Get the error text.
 *
 * @return Returns a error text when an error was caught or
 * return "No error.". If the parser does't know the error type,
 * it will return "Be more careful." attempting to conceal the facts
 * that it knows nothing.
 */
const char *pr_error_text(void);

/**
 * Free the expression. Should be called when delete
 * a expression.
 */
void pr_free(pr_expr *n);
#endif  // __PRPRPRPR_H
