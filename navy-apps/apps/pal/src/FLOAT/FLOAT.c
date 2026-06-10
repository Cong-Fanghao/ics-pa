#include "FLOAT.h"
#include <stdint.h>
#include <assert.h>

union float_ {
  struct {
    uint32_t m : 23;      // 尾数
    uint32_t e : 8;       // 指数
    uint32_t signal : 1;    // 符号位
  };
  uint32_t value;
};

FLOAT F_mul_F(FLOAT a, FLOAT b) {
  // assert(0);
  // return 0;
  int neg = (a < 0) ^ (b < 0);

  uint32_t ua = (a < 0) ? -a : a;
  uint32_t ub = (b < 0) ? -b : b;

  // 拆分成高16位和低16位
  uint32_t a_hi = ua >> 16;
  uint32_t a_lo = ua & 0xFFFF;
  uint32_t b_hi = ub >> 16;
  uint32_t b_lo = ub & 0xFFFF;

  uint32_t term1 = a_hi * b_hi;
  uint32_t term2 = a_hi * b_lo + a_lo * b_hi;
  uint32_t term3 = a_lo * b_lo;

  uint32_t carry = term3 >> 16;
  term2 += carry;
  
  uint32_t carry2 = term2 >> 16;
  term1 += carry2;
  
  term2 = term2 & 0xFFFF;

  uint32_t result = (term1 << 16) + term2 + (term3 >> 16);

  if (neg) {
    return -(int32_t)result;
  }
  return (int32_t)result;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
  // assert(0);
  // return 0;
  int neg = (a < 0) ^ (b < 0);
  
  uint32_t ua = (a < 0) ? -a : a;
  uint32_t ub = (b < 0) ? -b : b;

  if (ub == 0) {
    return neg ? (FLOAT)0x80000000 : (FLOAT)0x7FFFFFFF;
  }

  uint32_t result = ua / ub;
  uint32_t m = ua % ub;

  for (int i = 0; i < 16; i++) {
    m <<= 1;
    result <<= 1;
    if (m >= ub) {
      m -= ub;
      result++;
    }
  }

  if (neg) {
    return -(int32_t)result;
  }
  return (int32_t)result;
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
  union float_ f;
  f.value = *((uint32_t *)(void *)&a);

  int e = f.e - 127;

  FLOAT result;
  if (e <= 7) {
    result = (f.m | (1 << 23)) >> (7 - e);
  } else {
    result = (f.m | (1 << 23)) << (e - 7);
  }

  if (f.signal == 0) {
    return result;
  } else {
    return -result;
  }
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
