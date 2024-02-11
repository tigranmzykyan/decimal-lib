#include "../s21_decimal.h"

int s21_is_less(s21_decimal lhs, s21_decimal rhs) {
  s21_reverse(&lhs);
  s21_reverse(&rhs);
  if (lhs.bits[0] == 0 && lhs.bits[1] == 0 && lhs.bits[2] == 0 &&
      rhs.bits[0] == 0 && rhs.bits[1] == 0 && rhs.bits[2] == 0) {
    return 0;
  }

  if (s21_get_sign(lhs) != s21_get_sign(rhs)) {
    if (s21_get_sign(lhs) == 0) {
      return 0;
    } else {
      return 1;
    }
  } else if (s21_get_sign(lhs) == 0) {
    return s21_is_less_abs(lhs, rhs);
  } else {
    return s21_is_less_abs(rhs, lhs);
  }
}