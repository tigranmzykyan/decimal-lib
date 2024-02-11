#include "../s21_decimal.h"

int s21_from_decimal_to_int(s21_decimal src, int* dst) {
  int ret_code = 0;
  s21_reverse(&src);
  for (int i = 0; i < s21_get_scale(src); ++i) {
    s21_division2(&src);
    s21_division5(&src);
  }
  if (src.bits[0] != 0 || src.bits[1] != 0 || src.bits[2] > __INT32_MAX__) {
    ret_code = 1;
  } else {
    *dst = src.bits[2];
    if (s21_get_sign(src) == 1) {
      *dst = -(*dst);
    }
  }
  return ret_code;
}