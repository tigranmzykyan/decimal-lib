#include "../s21_decimal.h"

int s21_get_sign(const s21_decimal dc) { return (dc.bits[3] >> SIGNOFS); }

void s21_change_sign(s21_decimal* dc) { dc->bits[3] ^= (1u << SIGNOFS); }

int s21_set_scale(int scale, s21_decimal* dst) {
  int ret_code = 0;
  if (scale > MAXSCALE || scale < 0) {
    ret_code = 1;
  } else {
    dst->bits[3] &= (1u << SIGNOFS);
    dst->bits[3] += ((uint32_t)(scale << HALFINT));
  }
  return ret_code;
}

void s21_set_zeros(s21_decimal* dst) { memset(dst, 0, sizeof(int) * 4); }

int s21_get_scale(const s21_decimal val) {
  return (val.bits[3] >> HALFINT) & ENDMASK;
}

void s21_swap(s21_decimal* lhs, s21_decimal* rhs) {
  uint32_t tmp = lhs->bits[0];
  lhs->bits[0] = rhs->bits[0];
  rhs->bits[0] = tmp;
  tmp = lhs->bits[1];
  lhs->bits[1] = rhs->bits[1];
  rhs->bits[1] = tmp;
  tmp = lhs->bits[2];
  lhs->bits[2] = rhs->bits[2];
  rhs->bits[2] = tmp;
  tmp = lhs->bits[3];
  lhs->bits[3] = rhs->bits[3];
  rhs->bits[3] = tmp;
}
void s21_swap_int(int* lhs, int* rhs) {
  int tmp = *rhs;
  *rhs = *lhs;
  *lhs = tmp;
}

int s21_try_upscale(s21_decimal* dc) {
  int ret_code = 0;
  s21_decimal tmp;
  s21_set_zeros(&tmp);
  if (((dc->bits[0] & LASRTHRBIT) != 0) || s21_get_scale(*dc) == 28) {
    ret_code = 1;
  } else {
    for (int i = 0; i < 3; ++i) {
      tmp.bits[i] = (dc->bits[i] << 3);
    }
    tmp.bits[0] += (dc->bits[1] >> 29);
    tmp.bits[1] += (dc->bits[2] >> 29);
    unsigned long long a = 0;
    for (int i = 2; i >= 0; --i) {
      a += ((unsigned long long)tmp.bits[i]) + (dc->bits[i] << 1);
      tmp.bits[i] = a % UINT_MAX;
      a /= UINT_MAX;
    }
    if (a > 0) {
      ret_code = 1;
    } else {
      a = ((unsigned long long)tmp.bits[1]) + (dc->bits[2] >> 31);
      tmp.bits[1] = a % UINT_MAX;
      a /= UINT_MAX;
      a += ((unsigned long long)tmp.bits[0]) + (dc->bits[1] >> 31);
      tmp.bits[0] = a % UINT_MAX;
      a /= UINT_MAX;
      if (a > 0) {
        ret_code = 1;
      } else {
        memcpy(dc->bits, tmp.bits, 3 * sizeof(uint32_t));
        int sc = s21_get_scale(*dc);
        s21_set_scale(sc + 1, dc);
      }
    }
  }
  return ret_code;
}

void s21_division2(s21_decimal* dc) {
  dc->bits[2] >>= 1;
  dc->bits[2] |= ((dc->bits[1] & 1) << 31);
  dc->bits[1] >>= 1;
  dc->bits[1] |= ((dc->bits[0] & 1) << 31);
  dc->bits[0] >>= 1;
}

void s21_normalize(s21_decimal* dc) {
  while (s21_div5(*dc) == 0 && s21_div2(*dc) == 0 && (s21_get_scale(*dc) > 0)) {
    s21_division2(dc);
    s21_division5(dc);
    s21_set_scale(s21_get_scale(*dc) - 1, dc);
  }
}

int s21_div5(const s21_decimal dc) {
  int sum = 0;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 8; ++j) {
      sum += ((dc.bits[i] >> (j * 4)) & FIRSTWOBITS);
      sum -= ((dc.bits[i] >> (j * 4 + 2)) & FIRSTWOBITS);
    }
  }
  return (((sum % 5) + 5) % 5);
}

void s21_division5(s21_decimal* dc) {
  unsigned long long tmp = dc->bits[0] % 5;
  dc->bits[0] /= 5;
  tmp *= UINT_MAX;
  tmp += dc->bits[1];
  unsigned long long b = tmp % 5;
  dc->bits[1] = tmp / 5;
  b *= UINT_MAX;
  b += dc->bits[2];
  dc->bits[2] = b / 5;
}

int s21_div2(const s21_decimal dc) { return (dc.bits[2] % 2); }

void s21_inc(s21_decimal* dc) {
  if (dc->bits[2] == __UINT32_MAX__) {
    if (dc->bits[1] == __UINT32_MAX__) {
      dc->bits[0]++;
      dc->bits[1] = 0;
      dc->bits[2] = 0;
    } else {
      dc->bits[1]++;
      dc->bits[2] = 0;
    }
  } else {
    dc->bits[2]++;
  }
}

void s21_bank_round(s21_decimal* dc, int inv) {
  int flag = 0;
  if (s21_div2(*dc) != 0 || s21_div5(*dc) != 0) {
    if (s21_div5(*dc) == 0) {
      flag = 2;
    } else if (s21_div5(*dc) % 2 != s21_div2(*dc) && s21_div5(*dc) != 0) {
      flag = 1;
    }
  }
  s21_division2(dc);
  s21_division5(dc);
  if (flag == 1) {
    s21_inc(dc);
  } else if (flag == 2) {
    if (s21_div2(*dc) != inv) {
      s21_inc(dc);
    }
  }
}

void s21_bank_round_long(uint32_t* dc) {
  int flag = 0;
  if (s21_div2_long(dc) != 0 || s21_div5_long(dc) != 0) {
    if (s21_div5_long(dc) == 0) {
      flag = 2;
    } else if (s21_div5_long(dc) % 2 != s21_div2_long(dc) &&
               s21_div5_long(dc) != 0) {
      flag = 1;
    }
  }
  s21_division2_long(dc);
  s21_division5_long(dc);
  if (flag == 1) {
    s21_inc_long(dc);
  } else if (flag == 2) {
    if (s21_div2_long(dc) != 0) {
      s21_inc_long(dc);
    }
  }
}

void s21_bank_round_longl(unsigned long long* a, unsigned long long* b,
                          unsigned long long* c) {
  int ost = (*a) % 10;
  (*a) /= 10;

  (*b) += ost * UINT_MAX;

  ost = (*b) % 10;

  (*b) /= 10;

  (*c) += ost * UINT_MAX;
  ost = (*c) % 10;
  (*c) /= 10;

  if (ost > 5) {
    s21_inc_longl(a, b, c);
  } else if (ost == 5 && ((*c) % 2 == 1)) {
    s21_inc_longl(a, b, c);
  }
}

void s21_inc_longl(unsigned long long* a, unsigned long long* b,
                   unsigned long long* c) {
  if (*c == __UINT32_MAX__) {
    if (*b == __UINT32_MAX__) {
      (*a)++;
      (*b) = (*c) = 0;
    } else {
      (*b)++;
      (*c) = 0;
    }
  } else {
    (*c)++;
  }
}

int s21_div2_long(uint32_t* dc) { return (dc[5] % 2); }

int s21_div5_long(uint32_t* dc) {
  int sum = 0;
  for (int i = 0; i < 6; ++i) {
    for (int j = 0; j < 8; ++j) {
      sum += ((dc[i] >> (j * 4)) & FIRSTWOBITS);
      sum -= ((dc[i] >> (j * 4 + 2)) & FIRSTWOBITS);
    }
  }
  return (((sum % 5) + 5) % 5);
}

void s21_division5_long(uint32_t* dc) {
  unsigned long long tmp = dc[0] % 5;
  dc[0] /= 5;
  tmp *= UINT_MAX;
  tmp += dc[1];
  unsigned long long b = tmp % 5;
  dc[1] = tmp / 5;
  b *= UINT_MAX;
  b += dc[2];
  unsigned long long c = b % 5;
  dc[2] = b / 5;
  c *= UINT_MAX;
  c += dc[3];
  unsigned long long d = c % 5;
  dc[3] = c / 5;
  d *= UINT_MAX;
  d += dc[4];
  unsigned long long e = d % 5;
  dc[4] = d / 5;
  e *= UINT_MAX;
  e += dc[5];
  dc[5] = e / 5;
}

void s21_division2_long(uint32_t* dc) {
  dc[5] >>= 1;
  dc[5] |= ((dc[4] & 1) << 31);
  dc[4] >>= 1;
  dc[4] |= ((dc[3] & 1) << 31);
  dc[3] >>= 1;
  dc[3] |= ((dc[2] & 1) << 31);
  dc[2] >>= 1;
  dc[2] |= ((dc[1] & 1) << 31);
  dc[1] >>= 1;
  dc[1] |= ((dc[0] & 1) << 31);
  dc[0] >>= 1;
}

void s21_inc_long(uint32_t* dc) {
  if (dc[5] == __UINT32_MAX__) {
    if (dc[4] == __UINT32_MAX__) {
      if (dc[3] == __UINT32_MAX__) {
        if (dc[2] == __UINT32_MAX__) {
          if (dc[1] == __UINT32_MAX__) {
            dc[0]++;
            dc[1] = dc[2] = dc[3] = dc[4] = dc[5] = 0;
          } else {
            dc[1]++;
            dc[2] = dc[3] = dc[4] = dc[5] = 0;
          }
        } else {
          dc[2]++;
          dc[3] = dc[4] = dc[5] = 0;
        }
      } else {
        dc[3]++;
        dc[4] = dc[5] = 0;
      }
    } else {
      dc[4]++;
      dc[5] = 0;
    }
  } else {
    dc[5]++;
  }
}

void s21_move_point(s21_decimal* value_1, s21_decimal* value_2) {
  int sc1 = s21_get_scale(*value_1);
  int sc2 = s21_get_scale(*value_2);
  int flag = 0;
  if (sc1 < sc2) {
    s21_swap(value_1, value_2);
    s21_swap_int(&sc1, &sc2);
    flag = 1;
  }

  while (sc1 > sc2) {
    int ret = s21_try_upscale(value_2);
    if (ret == 0) {
      sc2++;
    } else {
      break;
    }
  }
  while (sc1 > sc2 + 1) {
    s21_bank_round(value_1, 0);
    --sc1;
  }
  if (sc1 > sc2) {
    s21_bank_round(value_1, s21_div2(*value_2));
    --sc1;
  }
  s21_set_scale(sc1, value_1);
  s21_set_scale(sc2, value_2);
  if (flag) {
    s21_swap(value_1, value_2);
  }
}

int s21_sum(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
  int ret_code = 0;

  unsigned long long a =
      ((unsigned long long)value_1.bits[2]) + value_2.bits[2];
  result->bits[2] = a % UINT_MAX;
  unsigned long long b =
      ((unsigned long long)value_1.bits[1]) + value_2.bits[1] + a / UINT_MAX;
  result->bits[1] = b % UINT_MAX;
  unsigned long long c =
      ((unsigned long long)value_1.bits[0]) + value_2.bits[0] + b / UINT_MAX;
  a = result->bits[2];
  b = result->bits[1];
  if (c > __UINT32_MAX__ && s21_get_scale(value_1) == 0) {
    if (s21_get_sign(value_1) == 0) {
      ret_code = 1;
    } else {
      ret_code = 2;
    }
  } else if (c > __UINT32_MAX__) {
    s21_bank_round_longl(&c, &b, &a);

    result->bits[0] = c;
    result->bits[1] = b;
    result->bits[2] = a;
    result->bits[3] = value_2.bits[3];
    s21_set_scale(s21_get_scale(value_1) - 1, result);

  } else {
    result->bits[0] = c;
    result->bits[3] = value_2.bits[3];
  }

  return ret_code;
}

void s21_sub_abs(s21_decimal value_1, s21_decimal value_2,
                 s21_decimal* result) {
  long long a = (long long)value_1.bits[2] - (long long)value_2.bits[2];

  int fg1 = 0;
  if (a < 0) {
    fg1 = 1;
  }
  if (fg1) {
    a += UINT_MAX;
  }
  result->bits[2] = a;
  long long b =
      (long long)value_1.bits[1] - (long long)value_2.bits[1] - (long long)fg1;
  int fg2 = 0;
  if (b < 0) {
    fg2 = 1;
  }
  if (fg2) {
    b += UINT_MAX;
  }
  result->bits[1] = b;
  result->bits[0] =
      (long long)value_1.bits[0] - (long long)value_2.bits[0] - (long long)fg2;
  s21_set_scale(s21_get_scale(value_2), result);
}

int s21_is_less_abs(s21_decimal lhs, s21_decimal rhs) {
  int sc1 = s21_get_scale(lhs);
  int sc2 = s21_get_scale(rhs);

  while (s21_try_upscale(&lhs) == 0) {
    ++sc1;
  }
  while (s21_try_upscale(&rhs) == 0) {
    ++sc2;
  }

  if (sc1 < sc2) {
    return 0;
  } else if (sc1 > sc2) {
    return 1;
  } else {
    return (lhs.bits[0] < rhs.bits[0]
                ? 1
                : (lhs.bits[0] > rhs.bits[0]
                       ? 0
                       : (lhs.bits[1] < rhs.bits[1]
                              ? 1
                              : (lhs.bits[1] > rhs.bits[1]
                                     ? 0
                                     : (lhs.bits[2] < rhs.bits[2] ? 1 : 0)))));
  }
}

void s21_fill_long(s21_decimal value_1, s21_decimal value_2, uint32_t* value) {
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      unsigned long long res =
          (unsigned long long)value_1.bits[i] * value_2.bits[j];
      for (int k = i + j + 1; k >= 0; --k) {
        unsigned long long ost = res / UINT_MAX;
        res %= UINT_MAX;
        res += value[k];
        value[k] = res % UINT_MAX;
        res /= UINT_MAX;
        res += ost;
      }
    }
  }
}

void s21_reverse(s21_decimal* dc) {
  uint32_t tmp = dc->bits[0];
  dc->bits[0] = dc->bits[2];
  dc->bits[2] = tmp;
}