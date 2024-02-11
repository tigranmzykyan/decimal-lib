#include "../s21_decimal.h"

int s21_from_decimal_to_float(s21_decimal src, float* dst) {
  s21_reverse(&src);
  char str[40];
  int ret_code = 0;
  memset(str, '0', 39);
  str[39] = '\0';
  int flag = 1;
  int sc = s21_get_scale(src);
  for (int i = 0; i < 29; ++i) {
    if (i == sc) {
      str[39 - i - flag] = '.';
      flag++;
    }
    int ost = 0;
    int f = s21_div5(src);
    int t = s21_div2(src);
    if (f % 2 != t % 2) ost += 5;
    ost += (f + 5) % 5;
    str[39 - i - flag] = (char)('0' + ost);
    s21_division2(&src);
    s21_division5(&src);
  }
  sscanf(str, "%f", dst);
  if (s21_get_sign(src) == 1) {
    (*dst) = -(*dst);
  }
  return ret_code;
}