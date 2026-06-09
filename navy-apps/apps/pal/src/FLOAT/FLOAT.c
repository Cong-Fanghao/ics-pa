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

  // 计算 (a * b) >> 16 的绝对值
  uint32_t result_lo = a_lo * b_lo;
  uint32_t result_mid = a_hi * b_lo + a_lo * b_hi + (result_lo >> 16);

  // result_mid 就是 (|a| * |b|) >> 16 的低32位

  int32_t result = (int32_t)result_mid;

  if ((a < 0) ^ (b < 0))
  {
    result = -result;
  }

  return result;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
  // assert(0);
  // return 0;
  int neg = (a < 0) ^ (b < 0);
  uint32_t ua = (a < 0) ? -a : a;
  uint32_t ub = (b < 0) ? -b : b;

  if (ub == 0) {
    return neg ? 0x80000000 : 0x7FFFFFFF;
  }

  // 拆分 ua = ua_hi * 2^16 + ua_lo
  uint32_t ua_hi = ua >> 16;
  uint32_t ua_lo = ua & 0xFFFF;

  // 注意：ua_hi << 16 可能溢出32位！
  // 所以用 (ua_hi / ub) * 2^16 + (ua_hi % ub) * 2^16 / ub
  uint32_t q_hi = ua_hi / ub;
  uint32_t r_hi = ua_hi % ub;
  
  uint32_t result = q_hi << 16;
  
  uint32_t sum = r_hi + ua_lo;
  // 需要处理 sum >= 65536 的情况
  uint32_t q_lo;
  
  if (sum >= 0x10000) {
    uint32_t sum_hi = sum >> 16;
    uint32_t sum_lo = sum & 0xFFFF;
    if (sum_hi >= ub) {
      return neg ? 0x80000000 : 0x7FFFFFFF;
    }
    q_lo = (sum_hi << 16) / ub;
    uint32_t r_lo = (sum_hi << 16) % ub;
    q_lo += ((r_lo << 16) + sum_lo) / ub;
  } else {
    q_lo = (sum << 16) / ub;
  }
  
  result += q_lo;

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
