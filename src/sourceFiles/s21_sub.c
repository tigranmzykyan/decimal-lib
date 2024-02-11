#include "../s21_decimal.h"

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
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
      ret_code = s21_add(value_1, value_2, result);
    } else {
      s21_change_sign(&value_2);
      s21_reverse(&value_1);
      s21_reverse(&value_2);
      ret_code = s21_add(value_2, value_1, result);
      // s21_negate(*result, result);
    }
    s21_reverse(result);
  } else {
    uint32_t res_sign = s21_get_sign(value_1);
    if ((value_1.bits[3] >> 31) != 0) value_1.bits[3] -= (1u << 31);
    if ((value_2.bits[3] >> 31) != 0) value_2.bits[3] -= (1u << 31);
    s21_reverse(&value_1);
    s21_reverse(&value_2);
    if (s21_is_less(value_1, value_2)) {
      s21_swap(&value_1, &value_2);
      if (res_sign == 0) {
        res_sign = 1;
      } else {
        res_sign = 0;
      }
    }
    s21_reverse(&value_1);
    s21_reverse(&value_2);

    s21_set_zeros(result);
    s21_move_point(&value_1, &value_2);

    s21_sub_abs(value_1, value_2, result);
    if (res_sign == 1) {
      s21_change_sign(result);
    }
  }
  s21_normalize(result);
  s21_reverse(result);
  return ret_code;
}
