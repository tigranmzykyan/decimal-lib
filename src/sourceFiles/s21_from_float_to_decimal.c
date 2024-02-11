#include "../s21_decimal.h"

int s21_from_float_to_decimal(float src, s21_decimal* dst) {
  int ret_code = 0;
  if (isnan(src) || isinf(src) || fabs(src) > MAXFLOAT) {
    ret_code = 1;
  }
  if (dst && (ret_code == 0)) {
    memset(dst->bits, 0, 4 * sizeof(uint32_t));
    int sign = ((src < 0) ? 1 : 0);
    src = fabs(src);
    int tmp = (int)src;
    int sc = 0;
    while (src - ((float)tmp / (int)(pow(10, sc))) != 0) {
      sc++;
      tmp = src * (int)(pow(10, sc));
    }
    s21_from_int_to_decimal(tmp, dst);
    if (sign) s21_change_sign(dst);
    s21_set_scale(sc, dst);
  } else {
    ret_code = 1;
  }
  s21_reverse(dst);
  s21_normalize(dst);
  s21_reverse(dst);
  return ret_code;
}