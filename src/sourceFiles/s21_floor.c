#include "../s21_decimal.h"

int s21_floor(s21_decimal value, s21_decimal* result) {
  s21_reverse(&value);
  int ret_code = 0;
  if (value.bits[0] == 0 && value.bits[1] == 0 && value.bits[2] == 0) {
    memset(value.bits, 0, 4 * sizeof(uint32_t));
  }
  int sc = s21_get_scale(value);
  memcpy(result->bits, value.bits, 4 * sizeof(uint32_t));
  for (int i = 0; i < sc; ++i) {
    s21_division2(result);
    s21_division5(result);
  }
  s21_set_scale(0, result);
  s21_reverse(result);
  if (s21_get_sign(*result) == 1) {
    s21_decimal s;
    s21_from_int_to_decimal(1, &s);
    ret_code = s21_sub(*result, s, result);
  }
  return ret_code;
}