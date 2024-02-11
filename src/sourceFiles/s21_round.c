#include "../s21_decimal.h"

int s21_round(s21_decimal value, s21_decimal* result) {
  int ret_code = 0;
  s21_decimal tr, rn;
  s21_truncate(value, &tr);
  s21_sub(value, tr, &rn);
  s21_decimal half = {{0x00000005, 0x00000000, 0x00000000, 0x00010000}};
  s21_decimal neghalf = {{0x00000005, 0x00000000, 0x00000000, 0x80010000}};
  s21_decimal one = {{0x00000001, 0x00000000, 0x00000000, 0x00000000}};
  if (s21_is_greater_or_equal(rn, half)) {
    ret_code = s21_add(tr, one, result);
  } else if (s21_is_less_or_equal(rn, neghalf)) {
    s21_negate(one, &one);
    ret_code = s21_add(tr, one, result);
  } else {
    memcpy(result->bits, tr.bits, 4 * sizeof(uint32_t));
  }
  return ret_code;
}