#include "../s21_decimal.h"

int s21_negate(s21_decimal value, s21_decimal* result) {
  int ret_code = 0;
  if (result) {
    memcpy(result->bits, value.bits, 4 * sizeof(uint32_t));
    s21_change_sign(result);
  } else {
    ret_code = 1;
  }
  return ret_code;
}