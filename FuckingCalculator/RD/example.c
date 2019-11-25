#include <stdio.h>
#include <string.h>
#include "exprpr.h"

#define READ_INPUT(string, terminal)                     \
  do {                                                   \
    char c;                                              \
    int i = 0;                                           \
    while ((c = getchar()) != terminal) string[i++] = c; \
    string[i] = '\0';                                    \
  } while (0)

void print_note(int n) {
  int i;
  for (i = 0; i < n - 3; i++) {
    putchar(' ');
  }
  for (; i < n - 1; i++) {
    putchar('~');
  }
  puts("^");
  return;
}

int main(void) {
  puts("A toy expression evaluator. Type exit= to quit.");
  int count = 0;
  char expression[1024];

  while (1) {
    printf(" In[%d]: ", count);
    READ_INPUT(expression, '=');
    pr_expr *ast = pr_parse(expression, 0, 0);
    double result = pr_eval(ast);
    if (pr_error()) {
      if (pr_error_code() > 0) {
        puts(expression);
        print_note(pr_error_code());
      }
      puts(pr_error_text());
    } else {
      printf("Out[%d]: %g\n", count, result);
    }
    pr_clear_error();
    pr_free(ast);
    count++;
  }
  return 0;
}
