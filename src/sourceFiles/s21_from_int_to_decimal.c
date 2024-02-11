#include "../s21_decimal.h"

int s21_from_int_to_decimal(int src, s21_decimal* dst) {
  s21_set_zeros(dst);
  if (src < 0) {
    src = -src;
    s21_change_sign(dst);
  }
  dst->bits[2] = src;
  s21_reverse(dst);
  return 0;
}