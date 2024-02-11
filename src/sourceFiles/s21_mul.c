#include "../s21_decimal.h"

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
  int ret_code = 0;
  s21_reverse(&value_1);
  s21_reverse(&value_2);
  s21_normalize(&value_1);
  s21_normalize(&value_2);

  uint32_t ans_sign = (s21_get_sign(value_1) ^ s21_get_sign(value_2));

  s21_set_zeros(result);
  int ans_sc = s21_get_scale(value_1) + s21_get_scale(value_2);
  uint32_t value[6];
  memset(value, 0, 6 * sizeof(uint32_t));

  s21_fill_long(value_1, value_2, value);

  while (value[0] != 0 || value[1] != 0 || value[2] != 0 || ans_sc > 28) {
    s21_bank_round_long(value);
    ans_sc--;
  }
  if (ans_sc < 0) {
    if (ans_sign == 0) {
      ret_code = 1;
    } else {
      ret_code = 2;
    }
  } else {
    memcpy(result->bits, value + 3, 3 * sizeof(uint32_t));
    s21_set_scale(ans_sc, result);
    result->bits[3] |= (ans_sign << SIGNOFS);
    s21_normalize(result);
  }
  s21_reverse(result);
  return ret_code;
}