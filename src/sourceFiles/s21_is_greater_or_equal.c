#include "../s21_decimal.h"

int s21_is_greater_or_equal(s21_decimal lhs, s21_decimal rhs) {
  return (!s21_is_less(lhs, rhs));
}
