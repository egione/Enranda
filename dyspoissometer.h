/*
Dyspoissometer
Copyright 2016 Russell Leidich
http://dyspoissonism.blogspot.com

This collection of files constitutes the Dyspoissometer Library. (This is a
library in the abstact sense; it's not intended to compile to a ".lib"
file.)

The Dyspoissometer Library is free software: you can redistribute it and/or
modify it under the terms of the GNU Limited General Public License as
published by the Free Software Foundation, version 3.

The Dyspoissometer Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Limited General Public License version 3 for more details.

You should have received a copy of the GNU Limited General Public
License version 3 along with the Dyspoissometer Library (filename
"COPYING"). If not, see http://www.gnu.org/licenses/ .
*/
/*
Dyspoissometer Constants
*/
#if !(defined(DYSPOISSOMETER_NUMBER_QUAD)||defined(DYSPOISSOMETER_NUMBER_DOUBLE)||defined(DYSPOISSOMETER_NUMBER_FLOAT))
  #error "You must select a numerical precision level. Use 'gcc -DDYSPOISSOMETER_NUMBER_QUAD', '-DDYSPOISSOMETER_NUMBER_DOUBLE', or 'DYSPOISSOMETER_NUMBER_FLOAT' for quad, double, or float floating precision, respectively. Quad, double, and float precision imply 64, 32, and 16 bits per integer, respectively."
#elif (defined(DYSPOISSOMETER_NUMBER_QUAD)&&defined(DYSPOISSOMETER_NUMBER_DOUBLE))||(defined(DYSPOISSOMETER_NUMBER_QUAD)&&defined(DYSPOISSOMETER_NUMBER_FLOAT))||(defined(DYSPOISSOMETER_NUMBER_DOUBLE)&&defined(DYSPOISSOMETER_NUMBER_FLOAT))
  #error "You have selected more than one numerical precision. Choose one only."
#elif defined(DYSPOISSOMETER_NUMBER_QUAD)
  #define DYSPOISSOMETER_NUMBER __float128
  #define DYSPOISSOMETER_NUMBER_SIZE 0x10U
  #define DYSPOISSOMETER_NUMBER_SIZE_LOG2 4U
  #define DYSPOISSOMETER_UINT uint64_t
  #define DYSPOISSOMETER_UINT_BIT_MAX 63U
  #define DYSPOISSOMETER_UINT_BITS 64U
  #define DYSPOISSOMETER_UINT_BITS_LOG2 6U
  #define DYSPOISSOMETER_UINT_MAX 0xFFFFFFFFFFFFFFFFULL
  #define DYSPOISSOMETER_UINT_SIZE 8U
  #define DYSPOISSOMETER_UINT_SIZE_LOG2 3U
  #define DYSPOISSOMETER_UINT_SPAN_HALF 0x8000000000000000ULL
  #define EXP(n) expq((__float128)(n))
  #define LOG(n) logq((__float128)(n))
  #define LOG_N_PLUS_1(n) logq((__float128)(n)+1.0f)
/*
LOG_SUM() computes Σ(i = (1, n), ln i), apparently using the convergent form of Stirling's approximation. Comparisons to 100-digit values produced by Mathematica show it to be accurate to within 4 floating-point epsilon units at the same scale as the result -- in other words, extremely accurate -- although in theory worse cases might exist. It should only be used for (n>=1) so that the stupid signgam variable (sign-of-gamma) in math.h need not be inspected after the fact, because it is not thread-safe; lgammaq_r() fixes this problem but requires another annoying parameter. We cast to DYSPOISSOMETER_NUMBER first because n might be subject to wrap otherwise.
*/
  #define LOG_SUM(n) lgammaq((__float128)(n)+1.0f)
/*
LOG_SUM_N_PLUS_1() computes Σ(i = (1, n+1), ln i) in the same manner as LOG_SUM(), but an additional increment is provided for use with indexes as opposed to counts. Otherwise, we might end up with inefficient code, or wose, adding one and 1.0f.
*/
  #define LOG_SUM_N_PLUS_1(n) lgammaq((__float128)(n)+2.0f)
  #define ROUND(n) roundq((__float128)(n))
  #define SQRT(n) sqrtq((__float128)(n))
#elif defined(DYSPOISSOMETER_NUMBER_DOUBLE)
  #define DYSPOISSOMETER_NUMBER double
  #define DYSPOISSOMETER_NUMBER_SIZE 8U
  #define DYSPOISSOMETER_NUMBER_SIZE_LOG2 3U
  #define DYSPOISSOMETER_UINT uint32_t
  #define DYSPOISSOMETER_UINT_BIT_MAX 31U
  #define DYSPOISSOMETER_UINT_BITS 32U
  #define DYSPOISSOMETER_UINT_BITS_LOG2 5U
  #define DYSPOISSOMETER_UINT_MAX 0xFFFFFFFFU
  #define DYSPOISSOMETER_UINT_SIZE 4U
  #define DYSPOISSOMETER_UINT_SIZE_LOG2 2U
  #define DYSPOISSOMETER_UINT_SPAN_HALF 0x80000000U
  #define EXP(n) exp((double)(n))
  #define LOG(n) log((double)(n))
  #define LOG_N_PLUS_1(n) log((double)(n)+1.0f)
  #define LOG_SUM(n) lgamma((double)(n)+1.0f)
  #define LOG_SUM_N_PLUS_1(n) lgamma((double)(n)+2.0f)
  #define ROUND(n) round((double)(n))
  #define SQRT(n) sqrt((double)(n))
#else
  #define DYSPOISSOMETER_NUMBER float
  #define DYSPOISSOMETER_NUMBER_SIZE 4U
  #define DYSPOISSOMETER_NUMBER_SIZE_LOG2 2U
  #define DYSPOISSOMETER_UINT uint16_t
  #define DYSPOISSOMETER_UINT_BIT_MAX 15U
  #define DYSPOISSOMETER_UINT_BITS 16U
  #define DYSPOISSOMETER_UINT_BITS_LOG2 4U
  #define DYSPOISSOMETER_UINT_MAX 0xFFFFU
  #define DYSPOISSOMETER_UINT_SIZE 2U
  #define DYSPOISSOMETER_UINT_SIZE_LOG2 1U
  #define DYSPOISSOMETER_UINT_SPAN_HALF 0x8000U
  #define EXP(n) expf((float)(n))
  #define LOG(n) logf((float)(n))
  #define LOG_N_PLUS_1(n) logf((float)(n)+1.0f)
  #define LOG_SUM(n) lgammaf((float)(n)+1.0f)
  #define LOG_SUM_N_PLUS_1(n) lgammaf((float)(n)+2.0f)
  #define ROUND(n) roundf((float)(n))
  #define SQRT(n) sqrtf((float)(n))
#endif
/*
DYSPOISSOMETER_MARSAGLIA_A is the largest A value which one can use with a 64-bit Marsaglia oscillator (for pseudorandom number generation) based on Sophie Germain primes. See the tables at https://en.wikipedia.org/wiki/Multiply-with-carry .
*/
#define DYSPOISSOMETER_MARSAGLIA_A (U32_MAX-177U)
#ifdef DYSPOISSOMETER_NUMBER_QUAD
  #define DEBUG_NUMBER(name_base, value) DEBUG_QUAD(name_base, value)
  #define DEBUG_NUMBER_LIST(context_string_base, quad_count, quad_list_base) DEBUG_QUAD_LIST(context_string_base, quad_count, quad_list_base)
#elif defined(DYSPOISSOMETER_NUMBER_DOUBLE)
  #define DEBUG_NUMBER(name_base, value) DEBUG_DOUBLE(name_base, value)
  #define DEBUG_NUMBER_LIST(context_string_base, double_count, double_list_base) DEBUG_DOUBLE_LIST(context_string_base, double_count, double_list_base)
#else
  #define DEBUG_NUMBER(name_base, value) DEBUG_FLOAT(name_base, value)
  #define DEBUG_NUMBER_LIST(context_string_base, float_count, float_list_base) DEBUG_FLOAT_LIST(context_string_base, float_count, float_list_base)
#endif
