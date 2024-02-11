#include "../s21_decimal.h"

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
  int ret_code = 0;
  s21_reverse(&value_1);
  s21_reverse(&value_2);

  s21_normalize(&value_1);
  s21_normalize(&value_2);

  if (s21_get_sign(value_2) != s21_get_sign(value_1)) {
    if (s21_get_sign(value_1) == 0) {
      s21_change_sign(&value_2);
      s21_reverse(&value_1);
      s21_reverse(&value_2);
      ret_code = s21_sub(value_1, value_2, result);

    } else {
      s21_change_sign(&value_1);
      s21_reverse(&value_1);
      s21_reverse(&value_2);
      ret_code = s21_sub(value_2, value_1, result);
    }
    s21_reverse(result);
  } else {
    s21_set_zeros(result);
    s21_move_point(&value_1, &value_2);

    ret_code = s21_sum(value_1, value_2, result);
  }
  s21_normalize(result);
  s21_reverse(result);
  return ret_code;
}