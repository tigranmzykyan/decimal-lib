#include "../s21_decimal.h"

int s21_truncate(s21_decimal value, s21_decimal* result) {
  int ret_code = 0;
  s21_reverse(&value);
  int sc = s21_get_scale(value);
  memcpy(result->bits, value.bits, 4 * sizeof(uint32_t));

  for (int i = 0; i < sc; ++i) {
    s21_division2(result);
    s21_division5(result);
  }
  s21_set_scale(0, result);
  s21_normalize(result);
  s21_reverse(result);
  return ret_code;
}
