#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read-str.h"
#include "biggie.h"

// biggie_read() reads in a string and creates a biggie from it
// note: returns NULL if unsuccessful or invalid integer representation
// effects: may allocate memory (caller must call biggie_destroy)
//          reads input
// time: O(n), where n is the length of the string read in
struct biggie *biggie_read(void) {
  char *s = read_str();
  if (s == NULL) {
    return NULL;
  }
  struct biggie *b = biggie_create(s);
  free(s);
  return b;
}


int main(void) {
  char *cmd = NULL;
  struct biggie *op1 = NULL;
  struct biggie *op2 = NULL;
  const char *bool_to_str[2] = {"false", "true"};
  while (1) {
    cmd = read_str();
    if (cmd == NULL || !strcmp(cmd, "quit")) {
      break;
    }
    op1 = biggie_read();
    op2 = biggie_read();
    if (op1 == NULL || op2 == NULL) {
      break;
    }
    printf("%s ", cmd);
    biggie_print(op1, false);
    printf(" ");
    biggie_print(op2, false);
    printf(" => ");
    if (!strcmp(cmd, "add")) {
      biggie_add(op1, op2);
      biggie_print(op1, true);
    } else if (!strcmp(cmd, "sub")) {
      biggie_sub(op1, op2);
      biggie_print(op1, true);
    } else if (!strcmp(cmd, "mult")) {
      biggie_mult(op1, op2);
      biggie_print(op1, true);
    } else if (!strcmp(cmd, "div")) {
      biggie_div(op1, op2);
      biggie_print(op1, true);
    } else if (!strcmp(cmd, "eq")) {
      printf("%s\n", bool_to_str[biggie_eq(op1, op2)]);
    } else if (!strcmp(cmd, "gt")) {
      printf("%s\n", bool_to_str[biggie_gt(op1, op2)]);
    } else {
      break;
    }
    free(cmd);
    biggie_destroy(op1);
    biggie_destroy(op2);
    cmd = NULL;
    op1 = NULL;
    op2 = NULL;
  }
  if (cmd) {
    free(cmd);
  }
  if (op1) {
    biggie_destroy(op1);
  }
  if (op2) {
    biggie_destroy(op2);
  }
}
