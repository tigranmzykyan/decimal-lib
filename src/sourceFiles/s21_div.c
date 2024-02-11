#include "../s21_decimal.h"

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
  int ret_code = 0;
  s21_reverse(&value_1);
  s21_reverse(&value_2);
  s21_normalize(&value_1);
  s21_normalize(&value_2);
  s21_reverse(&value_1);
  s21_reverse(&value_2);
  s21_set_zeros(result);
  uint32_t res_sign = (s21_get_sign(value_1) ^ s21_get_sign(value_2));
  if (memcmp(value_2.bits, result->bits, 3 * sizeof(uint32_t)) == 0) {
    ret_code = 3;
  } else {
    if (s21_get_sign(value_1) == 1) value_1.bits[3] -= (1u << SIGNOFS);
    if (s21_get_sign(value_2) == 1) value_2.bits[3] -= (1u << SIGNOFS);
    s21_decimal tmp, ten, res, ans, tmp2, next, con, del;
    s21_from_int_to_decimal(0, &ans);
    s21_from_int_to_decimal(1, &tmp);
    s21_from_int_to_decimal(10, &ten);

    for (int i = 0; i < MAXSCALE; ++i) {
      s21_mul(tmp, ten, &res);
      memcpy(tmp.bits, res.bits, 4 * sizeof(uint32_t));
    }

    s21_from_float_to_decimal(0.1, &ten);
    int err = 0;
    do {
      if (s21_mul(tmp, value_2, &tmp2) == 0) {
        s21_set_zeros(&con);
        s21_set_zeros(&next);
        s21_set_zeros(&del);
        int r = s21_add(next, tmp2, &next);
        if (memcmp(tmp2.bits, result->bits, 3 * sizeof(uint32_t)) == 0) {
          r = 1;
          err = 1;
        }
        while ((r == 0) && s21_is_less_or_equal(next, value_1)) {
          memcpy(del.bits, next.bits, 4 * sizeof(uint32_t));
          r = s21_add(next, tmp2, &next);
          r = (s21_add(tmp, con, &con) ? 1 : r);
        }
        s21_sub(value_1, del, &res);
        memcpy(value_1.bits, res.bits, 4 * sizeof(uint32_t));
        ret_code = (s21_add(ans, con, &res) == 0 ? ret_code : 1);
        memcpy(ans.bits, res.bits, 4 * sizeof(uint32_t));
      }
      s21_mul(tmp, ten, &res);
      if (res.bits[0] == 0 && res.bits[1] == 0 && res.bits[2] == 0) {
        err = 1;
      } else {
        memcpy(tmp.bits, res.bits, 4 * sizeof(uint32_t));
      }
    } while (err == 0);

    ///
    s21_reverse(&tmp2);
    s21_try_upscale(&tmp2);
    s21_division2(&tmp2);
    s21_reverse(&tmp2);
    if (s21_is_less(tmp2, value_1)) {
      s21_add(ans, tmp, &ans);
    } else if (s21_is_equal(value_1, tmp2)) {
      if (ans.bits[0] % 2) {
        s21_add(ans, tmp, &ans);
      }
    }

    memcpy(result->bits, ans.bits, 4 * sizeof(uint32_t));
    result->bits[3] |= (res_sign << SIGNOFS);
    s21_reverse(result);
    s21_normalize(result);
  }
  s21_reverse(result);
  return ret_code;
}