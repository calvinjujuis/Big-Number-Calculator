#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "biggie.h"

/////////////////////////////////////////////////////////////////////////////
// DO NOT CHANGE THIS STRUCTURE
/////////////////////////////////////////////////////////////////////////////
struct biggie {
  bool negative;
  char *digits;
};
// notes: digits is a properly null-terminated dynamic string
//        digits contains the number reversed, so 1203 => "3021"
//        digits cannot have leading zeros, so 1203 cannot be "30210"
//        zero is stored as "0" and cannot be negative (negative is false)
/////////////////////////////////////////////////////////////////////////////

struct biggie *biggie_create(const char *s) {
  assert(s);
  struct biggie *new = malloc(sizeof(struct biggie));
  new->negative = false;
  int len = strlen(s);
  if (len == 0) {
    // if s is empty
    free(new);
    return NULL;
  }
  if ((s[0] == '0') && (len > 1)) {
    // if s has leading zero
    free(new);
    return NULL;
  }
  // set a flag NEGATIVE to tell if s is negative in order to skip 
  //   the "-" sign while looping
  int NEGATIVE = 0;
  if (s[0] == '-') {
    if ((len == 1) || (s[1] == '0')){
      // if s only has a "-" sign or has "-0"
      free(new);
      return NULL;
    } else {
      new->negative = true;
      NEGATIVE = 1;
    }
  }
  int new_len = len - NEGATIVE;
  // negative sign isn't allocated
  new->digits = malloc((new_len + 1) * sizeof(char));
  for (int i = NEGATIVE; i < len; ++i) {
    if ((s[i] >= '0') && (s[i] <= '9')) {
      new->digits[new_len - 1] = s[i];
      --new_len;
    } else {
      biggie_destroy(new);
      return NULL;
    }
  }
  new->digits[len - NEGATIVE] = '\0';
  return new;
}

void biggie_destroy(struct biggie *big) {
  assert(big);
  free(big->digits);
  free(big);
}

void biggie_print(const struct biggie *big, bool newline) {
  assert(big);
  if (big->negative) {
    printf("-");
  }
  int len = strlen(big->digits);
  for (int i = len - 1; i >= 0; --i) {
    printf("%c", big->digits[i]);
  }
  if (newline) {
    printf("\n");
  }      
}

// delete_leading_zero(digits) deletes leading zeros in *digits
// effects: may modify *digits
// time: O(n) where n is the length of digits ( strlen(digits) )
static char *delete_leading_zero(char *digits) {
  int len = strlen(digits);
  int zero_count = 0;
  for (int i = len - 1; i > 0; --i) {
    if (digits[i] == '0') {
      ++zero_count;
    } else {
      break;
    }
  }
  digits = realloc(digits, 
                   (len - zero_count + 1) * sizeof(char));
  digits[len - zero_count] = '\0';
  return digits;
}

// plus_or_minus(n, m, operation, compare) returns the product of n
//   and m depending on operation.
// effects: may modify n
// time: O(log(n)) where n is the max of strlen(n->digits) and strlen(m->digits)
static void plus_or_minus(struct biggie *n, const struct biggie *m,
                          char operation, int compare) {
  int n_len = strlen(n->digits);
  int m_len = strlen(m->digits);
  int len = 0;
  if (n_len < m_len) {
    len = m_len;
  } else {
    len = n_len;
  }
  n->digits = realloc(n->digits, (len + 1) * sizeof(char));
  int digit = 0;
  int extra = 0;
  if (operation == '+') {
    for (int i = 0; i < len; ++i) {
      digit = 0;
      if (i >= n_len) {
        digit = m->digits[i] - '0';
      } else if (i >= m_len) {
        digit = n->digits[i] - '0';
      } else {
        digit = n->digits[i] - '0' + m->digits[i] - '0';
      }
      digit += extra;
      if (digit >= 10) {
        extra = 1;
        digit -= 10;
      } else {
        extra = 0;
      }
      digit += '0';
      n->digits[i] = digit;
    }
    if (extra > 0) {
      n->digits = realloc(n->digits, (len + 2) * sizeof(char));
      n->digits[len] = extra + '0';
      n->digits[len + 1] = '\0';
    } else {
      n->digits[len] = '\0';
    }
  } else if (operation == '-') {
    if (compare == 1) {
      for (int i = 0; i < len; ++i) {
        if (i >= m_len) {
          digit = n->digits[i] - '0';
        } else {
          digit = n->digits[i] - m->digits[i];
        }
        digit += extra;
        if (digit < 0) {
          extra = -1;
          digit += 10;
        } else {
          extra = 0;
        }
        digit += '0';
        n->digits[i] = digit;
      }
    } else if (compare == -1) {
      for (int i = 0; i < len; ++i) {
        if (i >= n_len) {
          digit = m->digits[i] - '0';
        } else {
          digit = m->digits[i] - n->digits[i];
        }
        digit += extra;
        if (digit < 0) {
          extra = -1;
          digit += 10;
        } else {
          extra = 0;
        }
        digit += '0';
        n->digits[i] = digit;
      }
    }
    n->digits[len] = '\0';
    n->digits = delete_leading_zero(n->digits);
  }
}

// compare(n, m) returns 1 if n->digits > m->digits and returns -1 otherwise
// time: O(n) where is the min of strlen(n->digits) and strlen(m->digits)
static int compare(const char *n, const char *m) {
  int n_len = strlen(n);
  int m_len = strlen(m);
  if (n_len != m_len) {
    if (n_len > m_len) {
      return 1;
    } else {
      return -1;
    }
  } else {
    for (int i = n_len - 1; i >= 0; --i) {
      if (n[i] > m[i]) {
        return 1;
      } else if (n[i] < m[i]){
        return -1;
      }
    }
  }
  return -1;
}

void biggie_add(struct biggie *n, const struct biggie *m) {
  assert(n);
  assert(m);
  int cmp = compare(n->digits, m->digits);
  if (n->negative == m->negative) {
    plus_or_minus(n, m, '+', cmp);
  } else {
    plus_or_minus(n, m, '-', cmp);
    if (cmp == -1) {
      if (n->negative) {
        n->negative = false;
      } else {
        n->negative = true;
      }
    }
  }
  if (! strcmp(n->digits, "0")) {
    n->negative = false;
  }
}

void biggie_sub(struct biggie *n, const struct biggie *m) {
  assert(n);
  assert(m);
  int cmp = compare(n->digits, m->digits);
  if (n->negative != m->negative) {
    plus_or_minus(n, m, '+', cmp);
  } else {
    plus_or_minus(n, m, '-', cmp);
    if (cmp == -1) {
      if (n->negative) {
        n->negative = false;
      } else {
        n->negative = true;
      }
    }
  }
  if (! strcmp(n->digits, "0")) {
    n->negative = false;
  }
}

void biggie_mult(struct biggie *n, const struct biggie *m) {
  assert(n);
  assert(m);
  int m_len = strlen(m->digits);
  struct biggie *n_copy = biggie_copy(n);
  struct biggie *m_copy = biggie_copy(m);
  // set signs to positive for calculation's convenience
  n_copy->negative = false;
  m_copy->negative = false;
  // sum stores and adds up every level of calculation
  struct biggie *sum = biggie_create("0");
  for (int i = 0; i < m_len; ++i) {
    struct biggie *product = biggie_create("0");
    for (int count = m_copy->digits[i] - '0'; count > 0; --count) {
      biggie_add(product, n_copy);
    }
    char* digits_copy = malloc((strlen(product->digits) + 1) 
                               * sizeof(char));
    strcpy(digits_copy, product->digits);
    product->digits = realloc(product->digits, 
                              (strlen(product->digits) + i + 1) *
                              sizeof(char));
    for (int j = 0; j < i; ++j) {
      // "leading" zeros on the front
      product->digits[j] = '0';
    } 
    strcpy(product->digits + i, digits_copy);
    biggie_add(sum, product);
    free(digits_copy);
    biggie_destroy(product);
  }
  biggie_destroy(n_copy);
  biggie_destroy(m_copy);
  if (n->negative == m->negative) {
    // positive if same sign
    n->negative = false;
  } else {
    n->negative = true;
  }
  n->digits = realloc(n->digits, (strlen(sum->digits) + 1)
                      * sizeof(char));
  strcpy(n->digits, sum->digits);
  biggie_destroy(sum);
  n->digits = delete_leading_zero(n->digits);
  if (! strcmp(n->digits, "0")) {
    n->negative = false;
  }
}


bool biggie_eq(const struct biggie *n, const struct biggie *m) {
  assert(n);
  assert(m);
  if (n->negative != m->negative) {
    return false;
  }
  struct biggie *copy = biggie_copy(n);
  biggie_sub(copy, m);
  if ((copy->digits[0] == '0') && (strlen(copy->digits) == 1)) {
    biggie_destroy(copy);
    return true;
  }
  biggie_destroy(copy);
  return false;
}

bool biggie_gt(const struct biggie *n, const struct biggie *m) {
  assert(n);
  assert(m);
  if (n->negative != m->negative) {
    if (n->negative) {
      return false;
    } else {
      return true;
    }
  } else {
    struct biggie *copy = biggie_copy(n);
    biggie_sub(copy, m);
    // check if diff is positive and greater than zero
    if (copy->negative) {
      // if (a - b) is negative, a < b
      biggie_destroy(copy);
      return false;
    } else {
      if ((strlen(copy->digits) == 1) && 
          (copy->digits[0] == '0')) {
        // single digit cannot be zero
        biggie_destroy(copy);
        return false;
      } else {
        biggie_destroy(copy);
        return true;
      }
      return false;
    }
  }
}

struct biggie *biggie_copy(const struct biggie *n) {
  assert(n);
  struct biggie *new = malloc(sizeof(struct biggie));
  new->negative = n->negative;
  int len = strlen(n->digits);
  new->digits = malloc((len + 1) * sizeof(char));
  strcpy(new->digits, n->digits);
  return new;
}

void biggie_div(struct biggie *n, const struct biggie *m) {
  assert(n);
  assert(m);
  struct biggie *zero = biggie_create("0");
  // m cannot equal to zero
  assert(! biggie_eq(m, zero));
  biggie_destroy(zero);
  int NEGATIVE = 0;
  if (n->negative == m->negative) {
    // positive if same sign
    NEGATIVE = 0;
  } else {
    NEGATIVE = 1;
  }
  n->negative = false;
  struct biggie *m_copy = biggie_copy(m);
  m_copy->negative = false;
  int current_digits = 1;
  struct biggie *current = biggie_create("0");
  for (int i = strlen(n->digits) - 1; i >= 0; --i) {
    char digit = '0';
    if (strcmp(current->digits, "0")) {
      // if current is not "0", move all digits backward to 
      //   create space for the new digit
      char *digits_copy = malloc((current_digits + 1) 
                                 * sizeof(char));
      strcpy(digits_copy, current->digits);
      ++current_digits;
      current->digits = realloc(current->digits, 
                                (current_digits + 1)* sizeof(char));
      strcpy(current->digits + 1, digits_copy);
      free(digits_copy);
    }
    current->digits[0] = n->digits[i];
    while (compare(m_copy->digits, current->digits) == -1) {
      // current >= m
      biggie_sub(current, m_copy);
      ++digit;
    }
    n->digits[i] = digit;
    current_digits = strlen(current->digits);
  }
  biggie_destroy(current);
  biggie_destroy(m_copy);
  n->digits = delete_leading_zero(n->digits);
  n->negative = NEGATIVE;
  if (! strcmp(n->digits, "0")) {
    n->negative = false;
  }
}