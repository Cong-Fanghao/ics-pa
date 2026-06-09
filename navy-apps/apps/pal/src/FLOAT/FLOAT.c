#include "FLOAT.h"
#include <stdint.h>
#include <assert.h>

FLOAT F_mul_F(FLOAT a, FLOAT b) {
  // assert(0);
  // return 0;
  uint32_t ua = (a < 0) ? -a : a;
  uint32_t ub = (b < 0) ? -b : b;

  uint32_t a_lo = ua & 0xFFFF;
  uint32_t a_hi = ua >> 16;
  uint32_t b_lo = ub & 0xFFFF;
  uint32_t b_hi = ub >> 16;

  uint32_t result_lo = a_lo * b_lo;
  uint32_t result_mid = a_hi * b_lo + a_lo * b_hi + (result_lo >> 16);

  int32_t result = (int32_t)result_mid;

  if ((a < 0) ^ (b < 0))
  {
    result = -result;
  }

  return result;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
  int neg = (a < 0) ^ (b < 0);
  uint32_t ua = (a < 0) ? -a : a;
  uint32_t ub = (b < 0) ? -b : b;

  if (ub == 0)
  {
    assert(0);
    return 0;
  }

  uint32_t result = 0;
  uint32_t remainder = 0;

  for (int i = 0; i < 32; i++) {
    remainder = (remainder << 1) | ((ua >> 31) & 1);
    ua <<= 1;
    
    if (remainder >= ub) {
      remainder -= ub;
      result |= (1 << (31 - i));
    }
  }

  if (neg)
  {
    if (result == 0x80000000) {
        return (FLOAT)0x80000000;
    }
    return -(int32_t)result;
  }
  
  return (FLOAT)result;
}

FLOAT f2F(float a) {
  /* You should figure out how to convert `a' into FLOAT without
   * introducing x87 floating point instructions. Else you can
   * not run this code in NEMU before implementing x87 floating
   * point instructions, which is contrary to our expectation.
   *
   * Hint: The bit representation of `a' is already on the
   * stack. How do you retrieve it to another variable without
   * performing arithmetic operations on it directly?
   */

  // assert(0);
  // return 0;
  union
  {
    float f;
    uint32_t u;
  } uf;
  uf.f = a;

  uint32_t bits = uf.u;
  uint32_t sign = bits >> 31;         // 符号位
  uint32_t exp = (bits >> 23) & 0xFF; // 指数部分（偏移 127）
  uint32_t frac = bits & 0x7FFFFF;    // 尾数部分（23 位）

  if (exp == 0xFF)
  {
    return 0;
  }

  int32_t E;  // 实际指数
  uint32_t M; // 实际尾数

  if (exp == 0)
  {
    E = 1 - 127;
    M = frac;
  }
  else
  {
    E = (int32_t)exp - 127;
    M = frac | 0x800000;
  }

  int32_t result = (int32_t)M;
  int32_t shift = E - 7;

  if (shift >= 0)
  {
    result = result << shift;
  }
  else
  {
    result = result >> (-shift);
  }

  if (sign)
  {
    result = -result;
  }

  return result;
}

FLOAT Fabs(FLOAT a) {
  // assert(0);
  // return 0;
  return a < 0 ? -a : a;
}

/* Functions below are already implemented */

FLOAT Fsqrt(FLOAT x) {
  FLOAT dt, t = int2F(2);

  do {
    dt = F_div_int((F_div_F(x, t) - t), 2);
    t += dt;
  } while(Fabs(dt) > f2F(1e-4));

  return t;
}

FLOAT Fpow(FLOAT x, FLOAT y) {
  /* we only compute x^0.333 */
  FLOAT t2, dt, t = int2F(2);

  do {
    t2 = F_mul_F(t, t);
    dt = (F_div_F(x, t2) - t) / 3;
    t += dt;
  } while(Fabs(dt) > f2F(1e-4));

  return t;
}
