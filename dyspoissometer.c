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
Dyspoissonism Functions
*/
#include "flag.h"
#include "flag_dyspoissometer.h"
#include <math.h>
#ifdef DYSPOISSOMETER_NUMBER_QUAD
  #include <quadmath.h>
#endif
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "constant.h"
#include "debug.h"
#include "debug_xtrn.h"
#ifdef DYSPOISSOMETER_NUMBER_QUAD
  #include "debug_quad.h"
  #include "debug_quad_xtrn.h"
#endif
#include "dyspoissometer.h"

u8
dyspoissometer_init(u32 build_break_count, u32 build_feature_count){
/*
Verify that the source code is sufficiently updated.

In:

  build_break_count is the caller's most recent knowledge of DYSPOISSOMETER_BUILD_BREAK_COUNT, which will fail if the caller is unaware of all critical updates.

  build_feature_count is the caller's most recent knowledge of DYSPOISSOMETER_BUILD_FEATURE_COUNT, which will fail if this library is not up to date with the caller's expectations.

Out:

  Returns one if (build_break_count!=DYSPOISSOMETER_BUILD_BREAK_COUNT) or (build_feature_count>DYSPOISSOMETER_BUILD_FEATURE_COUNT). Otherwise, returns zero.
*/
  u8 status;

  status=(u8)(build_break_count!=DYSPOISSOMETER_BUILD_BREAK_COUNT);
  status=(u8)(status|(DYSPOISSOMETER_BUILD_FEATURE_COUNT<build_feature_count));
  return status;
}

void *
dyspoissometer_free(void *base){
/*
To maximize portability and debuggability, this is the only place where Dyspoissometer frees memory.

In:

  base is the base of a memory region to free. May be NULL.

Out:

  Returns NULL so that the caller can easily maintain the good practice of NULLing out invalid pointers.

  *base is freed.
*/
  DEBUG_FREE_PARANOID(base);
  return NULL;
}

DYSPOISSOMETER_NUMBER *
dyspoissometer_number_list_malloc(DYSPOISSOMETER_UINT number_idx_max){
/*
To maximize portability and debuggability, this is one of the few functions in which Dyspoissometer calls the compiler's memory allocator directly.

In:

  number_idx_max is the number of (DYSPOISSOMETER_NUMBER)s to allocate, less one, on [0, DYSPOISSOMETER_UINT_MAX-1]. The upper bound is enforced to prevent the caller from wasting time on mask lists or frequency lists whose implied mask count or mask span, respectively, would wrap DYSPOISSOMETER_UINT.

Out:

  Returns NULL on failure, else the base of (number_idx_max+1) undefined items.
*/
  DYSPOISSOMETER_NUMBER *list_base;
  ULONG list_size;
  ULONG number_count;

  list_base=NULL;
  number_count=(ULONG)(number_idx_max+1);
  if(number_count){
    list_size=(ULONG)(number_count<<DYSPOISSOMETER_NUMBER_SIZE_LOG2);
/*
Make sure that list_size is small enough to fit within this machine's address space, as opposed to merely DYSPOISSOMETER_UINT.
*/
    if((list_size>>DYSPOISSOMETER_NUMBER_SIZE_LOG2)==number_count){
      list_base=DEBUG_MALLOC_PARANOID(list_size);
    }
  }
  return list_base;
}

DYSPOISSOMETER_UINT *
dyspoissometer_uint_list_malloc(DYSPOISSOMETER_UINT uint_idx_max){
/*
To maximize portability and debuggability, this is one of the few functions in which Dyspoissometer calls the compiler's memory allocator directly.

In:

  uint_idx_max is the number of (DYSPOISSOMETER_UINT)s to allocate, less one, on [0, DYSPOISSOMETER_UINT_MAX-1]. The upper bound is enforced to prevent the caller from wasting time on mask lists or frequency lists whose implied mask count or mask span, respectively, would wrap DYSPOISSOMETER_UINT.

Out:

  Returns NULL on failure, else the base of (uint_idx_max+1) undefined items.
*/
  DYSPOISSOMETER_UINT *list_base;
  ULONG list_size;
  ULONG uint_count;

  list_base=NULL;
  uint_count=(ULONG)(uint_idx_max+1);
  if(uint_count){
    list_size=(ULONG)(uint_count<<DYSPOISSOMETER_UINT_SIZE_LOG2);
/*
Make sure that list_size is small enough to fit within this machine's address space, as opposed to merely DYSPOISSOMETER_UINT.
*/
    if((list_size>>DYSPOISSOMETER_UINT_SIZE_LOG2)==uint_count){
      list_base=DEBUG_MALLOC_PARANOID(list_size);
    }
  }
  return list_base;
}

void
dyspoissometer_uint_list_copy(DYSPOISSOMETER_UINT uint_count_minus_1, DYSPOISSOMETER_UINT uint_idx0_min, DYSPOISSOMETER_UINT uint_idx1_min, DYSPOISSOMETER_UINT *uint_list0_base, DYSPOISSOMETER_UINT *uint_list1_base){
/*
Copy a list of (DYSPOISSOMETER_UINT)s. To maximize portability and debuggability, this the only place in which Dyspoissometer uses memcpy(). 

In:

  uint_count_minus_1 is the number of items to copy, less one, on [0, DYSPOISSOMETER_UINT_MAX-1-MAX(uint_idx0_min, uint_idx1_min)].

  uint_idx0_min is index of the first item to read within *uint_list0_base, on [0, DYSPOISSOMETER_UINT_MAX-1].

  uint_idx1_min is index of the first item to write within *uint_list1_base, on [0, DYSPOISSOMETER_UINT_MAX-1].

  *uint_list0_base is the list to read.

  *uint_list1_base is the list to write.

Out:

  (uint_count_minus_1) items have been copied from &uint_list0_base[uint_idx0_min] to &uint_list1_base[uint_idx1_min].
*/
  ULONG list_size;

  list_size=(ULONG)(((ULONG)(uint_count_minus_1)+1)<<DYSPOISSOMETER_UINT_SIZE_LOG2);
  memcpy(&uint_list1_base[uint_idx1_min], &uint_list0_base[uint_idx0_min], (size_t)(list_size));
  return;
}

void *
dyspoissometer_uint_list_list_free(DYSPOISSOMETER_UINT list_idx_max, DYSPOISSOMETER_UINT **list_list_base){
/*
Free a list of lists using dyspoissometer_free().

In:

  list_list_base is the base of (a list of the bases of (list_idx_max+1) memory regions), all of which, including the parent base, to free. The parent base or any child base may be NULL.

Out:

  Returns NULL so that the caller can easily maintain the good practice of NULLing out invalid pointers.

  *list_list_base and its children are freed.
*/
  if(list_list_base){
    do{
      dyspoissometer_free(list_list_base[list_idx_max]);
    }while(list_idx_max--);
    list_list_base=dyspoissometer_free(list_list_base);
  }
  return list_list_base;
}

void
dyspoissometer_uint_list_zero(DYSPOISSOMETER_UINT uint_idx_max, DYSPOISSOMETER_UINT *uint_list_base){
/*
Zero a list of (DYSPOISSOMETER_UINT)s.

In:

  uint_idx_max is the number of (DYSPOISSOMETER_UINT)s to zero, less one, on [0, DYSPOISSOMETER_UINT_MAX-1].

  uint_list_base is the base at which to start zeroing, writable for (uint_idx_max+1) items.

Out:

  *uint_list_base contains (uint_idx_max+1) zeroes.
*/
  ULONG list_size;

  list_size=(ULONG)(((ULONG)(uint_idx_max)+1)<<DYSPOISSOMETER_UINT_SIZE_LOG2);
/*
We don't need to check bounds here because list_size cannot wrap because *uint_list_base was implicitly allocated successfully and therefore has a size which fits in a ULONG.
*/
  memset(uint_list_base, 0, (size_t)(list_size));
  return;
}

DYSPOISSOMETER_UINT *
dyspoissometer_uint_list_malloc_zero(DYSPOISSOMETER_UINT uint_idx_max){
/*
Allocate and zero a list of (DYSPOISSOMETER_UINT)s.

In:

  uint_idx_max is the number of (DYSPOISSOMETER_UINT)s to reserve and then zero, less one, on [0, DYSPOISSOMETER_UINT_MAX-1].

Out:

  Returns NULL on failure, else the base of a list of (uint_idx_max+1) zeroes.
*/
  DYSPOISSOMETER_UINT *list_base;

  list_base=dyspoissometer_uint_list_malloc(uint_idx_max);
  if(list_base){
    dyspoissometer_uint_list_zero(uint_idx_max, list_base);
  }
  return list_base;
}

void
dyspoissometer_uint_list_sort(DYSPOISSOMETER_UINT uint_idx_max, DYSPOISSOMETER_UINT *uint_list0_base, DYSPOISSOMETER_UINT *uint_list1_base){
/*
Sort a list of (DYSPOISSOMETER_UINT)s ascending as though they were unsigned integers equal to their implementation-specific representations, using double buffering for speed.

In:

  uint_idx_max is one less than the number of items in each list.

  *uint_list0_base contains (uint_idx_max+1) items to sort.

  *uint_list1_base is undefined and writable for (uint_idx_max+1) items.

Out:

  *uint_list0_base is sorted ascending.

  *uint_list1_base is undefined. 
*/
  u8 continue_status;
  ULONG list_size;
  DYSPOISSOMETER_UINT uint;
  DYSPOISSOMETER_UINT uint_idx_list_base[U8_SPAN];
  DYSPOISSOMETER_UINT uint_idx0;
  DYSPOISSOMETER_UINT uint_idx1;
  DYSPOISSOMETER_UINT *uint_list2_base;
  DYSPOISSOMETER_UINT uint_old;
  DYSPOISSOMETER_UINT uint_u8;
  DYSPOISSOMETER_UINT uint_u8_freq;
  DYSPOISSOMETER_UINT uint_u8_idx;
/*
Perform a bytewise radix sort of *uint_list0_base.
*/
  uint_u8_idx=0;
  do{
/*
*uint_idx_list_base will serve as a list of base indexes for all possible (u8)s. Zero it.
*/
    memset(uint_idx_list_base, 0, (size_t)(U8_SPAN<<DYSPOISSOMETER_UINT_SIZE_LOG2));
    continue_status=0;
/*
Count u8 frequencies and save them to *uint_idx_list_base. Move backwards to maximize cache hits in most cases.
*/
    uint_idx0=uint_idx_max;
    uint_old=uint_list0_base[uint_idx_max];
    do{
      uint=uint_list0_base[uint_idx0];
      continue_status=(u8)(continue_status|(uint_old<uint));
      uint_u8=((unsigned char *)(&uint))[uint_u8_idx];
      uint_old=uint;
      uint_idx_list_base[uint_u8]++;
    }while(uint_idx0--);
    if(continue_status){
/*
Convert uint_idx_list_base from a list of frequencies of (u8)s to a list of preceding cummulative sums of frequencies of (u8)s. This will tell us how to sort this particular u8 lane.
*/
      uint_u8=0;
      uint_idx0=0;
      do{
        uint_u8_freq=uint_idx_list_base[uint_u8];
        uint_idx_list_base[uint_u8]=uint_idx0;
        uint_idx0=(DYSPOISSOMETER_UINT)(uint_idx0+uint_u8_freq);
      }while((uint_u8++)!=U8_MAX);
/*
Sort *uint_list1_base ascending by the u8 lane designated by uint_u8_idx.
*/
      uint_idx0=0;
      do{
        uint=uint_list0_base[uint_idx0];
        uint_u8=((unsigned char *)(&uint))[uint_u8_idx];
        uint_idx1=uint_idx_list_base[uint_u8];
        uint_list1_base[uint_idx1]=uint;
        uint_idx1++;
        uint_idx_list_base[uint_u8]=uint_idx1;
      }while((uint_idx0++)!=uint_idx_max);
    }
/*
Swap list bases in order to accomplish double buffering.
*/
    uint_list2_base=uint_list0_base;
    uint_list0_base=uint_list1_base;
    uint_list1_base=uint_list2_base;
/*
continue_status should always be zero when the items have been sorted. However, due to floating-point undefinedness, it might be one even though the objects have actually been sorted when viewed as unsigned integers (which we could not have done because it would have been an aliasing violation, hence our use of (unsigned char *)).
*/
    uint_u8_idx++;
  }while(continue_status&&(uint_u8_idx!=sizeof(DYSPOISSOMETER_UINT)));
  if((uint_u8_idx&1)^(!continue_status)){
/*
Copy the object list back to what is, from the caller's perspective, uint_list0_base.
*/
    list_size=(ULONG)(uint_idx_max);
    list_size++;
    list_size=(ULONG)(list_size*sizeof(DYSPOISSOMETER_UINT));
    memcpy(uint_list0_base, uint_list1_base, (size_t)(list_size));
  }
  return;
}

DYSPOISSOMETER_NUMBER
dyspoissometer_logfreedom_sparse_get(DYSPOISSOMETER_UINT *freq_list0_base, DYSPOISSOMETER_UINT *freq_list1_base, DYSPOISSOMETER_UINT mask_max){
/*
Compute logfreedom directly from a frequency list (as opposed to a population list).

In:

  *freq_list0_base contains (mask_max+1) items, each giving the frequency of the corresponding zero-based mask. The sum of all frequencies must be on [1, DYSPOISSOMETER_UINT_MAX].

  *freq_list1_base is undefined but writable for the same size as *freq_list0_base.

  mask_mask is the number of items at freq_list0_base, less one.

Out:

  Returns the logfreedom of the population list implied by *freq_list0_base, guaranteed to be nonnegative.

  *freq_list0_base is undefined.

  *freq_list1_base is undefined.
*/
  DYSPOISSOMETER_UINT freq0;
  DYSPOISSOMETER_UINT freq1;
  DYSPOISSOMETER_UINT freq_idx_max;
  DYSPOISSOMETER_UINT freq_idx_min;
  DYSPOISSOMETER_UINT freq_idx0;
  DYSPOISSOMETER_UINT freq_idx1;
  DYSPOISSOMETER_UINT freq_idx2;
  DYSPOISSOMETER_UINT h0;
  DYSPOISSOMETER_NUMBER logfreedom;
  DYSPOISSOMETER_UINT mask;
  DYSPOISSOMETER_UINT mask_idx_max;
  DYSPOISSOMETER_UINT pop;

  logfreedom=0.0f;
  if(mask_max){
    freq_idx_max=0;
    mask_idx_max=0;
    mask=0;
    do{
/*
Determine the number of nonzero frequencies at freq_list0_base.
*/
      freq0=freq_list0_base[mask];
      if(freq0){
        freq_list1_base[freq_idx_max]=freq0;
        freq_idx_max++;
        mask_idx_max=(DYSPOISSOMETER_UINT)(mask_idx_max+freq0);
      }
    }while((mask++)!=mask_max);
    freq_idx_max--;
    mask_idx_max--;
/*
Set logfreedom to its first 2 terms, namely (LOG_SUM(Q)+LOG_SUM(Z)).
*/
    logfreedom=LOG_SUM_N_PLUS_1(mask_idx_max)+LOG_SUM_N_PLUS_1(mask_max);
    h0=(DYSPOISSOMETER_UINT)(mask_max-freq_idx_max);
/*
Account for h0, the number of masks which do not occur.
*/
    logfreedom-=LOG_SUM(h0);
    dyspoissometer_uint_list_sort(freq_idx_max, freq_list1_base, freq_list0_base);
/*
*freq_list1_base has been sorted ascending by frequency and contains only nonzero values.
*/
    freq_idx_min=0;
    do{
      freq0=freq_list1_base[freq_idx_min];
      freq_idx0=freq_idx_min;
      freq_idx2=freq_idx_max;
/*
Set [freq_idx_min, freq_idx0] to the interval of indexes into freq_list1_base at which frequency freq0 occurs. For example, if (freq0==9) and *freq_list1_base={1, 1, 3, 3, 4, 9, 9, 9, 10}, then [freq_idx_min, freq_idx0] would be set to [5, 7]. We do this by binary searching for the last index, freq_idx0, at which frequency freq0 occurs. Note that the index math was carefully crafted to avoid wrap and guarantee loop termination.
*/
      while(freq_idx0!=freq_idx2){
        freq_idx1=(DYSPOISSOMETER_UINT)(freq_idx2-((freq_idx2-freq_idx0)>>1));
        freq1=freq_list1_base[freq_idx1];
        if(freq0!=freq1){
          freq_idx2=(DYSPOISSOMETER_UINT)(freq_idx1-1);
        }else{
          freq_idx0=freq_idx1;
        }
      }
/*
freq_idx0 is the index of the last occurrence of freq0 at freq_list1_base, so the next greater index is the first occurrence of some frequency greater than freq0 (or the post index of freq_list1_base). Set pop to the population (H[i] in the logfreedom formula) of frequencies freq0, which is guaranteed to be nonzero (and cannot wrap, given the constraints imposed by In).
*/
      freq_idx0++;
      pop=(DYSPOISSOMETER_UINT)(freq_idx0-freq_idx_min);
      freq_idx_min=freq_idx0;
      logfreedom-=LOG_SUM(pop)+(pop*LOG_SUM(freq0));
    }while(freq_idx_min<=freq_idx_max);
    if(logfreedom<=0.0f){
/*
logfreedom might be negative (or negative zero) due to numerical error, so flush it to zero.
*/
      logfreedom=0.0f;
    }
  }
  return logfreedom;
}

DYSPOISSOMETER_NUMBER
dyspoissometer_logfreedom_dense_get(DYSPOISSOMETER_UINT freq_max_minus_1, DYSPOISSOMETER_UINT freq_min, DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_max, DYSPOISSOMETER_UINT *pop_list_base){
/*
Compute the logfreedom of a population list.

In:

  freq_max_minus_1 is one less than the maximum frequency represented at pop_list_base, on [freq_min, DYSPOISSOMETER_UINT_MAX-1].

  freq_min is the minimum frequency represented at pop_list_base. Conventionally, a population list always begins with the population of frequency one. However, in practice, real world distributions tend to be concentrated in a small range of frequencies, and not scattered over all possible frequencies. For this reason, freq_min is provided so as to eliminate the overhead of leading zeroes. The populations of all frequencies outside [freq_min, freq_max_minus_1+1] _except_ the population h0 of frequency zero will be assumed to be zero; h0 is implied by mask_max and the sum of the nonzero populations.

  mask_idx_max is one less than the sum-of-products N*pop_list_base[N-freq_min] where N is on [freq_min, freq_max_minus_1+1]. mask_idx_max itself is on [0, DYSPOISSOMETER_UINT_MAX-1].

  mask_max is the sum of all populations at pop_list_base which correspond to nonzero frequencies, plus (h0-1) -- in other words, the number of _possible_ masks, less one. Otherwise, all values are legal.

  pop_list_base is the base of (freq_max_minus_1-freq_min+2) items giving the populations of frequencies on [freq_min, freq_max_minus_1+1]. The sum of the populations must be (mask_max+1). The sum of (each population times its frequency) must be (mask_idx_max+1). May be readonly.

Out:

  Returns logfreedom, guaranteed to be nonnegative.
*/
  DYSPOISSOMETER_UINT freq;
  DYSPOISSOMETER_UINT h0;
  DYSPOISSOMETER_NUMBER log_sum;
  DYSPOISSOMETER_NUMBER logfreedom;
  DYSPOISSOMETER_UINT mask_count;
  DYSPOISSOMETER_UINT pop;
  DYSPOISSOMETER_UINT pop_idx;
  DYSPOISSOMETER_UINT pop_idx_max;

  logfreedom=0.0f;
  if(mask_max){
/*
Set logfreedom to its first 2 terms, namely (LOG_SUM(Q)+LOG_SUM(Z)).
*/
    mask_count=(DYSPOISSOMETER_UINT)(mask_idx_max+1);
    logfreedom=LOG_SUM(mask_count);
/*
It's often the case that the mask count equals the mask span; this would constitute a lambda-one poisson distribution (LOPD). Optimize accordingly because LOG_SUM() is expensive. Note that we actually use LOG_SUM_N_PLUS_1() because mask_max, unlike mask_idx_max, can wrap if incremented, so we first need to promote it to a DYSPOISSOMETER_NUMBER.
*/
    if(mask_idx_max==mask_max){
      logfreedom+=logfreedom;
    }else{
      logfreedom+=LOG_SUM_N_PLUS_1(mask_max);
    }
/*
Sum the logfreedom contributions over all nonzero frequencies while computing h0, the population corresponding to frequency zero.
*/
    pop_idx=!freq_min;
    pop_idx_max=(DYSPOISSOMETER_UINT)(freq_max_minus_1+1-freq_min);
/*
(mask_max+1) might wrap, but that's not a problem because we're guaranteed to have at least one population unit of nonzero frequency.
*/
    h0=(DYSPOISSOMETER_UINT)(mask_max+1);
/*
It's moderately likely that pop_list_base[pop_idx_max] is nonzero, but then every other population is zero until very close to the base of the list. So process that last entry first, then restart from the base, in order to maximize the likelihood that we can exit early by accounting for all mask_count masks.
*/
    pop=pop_list_base[pop_idx_max];
    if(pop){
/*
The logfreedom contribution of nonzero population H[N] at nonzero frequency N is: (-LOG_SUM(H[N])-H[N]*LOG_SUM(N)). (It's negative because we overshot by starting with (LOG_SUM(Q)+LOG_SUM(Z)).)
*/
      freq=(DYSPOISSOMETER_UINT)(freq_min+pop_idx_max);
      log_sum=LOG_SUM(freq);
      mask_count=(DYSPOISSOMETER_UINT)(mask_count-(pop*freq));
      h0=(DYSPOISSOMETER_UINT)(h0-pop);
      logfreedom-=LOG_SUM(pop)+(pop*log_sum);
    }
    if((pop_idx!=pop_idx_max)&&mask_count){
      do{
        pop=pop_list_base[pop_idx];
        if(pop){
          freq=(DYSPOISSOMETER_UINT)(pop_idx+freq_min);
          log_sum=LOG_SUM(freq);
          mask_count=(DYSPOISSOMETER_UINT)(mask_count-(pop*freq));
          h0=(DYSPOISSOMETER_UINT)(h0-pop);
          logfreedom-=LOG_SUM(pop)+(pop*log_sum);
          if(!mask_count){
/*
It's highly likely that the entire population is concentrated at the bottom of *pop_list_base. If we've accounted for all (mask_idx_max+1) masks, exit early.
*/
            break;
          }
        }
      }while((pop_idx++)<pop_idx_max);
    }
/*
Finally, account for the contribution due to the number of masks, h0, which do not occur and therefore have frequency zero.
*/
    logfreedom-=LOG_SUM(h0);
    if(logfreedom<=0.0f){
/*
logfreedom might be negative (or negative zero) due to numerical error, so flush it to zero.
*/
      logfreedom=0.0f;
    }
  }
  return logfreedom;
}

DYSPOISSOMETER_NUMBER
dyspoissometer_dyspoissonism_get(DYSPOISSOMETER_NUMBER logfreedom, DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_max){
/*
Given precomputed logfreedom, compute dyspoissonism.

In:

  mask_idx_max is the number of masks used to compute logfreedom, less one. On [0, DYSPOISSOMETER_UINT_MAX-1].

  logfreedom is the return value of dyspoissometer_logfreedom_dense_get() or dyspoissometer_logfreedom_sparse_get().

  mask_max the same value which was used to evaluate logfreedom.

Out:

  Returns dyspoissonism, guaranteed to be on [0.0f, 1.0f]. It is not, however, guaranteed to be at least the dyspoissonism floor, D0. This is because D0 is expensive if not generally intractable to find, and numerical error might lead us to return a value which is actually impossibly low -- zero in the extreme case. If precision is a problem, consider "make dyspoissometer_quad" for quad precision.
*/
  DYSPOISSOMETER_NUMBER dyspoissonism;
  DYSPOISSOMETER_NUMBER mask_count_x_log_mask_span;

  dyspoissonism=0.0f;
/*
By definition, (D=0) if (Q==0) or (Z==0). Otherwise proceed.
*/
  if(mask_idx_max&&mask_max){
/*
Compute the denominator, (Q log(Z)). Note that mask_max must be converted to a DYSPOISSOMETER_NUMBER prior to incrementation, whereas mask_idx_max need not, due to domain constraint differences.
*/
    mask_count_x_log_mask_span=LOG_N_PLUS_1(mask_max)*(DYSPOISSOMETER_UINT)(mask_idx_max+1);
/*
Make sure that dyspoissonism ends up on [0.0f, 1.0f] (which might not occur due to numerical error). If logfreedom is found to exceed mask_count_x_log_mask_span (which is theoretically impossible), then the most reasonable return value would be D0. However, we don't have D0 (and it might be too expensive to generate), so return 0.0f and hope that the caller gets a clue that something went nuts. It's the best we can do with no known lower bound.
*/
    if(logfreedom<mask_count_x_log_mask_span){
      dyspoissonism=1.0f-(logfreedom/mask_count_x_log_mask_span);
/*
Do some CYA for horrendous precision problems (and negative zero baloney) that shoudn't happen (but who knows, with C).
*/
      if(dyspoissonism<=0.0f){
        dyspoissonism=0.0f;
      }else if(1.0f<dyspoissonism){
        dyspoissonism=1.0f;
      }
    }
  }
  return dyspoissonism;
}

DYSPOISSOMETER_NUMBER
dyspoissometer_sparsity_get(DYSPOISSOMETER_NUMBER logfreedom, DYSPOISSOMETER_NUMBER logfreedom_max){
/*
Given precomputed logfreedom and logfreedom_max, compute information sparsity. The information _density_ would be simpler and slightly more precise to compute, but far less useful as a metric of randomness quality.

In:

  logfreedom is the return value of dyspoissometer_logfreedom_dense_get() or dyspoissometer_logfreedom_sparse_get().

  logfreedom_max is the return value of dyspoissometer_logfreedom_max_get() (watch for error conditions!).

Out:

  Returns information sparsity, guaranteed to be on [0.0f, 1.0f]. It's not that this is so complicated to compute, but this function provides safeguards against floating-point shenanigans.
*/
  DYSPOISSOMETER_NUMBER information_sparsity;

  information_sparsity=0.0f;
  if(logfreedom<logfreedom_max){
    information_sparsity=1.0f-(logfreedom/logfreedom_max);
/*
Do some CYA for horrendous precision problems (and negative zero baloney) that shoudn't happen (but who knows, with C).
*/
    if(information_sparsity<=0.0f){
      information_sparsity=0.0f;
    }else if(1.0f<information_sparsity){
      information_sparsity=1.0f;
    }
  }
  return information_sparsity;
}

u8
dyspoissometer_freq_list_update_autoscale(DYSPOISSOMETER_UINT *freq_list_base, DYSPOISSOMETER_UINT *mask_count_implied_base, DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT *mask_list_base, DYSPOISSOMETER_UINT mask_max){
/*
Accrue a mask list onto a frequency list, while autoscaling as necessary to avoid the catastrophic numerical effects of counter wrap. If you hate the idea of intelligent autoscaling, try dyspoissometer_freq_list_get() instead.

In:

  *mask_count_implied_base is the implied mask count at *freq_list_base. The implied mask count is simply the sum of all frequencies at *freq_list_base; multiple occurrences of the same frequency still count toward implied mask count. Because *freq_list_base is initialized to zeroes by dyspoissometer_uint_list_malloc_zero(mask_max), the initial value of this field must be zero. Subsequently, it should be passed back exactly as it was previously returned, which will never be zero. The only way to reset this cycle for a given *freq_list_base is to zero it with dyspoissometer_uint_list_zero(mask_max).

  mask_idx_max is the number of masks at mask_list_base to accrue to *freq_list_base, less one. On [0, DYSPOISSOMETER_UINT_MAX-1]. It may be less than the mask_idx_max that will ultimately be used to compute logfreedom.

  freq_list_base is the return value of dyspoissometer_uint_list_malloc_zero(mask_max). The caller must not modify it between calls to this function. It contains the frequency of mask N at index N, for N on [0, mask_max].

  mask_list_base is the return value of dyspoissometer_uint_list_malloc(mask_idx_max). *mask_list_base contains (mask_idx_max+1) (DYSPOISSOMETER_UINT)s to accrue to *freq_list_base. Eventually, after accrual is complete, the logfreedom of all of the accrued masks can be computed. All masks must be on [0, mask_max]. No verification is done to this effect, so the caller must perform this verification if the data is untrusted. If the data is temporal, store values first at index 0, and last at index mask_idx_max; this is important to maintaining relevant statistics after an autoscaling event.

  mask_max is the maximum possible mask (which is at least as large as the maximum mask at mask_list_base). (The minimum mask is implied to be zero.)

Out:

  Returns zero if freq_list_base is exactly consistent with its previous state plus the frequency additions due to all (mask_idx_max+1) masks at mask_list_base. Else one to indicate that autoscaling occurred at least once, in which case *mask_count_implied_base will contain the new implied mask count at freq_list_base. Autoscaling amounts to dividing all frequencies by 2, then rounding to nearest-or-even. Autoscaling will not occur until the implied mask count reaches DYSPOISSOMETER_UINT_MAX _and_ we still have at least one mask to accrue.

  *mask_count_implied_base is the sum of all frequencies at freq_list_base. On [1, DYSPOISSOMETER_UINT_MAX]. If mask accrual is complete, the caller should use (mask_idx_max==(*mask_count_implied_base-1)) for the purposes of computing logfreedom. But note that the logfreedom thus obtained can only be compared to other such values of the same *mask_count_implied_base and mask_max. But because *mask_count_implied_base is rather unpredictable after autoscaling, a good policy is to consider an autoscaling event to be a rebasing of logfreedom, from which point small increases in logfreedom can be accurately measured because the statistics of the system are well known after autoscaling (and usually long before that).

  *freq_list_base has been updated in a manner consistent with the return value and *mask_count_implied_base. Guaranteed not to be all zeroes. If autoscaling occurred, it resulted in more damage to the memory of masks with lower indexes into mask_list_base and less damage to those with higher indexes; thus in the case of temporal data, more recent data is considered more important. In other words, autoscaling can happen at any time, and in theory multiple times per call, so it's much more likely to affect the precision with which old statistics are recalled, as opposed to more recent ones. It's exponential decay, essentially.
*/
  u8 autoscale_status;
  DYSPOISSOMETER_UINT mask_count;
  DYSPOISSOMETER_UINT mask_count_implied;
  DYSPOISSOMETER_UINT mask_count_no_autoscale;
  DYSPOISSOMETER_UINT mask_idx;
  DYSPOISSOMETER_UINT mask_idx_min;
  DYSPOISSOMETER_UINT freq;
  DYSPOISSOMETER_UINT mask;

  autoscale_status=0;
  mask_count_implied=*mask_count_implied_base;
/*
It's important that we start accruing masks at mask_list_base[0] even though mask_list_base[mask_idx_max] would usually be more cache-efficient. The reason is that autoscaling causes deterioration of past statistics, and we want the distant past (lesser (mask_idx)s) to be less relevant than the near past (greater (mask_idx)s) with respect to approximating dyspoissonism. If this sounds ugly, then limit the sum of all mask counts accrued to at most DYSPOISSOMETER_UINT_MAX; setting the build flag DYSPOISSOMETER_NUMBER_QUAD at least lifts this ceiling. Or use dyspoissometer_freq_list_get().
*/
  mask_idx_min=0;
  do{
    if(mask_count_implied==DYSPOISSOMETER_UINT_MAX){
/*
We cannot accrue another mask without causing mask_count_implied to wrap. So divide all the frequencies by 2 using nearest-or-even rounding in order to minimize cummulative error.
*/
      autoscale_status=1;       
      mask=mask_max;
      mask_count_implied=0;
      do{
        freq=freq_list_base[mask];
        freq=(DYSPOISSOMETER_UINT)(((freq&3)==3)+(freq>>1));
        freq_list_base[mask]=freq;
        mask_count_implied=(DYSPOISSOMETER_UINT)(mask_count_implied+freq);
      }while(mask--);
    }
/*
Compute the number of masks we can accrue without causing another mask_count_implied wrap, which is guaranteed to be at least one. Then accrue as many masks as possible subject to this constraint. It's possible that we'll need to autoscale more than once in a single call. Note that mask_count_implied might be zero (like on the first call or after rounding down all ones to all zeroes), but it will never exit this function that way.
*/
    mask_count_no_autoscale=(DYSPOISSOMETER_UINT)(~mask_count_implied);
    mask_count=(DYSPOISSOMETER_UINT)((DYSPOISSOMETER_UINT)(mask_idx_max-mask_idx_min)+1);
    mask_count_no_autoscale=MIN(mask_count, mask_count_no_autoscale);
    mask_idx=mask_idx_min;
    mask_idx_min=(DYSPOISSOMETER_UINT)(mask_idx_min+mask_count_no_autoscale);
    mask_count_implied=(DYSPOISSOMETER_UINT)(mask_count_implied+mask_count_no_autoscale);
    do{
      mask=mask_list_base[mask_idx];
      freq=freq_list_base[mask];
      mask_idx++;
      mask_count_no_autoscale--;
      freq++;
      freq_list_base[mask]=freq;
    }while(mask_count_no_autoscale);
  }while(mask_idx_min<=mask_idx_max);
/*
Return mask_count_implied for use on the next call. It's on [1, DYSPOISSOMETER_UINT_MAX].
*/
  *mask_count_implied_base=mask_count_implied;
  return autoscale_status;
}

void
dyspoissometer_freq_list_get(DYSPOISSOMETER_UINT *freq_list_base, DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT *mask_list_base, DYSPOISSOMETER_UINT mask_max){
/*
Fill out a frequency list so as to make it consistent with a mask list after zeroing the former.

In:

  mask_idx_max is the number of masks at mask_list_base to accrue to *freq_list_base, less one. On [0, DYSPOISSOMETER_UINT_MAX-1].

  freq_list_base is the return value of dyspoissometer_uint_list_malloc(mask_max). Its contents are undefined.

  mask_list_base is the return value of dyspoissometer_uint_list_malloc(mask_idx_max). *mask_list_base contains (mask_idx_max+1) (DYSPOISSOMETER_UINT)s to accrue to *freq_list_base. All masks must be on [0, mask_max]. No verification is done to this effect, so the caller must perform this verification if the data is untrusted.

  mask_max is the maximum possible mask (which is at least as large as the maximum mask at mask_list_base). (The minimum mask is implied to be zero.)

Out:

  *freq_list_base is consistent with *mask_list_base. Its total is mask_idx_max+1.
*/
  DYSPOISSOMETER_UINT mask;
  DYSPOISSOMETER_UINT mask_idx;

  dyspoissometer_uint_list_zero(mask_max, freq_list_base);
  for(mask_idx=0; mask_idx<=mask_idx_max; mask_idx++){
    mask=mask_list_base[mask_idx];
    freq_list_base[mask]++;
  }
  return;
}

DYSPOISSOMETER_UINT
dyspoissometer_freq_max_minus_1_get(DYSPOISSOMETER_UINT *freq_list_base, DYSPOISSOMETER_UINT mask_max){
/*
Return the maximum frequency from a list of frequencies, less one.

In:

  *freq_list_base contains (mask_max+1) items, not all of which being zero, each giving the frequency of the corresponding mask.

  mask_max is as defined in dyspoissometer_pop_list_get().

Out:

  Returns the maximum of all (mask_max+1) frequencies at freq_list_base, less one. On [0, DYSPOISSOMETER_UINT_MAX-1].
*/
  DYSPOISSOMETER_UINT freq;
  DYSPOISSOMETER_UINT freq_max;
  DYSPOISSOMETER_UINT freq_max_minus_1;
  DYSPOISSOMETER_UINT mask;

  mask=mask_max;
  freq_max=0;
  do{
    freq=freq_list_base[mask];
    freq_max=MAX(freq_max, freq);
  }while(mask--);
  freq_max_minus_1=(DYSPOISSOMETER_UINT)(freq_max-1);
  return freq_max_minus_1;
}

DYSPOISSOMETER_UINT
dyspoissometer_pop_list_get(DYSPOISSOMETER_UINT *freq_list_base, DYSPOISSOMETER_UINT freq_max_minus_1, DYSPOISSOMETER_UINT mask_max, DYSPOISSOMETER_UINT *pop_list_base){
/*
Initialize a previously allocated population list to be consistent with a given frequency list. Note that this function creates conventional population lists, in the sense that they begin with the population of frequency one, but dyspoissometer_logfreedom_dense_get() can accept population lists with other base frequencies.

In:

  *freq_list_base contains a list of (mask_max+1) frequency items, each giving the frequency of the corresponding mask. The sum of its frequencies is on [1, DYSPOISSOMETER_UINT_MAX]. May be readonly.

  freq_max_minus_1 can be the return value of dyspoissometer_freq_max_minus_1_get(freq_list_base, mask_max) or any greater value on [0, DYSPOISSOMETER_UINT_MAX-1] which does not exceed the greatest index of pop_list_base.

  mask_max is the number of possible masks which could _possibly_ have nonzero frequency, less one.

  *pop_list_base is undefined and writable for (freq_max_minus_1+1) items.

Out:

  Returns h0, the number of masks having nonzero population.

  *pop_list_base contains a list of (freq_max_minus_1+1) populations of the frequencies at freq_list_base. The first gives the population of frequency one; the next gives the population of frequency 2; etc.
*/
  DYSPOISSOMETER_UINT freq;
  DYSPOISSOMETER_UINT h0;
  DYSPOISSOMETER_UINT mask;

  dyspoissometer_uint_list_zero(freq_max_minus_1, pop_list_base);
  mask=mask_max;
  h0=(DYSPOISSOMETER_UINT)(mask_max+1);
  do{
    freq=freq_list_base[mask];
    if(freq){
      pop_list_base[freq-1]++;
      h0--;
    }
  }while(mask--);
  return h0;
}

DYSPOISSOMETER_UINT *
dyspoissometer_pop_list_init(DYSPOISSOMETER_UINT *freq_list_base, DYSPOISSOMETER_UINT *freq_max_minus_1_base, DYSPOISSOMETER_UINT *h0_base, DYSPOISSOMETER_UINT mask_max){
/*
Allocate and fill a population list corresponding to a frequency list of unknown maximum frequency.

In:

  *freq_list_base is as defined in dyspoissometer_pop_list_get().

  *freq_max_minus_1_base is undefined.

  *h0_base is undefined.

  mask_max is as defined in dyspoissometer_pop_list_get().

Out:

  Returns NULL on failure, else the base of a list of populations of frequencies on [1, (*freq_max_minus_1_base)+1]. That is, the first DYSPOISSOMETER_UINT contains the number of ones at freq_list_base; the next contains the number of 2s, etc., up to and including the number of ((*freq_max_minus_1_base)+1)s. Note that, for purposes of dyspoissometer_logfreedom_dense_get(), this corresponds to (freq_min==1).

  *freq_max_minus_1_base is the return value of dyspoissometer_freq_max_minus_1_get(freq_list_base, mask_max).

  *h0_base contains h0, the population of symbols having frequency zero. The reason that h0 is returned independently is that, otherwise, the number of items in the returned population list could potentially wrap DYSPOISSOMETER_UINT_MAX. On [0, mask_max]. (The upper limit is _not_ (mask_max+1), which might wrap, because conditions on In:*freq_list_base require it to contain at least one nonzero entry.)
*/
  DYSPOISSOMETER_UINT freq_max_minus_1;
  DYSPOISSOMETER_UINT *pop_list_base;

  freq_max_minus_1=dyspoissometer_freq_max_minus_1_get(freq_list_base, mask_max);
  pop_list_base=dyspoissometer_uint_list_malloc(freq_max_minus_1);
  if(pop_list_base){
    *freq_max_minus_1_base=freq_max_minus_1;
    *h0_base=dyspoissometer_pop_list_get(freq_list_base, freq_max_minus_1, mask_max, pop_list_base);
  }
  return pop_list_base;
}

DYSPOISSOMETER_UINT *
dyspoissometer_pop_list_obtuse_init(DYSPOISSOMETER_UINT *freq_list_base, DYSPOISSOMETER_UINT freq_max_minus_1, DYSPOISSOMETER_UINT *h0_base, DYSPOISSOMETER_UINT mask_max){
/*
Allocate and fill a population list corresponding to a frequency list of predetermined maximum frequency.

In:

  *freq_list_base is as defined in dyspoissometer_pop_list_get(). May be readonly.

  freq_max_minus_1 is the maximum zero-based frequency which can be tracked at freq_list_base, less one. So for example if the maximum number of masks that might be used with freq_list_base is 7, then the maximum possible frequency is 7, so this value is 6. On [0, DYSPOISSOMETER_UINT_MAX-1].

  *h0_base is undefined.

  mask_max is as defined in dyspoissometer_pop_list_get().

Out:

  Returns the same value and conditions as dyspoissometer_pop_list_init().

  *h0_base is as defined in dyspoissometer_pop_list_init():Out.
*/
  DYSPOISSOMETER_UINT freq;
  DYSPOISSOMETER_UINT h0;
  DYSPOISSOMETER_UINT mask;
  DYSPOISSOMETER_UINT *pop_list_base;

  h0=0;
  pop_list_base=dyspoissometer_uint_list_malloc(freq_max_minus_1);
  if(pop_list_base){
    dyspoissometer_uint_list_zero(freq_max_minus_1, pop_list_base);
    mask=mask_max;
    do{
      freq=freq_list_base[mask];
      if(freq){
        pop_list_base[freq-1]++;
      }else{
        h0++;
      }
    }while(mask--);
  }
  *h0_base=h0;
  return pop_list_base;
}

DYSPOISSOMETER_NUMBER
dyspoissometer_poisson_term_get(DYSPOISSOMETER_UINT freq, DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_max){
/*
Evaluate the probability amplitude at a given frequency in a Poisson distribution with (lambda==((mask_idx_max+1)/(mask_max+1))). This function is intended for research purposes with regards to evaluating the efficiency of Poisson distributions as approximations of maximum-entropy distributions; as such, code simplicity and numerical accuracy is more important than speed.

In:

  mask_idx_max is as defined in dyspoissometer_logfreedom_max_get():In.

  mask_max is as defined in dyspoissometer_logfreedom_max_get():In.

  freq is the frequency at which to evaluate the probability amplitude.

Out:

  Returns the Poisson distribution probability amplitude at the indicated frequency.
*/
  DYSPOISSOMETER_NUMBER lambda;
  DYSPOISSOMETER_NUMBER lambda_log;
  DYSPOISSOMETER_UINT mask_count;
  DYSPOISSOMETER_NUMBER mask_span;
  DYSPOISSOMETER_NUMBER poisson;
  DYSPOISSOMETER_NUMBER poisson_log;

  mask_count=(DYSPOISSOMETER_UINT)(mask_idx_max+1);
  mask_span=(DYSPOISSOMETER_NUMBER)(mask_max)+1.0f;
  lambda=mask_count/mask_span;
  lambda_log=LOG(lambda);
  poisson_log=(freq*lambda_log)-lambda;
  poisson_log-=LOG_SUM(freq);
/*
Had we attempted to compute poisson directly, the factorial would have cost huge amounts of time and accuracy, hence the log method.
*/
  poisson=EXP(poisson_log);
  return poisson;
}

DYSPOISSOMETER_NUMBER
dyspoissometer_logfreedom_max_get(u64 iteration_max, DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_max, u64 *random_seed_base){
/*
Approximate the maximum logfreedom possible with a population list having an implied mask count of (mask_idx_max+1) and an implied mask count of (mask_max+1). The same could be done by finding a discrete Poisson distribution consistent with those constraints, but this approach seems to work better with less code complexity, yielding larger logfreedom especially at low mask counts or mask spans. It may be more computationally expensive, however.

In some cases, it may be faster to generate mask lists using dyspoissometer_mask_list_pseudorandom_get(), and just compute the logfreedom repeatedly in order to approximate the maximum thereof. However, in the general case, such mask lists would exhaust available memory.

Therefore, we use a Monte Carlo method involving piecemeal gradient riding with noise injection to prevent settling onto local maxima. Essentially, we maintain a neighboring pair of "up" frequencies, where the left transfers one population unit to the right; and a neighboring pair of "down" frequencies which do the opposite. "Up" and "down" neighbors are allowed to overlap at a single frequency so that the middle population changes by 2 while the outside populations change by one; in most cases, however, we have 4 disjoint frequencies, in which case all populations change by one. This technique permits us to perform minimal changes, in the sense of the population changes which would occur if only a single mask in a mask set were changed. We perform such a minimal change whether or not it is profitable, merely for the sake of avoiding the trap of local maxima. But if indeed the change proves to be profitable, then we ride a gradient whose first derivative is monotonic in order to find the number of such minimal changes which maximize the resulting increase in logfreedom. This process continues through a specified number of iterations, whereupon the maximum result is returned.

In:

  mask_idx_max is the maximum mask index, on [0, DYSPOISSOMETER_UINT_MAX-1].

  iteration_max is the number of iterations to run, less one. Note that unlike dyspoissometer_logfreedom_median_get(), this is a u64, not a DYSPOISSOMETER_UINT. Experimentation is the only way to determine its significance with respect to accuracy.

  mask_max is the maximum possible mask value. Unlike with other Dyspoissometer functions, no restriction is placed on it.

  *random_seed_base facilitates easy parallelization (for example, by using Spawn to create many copies of this function). On [1, ((DYSPOISSOMETER_MARSAGLIA_A-1)<<U32_BITS)+U32_MAX-1].

Out:

  Returns a rational but otherwise undefined negative value if we ran out of memory; else an approximation of maximum logfreedom, guaranteed to be nonnegative. On success, the return value is virtually certain to be less than the infinitely accurate maximum logfreedom, not the least of which because logfreedom is irrational, but also because it is derived from a population set consistent with the input constraints, which may not happen to have maximum logfreedom despite having been evolved for such.

  *random_seed_base contains an updated random seed.
*/
  DYSPOISSOMETER_UINT freq_down_right;
  DYSPOISSOMETER_NUMBER freq_expression;
  DYSPOISSOMETER_UINT freq_max;
  DYSPOISSOMETER_UINT freq_max_best;
  DYSPOISSOMETER_UINT freq_max_minus_1;
  DYSPOISSOMETER_UINT freq_min;
  DYSPOISSOMETER_UINT freq_min_best;
  DYSPOISSOMETER_UINT freq_min_delta;
  DYSPOISSOMETER_UINT freq_up_left;
  u64 iteration;
  u64 iteration_last_change;
  DYSPOISSOMETER_UINT lambda;
  DYSPOISSOMETER_UINT lambda_plus_1;
  DYSPOISSOMETER_UINT lambda_x_mask_span;
  DYSPOISSOMETER_NUMBER logfreedom_delta;
  DYSPOISSOMETER_NUMBER logfreedom_delta_delta;
  DYSPOISSOMETER_NUMBER logfreedom_delta_sum;
  DYSPOISSOMETER_NUMBER logfreedom_max;
  u32 marsaglia_c;
  u64 marsaglia_p;
  u32 marsaglia_x;
  DYSPOISSOMETER_UINT mask_count;
  DYSPOISSOMETER_UINT mask_count_part0;
  DYSPOISSOMETER_UINT mask_count_part1;
  DYSPOISSOMETER_UINT mask_span;
  u8 overflow_status;
  DYSPOISSOMETER_UINT pop;
  DYSPOISSOMETER_UINT pop_delta;
  DYSPOISSOMETER_UINT pop_delta_max;
  DYSPOISSOMETER_UINT pop_delta_max_minus_1;
  DYSPOISSOMETER_UINT pop_delta_min;
  DYSPOISSOMETER_UINT pop_down_left;
  DYSPOISSOMETER_UINT pop_down_right;
  DYSPOISSOMETER_UINT pop_idx;
  DYSPOISSOMETER_UINT pop_idx_count_half;
  DYSPOISSOMETER_UINT pop_idx_delta;
  DYSPOISSOMETER_UINT pop_idx_down;
  DYSPOISSOMETER_UINT pop_idx_lambda;
  DYSPOISSOMETER_UINT pop_idx_lambda_plus_1;
  DYSPOISSOMETER_UINT pop_idx_max;
  DYSPOISSOMETER_UINT pop_idx_max_best;
  DYSPOISSOMETER_UINT pop_idx_max_old;
  DYSPOISSOMETER_UINT pop_idx_up;
  DYSPOISSOMETER_UINT pop_lambda;
  DYSPOISSOMETER_UINT pop_lambda_plus_1;
  DYSPOISSOMETER_UINT *pop_list_base;
  DYSPOISSOMETER_UINT *pop_list_base_new;
  DYSPOISSOMETER_UINT *pop_list_best_base;
  DYSPOISSOMETER_UINT pop_nonzero_idx_max;
  DYSPOISSOMETER_UINT pop_nonzero_idx_max_best;
  DYSPOISSOMETER_UINT pop_nonzero_idx_min;
  DYSPOISSOMETER_UINT pop_nonzero_idx_min_best;
  DYSPOISSOMETER_UINT pop_nonzero_idx_span;
  DYSPOISSOMETER_UINT pop_up_left;
  DYSPOISSOMETER_UINT pop_up_right;
  u64 u64_product_hi;

  overflow_status=0;
  logfreedom_max=0.0f;
  if((2<mask_idx_max)&&(1<mask_max)){
    overflow_status=1;
    mask_count=(DYSPOISSOMETER_UINT)(mask_idx_max+1);
    mask_span=(DYSPOISSOMETER_UINT)(mask_max+1);
    lambda=0;
/*
Create an initial distribution at frequencies lambda and (lambda+1) in order to comply with mask_count and mask_span (where the latter might have wrapped). Although this is obviously far from the maximum logfreedom, it gives us a starting point from which zero-sum transfers will still be in constraint compliance.
*/
    pop_lambda=(DYSPOISSOMETER_UINT)(mask_span-mask_count);
    pop_lambda_plus_1=mask_count;
    if(mask_span){
      lambda=mask_count/mask_span;
      lambda_x_mask_span=(DYSPOISSOMETER_UINT)(lambda*mask_span);
      pop_lambda=(DYSPOISSOMETER_UINT)(pop_lambda+lambda_x_mask_span);
      pop_lambda_plus_1=(DYSPOISSOMETER_UINT)(pop_lambda_plus_1-lambda_x_mask_span);
    }
    pop_lambda=(DYSPOISSOMETER_UINT)(mask_span-pop_lambda_plus_1);
/*
Frequencies will be on [freq_min, freq_max], where pop_idx_max is always (freq_max-freq_min).
*/
    pop_idx_max=3;
    pop_idx_max_best=pop_idx_max;
    freq_max=pop_idx_max;
    freq_min=0;
/*
pop_idx_max needs to be large enough to accomodate frequency (lambda+1) with population pop_lambda_plus_1 followed by frequency (lambda+2) with population zero. (lambda+2) won't wrap because (lambda<DYSPOISSOMETER_UINT_SPAN_HALF).
*/
    lambda_plus_1=(DYSPOISSOMETER_UINT)(lambda+1);
    if(pop_idx_max<=lambda_plus_1){
      freq_max=(DYSPOISSOMETER_UINT)(lambda_plus_1+1);
      freq_min=(DYSPOISSOMETER_UINT)(lambda-1);
    }
    freq_max_best=freq_max;
    freq_min_best=freq_min;
    pop_list_base=dyspoissometer_uint_list_malloc_zero(pop_idx_max);
    pop_list_best_base=dyspoissometer_uint_list_malloc_zero(pop_idx_max);
    if(pop_list_base&&pop_list_best_base){
      overflow_status=0;
      pop_idx_lambda=(DYSPOISSOMETER_UINT)(lambda-freq_min);
      pop_idx_lambda_plus_1=(DYSPOISSOMETER_UINT)(lambda_plus_1-freq_min);
      pop_list_base[pop_idx_lambda]=pop_lambda;
      pop_list_base[pop_idx_lambda_plus_1]=pop_lambda_plus_1;
      pop_list_best_base[pop_idx_lambda]=pop_lambda;
      pop_list_best_base[pop_idx_lambda_plus_1]=pop_lambda_plus_1;
      pop_nonzero_idx_max=pop_idx_lambda_plus_1;
      pop_nonzero_idx_max_best=pop_idx_lambda_plus_1;
      pop_nonzero_idx_min=pop_idx_lambda;
      pop_nonzero_idx_min_best=pop_idx_lambda;
      pop_nonzero_idx_span=2;
/*
logfreedom_delta_sum will be the sum of the logfreedom deltas due to each individual transfer. When it goes positive, it's time to record a new "best" population list, from which the final logfreedom will be evaluated.
*/
      logfreedom_delta_sum=0.0f;
      marsaglia_p=*random_seed_base;
      pop_down_left=0;
      pop_down_right=0;
      pop_up_left=0;
      pop_up_right=0;
      iteration=0;
      iteration_last_change=0;
      do{
/*
Use the largest available 64-bit Marsaglia oscillator to produce random pairs (pop_idx_down, pop_idx_up), corresponding to the move-down and move-up indexes into the population list, respectively. The multiplication is kind of disgusting, and the pseudorandom sequence is somewhat biased, but it doesn't matter for our purposes here.
*/
        do{
          do{
            marsaglia_c=(u32)(marsaglia_p>>U32_BITS);
            marsaglia_x=(u32)(marsaglia_p);
            marsaglia_p=((u64)(marsaglia_x)*DYSPOISSOMETER_MARSAGLIA_A)+marsaglia_c;
            U64_PRODUCT_HI(marsaglia_p, (u64)(pop_nonzero_idx_span), u64_product_hi);
            pop_idx_down=(DYSPOISSOMETER_UINT)(u64_product_hi+pop_nonzero_idx_min);
            pop_down_right=pop_list_base[pop_idx_down];
          }while(!(pop_down_right&&(pop_idx_down|freq_min)));
          do{
            marsaglia_c=(u32)(marsaglia_p>>U32_BITS);
            marsaglia_x=(u32)(marsaglia_p);
            marsaglia_p=((u64)(marsaglia_x)*DYSPOISSOMETER_MARSAGLIA_A)+marsaglia_c;
            U64_PRODUCT_HI(marsaglia_p, (u64)(pop_nonzero_idx_span), u64_product_hi);
            pop_idx_up=(DYSPOISSOMETER_UINT)(u64_product_hi+pop_nonzero_idx_min);
            pop_up_left=pop_list_base[pop_idx_up];
            pop_idx_delta=(DYSPOISSOMETER_UINT)(pop_idx_down-pop_idx_up);
          }while((!pop_up_left)||((!pop_idx_delta)&&(pop_up_left==1)));
/*
If (pop_idx_delta==1), we're trying to exchange population units between 2 frequencies, which could result in a stuck state, in which case we need to start over with a new pop_idx_down.
*/
        }while(pop_idx_delta==1);
        if((!pop_idx_down)||(pop_idx_up==pop_idx_max)){
/*
We're attempting to slide off the left or right side of the distribution. If the former, then we already guaranteed above that we're not trying to move below frequency zero. So attempt to expand pop_list_base accordingly, in particular, by a factor of 2 so as to minimize memory map entropy.
*/
          pop_idx_max_old=pop_idx_max;
          pop_idx_count_half=(DYSPOISSOMETER_UINT)((pop_idx_max>>1)+1);
/*
Adjust [freq_min, freq_max] to fit this new interval size.
*/
          freq_max=(DYSPOISSOMETER_UINT)(freq_max+pop_idx_count_half);
          freq_min_delta=freq_min;
          freq_min=(DYSPOISSOMETER_UINT)(freq_min-pop_idx_count_half);
/*
Set pop_idx_max to a power of 2, less one. It will never wrap because the highest possible frequency is DYSPOISSOMETER_UINT_MAX, so we would never be required to expand beyond that based on the while() conditions above.
*/
          pop_idx_max=(DYSPOISSOMETER_UINT)((DYSPOISSOMETER_UINT)(pop_idx_max<<1)+1);
          if(((freq_min+pop_idx_count_half)<pop_idx_count_half)||(freq_max<pop_idx_count_half)){
            freq_max=pop_idx_max;
            freq_min=0;
          }
          freq_min_delta=(DYSPOISSOMETER_UINT)(freq_min_delta-freq_min);
          pop_list_base_new=dyspoissometer_uint_list_malloc_zero(pop_idx_max);
          if(!pop_list_base_new){
            overflow_status=1;
            break;
          }
/*
Copy the old population list to its appropriate place (usually, the middle) in the corresponding new list. Adjust (pop_idx_down, pop_idx_up) accordingly.
*/
          dyspoissometer_uint_list_copy(pop_idx_max_old, 0, freq_min_delta, pop_list_base, pop_list_base_new);
          dyspoissometer_free(pop_list_base);
          pop_list_base=pop_list_base_new;
          pop_idx_down=(DYSPOISSOMETER_UINT)(pop_idx_down+freq_min_delta);
          pop_idx_up=(DYSPOISSOMETER_UINT)(pop_idx_up+freq_min_delta);
          pop_nonzero_idx_max=(DYSPOISSOMETER_UINT)(pop_nonzero_idx_max+freq_min_delta);
          pop_nonzero_idx_min=(DYSPOISSOMETER_UINT)(pop_nonzero_idx_min+freq_min_delta);
        }
        pop_down_left=pop_list_base[pop_idx_down-1];
        pop_up_right=pop_list_base[pop_idx_up+1];
        freq_down_right=(DYSPOISSOMETER_UINT)(pop_idx_down+freq_min);
        freq_up_left=(DYSPOISSOMETER_UINT)(pop_idx_up+freq_min);
/*
We now have pop_down_left, pop_down_right, pop_up_left, and pop_up_right, which are the populations of frequencies (freq_down_right-1), freq_down_right, freq_up_left, and (freq_up_left+1), respectively. We're going to move at least one population unit from freq_down_right to (freq_down_right-1), and simultaneously from freq_up_left to (freq_up_left+1), thereby maintaining both the implied mask count and the implied mask span. We will do this regardless of whether or not doing so increases the logfreedom of *pop_list_base (because otherwise we might get stuck a local maximum which is not the global maximum). However, it's possible that moving pop_delta population units -- instead of just one -- in this zero-sum manner would increase the logfreedom even more. So if we force pop_delta to be at least one, then its optimum value (which results in maximum logfreedom) must exist on [1, MIN(pop_down_right, pop_up_left)] if (freq_down_right!=freq_up_left), or [1, pop_down_right>>1] otherwise.

The question is, which allowed value of pop_delta maximizes logfreedom, or equivalently, maximizes the delta in logfreedom between its current value and its value after shifting pop_delta population units in the aforementioned topology? To answer this, we need first find an expression for logfreedom_delta, the delta in logfreedom between where we are now (no population shift) and where will would be after shifting pop_delta. The logfreedom formula gives (ignoring terms common to both cases):

logfreedom_delta=
+logfreedom(pop_down_left+pop_delta, pop_down_right-pop_delta, pop_up_left-pop_delta, pop_up_right+pop_delta)
-logfreedom(pop_down_left, pop_down_right, pop_up_left, pop_up_right)

logfreedom_delta=
-LOG_SUM(pop_down_left+pop_delta)-(pop_down_left+pop_delta)*LOG_SUM(freq_down_right-1)
-LOG_SUM(pop_down_right-pop_delta)-(pop_down_right-pop_delta)*LOG_SUM(freq_down_right)
-LOG_SUM(pop_up_left-pop_delta)-(pop_up_left-pop_delta)*LOG_SUM(freq_up_left)
-LOG_SUM(pop_up_right+pop_delta)-(pop_up_right+pop_delta)*LOG_SUM(freq_up_left+1)
+LOG_SUM(pop_down_left)+pop_down_left*LOG_SUM(freq_down_right-1)
+LOG_SUM(pop_down_right)+pop_down_right*LOG_SUM(freq_down_right)
+LOG_SUM(pop_up_left)+pop_up_left*LOG_SUM(freq_up_left)
+LOG_SUM(pop_up_right)+pop_up_right*LOG_SUM(freq_up_left+1)

logfreedom_delta=
+pop_delta*(LOG(freq_down_right)-LOG(freq_up_left+1))
+LOG_SUM(pop_down_left)-LOG_SUM(pop_down_left+pop_delta)
+LOG_SUM(pop_down_right)-LOG_SUM(pop_down_right-pop_delta)
+LOG_SUM(pop_up_left)-LOG_SUM(pop_up_left-pop_delta)
+LOG_SUM(pop_up_right)-LOG_SUM(pop_up_right+pop_delta)

Note that we exploited the fact that LOG_SUM() expressions can sometimes sum to LOG() expressions.

If you look at the above expression, it's evident that the pop_delta terms come in just 2 types: (1) a negative LOG_SUM() which becomes increasingly negative at an accelerating rate and (2) another negative LOG_SUM() which becomes increasingly positive at a decelerating rate. The sum of such functions is an inverted "U". This is good news because it means that either: (1) we can binary search for the point at which the rate of change in logfreedom_delta (logfreedom_delta_delta) is as close to zero as possible or (2) the maximum logfreedom is found at (pop_delta==1). Note that logfreedom_delta is simply the logfreedom minus a constant -- _not_ the discrete derivative of logfreedom; however, logfreedom_delta_delta is indeed the discrete derivative of logfreedom_delta. Anyway, we now need to derive logfreedom_delta_delta:

logfreedom_delta_delta=
+logfreedom(pop_down_left+pop_delta+1, pop_down_right-pop_delta-1, pop_up_left-pop_delta-1, pop_up_right+pop_delta+1)
-logfreedom(pop_down_left+pop_delta, pop_down_right-pop_delta, pop_up_left-pop_delta, pop_up_right+pop_delta)

logfreedom_delta_delta=
+(pop_delta+1)*(LOG(freq_down_right)-LOG(freq_up_left+1))
+LOG_SUM(pop_down_left)-LOG_SUM(pop_down_left+pop_delta+1)
+LOG_SUM(pop_down_right)-LOG_SUM(pop_down_right-pop_delta-1)
+LOG_SUM(pop_up_left)-LOG_SUM(pop_up_left-pop_delta-1)
+LOG_SUM(pop_up_right)-LOG_SUM(pop_up_right+pop_delta+1)
-pop_delta*(LOG(freq_down_right)-LOG(freq_up_left+1))
-LOG_SUM(pop_down_left)+LOG_SUM(pop_down_left+pop_delta)
-LOG_SUM(pop_down_right)+LOG_SUM(pop_down_right-pop_delta)
-LOG_SUM(pop_up_left)+LOG_SUM(pop_up_left-pop_delta)
-LOG_SUM(pop_up_right)+LOG_SUM(pop_up_right+pop_delta)

logfreedom_delta_delta=
+LOG(freq_down_right)-LOG(freq_up_left+1)
-LOG(pop_down_left+pop_delta+1)
+LOG(pop_down_right-pop_delta)
+LOG(pop_up_left-pop_delta)
-LOG(pop_up_right+pop_delta+1)

Now, this is all complicated by the possibility that we have (pop_idx_down==pop_idx_up) or (pop_idx_down=(pop_idx_up+2)), in which case ((pop_down_right==pop_up_left) and (freq_down_right==freq_up_left)) or ((pop_down_left==pop_up_right) and (freq_up_left==(freq_down_right-2))), respectively. In the case of (pop_idx_down==pop_idx_up), going back to the logfreedom formula and progressing in a similar manner produces:

logfreedom_delta=
+pop_delta*(LOG(freq_down_right)-LOG(freq_down_right+1))
+LOG_SUM(pop_down_left)-LOG_SUM(pop_down_left+pop_delta)
+LOG_SUM(pop_down_right)-LOG_SUM(pop_down_right-(pop_delta<<1))
+LOG_SUM(pop_up_right)-LOG_SUM(pop_up_right+pop_delta)

and

logfreedom_delta_delta=
+LOG(freq_down_right)-LOG(freq_down_right+1)
-LOG(pop_down_left+pop_delta+1)
+LOG(pop_down_right-(pop_delta<<1)-1)
+LOG(pop_down_right-(pop_delta<<1))
-LOG(pop_up_right+pop_delta+1)

Or in the case of (pop_idx_down=(pop_idx_up+2)):

logfreedom_delta=
+pop_delta*(LOG(freq_down_right)-LOG(freq_down_right-1))
+LOG_SUM(pop_down_left)-LOG_SUM(pop_down_left+(pop_delta<<1))
+LOG_SUM(pop_down_right)-LOG_SUM(pop_down_right-pop_delta)
+LOG_SUM(pop_up_left)-LOG_SUM(pop_up_left-pop_delta)

logfreedom_delta_delta=
+LOG(freq_down_right)-LOG(freq_down_right-1)
-LOG(pop_down_left+(pop_delta<<1)+1)
-LOG(pop_down_left+(pop_delta<<1)+2)
+LOG(pop_down_right-pop_delta)
+LOG(pop_up_left-pop_delta)

Having said all that, we will use multiplication and division to accelerate the computation, using (LOG(A*B)==(LOG(A)+LOG(B))) and (LOG(A/B)==(LOG(A)-LOG(B))).

The following code uses logfreedom_delta and logfreedom_delta_delta to compute the value of pop_delta which induces the most positive change in logfreedom_delta (which might still be negative):
*/
        freq_expression=LOG((DYSPOISSOMETER_NUMBER)(freq_down_right)/(DYSPOISSOMETER_UINT)(freq_up_left+1));
        logfreedom_delta=freq_expression-LOG((DYSPOISSOMETER_NUMBER)(pop_down_left+1));
/*
Handle each of the aforementioned 3 possible "up" and "down" interaction topologies, with the most likely first.
*/
        if(2<pop_idx_delta){
          logfreedom_delta+=LOG((DYSPOISSOMETER_NUMBER)(pop_down_right)*pop_up_left/(DYSPOISSOMETER_UINT)(pop_up_right+1));
        }else if(!pop_idx_delta){
          logfreedom_delta+=LOG((DYSPOISSOMETER_NUMBER)(pop_down_right-1)*pop_down_right/(DYSPOISSOMETER_UINT)(pop_up_right+1));
        }else{
          logfreedom_delta-=LOG((DYSPOISSOMETER_NUMBER)(pop_down_left+2)/((DYSPOISSOMETER_NUMBER)(pop_down_right)*pop_up_left));
        }
        pop_delta_max=MIN(pop_down_right, pop_up_left);
        if(pop_idx_down==pop_idx_up){
          pop_delta_max>>=1;
        }
        pop_delta_min=1;
/*
Set pop_delta_max_minus_1 to (pop_delta_max-1) because we can't evaluate logfreedom_delta_delta at pop_delta_max because it's a discrete first derivative, meaning that it's the difference of 2 neighboring sample points, so we need to bound our search accordingly.
*/
        pop_delta_max_minus_1=(DYSPOISSOMETER_UINT)(pop_delta_max-pop_delta_min);
        if(pop_delta_max_minus_1){
          logfreedom_delta_delta=freq_expression;
          if(2<pop_idx_delta){
            logfreedom_delta_delta-=LOG((DYSPOISSOMETER_NUMBER)(pop_down_left+2)*(DYSPOISSOMETER_UINT)(pop_up_right+2)/((DYSPOISSOMETER_NUMBER)(pop_down_right-1)*(DYSPOISSOMETER_UINT)(pop_up_left-1)));
          }else if(!pop_idx_delta){
            logfreedom_delta_delta-=LOG((DYSPOISSOMETER_NUMBER)(pop_down_left+2)*(DYSPOISSOMETER_UINT)(pop_up_right+2)/((DYSPOISSOMETER_NUMBER)(pop_down_right-3)*(DYSPOISSOMETER_UINT)(pop_down_right-2)));
          }else{
            logfreedom_delta_delta-=LOG((DYSPOISSOMETER_NUMBER)(pop_down_left+3)*(DYSPOISSOMETER_UINT)(pop_down_left+4)/((DYSPOISSOMETER_NUMBER)(pop_down_right-1)*(DYSPOISSOMETER_UINT)(pop_up_left-1)));
          }
/*
If the discrete derivative of logfreedom_delta is positive, then we should binary search for the apex of the inverted "U". Otherwise stick with (pop_delta_min==1) because there is no improvement to be found.
*/
          if(0.0f<logfreedom_delta_delta){
            while(pop_delta_max_minus_1!=pop_delta_min){
              pop_delta=(DYSPOISSOMETER_UINT)(pop_delta_max_minus_1-((pop_delta_max_minus_1-pop_delta_min)>>1));
              logfreedom_delta_delta=freq_expression;
              if(2<pop_idx_delta){
                logfreedom_delta_delta-=LOG((DYSPOISSOMETER_NUMBER)((DYSPOISSOMETER_UINT)(pop_down_left+pop_delta)+1)*(DYSPOISSOMETER_UINT)((DYSPOISSOMETER_UINT)(pop_up_right+pop_delta)+1)/((DYSPOISSOMETER_NUMBER)(pop_down_right-pop_delta)*(DYSPOISSOMETER_UINT)(pop_up_left-pop_delta)));
              }else if(!pop_idx_delta){
                logfreedom_delta_delta-=LOG((DYSPOISSOMETER_NUMBER)((DYSPOISSOMETER_UINT)(pop_down_left+pop_delta)+1)*(DYSPOISSOMETER_UINT)((DYSPOISSOMETER_UINT)(pop_up_right+pop_delta)+1)/((DYSPOISSOMETER_NUMBER)((DYSPOISSOMETER_UINT)(pop_down_right-(DYSPOISSOMETER_UINT)(pop_delta<<1))-1)*(DYSPOISSOMETER_UINT)(pop_down_right-(DYSPOISSOMETER_UINT)(pop_delta<<1))));
              }else{
                logfreedom_delta_delta-=LOG((DYSPOISSOMETER_NUMBER)((DYSPOISSOMETER_UINT)(pop_down_left+(DYSPOISSOMETER_UINT)(pop_delta<<1))+1)*(DYSPOISSOMETER_UINT)((DYSPOISSOMETER_UINT)(pop_down_left+(pop_delta<<1))+2)/((DYSPOISSOMETER_NUMBER)(pop_down_right-pop_delta)*(DYSPOISSOMETER_UINT)(pop_up_left-pop_delta)));
              }
              if(0.0f<logfreedom_delta_delta){
                pop_delta_min=pop_delta;
              }else{
                pop_delta_max_minus_1=(DYSPOISSOMETER_UINT)(pop_delta-1);
              }
            }
/*
logfreedom_delta_delta is positive, so go with the righthand pop_delta instead of the lefthand one.
*/
            pop_delta_min++;
          }
        }
        pop_delta=pop_delta_min;
/*
Evaluate the logfreedom_delta, now that we've found the optimal pop_delta.
*/
        logfreedom_delta=LOG_SUM(pop_down_left)+LOG_SUM(pop_down_right)+(pop_delta*freq_expression);
        if(2<pop_idx_delta){
          logfreedom_delta+=LOG_SUM(pop_up_left)+LOG_SUM(pop_up_right);
          pop_down_left=(DYSPOISSOMETER_UINT)(pop_down_left+pop_delta);
          pop_down_right=(DYSPOISSOMETER_UINT)(pop_down_right-pop_delta);
          pop_up_left=(DYSPOISSOMETER_UINT)(pop_up_left-pop_delta);
          pop_up_right=(DYSPOISSOMETER_UINT)(pop_up_right+pop_delta);
          logfreedom_delta-=LOG_SUM(pop_down_left)+LOG_SUM(pop_down_right)+LOG_SUM(pop_up_left)+LOG_SUM(pop_up_right);
        }else if(!pop_idx_delta){
          logfreedom_delta+=LOG_SUM(pop_up_right);
          pop_down_left=(DYSPOISSOMETER_UINT)(pop_down_left+pop_delta);
          pop_down_right=(DYSPOISSOMETER_UINT)(pop_down_right-(pop_delta<<1));
          pop_up_left=pop_down_right;
          pop_up_right=(DYSPOISSOMETER_UINT)(pop_up_right+pop_delta);
          logfreedom_delta-=LOG_SUM(pop_down_left)+LOG_SUM(pop_down_right)+LOG_SUM(pop_up_right);
        }else{
          logfreedom_delta+=LOG_SUM(pop_up_left);
          pop_down_left=(DYSPOISSOMETER_UINT)(pop_down_left+(pop_delta<<1));
          pop_down_right=(DYSPOISSOMETER_UINT)(pop_down_right-pop_delta);
          pop_up_left=(DYSPOISSOMETER_UINT)(pop_up_left-pop_delta);
          pop_up_right=pop_down_left;
          logfreedom_delta-=LOG_SUM(pop_down_left)+LOG_SUM(pop_down_right)+LOG_SUM(pop_up_left);
        }
        pop_list_base[pop_idx_down-1]=pop_down_left;
        pop_list_base[pop_idx_down]=pop_down_right;
        pop_list_base[pop_idx_up]=pop_up_left;
        pop_list_base[pop_idx_up+1]=pop_up_right;
/*
All nonzero populations lie on indexes on [pop_nonzero_idx_min, pop_nonzero_idx_max]. Some zero populations also lie on that interval, which is OK and not worth the trouble to trim. Update them as needed. All we're trying to do, in the interest of speed, is to minimize memory and time consumed dealing with zero populations at the periphery of the distribution.
*/
        if(pop_idx_down==pop_nonzero_idx_min){
          pop_nonzero_idx_min--;
          pop_nonzero_idx_span++;
        }else if((pop_idx_down==pop_nonzero_idx_max)&&(!pop_down_right)&&(pop_idx_up<pop_idx_down)){
          pop_nonzero_idx_max--;
          pop_nonzero_idx_span--;
        }
        if(pop_idx_up==pop_nonzero_idx_max){
          pop_nonzero_idx_max++;
          pop_nonzero_idx_span++;
        }else if((pop_idx_up==pop_nonzero_idx_min)&&(!pop_up_left)&&(pop_idx_up<pop_idx_down)){
          pop_nonzero_idx_min++;
          pop_nonzero_idx_span--;
        }
        logfreedom_delta_sum+=logfreedom_delta;
        if(0.0f<logfreedom_delta_sum){
/*
The sum of all the (logfreedom_delta)s that we've incurred by a combination of logfreedom gradient riding and random single unit population transfers has resulted in a population list with a logfreedom greater than the existing best. So copy pop_list_base to pop_list_best_base where they differ. Note that the resulting logfreedom might not increase, and in theory could even decrease, due to numerical error. But we'll do the best we can.
*/
          logfreedom_delta_sum=0.0f;
          iteration_last_change=iteration;
          if(pop_idx_max!=pop_idx_max_best){
            dyspoissometer_free(pop_list_best_base);
            pop_list_best_base=dyspoissometer_uint_list_malloc_zero(pop_idx_max);
            if(!pop_list_best_base){
              overflow_status=1;
              break;
            }
            pop_idx_max_best=pop_idx_max;
          }
          dyspoissometer_uint_list_copy(pop_idx_max, 0, 0, pop_list_base, pop_list_best_base);
          pop_nonzero_idx_max_best=pop_nonzero_idx_max;
          pop_nonzero_idx_min_best=pop_nonzero_idx_min;
          freq_max_best=freq_max;
          freq_min_best=freq_min;
        }else{
/*
If at least pop_nonzero_idx_span iterations have passed since the last "best" was found, then, empirically, it's time to revert to the known best and try mutating it some other way. There's surely some better thresholding value, but this seems to work well.
*/
          if(pop_nonzero_idx_span<=(iteration-iteration_last_change)){
/*
Revert to the known best and try again. This is simple because *pop_list_base is at least as large as *pop_list_best_base.
*/
            logfreedom_delta_sum=0.0f;
            iteration_last_change=iteration;
            dyspoissometer_uint_list_copy(pop_idx_max_best, 0, 0, pop_list_best_base, pop_list_base);
            pop_idx_max=pop_idx_max_best;
            pop_nonzero_idx_max=pop_nonzero_idx_max_best;
            pop_nonzero_idx_min=pop_nonzero_idx_min_best;
            freq_max=freq_max_best;
            freq_min=freq_min_best;
            pop_nonzero_idx_span=(DYSPOISSOMETER_UINT)((DYSPOISSOMETER_UINT)(pop_nonzero_idx_max_best-pop_nonzero_idx_min_best)+1);
          }
        }
      }while((iteration++)!=iteration_max);
      *random_seed_base=marsaglia_p;
      if(!overflow_status){
/*
Compute the logfreedom of the best population list.
*/
        freq_max_minus_1=(DYSPOISSOMETER_UINT)((DYSPOISSOMETER_UINT)(freq_min_best+pop_nonzero_idx_max_best)-1);
        logfreedom_max=dyspoissometer_logfreedom_dense_get(freq_max_minus_1, freq_min_best, mask_idx_max, mask_max, pop_list_best_base);
/*
Considering the complexity of this process, a little paranoia is reasonable. Check to ensure that the best population list is actually consistent with mask_idx_max and mask_max.
*/
        pop_idx=pop_nonzero_idx_min_best;
        do{
          pop=pop_list_best_base[pop_idx];
          mask_idx_max=(DYSPOISSOMETER_UINT)(mask_idx_max-(pop*(pop_idx+freq_min_best)));
          mask_max=(DYSPOISSOMETER_UINT)(mask_max-pop);
        }while((pop_idx++)!=pop_nonzero_idx_max_best);
        overflow_status=!!((++mask_idx_max)|(++mask_max));
      }
    }
    dyspoissometer_free(pop_list_best_base);
    dyspoissometer_free(pop_list_base);
  }else if(mask_max){
    if(2<mask_idx_max){
/*
There are exactly 2 masks. Maximum logfreedom occurs when as close as possible to half of them are in either state. Compute the log of the possible number of combinations.
*/
      mask_count=(DYSPOISSOMETER_UINT)(mask_idx_max+1);
      mask_count_part0=mask_count>>1;
      mask_count_part1=(DYSPOISSOMETER_UINT)(mask_count-mask_count_part0);
      logfreedom_max=LOG_SUM(mask_count)-LOG_SUM(mask_count_part0)-LOG_SUM(mask_count_part1);
      if(mask_count_part0!=mask_count_part1){
/*
Mask count is odd, so we need to double the way count (i.e. add (ln 2) to the logfreedom) because we could have majority zeroes or majority ones.
*/
        logfreedom_max+=LOG(2.0f);
      }
    }else{
/*
mask_idx_max is too small to deal with. Compute logfreedom_max formulaically.

With only one mask index (mask_idx_max==0), the logfreedom is just the log of the number of possible masks. (H=={1}), meaning that we have one mask with frequency one, and no other masks. In this case, set (logfreedom_max==LOG(mask_max+1)).

With 2 mask indexes, and in the case of maximum logfreedom, we have mask_span choices for the first and mask_max choices for the 2nd; (H=={2}). In this case, set (logfreedom_max==LOG(mask_max+1)+LOG(mask_max)).

With 3 mask indexes, logfreedom is maximized when (H=={1, 1}) or (H=={3}). If (4<=mask_max), then the latter provides maximum logfreedom; otherwise the former does so. So if (4<=mask_max), set (logfreedom_max==LOG(mask_max+1)+LOG(mask_max)+LOG(mask_max-1)), else set (logfreedom_max==LOG(mask_max+1)+LOG(mask_max)+LOG(3.0f)).
*/
      logfreedom_max=LOG_N_PLUS_1(mask_max);
      if(mask_idx_max){
        logfreedom_max+=LOG(mask_max);
        if(mask_idx_max==2){
          logfreedom_max+=LOG((DYSPOISSOMETER_NUMBER)(MAX(4, mask_max)-1));
        }
      }
    }
  }
  if(overflow_status){
/*
It's impossible to have a negative logfreedom, legitmately. Flag overflow to the caller.
*/
    logfreedom_max=-1.0f;
  }else if(logfreedom_max<=0.0f){
/*
logfreedom might be negative (or negative zero) due to numerical error, so flush it to zero.
*/
    logfreedom_max=0.0f;
  }
  return logfreedom_max;
}

void
dyspoissometer_mask_list_pseudorandom_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT *mask_list_base, DYSPOISSOMETER_UINT mask_max, u64 *random_seed_base){
/*
Generate a pseudorandom mask list consistent with a given mask count and mask span.

In:

  mask_idx_max is as defined in dyspoissometer_logfreedom_max_get().

  *mask_list_base is as it was previously returned from this function, or (mask_idx_max+1) zeroes on the first call.

  mask_max is as defined in dyspoissometer_logfreedom_max_get().

  *random_seed_base is as defined in dyspoissometer_logfreedom_max_get():In.

Out:

  Returns the value of random_seed to use on the next call.

  *mask_list_base will contain a mask list consistent with mask_idx_max and mask_max, which was selected from an essentially uniform distribution of all possible mask lists.

  *random_seed_base is as defined in dyspoissometer_logfreedom_max_get():Out.
*/
  u32 marsaglia_c;
  u64 marsaglia_p;
  u32 marsaglia_x;
  DYSPOISSOMETER_UINT mask;
  DYSPOISSOMETER_UINT mask_idx;
  DYSPOISSOMETER_UINT mask_span;

  mask_idx=0;
  marsaglia_p=*random_seed_base;
  mask_span=(DYSPOISSOMETER_UINT)(mask_max+1);
/*
Use modulo summation, over mask_span, of the previous mask value and a Marsaglia pseudorandom value to obtain new masks. This leads to essentially unbiased mask lists, despite the slight high bit bias of the oscillator.
*/
  if(!(mask_max&mask_span)){
/*
mask_span is a power of 2, so we can just use "&" to obtain masks instead of expensive "%".
*/
    do{
      mask=mask_list_base[mask_idx];
      marsaglia_c=(u32)(marsaglia_p>>U32_BITS);
      marsaglia_x=(u32)(marsaglia_p);
      mask=(DYSPOISSOMETER_UINT)((marsaglia_p+mask)&mask_max);
      marsaglia_p=((u64)(marsaglia_x)*DYSPOISSOMETER_MARSAGLIA_A)+marsaglia_c;
      mask_list_base[mask_idx]=mask;
    }while((mask_idx++)!=mask_idx_max);
  }else{
    do{
      mask=mask_list_base[mask_idx];
      marsaglia_c=(u32)(marsaglia_p>>U32_BITS);
      marsaglia_x=(u32)(marsaglia_p);
      mask=(DYSPOISSOMETER_UINT)((marsaglia_p+mask)%mask_span);
      marsaglia_p=((u64)(marsaglia_x)*DYSPOISSOMETER_MARSAGLIA_A)+marsaglia_c;
      mask_list_base[mask_idx]=mask;
    }while((mask_idx++)!=mask_idx_max);
  }
  *random_seed_base=marsaglia_p;
  return;
}

void
dyspoissometer_number_list_sort(DYSPOISSOMETER_UINT number_idx_max, DYSPOISSOMETER_NUMBER *number_list0_base, DYSPOISSOMETER_NUMBER *number_list1_base){
/*
Sort a list of (DYSPOISSOMETER_NUMBER)s ascending as though they were unsigned integers equal to their implementation-specific representations, using double buffering for speed.
In:

  number_idx_max is one less than the number of items in each list.

  *number_list0_base contains (number_idx_max+1) items to sort.

  *number_list1_base is undefined and writable for (number_idx_max+1) items.

Out:

  *number_list0_base is sorted ascending.

  *number_list1_base is undefined. 
*/
  u8 continue_status;
  ULONG list_size;
  DYSPOISSOMETER_NUMBER number;
  DYSPOISSOMETER_UINT number_idx_list_base[U8_SPAN];
  DYSPOISSOMETER_UINT number_idx0;
  DYSPOISSOMETER_UINT number_idx1;
  DYSPOISSOMETER_NUMBER *number_list2_base;
  DYSPOISSOMETER_NUMBER number_old;
  DYSPOISSOMETER_UINT number_u8;
  DYSPOISSOMETER_UINT number_u8_freq;
  DYSPOISSOMETER_UINT number_u8_idx;
/*
Perform a bytewise radix sort of *number_list0_base.
*/
  number_u8_idx=0;
  do{
/*
*number_idx_list_base will serve as a list of base indexes for all possible (u8)s. Zero it.
*/
    memset(number_idx_list_base, 0, (size_t)(U8_SPAN<<DYSPOISSOMETER_UINT_SIZE_LOG2));
    continue_status=0;
/*
Count u8 frequencies and save them to *number_idx_list_base. Move backwards to maximize cache hits in most cases.
*/
    number_idx0=number_idx_max;
    number_old=number_list0_base[number_idx_max];
    do{
      number=number_list0_base[number_idx0];
      continue_status=(u8)(continue_status|(number_old<number));
      number_u8=((unsigned char *)(&number))[number_u8_idx];
      number_old=number;
      number_idx_list_base[number_u8]++;
    }while(number_idx0--);
    if(continue_status){
/*
Convert number_idx_list_base from a list of frequencies of (u8)s to a list of preceding cummulative sums of frequencies of (u8)s. This will tell us how to sort this particular u8 lane.
*/
      number_u8=0;
      number_idx0=0;
      do{
        number_u8_freq=number_idx_list_base[number_u8];
        number_idx_list_base[number_u8]=number_idx0;
        number_idx0=(DYSPOISSOMETER_UINT)(number_idx0+number_u8_freq);
      }while((number_u8++)!=U8_MAX);
/*
Sort *number_list1_base ascending by the u8 lane designated by number_u8_idx.
*/
      number_idx0=0;
      do{
        number=number_list0_base[number_idx0];
        number_u8=((unsigned char *)(&number))[number_u8_idx];
        number_idx1=number_idx_list_base[number_u8];
        number_list1_base[number_idx1]=number;
        number_idx1++;
        number_idx_list_base[number_u8]=number_idx1;
      }while((number_idx0++)!=number_idx_max);
    }
/*
Swap list bases in order to accomplish double buffering.
*/
    number_list2_base=number_list0_base;
    number_list0_base=number_list1_base;
    number_list1_base=number_list2_base;
/*
continue_status should always be zero when the items have been sorted. However, due to floating-point undefinedness, it might be one even though the objects have actually been sorted when viewed as unsigned integers (which we could not have done because it would have been an aliasing violation, hence our use of (unsigned char *)).
*/
    number_u8_idx++;
  }while(continue_status&&(number_u8_idx!=sizeof(DYSPOISSOMETER_NUMBER)));
  if((number_u8_idx&1)^(!continue_status)){
/*
Copy the object list back to what is, from the caller's perspective, number_list0_base.
*/
    list_size=(ULONG)(number_idx_max);
    list_size++;
    list_size=(ULONG)(list_size*sizeof(DYSPOISSOMETER_NUMBER));
    memcpy(number_list0_base, number_list1_base, (size_t)(list_size));
  }
  return;
}

DYSPOISSOMETER_NUMBER
dyspoissometer_logfreedom_median_get(DYSPOISSOMETER_UINT iteration_max, DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_max, u64 *random_seed_base){
/*
Approximate the median (not to be confused with mean) logfreedom of all mask lists consistent with a given mask count and mask span. Median logfreedom is important because it provides a threshold above and below which there is essentially equal probability of decreasing or increasing logfreedom. Mask lists generated by a strong true random number generator should asymptotically exhibit this median. In other words, consistently exhibiting improbably low or high logfreedom is an indicator of nonrandomness; the former case indicates a lack of entropy and the latter indicates an attempt to artificially contrive maximum randomness.

In:

  mask_idx_max is as defined in dyspoissometer_logfreedom_max_get().

  iteration_max is as defined in dyspoissometer_logfreedom_max_get(), except that it is restricted to [0, DYSPOISSOMETER_UINT_MAX-1].

  mask_max is as defined in dyspoissometer_logfreedom_max_get().

  *random_seed_base is as defined in dyspoissometer_logfreedom_max_get():In.

Out:

  Returns a rational but otherwise undefined negative value if we ran out of memory; else an approximation of median logfreedom, guaranteed to be nonnegative. Critically, this value is in fact an actual logfreedom of at least one generated mask list; it is not an interpolated or value. For this reason, given sufficiently high iteration_max, it becomes asymptotically certain to be the actual median, within the limits of numerical precision.

  *random_seed_base is as defined in dyspoissometer_logfreedom_max_get():Out.
*/
  DYSPOISSOMETER_UINT *freq_list_base;
  DYSPOISSOMETER_UINT freq_max_minus_1;
  DYSPOISSOMETER_NUMBER logfreedom;
  DYSPOISSOMETER_UINT logfreedom_idx;
  DYSPOISSOMETER_NUMBER *logfreedom_list0_base;
  DYSPOISSOMETER_NUMBER *logfreedom_list1_base;
  DYSPOISSOMETER_UINT *mask_list_base;
  DYSPOISSOMETER_NUMBER median;
  u8 overflow_status;
  DYSPOISSOMETER_UINT *pop_list_base;

  overflow_status=0;
  median=0.0f;
  if(mask_max&&mask_idx_max){
    overflow_status=1;
    freq_list_base=dyspoissometer_uint_list_malloc(mask_max);
    logfreedom_list0_base=dyspoissometer_number_list_malloc(iteration_max);
    logfreedom_list1_base=dyspoissometer_number_list_malloc(iteration_max);
    mask_list_base=dyspoissometer_uint_list_malloc_zero(mask_idx_max);
    pop_list_base=dyspoissometer_uint_list_malloc_zero(mask_idx_max);
    if(freq_list_base&&logfreedom_list0_base&&logfreedom_list1_base&&mask_list_base&&pop_list_base){
/*
Generate pseudorandom mask lists one at a time based on *random_seed_base. For each mask list, find the logfreedom and save it to *logfreedom_list_base.
*/
      logfreedom_idx=0;
      do{
        dyspoissometer_mask_list_pseudorandom_get(mask_idx_max, mask_list_base, mask_max, random_seed_base);
        dyspoissometer_uint_list_zero(mask_max, freq_list_base);
        dyspoissometer_freq_list_get(freq_list_base, mask_idx_max, mask_list_base, mask_max);
        freq_max_minus_1=dyspoissometer_freq_max_minus_1_get(freq_list_base, mask_max);
        dyspoissometer_pop_list_get(freq_list_base, freq_max_minus_1, mask_max, pop_list_base);
        logfreedom=dyspoissometer_logfreedom_dense_get(freq_max_minus_1, 1, mask_idx_max, mask_max, pop_list_base);
        logfreedom_list0_base[logfreedom_idx]=logfreedom;
      }while((logfreedom_idx++)!=iteration_max);
      dyspoissometer_number_list_sort(iteration_max, logfreedom_list0_base, logfreedom_list1_base);
      median=logfreedom_list0_base[iteration_max>>1];
      overflow_status=0;
    }
    dyspoissometer_free(pop_list_base);
    dyspoissometer_free(mask_list_base);
    dyspoissometer_free(logfreedom_list1_base);
    dyspoissometer_free(logfreedom_list0_base);
    dyspoissometer_free(freq_list_base);
  }else{
    median=LOG_SUM_N_PLUS_1(mask_max);
  }
  if(overflow_status){
    median=-1.0f;
  }else if(median<=0.0f){
    median=0.0f;
  }
  return median;
}

DYSPOISSOMETER_NUMBER
dyspoissometer_uint_list_logfreedom_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u32 mask_max, u16 *u16_list_base, u8 *u24_list_base, u32 *u32_list_base, u8 *u8_list_base){
/*
Get the logfreedom of a mask list.

In:

  mask_idx_max is the number of masks whose logfreedom to compute, less (mask_idx_min+1). On [mask_idx_min, DYSPOISSOMETER_UINT_MAX-1].

  mask_idx_min is the index of the first mask in the list whose logfreedom to compute. On [0, mask_idx_max].

  mask_max is the number of possible masks, whether or not they actually occur, less one.

  Exactly one of the following must be non(NULL):

    u16_list_base is the base of a list of (mask_idx_max+1) (u16)s on [0, mask_max].

    u24_list_base is the base of a list of (mask_idx_max+1) (u24)s on [0, mask_max]. Note that their alignment is U8_SIZE, not U32_SIZE.

    u32_list_base is the base of a list of (mask_idx_max+1) (u32)s on [0, mask_max].

    u8_list_base is the base of a list of (mask_idx_max+1) (u8)s on [0, mask_max].

Out:

  Returns a rational but otherwise undefined negative value if we ran out of memory; else the logfreedom of the mask list, guaranteed to be nonnegative.
*/
  DYSPOISSOMETER_UINT *freq_list0_base;
  DYSPOISSOMETER_UINT *freq_list1_base;
  DYSPOISSOMETER_NUMBER logfreedom;
  u32 mask;
  DYSPOISSOMETER_UINT mask_idx;
  DYSPOISSOMETER_UINT mask_max_uint;
  ULONG u8_idx;

  logfreedom=0.0f;
  if(mask_max){
    logfreedom=-1.0f;
    mask_max_uint=(DYSPOISSOMETER_UINT)(mask_max);
    if(mask_max==mask_max_uint){
      freq_list0_base=dyspoissometer_uint_list_malloc_zero(mask_max_uint);
      freq_list1_base=dyspoissometer_uint_list_malloc(mask_max_uint);
      if(freq_list0_base&&freq_list1_base){
        mask_idx=mask_idx_min;
        if(u8_list_base){
          do{
            mask=u8_list_base[mask_idx];
            freq_list0_base[mask]++;
          }while((mask_idx++)!=mask_idx_max);
        }else if(u16_list_base){
          do{
            mask=u16_list_base[mask_idx];
            freq_list0_base[mask]++;
          }while((mask_idx++)!=mask_idx_max);
        }else if(u24_list_base){
          u8_idx=(ULONG)((ULONG)(mask_idx_min)*3);
          do{
            mask=(u32)((u32)(u24_list_base[u8_idx+2])<<U16_BITS)+(u16)((u16)(u24_list_base[u8_idx+1])<<U8_BITS)+u24_list_base[u8_idx];
            freq_list0_base[mask]++;
            u8_idx=(ULONG)(u8_idx+3);
          }while((mask_idx++)!=mask_idx_max);
        }else{
          do{
            mask=u32_list_base[mask_idx];
            freq_list0_base[mask]++;
          }while((mask_idx++)!=mask_idx_max);
        }
        logfreedom=dyspoissometer_logfreedom_sparse_get(freq_list0_base, freq_list1_base, mask_max_uint);
      }
      dyspoissometer_free(freq_list1_base);
      dyspoissometer_free(freq_list0_base);
    }
  }
  return logfreedom;
}

DYSPOISSOMETER_NUMBER
dyspoissometer_uint_list_mean_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u8 sign_status, u16 *u16_list_base, u8 *u24_list_base, u32 *u32_list_base, u8 *u8_list_base){
/*
Get the mean of a mask list consisting of (un)signed integers.

In:

  mask_idx_max is the number of masks whose mean to compute, less (mask_idx_min+1). On [mask_idx_min, DYSPOISSOMETER_UINT_MAX-1].

  mask_idx_min is the index of the first mask in the list whose mean to compute. On [0, mask_idx_max].

  sign_status is one if the list of so-called uints actually consists of (signed) ints, else zero. (As far as the compiler is concerned, they're uints in either case so we don't get into signed overflow badness.)

  Exactly one of the following must be non(NULL):

    u16_list_base is the base of a list of (mask_idx_max+1) (u16)s on [0, mask_max].

    u24_list_base is the base of a list of (mask_idx_max+1) (u24)s on [0, mask_max]. Note that their alignment is U8_SIZE, not U32_SIZE.

    u32_list_base is the base of a list of (mask_idx_max+1) (u32)s on [0, mask_max].

    u8_list_base is the base of a list of (mask_idx_max+1) (u8)s on [0, mask_max].

Out:

  Returns the mean of the mask list, guaranteed to be at least the minimum and at most the maximum (un)signed integer representable as a mask of the same format as in the nonnull input list. However, the mean is rational and generally not an integer. It may also be less than the minimum or greater than the maximum mask in the list due to numerical error.
*/
  u64 int_sum;
  u32 mask;
  u64 mask_count;
  DYSPOISSOMETER_UINT mask_idx;
  u32 mask_max;
  i64 mask_max_max;
  u32 mask_min;
  i64 mask_min_min;
  DYSPOISSOMETER_NUMBER mean;
  ULONG u8_idx;
  u64 uint_sum;

  mask_idx=mask_idx_min;
  int_sum=0;
  uint_sum=0;
  if(!sign_status){
    mask_max=0;
    mask_min=U32_MAX;
  }else{
    mask_max=U32_SPAN_HALF;
    mask_min=I32_MAX;
  }
  if(u8_list_base){
    mask_max>>=U32_BITS-U8_BITS;
    mask_min>>=U32_BITS-U8_BITS;
  }else if(u16_list_base){
    mask_max>>=U32_BITS-U16_BITS;
    mask_min>>=U32_BITS-U16_BITS;
  }else if(u24_list_base){
    mask_max>>=U32_BITS-U24_BITS;
    mask_min>>=U32_BITS-U24_BITS;
  }
  mask_max_max=mask_min;
  mask_min_min=0;
  if(sign_status){
    mask_min_min=(~mask_max)+1;
    mask_min_min=-mask_min_min;
  }
  if(u8_list_base){
    do{
      mask=u8_list_base[mask_idx];
      if(!sign_status){
        mask_max=MAX(mask, mask_max);
        mask_min=MIN(mask, mask_min);
        uint_sum+=mask;
      }else{
        if(mask<=I8_MAX){
          int_sum+=mask;
          mask_max=MAX(mask, mask_max);
          if(mask_min<=I8_MAX){
            mask_min=MIN(mask, mask_min);
          }
        }else{
          int_sum--;
          int_sum-=(u8)(~mask);
          if(I8_MAX<mask_max){
            mask_max=MAX(mask, mask_max);
          }
          mask_min=MIN(mask, mask_min);
        }
      }
    }while((mask_idx++)!=mask_idx_max);
  }else if(u16_list_base){
    do{
      mask=u16_list_base[mask_idx];
      if(!sign_status){
        mask_max=MAX(mask, mask_max);
        mask_min=MIN(mask, mask_min);
        uint_sum+=mask;
      }else{
        if(mask<=I16_MAX){
          int_sum+=mask;
          mask_max=MAX(mask, mask_max);
          if(mask_min<=I16_MAX){
            mask_min=MIN(mask, mask_min);
          }
        }else{
          int_sum--;
          int_sum-=(u16)(~mask);
          if(I16_MAX<mask_max){
            mask_max=MAX(mask, mask_max);
          }
          mask_min=MIN(mask, mask_min);
        }
      }
    }while((mask_idx++)!=mask_idx_max);
  }else if(u24_list_base){
    u8_idx=(ULONG)((ULONG)(mask_idx_min)*3);
    do{
      mask=(u32)((u32)(u24_list_base[u8_idx+2])<<U16_BITS)+(u16)((u16)(u24_list_base[u8_idx+1])<<U8_BITS)+u24_list_base[u8_idx];
      u8_idx=(ULONG)(u8_idx+3);
      if(!sign_status){
        mask_max=MAX(mask, mask_max);
        mask_min=MIN(mask, mask_min);
        uint_sum+=mask;
      }else{
        if(mask<=I24_MAX){
          int_sum+=mask;
          mask_max=MAX(mask, mask_max);
          if(mask_min<=I24_MAX){
            mask_min=MIN(mask, mask_min);
          }
        }else{
          int_sum--;
          int_sum-=(u32)((~mask)&U24_MAX);
          if(I24_MAX<mask_max){
            mask_max=MAX(mask, mask_max);
          }
          mask_min=MIN(mask, mask_min);
        }
      }
    }while((mask_idx++)!=mask_idx_max);
  }else{
    do{
      mask=u32_list_base[mask_idx];
      if(!sign_status){
        mask_max=MAX(mask, mask_max);
        mask_min=MIN(mask, mask_min);
        uint_sum+=mask;
      }else{
        if(mask<=I32_MAX){
          int_sum+=mask;
          mask_max=MAX(mask, mask_max);
          if(mask_min<=INT32_MAX){
            mask_min=MIN(mask, mask_min);
          }
        }else{
          int_sum--;
          int_sum-=(u32)(~mask);
          if(I32_MAX<mask_max){
            mask_max=MAX(mask, mask_max);
          }
          mask_min=MIN(mask, mask_min);
        }
      }
    }while((mask_idx++)!=mask_idx_max);
  }
  mean=(DYSPOISSOMETER_NUMBER)(uint_sum);
  if(sign_status){
    mean=(DYSPOISSOMETER_NUMBER)(int_sum);
    if(int_sum>>U64_BIT_MAX){
      mean=(DYSPOISSOMETER_NUMBER)((~int_sum)+1);
      mean=-mean;
    }
  }
  mask_count=((u64)(mask_idx_max)-mask_idx_min)+1;
  mean=mean/(DYSPOISSOMETER_NUMBER)(mask_count);
  if(mean<mask_min_min){
    mean=(DYSPOISSOMETER_NUMBER)(mask_min_min);
  }else if(mask_max_max<mean){
    mean=(DYSPOISSOMETER_NUMBER)(mask_max_max);
  }
  return mean;
}

DYSPOISSOMETER_NUMBER
dyspoissometer_uint_list_stats_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u8 sign_status, DYSPOISSOMETER_NUMBER *stat_kurtosis_base, DYSPOISSOMETER_NUMBER *stat_sigma_base, DYSPOISSOMETER_NUMBER *stat_variance_base, u16 *u16_list_base, u8 *u24_list_base, u32 *u32_list_base, u8 *u8_list_base){
/*
Get the mean, variance, standard deviation, and kurtosis of a mask list consisting of (un)signed integers.

In:

  mask_idx_max is the number of masks whose mean to compute, less (mask_idx_min+1). On [mask_idx_min, DYSPOISSOMETER_UINT_MAX-1].

  mask_idx_min is the index of the first mask in the list whose mean to compute. On [0, mask_idx_max].

  sign_status is one if the list of so-called uints actually consists of (signed) ints, else zero. (As far as the compiler is concerned, they're uints in either case so we don't get into signed overflow badness.)

  *stat_kurtosis_base is undefined.

  *stat_sigma_base is undefined.

  *stat_variance_base is undefined.

  Exactly one of the following must be non(NULL):

    u16_list_base is the base of a list of (mask_idx_max+1) (u16)s on [0, mask_max].

    u24_list_base is the base of a list of (mask_idx_max+1) (u24)s on [0, mask_max]. Note that their alignment is U8_SIZE, not U32_SIZE.

    u32_list_base is the base of a list of (mask_idx_max+1) (u32)s on [0, mask_max].

    u8_list_base is the base of a list of (mask_idx_max+1) (u8)s on [0, mask_max].

Out:

  Returns the mean of the mask list, guaranteed to be at least the minimum and at most the maximum (un)signed integer representable as a mask of the same format as in the nonnull input list. However, the mean is rational and generally not an integer. It may also be less than the minimum or greater than the maximum mask in the list due to numerical error.

  *stat_kurtosis_base is the kurtosis of the list. Bessel's correction is not applied.

  *stat_sigma_base is the standard deviation of the list. Bessel's correction is not applied.

  *stat_variance_base is the variance of the list. Bessel's correction is not applied.
*/
  DYSPOISSOMETER_NUMBER delta;
  DYSPOISSOMETER_NUMBER delta_squared;
  u32 mask;
  u64 mask_count;
  DYSPOISSOMETER_UINT mask_idx;
  DYSPOISSOMETER_NUMBER stat_kurtosis;
  DYSPOISSOMETER_NUMBER stat_mean;
  DYSPOISSOMETER_NUMBER stat_sigma;
  DYSPOISSOMETER_NUMBER stat_variance;
  ULONG u8_idx;

  mask_idx=mask_idx_min;
  stat_mean=dyspoissometer_uint_list_mean_get(mask_idx_max, mask_idx_min, sign_status, u16_list_base, u24_list_base, u32_list_base, u8_list_base);
  stat_kurtosis=0.0f;
  stat_variance=0.0f;
  if(u8_list_base){
    do{
      mask=u8_list_base[mask_idx];
      if(!sign_status){
        delta=stat_mean-(DYSPOISSOMETER_NUMBER)(mask);
      }else{
        if(mask<=I8_MAX){
          delta=stat_mean-(DYSPOISSOMETER_NUMBER)(mask);
        }else{
          delta=stat_mean+(u8)((~mask)+1);
        }
      }
      delta_squared=delta*delta;
      stat_variance+=delta_squared;
      stat_kurtosis+=delta_squared*delta_squared;
    }while((mask_idx++)!=mask_idx_max);
  }else if(u16_list_base){
    do{
      mask=u16_list_base[mask_idx];
      if(!sign_status){
        delta=stat_mean-(DYSPOISSOMETER_NUMBER)(mask);
      }else{
        if(mask<=I16_MAX){
          delta=stat_mean-(DYSPOISSOMETER_NUMBER)(mask);
        }else{
          delta=stat_mean+(u16)((~mask)+1);
        }
      }
      delta_squared=delta*delta;
      stat_variance+=delta_squared;
      stat_kurtosis+=delta_squared*delta_squared;
    }while((mask_idx++)!=mask_idx_max);
  }else if(u24_list_base){
    u8_idx=(ULONG)((ULONG)(mask_idx_min)*3);
    do{
      mask=(u32)((u32)(u24_list_base[u8_idx+2])<<U16_BITS)+(u16)((u16)(u24_list_base[u8_idx+1])<<U8_BITS)+u24_list_base[u8_idx];
      u8_idx=(ULONG)(u8_idx+3);
      if(!sign_status){
        delta=stat_mean-(DYSPOISSOMETER_NUMBER)(mask);
      }else{
        if(mask<=I24_MAX){
          delta=stat_mean-(DYSPOISSOMETER_NUMBER)(mask);
        }else{
          delta=stat_mean+(DYSPOISSOMETER_NUMBER)((u32)(((~mask)+1)&U24_MAX));
        }
      }
      delta_squared=delta*delta;
      stat_variance+=delta_squared;
      stat_kurtosis+=delta_squared*delta_squared;
    }while((mask_idx++)!=mask_idx_max);
  }else{
    do{
      mask=u32_list_base[mask_idx];
      if(!sign_status){
        delta=stat_mean-(DYSPOISSOMETER_NUMBER)(mask);
      }else{
        if(mask<=I32_MAX){
          delta=stat_mean-(DYSPOISSOMETER_NUMBER)(mask);
        }else{
          delta=stat_mean+(DYSPOISSOMETER_NUMBER)((u32)((~mask)+1));
        }
      }
      delta_squared=delta*delta;
      stat_variance+=delta_squared;
      stat_kurtosis+=delta_squared*delta_squared;
    }while((mask_idx++)!=mask_idx_max);
  }
  mask_count=((u64)(mask_idx_max)-mask_idx_min)+1;
  stat_kurtosis*=(DYSPOISSOMETER_NUMBER)(mask_count)/(stat_variance*stat_variance);
  stat_variance/=(DYSPOISSOMETER_NUMBER)(mask_count);
  stat_sigma=SQRT(stat_variance);
  *stat_kurtosis_base=stat_kurtosis;
  *stat_sigma_base=stat_sigma;
  *stat_variance_base=stat_variance;
  return stat_mean;
}

DYSPOISSOMETER_NUMBER
dyspoissometer_u16_list_logfreedom_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u16 mask_max, u16 *u16_list_base){
/*
Get the logfreedom of a list of u16 masks.

In:

  mask_idx_max is as defined in dyspoissometer_uint_list_logfreedom_get().

  mask_idx_min is as defined in dyspoissometer_uint_list_logfreedom_get().

  mask_max is as defined in dyspoissometer_uint_list_logfreedom_get(), except that it's a u16.

  u16_list_base is as defined in dyspoissometer_uint_list_logfreedom_get().

Out:

  Return value is as defined in dyspoissometer_uint_list_logfreedom_get().
*/
  DYSPOISSOMETER_NUMBER logfreedom;

  logfreedom=dyspoissometer_uint_list_logfreedom_get(mask_idx_max, mask_idx_min, mask_max, u16_list_base, NULL, NULL, NULL);
  return logfreedom;
}

DYSPOISSOMETER_NUMBER
dyspoissometer_u16_list_mean_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u8 sign_status, u16 *u16_list_base){
/*
Get the mean of a list of u16 or i16 masks.

In:

  mask_idx_max is as defined in dyspoissometer_uint_list_mean_get().

  mask_idx_min is as defined in dyspoissometer_uint_list_mean_get().

  sign_status is as defined in dyspoissometer_uint_list_mean_get().

  u16_list_base is as defined in dyspoissometer_uint_list_mean_get().

Out:

  Return value is as defined in dyspoissometer_uint_list_mean_get().
*/
  DYSPOISSOMETER_NUMBER mean;

  mean=dyspoissometer_uint_list_mean_get(mask_idx_max, mask_idx_min, sign_status, u16_list_base, NULL, NULL, NULL);
  return mean;
}

DYSPOISSOMETER_NUMBER
dyspoissometer_u16_list_stats_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u8 sign_status, DYSPOISSOMETER_NUMBER *stat_kurtosis_base, DYSPOISSOMETER_NUMBER *stat_sigma_base, DYSPOISSOMETER_NUMBER *stat_variance_base, u16 *u16_list_base){
/*
Get the mean, variance, standard deviation, and kurtosis of a list consisting of u16 or i16 masks.

In:

  mask_idx_max is as defined in dyspoissometer_uint_list_stats_get().

  mask_idx_min is as defined in dyspoissometer_uint_list_stats_get().

  sign_status is as defined in dyspoissometer_uint_list_stats_get().

  *stat_kurtosis_base is as undefined.

  *stat_sigma_base is as undefined.

  *stat_variance_base is as undefined.

  u16_list_base is as defined in dyspoissometer_uint_list_stats_get().

Out:

  Return value is as defined in dyspoissometer_uint_list_stats_get().

  *stat_kurtosis_base is as defined in dyspoissometer_uint_list_stats_get().

  *stat_sigma_base is as defined in dyspoissometer_uint_list_stats_get().

  *stat_variance_base is as defined in dyspoissometer_uint_list_stats_get().
*/
  DYSPOISSOMETER_NUMBER stat_mean;

  stat_mean=dyspoissometer_uint_list_stats_get(mask_idx_max, mask_idx_min, sign_status, stat_kurtosis_base, stat_sigma_base, stat_variance_base, u16_list_base, NULL, NULL, NULL);
  return stat_mean;
}

DYSPOISSOMETER_NUMBER
dyspoissometer_u24_list_logfreedom_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u32 mask_max, u8 *u24_list_base){
/*
Get the logfreedom of a list of u24 masks. This function is provided as a convenient compromise between u16 and u32, which affords rich statistics without having to deal with the annoyance of bit packing.

In:

  mask_idx_max is as defined in dyspoissometer_uint_list_logfreedom_get().

  mask_idx_min is as defined in dyspoissometer_uint_list_logfreedom_get().

  mask_max is as defined in dyspoissometer_uint_list_logfreedom_get(). On [0, U24_MAX].

  u24_list_base is as defined in dyspoissometer_uint_list_logfreedom_get().

Out:

  Return value is as defined in dyspoissometer_uint_list_logfreedom_get().
*/
  DYSPOISSOMETER_NUMBER logfreedom;

  logfreedom=dyspoissometer_uint_list_logfreedom_get(mask_idx_max, mask_idx_min, mask_max, NULL, u24_list_base, NULL, NULL);
  return logfreedom;
}

DYSPOISSOMETER_NUMBER
dyspoissometer_u24_list_mean_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u8 sign_status, u8 *u24_list_base){
/*
Get the mean of a list of u24 or i24 masks. This function is provided as a convenient compromise between u16 and u32, which affords rich statistics without having to deal with the annoyance of bit packing.

In:

  mask_idx_max is as defined in dyspoissometer_uint_list_mean_get().

  mask_idx_min is as defined in dyspoissometer_uint_list_mean_get().

  sign_status is as defined in dyspoissometer_uint_list_mean_get().

  u24_list_base is as defined in dyspoissometer_uint_list_mean_get().

Out:

  Return value is as defined in dyspoissometer_uint_list_mean_get().
*/
  DYSPOISSOMETER_NUMBER mean;

  mean=dyspoissometer_uint_list_mean_get(mask_idx_max, mask_idx_min, sign_status, NULL, u24_list_base, NULL, NULL);
  return mean;
}

DYSPOISSOMETER_NUMBER
dyspoissometer_u24_list_stats_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u8 sign_status, DYSPOISSOMETER_NUMBER *stat_kurtosis_base, DYSPOISSOMETER_NUMBER *stat_sigma_base, DYSPOISSOMETER_NUMBER *stat_variance_base, u8 *u24_list_base){
/*
Get the mean, variance, standard deviation, and kurtosis of a mask list consisting of u24 or i24 masks. This function is provided as a convenient compromise between u16 and u32, which affords rich statistics without having to deal with the annoyance of bit packing.

In:

  mask_idx_max is as defined in dyspoissometer_uint_list_stats_get().

  mask_idx_min is as defined in dyspoissometer_uint_list_stats_get().

  sign_status is as defined in dyspoissometer_uint_list_stats_get().

  *stat_kurtosis_base is as undefined.

  *stat_sigma_base is as undefined.

  *stat_variance_base is as undefined.

  u24_list_base is as defined in dyspoissometer_uint_list_stats_get().

Out:

  Return value is as defined in dyspoissometer_uint_list_stats_get().

  *stat_kurtosis_base is as defined in dyspoissometer_uint_list_stats_get().

  *stat_sigma_base is as defined in dyspoissometer_uint_list_stats_get().

  *stat_variance_base is as defined in dyspoissometer_uint_list_stats_get().
*/
  DYSPOISSOMETER_NUMBER stat_mean;

  stat_mean=dyspoissometer_uint_list_stats_get(mask_idx_max, mask_idx_min, sign_status, stat_kurtosis_base, stat_sigma_base, stat_variance_base, NULL, u24_list_base, NULL, NULL);
  return stat_mean;
}

DYSPOISSOMETER_NUMBER
dyspoissometer_u32_list_logfreedom_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u32 mask_max, u32 *u32_list_base){
/*
Get the logfreedom of a list of u32 masks.

In:

  mask_idx_max is as defined in dyspoissometer_uint_list_logfreedom_get().

  mask_idx_min is as defined in dyspoissometer_uint_list_logfreedom_get().

  mask_max is as defined in dyspoissometer_uint_list_logfreedom_get().

  u32_list_base is as defined in dyspoissometer_uint_list_logfreedom_get().

Out:

  Return value is as defined in dyspoissometer_uint_list_logfreedom_get().
*/
  DYSPOISSOMETER_NUMBER logfreedom;

  logfreedom=dyspoissometer_uint_list_logfreedom_get(mask_idx_max, mask_idx_min, mask_max, NULL, NULL, u32_list_base, NULL);
  return logfreedom;
}

DYSPOISSOMETER_NUMBER
dyspoissometer_u32_list_mean_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u8 sign_status, u32 *u32_list_base){
/*
Get the mean of a list of u32 or i32 masks.

In:

  mask_idx_max is as defined in dyspoissometer_uint_list_mean_get().

  mask_idx_min is as defined in dyspoissometer_uint_list_mean_get().

  sign_status is as defined in dyspoissometer_uint_list_mean_get().

  u32_list_base is as defined in dyspoissometer_uint_list_mean_get().

Out:

  Return value is as defined in dyspoissometer_uint_list_mean_get().
*/
  DYSPOISSOMETER_NUMBER mean;

  mean=dyspoissometer_uint_list_mean_get(mask_idx_max, mask_idx_min, sign_status, NULL, NULL, u32_list_base, NULL);
  return mean;
}

DYSPOISSOMETER_NUMBER
dyspoissometer_u32_list_stats_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u8 sign_status, DYSPOISSOMETER_NUMBER *stat_kurtosis_base, DYSPOISSOMETER_NUMBER *stat_sigma_base, DYSPOISSOMETER_NUMBER *stat_variance_base, u32 *u32_list_base){
/*
Get the mean, variance, standard deviation, and kurtosis of a mask list consisting of u32 or i32 masks.

In:

  mask_idx_max is as defined in dyspoissometer_uint_list_stats_get().

  mask_idx_min is as defined in dyspoissometer_uint_list_stats_get().

  sign_status is as defined in dyspoissometer_uint_list_stats_get().

  *stat_kurtosis_base is as undefined.

  *stat_sigma_base is as undefined.

  *stat_variance_base is as undefined.

  u32_list_base is as defined in dyspoissometer_uint_list_stats_get().

Out:

  Return value is as defined in dyspoissometer_uint_list_stats_get().

  *stat_kurtosis_base is as defined in dyspoissometer_uint_list_stats_get().

  *stat_sigma_base is as defined in dyspoissometer_uint_list_stats_get().

  *stat_variance_base is as defined in dyspoissometer_uint_list_stats_get().
*/
  DYSPOISSOMETER_NUMBER stat_mean;

  stat_mean=dyspoissometer_uint_list_stats_get(mask_idx_max, mask_idx_min, sign_status, stat_kurtosis_base, stat_sigma_base, stat_variance_base, NULL, NULL, u32_list_base, NULL);
  return stat_mean;
}

DYSPOISSOMETER_NUMBER
dyspoissometer_u8_list_logfreedom_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u8 mask_max, u8 *u8_list_base){
/*
Get the logfreedom of a list of u8 masks.

In:

  mask_idx_max is as defined in dyspoissometer_uint_list_logfreedom_get().

  mask_idx_min is as defined in dyspoissometer_uint_list_logfreedom_get().

  mask_max is as defined in dyspoissometer_uint_list_logfreedom_get(), except that it's a u8.

  u8_list_base is as defined in dyspoissometer_uint_list_logfreedom_get().

Out:

  Return value is as defined in dyspoissometer_uint_list_logfreedom_get().
*/
  DYSPOISSOMETER_NUMBER logfreedom;

  logfreedom=dyspoissometer_uint_list_logfreedom_get(mask_idx_max, mask_idx_min, mask_max, NULL, NULL, NULL, u8_list_base);
  return logfreedom;
}

DYSPOISSOMETER_NUMBER
dyspoissometer_u8_list_mean_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u8 sign_status, u8 *u8_list_base){
/*
Get the mean of a list of u8 or i8 masks.

In:

  mask_idx_max is as defined in dyspoissometer_uint_list_mean_get().

  mask_idx_min is as defined in dyspoissometer_uint_list_mean_get().

  sign_status is as defined in dyspoissometer_uint_list_mean_get().

  u8_list_base is as defined in dyspoissometer_uint_list_mean_get().

Out:

  Return value is as defined in dyspoissometer_uint_list_mean_get().
*/
  DYSPOISSOMETER_NUMBER mean;

  mean=dyspoissometer_uint_list_mean_get(mask_idx_max, mask_idx_min, sign_status, NULL, NULL, NULL, u8_list_base);
  return mean;
}

DYSPOISSOMETER_NUMBER
dyspoissometer_u8_list_stats_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_idx_min, u8 sign_status, DYSPOISSOMETER_NUMBER *stat_kurtosis_base, DYSPOISSOMETER_NUMBER *stat_sigma_base, DYSPOISSOMETER_NUMBER *stat_variance_base, u8 *u8_list_base){
/*
Get the mean, variance, standard deviation, and kurtosis of a mask list consisting of u8 or i8 masks.

In:

  mask_idx_max is as defined in dyspoissometer_uint_list_stats_get().

  mask_idx_min is as defined in dyspoissometer_uint_list_stats_get().

  sign_status is as defined in dyspoissometer_uint_list_stats_get().

  *stat_kurtosis_base is as undefined.

  *stat_sigma_base is as undefined.

  *stat_variance_base is as undefined.

  u8_list_base is as defined in dyspoissometer_uint_list_stats_get().

Out:

  Return value is as defined in dyspoissometer_uint_list_stats_get().

  *stat_kurtosis_base is as defined in dyspoissometer_uint_list_stats_get().

  *stat_sigma_base is as defined in dyspoissometer_uint_list_stats_get().

  *stat_variance_base is as defined in dyspoissometer_uint_list_stats_get().
*/
  DYSPOISSOMETER_NUMBER stat_mean;

  stat_mean=dyspoissometer_uint_list_stats_get(mask_idx_max, mask_idx_min, sign_status, stat_kurtosis_base, stat_sigma_base, stat_variance_base, NULL, NULL, NULL, u8_list_base);
  return stat_mean;
}

DYSPOISSOMETER_NUMBER
dyspoissometer_kernel_density_expected_fast_get(DYSPOISSOMETER_UINT mask_idx_max){
/*
Find the expected value of kernel density. For maximum accuracy at high mask_idx_max, use dyspoissometer_kernel_density_expected_slow_get() instead.

See also: http://cognomicon.blogspot.com/2014/12/the-kernel-density-randomness-metric.html .

In:

  mask_idx_max is the mask count of the mask list, less one, on [0, DYSPOISSOMETER_UINT_MAX-1]. By the definition of kernel density, (mask_max==mask_idx_max) is implied.

Out:

  Returns the expected kernel density, guaranteed to be on [0.0, 1.0].
*/
  DYSPOISSOMETER_NUMBER density;
  DYSPOISSOMETER_NUMBER density_old;
  DYSPOISSOMETER_UINT mask_count;
  DYSPOISSOMETER_NUMBER mask_count_log;
  DYSPOISSOMETER_UINT mask_count_minus_period;
  DYSPOISSOMETER_NUMBER mask_idx_max_logsum;
  DYSPOISSOMETER_UINT period_minus_1;
  DYSPOISSOMETER_NUMBER term;
/*
Use logarithmic summation to evaluate the expected value of kernel density. Continue evaluating terms until either we run out of them, or they become so small as to not make any difference to the final output. (This is slightly inaccurate because many such negligible terms could sum to a nonnegligible term. We account for that possibility in dyspoissometer_kernel_density_expected_slow_get().)
*/
  mask_idx_max_logsum=LOG_SUM(mask_idx_max);
  density=0.0f;
  mask_count=(DYSPOISSOMETER_UINT)(mask_idx_max+1);
  mask_count_log=LOG(mask_count);
  period_minus_1=0;
  if(mask_idx_max){
    period_minus_1=1;
    mask_count_minus_period=(DYSPOISSOMETER_UINT)(mask_idx_max-period_minus_1);
    do{
      term=EXP(mask_idx_max_logsum-LOG_SUM(mask_count_minus_period)-(period_minus_1*mask_count_log));
      density_old=density;
      density+=term;
      mask_count_minus_period--;
    }while(((period_minus_1++)!=mask_idx_max)&&(density!=density_old));
  }
  density+=1.0f;
  density/=mask_count;
/*
Do some CYA for horrendous precision problems (and negative zero baloney) that shoudn't happen (but who knows, with C).
*/
  if(density<=0.0f){
    density=0.0f;
  }else if(1.0f<density){
    density=1.0f;
  }
  return density;
}

DYSPOISSOMETER_NUMBER
dyspoissometer_kernel_density_expected_slow_get(DYSPOISSOMETER_UINT mask_idx_max){
/*
Find the expected value of kernel density using maximum accuracy at high mask_idx_max. For most practical purposes, use dyspoissometer_kernel_density_expected_fast_get() instead.

In:

  See dyspoissometer_kernel_density_expected_fast_get().

Out:

  See dyspoissometer_kernel_density_expected_fast_get().
*/
  DYSPOISSOMETER_NUMBER density;
  DYSPOISSOMETER_UINT mask_count;
  DYSPOISSOMETER_NUMBER mask_count_log;
  DYSPOISSOMETER_UINT mask_count_minus_period;
  DYSPOISSOMETER_NUMBER mask_idx_max_logsum;
  DYSPOISSOMETER_UINT period_minus_1;
  DYSPOISSOMETER_UINT period_minus_1_max;
  DYSPOISSOMETER_UINT period_minus_1_min;
  DYSPOISSOMETER_NUMBER term;

  mask_idx_max_logsum=LOG_SUM(mask_idx_max);
  density=0.0f;
  mask_count=(DYSPOISSOMETER_UINT)(mask_idx_max+1);
  mask_count_log=LOG(mask_count);
  period_minus_1=0;
  if(mask_idx_max){
    period_minus_1_max=mask_idx_max;
    period_minus_1_min=1;
/*
Binary search for the first nonzero term. Granted, it may still be too small to matter to the final output, but at least it saves us the vast majority of compute time with large mask counts.
*/
    while(period_minus_1_max!=period_minus_1_min){
      period_minus_1=(DYSPOISSOMETER_UINT)(period_minus_1_max-((period_minus_1_max-period_minus_1_min)>>1));
      mask_count_minus_period=(DYSPOISSOMETER_UINT)(mask_idx_max-period_minus_1);
      term=EXP(mask_idx_max_logsum-LOG_SUM(mask_count_minus_period)-(period_minus_1*mask_count_log));
      if(term!=0.0f){
        period_minus_1_min=period_minus_1;
      }else{
        period_minus_1_max=(DYSPOISSOMETER_UINT)(period_minus_1-1);
      }
    }
/*
Use logarithmic summation from the smallest to largest magnitude term in order to maximize accuracy. Otherwise, this process is identical to dyspoissometer_kernel_density_expected_fast_get().
*/
    period_minus_1=period_minus_1_max;
    mask_count_minus_period=(DYSPOISSOMETER_UINT)(mask_idx_max-period_minus_1);
    do{
      term=EXP(mask_idx_max_logsum-LOG_SUM(mask_count_minus_period)-(period_minus_1*mask_count_log));
      density+=term;
      mask_count_minus_period++;
    }while(--period_minus_1);
  }
  density+=1.0f;
  density/=mask_count;
/*
Do some CYA for horrendous precision problems (and negative zero baloney) that shoudn't happen (but who knows, with C).
*/
  if(density<=0.0f){
    density=0.0f;
  }else if(1.0f<density){
    density=1.0f;
  }
  return density;
}

DYSPOISSOMETER_UINT
dyspoissometer_kernel_size_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT *mask_list_base){
/*
Find the kernel size of a mask set.

See also: http://cognomicon.blogspot.com/2014/12/measuring-kernel-density.html .

In:

  mask_idx_max is as defined in dyspoissometer_kernel_density_expected_fast_get().

  *mask_list_base contains (mask_idx_max+1) masks, each on [0, mask_idx_max].

Out:

  Returns zero if we ran out of memory, else the kernel size of *mask_list_base. Notionally, the kernel size is just the number of unique integers remaining in the list after iterating its contents through the list itself (mask_idx_max+1) times.
*/
  u8 bit_idx;
  DYSPOISSOMETER_UINT bitmap_idx;
  DYSPOISSOMETER_UINT bitmap_uint;
  DYSPOISSOMETER_UINT bitmap_uint_count_minus_1;
  DYSPOISSOMETER_UINT *bitmap0_base;
  DYSPOISSOMETER_UINT *bitmap1_base;
  DYSPOISSOMETER_UINT iteration;
  DYSPOISSOMETER_UINT iteration_idx;
  DYSPOISSOMETER_UINT *iteration_list0_base;
  DYSPOISSOMETER_UINT *iteration_list1_base;
  DYSPOISSOMETER_UINT kernel_size;

  bitmap_uint_count_minus_1=mask_idx_max>>DYSPOISSOMETER_UINT_BITS_LOG2;
  bitmap0_base=dyspoissometer_uint_list_malloc(bitmap_uint_count_minus_1);
  bitmap1_base=dyspoissometer_uint_list_malloc(bitmap_uint_count_minus_1);
  iteration_list0_base=dyspoissometer_uint_list_malloc(mask_idx_max);
  iteration_list1_base=dyspoissometer_uint_list_malloc(mask_idx_max);
  kernel_size=0;
  if(bitmap0_base&&bitmap1_base&&iteration_list0_base&&iteration_list1_base){
    dyspoissometer_uint_list_copy(mask_idx_max, 0, 0, mask_list_base, iteration_list0_base);
    do{
/*
Iterate the list through itself via a double buffer until the bitmap of remaining integers is constant from one iteration to the next.
*/
      dyspoissometer_uint_list_zero(bitmap_uint_count_minus_1, bitmap0_base);
      iteration_idx=0;
      do{
        iteration=iteration_list0_base[iteration_list0_base[iteration_idx]];
        iteration_list1_base[iteration_idx]=iteration;
        bitmap_idx=iteration>>DYSPOISSOMETER_UINT_BITS_LOG2;
        bitmap_uint=(DYSPOISSOMETER_UINT)(bitmap0_base[bitmap_idx]|((DYSPOISSOMETER_UINT)(1)<<(iteration&DYSPOISSOMETER_UINT_BIT_MAX)));
        bitmap0_base[bitmap_idx]=bitmap_uint;
      }while((iteration_idx++)!=mask_idx_max);
      dyspoissometer_uint_list_zero(bitmap_uint_count_minus_1, bitmap1_base);
      iteration_idx=mask_idx_max;
      do{
        iteration=iteration_list1_base[iteration_list1_base[iteration_idx]];
        iteration_list0_base[iteration_idx]=iteration;
        bitmap_idx=iteration>>DYSPOISSOMETER_UINT_BITS_LOG2;
        bitmap_uint=(DYSPOISSOMETER_UINT)(bitmap1_base[bitmap_idx]|((DYSPOISSOMETER_UINT)(1)<<(iteration&DYSPOISSOMETER_UINT_BIT_MAX)));
        bitmap1_base[bitmap_idx]=bitmap_uint;
      }while(iteration_idx--);
      bitmap_idx=0;
      do{
        bitmap_uint=bitmap0_base[bitmap_idx]^bitmap1_base[bitmap_idx];
      }while(((bitmap_idx++)!=bitmap_uint_count_minus_1)&&(!bitmap_uint));
    }while(bitmap_uint);
/*
Count the ones in the bitmap, which equals the kernel size.
*/
    bitmap_idx=bitmap_uint_count_minus_1;
    do{
      bitmap_uint=bitmap0_base[bitmap_idx];
      for(bit_idx=0; bit_idx<=DYSPOISSOMETER_UINT_BIT_MAX; bit_idx++){
        kernel_size=(DYSPOISSOMETER_UINT)(kernel_size+(bitmap_uint&1));
        bitmap_uint>>=1;
      }
    }while(bitmap_idx--);
  }
  dyspoissometer_free(iteration_list1_base);
  dyspoissometer_free(iteration_list0_base);
  dyspoissometer_free(bitmap1_base);
  dyspoissometer_free(bitmap0_base);
  return kernel_size;
}

DYSPOISSOMETER_NUMBER
dyspoissometer_kernel_density_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT kernel_size){
/*
Given a mask list size and its kernel size, compute its kernel density.

In:

  mask_idx_max is as defined in dyspoissometer_kernel_density_expected_fast_get().

  kernel_size is the (nonzero) return value of dyspoissometer_kernel_size_get().

Out:

  Returns the kernel density corresponding to the input parameters, guaranteed to be on [0.0, 1.0].
*/
  DYSPOISSOMETER_NUMBER density;

  density=(DYSPOISSOMETER_NUMBER)(kernel_size)/(DYSPOISSOMETER_UINT)(mask_idx_max+1);
/*
Do some CYA for horrendous precision problems (and negative zero baloney) that shoudn't happen (but who knows, with C).
*/
  if(density<=0.0f){
    density=0.0f;
  }else if(1.0f<density){
    density=1.0f;
  }
  return density;
}

DYSPOISSOMETER_NUMBER
dyspoissometer_kernel_skew_get(DYSPOISSOMETER_NUMBER kernel_density, DYSPOISSOMETER_NUMBER kernel_density_expected){
/*
Given the kernel size of a mask list and the statistical expectation thereof, compute kernel skew.

See also: http://cognomicon.blogspot.com/2016/05/kernel-skew-normalized-randomness-metric.html .

In:

  kernel_density is the return value of dyspoissometer_kernel_density_get().

  kernel_density_expected is the corresponding return value of dyspoissometer_kernel_density_expected_fast_get() or dyspoissometer_kernel_density_expected_slow_get().

Out:

  Returns the kernel skew corresponding to the input parameters, guaranteed to be on [0.0, 1.0].
*/
  DYSPOISSOMETER_NUMBER skew;

  skew=0.5f;
  if(kernel_density<=kernel_density_expected){
    skew*=kernel_density/kernel_density_expected;
  }else{
    skew*=2.0f-(kernel_density_expected/kernel_density);
  }
/*
Do some CYA for horrendous precision problems (and negative zero baloney) that shoudn't happen (but who knows, with C).
*/
  if(skew<=0.0f){
    skew=0.0f;
  }else if(1.0f<skew){
    skew=1.0f;
  }
  return skew;
}

DYSPOISSOMETER_NUMBER
dyspoissometer_mibr_expected_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT mask_max){
/*
Return the expected (average or mean) MIBR (EMIBR) for a given mask count and mask span.

See also: http://dyspoissonism.blogspot.com/2016/06/mibr-randomness-metric-of-last-resort.html .

In:

  mask_idx_max is as defined in dyspoissometer_mibr_get().

  mask_max is as defined in dyspoissometer_mibr_get().

Out:

  Returns the EMIBR.

  Queries with large input values may take a long time. In such cases, and if and only if (mask_idx_max==max_max), the following approximation of EMIBR, appears to be accurate to within one unit, although it is not used by this function:

  E=(((mask_max+1)*(pi/2))^0.5)-(4/3)

  Note also that EMIBR is close to ((mask_max+1)*S0), where S0 is the expected kernel density. Their asymptotic ratio appears to be one.
*/
  DYSPOISSOMETER_NUMBER mask_span;
  DYSPOISSOMETER_NUMBER mask_span_recip;
  DYSPOISSOMETER_UINT mibr;
  DYSPOISSOMETER_NUMBER mibr_expected;
  DYSPOISSOMETER_NUMBER mibr_expected_old;
  DYSPOISSOMETER_UINT mibr_max;
  DYSPOISSOMETER_NUMBER weight;
  DYSPOISSOMETER_NUMBER weight_partial;
  DYSPOISSOMETER_NUMBER weight_sum;

  mibr_expected=0.0f;
  if(mask_idx_max&&mask_max){
    mask_span=(DYSPOISSOMETER_NUMBER)(mask_max)+1.0f;
    mask_span_recip=1.0f/mask_span;
    mibr=1;
    mibr_max=MIN(mask_idx_max, mask_max);
    weight_partial=mask_max*mask_span_recip*mask_span_recip;
    weight_sum=mask_span_recip;
    do{
      mibr_expected_old=mibr_expected;
      weight=((DYSPOISSOMETER_NUMBER)(mibr)+1.0f)*weight_partial;
      mibr_expected+=mibr*weight;
      weight_sum+=weight;
      weight_partial*=(DYSPOISSOMETER_NUMBER)(mask_max-mibr)*mask_span_recip;
    }while((mibr_expected!=mibr_expected_old)&&((mibr++)!=mibr_max));
    mibr_expected+=mibr_max*(1.0f-weight_sum);
    if(mibr_expected<=0.0f){
      mibr_expected=0.0f;
    }else if(mibr_max<mibr_expected){
      mibr_expected=mibr_max;
    }
  }
  return mibr_expected;
}

void
dyspoissometer_uint_idx_list_sort(DYSPOISSOMETER_UINT *idx_list_base0, DYSPOISSOMETER_UINT *idx_list_base1, DYSPOISSOMETER_UINT uint_idx_max, DYSPOISSOMETER_UINT *uint_list_base0, DYSPOISSOMETER_UINT *uint_list_base1){
/*
Sort a list of (DYSPOISSOMETER_UINT)s ascending as though they were unsigned integers equal to their implementation-specific representations, using double buffering for speed. Perform the same permutation on their corresponding indexes. This is designed to permit sorting a list of variably sized strings via a list of scalars. Objects of equal value will remain in their original order regardless of sort direction.

In:

  *idx_list_base0 contains (uint_idx_max+1) items, each corresponding to the respective DYSPOISSOMETER_UINT at uint_list_base0. For example, they might be the base indexes of strings whose hashes are at *uint_list_base0.

  *idx_list_base1 is undefined and writable for (uint_idx_max+1) items.

  uint_idx_max is one less than the number of items in each list.

  *uint_list_base0 contains (uint_idx_max+1) items to sort.

  *uint_list_base1 is undefined and writable for (uint_idx_max+1) items.

Out:

  *idx_list_base0 is reordered according to the permutation of *uint_list_base0 relative to its input state.

  *idx_list_base1 is undefined. 

  *uint_list_base0 is sorted ascending.

  *uint_list_base1 is undefined. 
*/
  u8 continue_status;
  DYSPOISSOMETER_UINT idx;
  ULONG list_size;
  DYSPOISSOMETER_UINT *idx_list_base2;
  DYSPOISSOMETER_UINT uint;
  DYSPOISSOMETER_UINT uint_idx_list_base[U8_SPAN];
  DYSPOISSOMETER_UINT uint_idx0;
  DYSPOISSOMETER_UINT uint_idx1;
  DYSPOISSOMETER_UINT *uint_list_base2;
  DYSPOISSOMETER_UINT uint_old;
  DYSPOISSOMETER_UINT uint_u8;
  DYSPOISSOMETER_UINT uint_u8_freq;
  DYSPOISSOMETER_UINT uint_u8_idx;
/*
Perform a bytewise radix sort of *uint_list_base0.
*/
  uint_u8_idx=0;
  do{
/*
*uint_idx_list_base will serve as a list of base indexes for all possible (u8)s. Zero it.
*/
    memset(uint_idx_list_base, 0, (size_t)(U8_SPAN*sizeof(DYSPOISSOMETER_UINT)));
    continue_status=0;
/*
Count u8 frequencies and save them to *uint_idx_list_base. Move backwards to maximize cache hits in most cases.
*/
    uint_idx0=uint_idx_max;
    uint_old=uint_list_base0[uint_idx_max];
    do{
      uint=uint_list_base0[uint_idx0];
      continue_status=(u8)(continue_status|(uint_old<uint));
      uint_u8=((unsigned char *)(&uint))[uint_u8_idx];
      uint_old=uint;
      uint_idx_list_base[uint_u8]++;
    }while(uint_idx0--);
    if(continue_status){
/*
Convert uint_idx_list_base from a list of frequencies of (u8)s to a list of preceding cummulative sums of frequencies of (u8)s. This will tell us how to sort this particular u8 lane.
*/
      uint_u8=0;
      uint_idx0=0;
      do{
        uint_u8_freq=uint_idx_list_base[uint_u8];
        uint_idx_list_base[uint_u8]=uint_idx0;
        uint_idx0=(DYSPOISSOMETER_UINT)(uint_idx0+uint_u8_freq);
      }while((uint_u8++)!=U8_MAX);
/*
Sort *idx_list_base1 and *uint_list_base1 ascending by the u8 lane designated by uint_u8_idx.
*/
      uint_idx0=0;
      do{
        idx=idx_list_base0[uint_idx0];
        uint=uint_list_base0[uint_idx0];
        uint_u8=((unsigned char *)(&uint))[uint_u8_idx];
        uint_idx1=uint_idx_list_base[uint_u8];
        idx_list_base1[uint_idx1]=idx;
        uint_list_base1[uint_idx1]=uint;
        uint_idx1++;
        uint_idx_list_base[uint_u8]=uint_idx1;
      }while((uint_idx0++)!=uint_idx_max);
    }
/*
Swap list bases in order to accomplish double buffering.
*/
    idx_list_base2=idx_list_base0;
    idx_list_base0=idx_list_base1;
    idx_list_base1=idx_list_base2;
    uint_list_base2=uint_list_base0;
    uint_list_base0=uint_list_base1;
    uint_list_base1=uint_list_base2;
/*
continue_status should always be zero when the items have been sorted. However, due to floating-point undefinedness, it might be one even though the objects have actually been sorted when viewed as unsigned integers (which we could not have done because it would have been an aliasing violation, hence our use of (unsigned char *)).
*/
    uint_u8_idx++;
  }while(continue_status&&(uint_u8_idx!=sizeof(DYSPOISSOMETER_UINT)));
  if((uint_u8_idx&1)^(!continue_status)){
/*
Copy the index and object lists back to what is, from the caller's perspective, idx_list_base0 and uint_list_base0, respectively.
*/
    list_size=(ULONG)(uint_idx_max);
    list_size++;
    list_size=(ULONG)(list_size*sizeof(DYSPOISSOMETER_UINT));
    memcpy(idx_list_base0, idx_list_base1, (size_t)(list_size));
    memcpy(uint_list_base0, uint_list_base1, (size_t)(list_size));
  }
  return;
}

u8
dyspoissometer_mibr_get(DYSPOISSOMETER_UINT mask_idx_max, DYSPOISSOMETER_UINT *mask_list_base, DYSPOISSOMETER_UINT *mibr_base){
/*
Return the maximum index before repetition (MIBR) of a give mask list.

See also: http://dyspoissonism.blogspot.com/2016/06/mibr-randomness-metric-of-last-resort.html .

In:

  mask_idx_max is the number of masks in the list, less one, AKA (Q - 1).

  mask_max is the maximum possible mask, AKA (Z - 1).

Out:

  Returns zero on failure, else one.

  *mibr_base is the MIBR.
*/
  DYSPOISSOMETER_UINT ibr;
  DYSPOISSOMETER_UINT mask;
  DYSPOISSOMETER_UINT mask_idx;
  DYSPOISSOMETER_UINT *mask_idx_list0_base;
  DYSPOISSOMETER_UINT *mask_idx_list1_base;
  DYSPOISSOMETER_UINT *mask_list1_base;
  DYSPOISSOMETER_UINT mask_old;
  DYSPOISSOMETER_UINT mibr;
  u8 status;

  mask_idx_list0_base=dyspoissometer_uint_list_malloc(mask_idx_max);
  mask_idx_list1_base=dyspoissometer_uint_list_malloc(mask_idx_max);
  mask_list1_base=dyspoissometer_uint_list_malloc(mask_idx_max);
  status=1;
  if(mask_idx_list0_base&&mask_idx_list1_base&&mask_list1_base){
    mask_idx=0;
    do{
      mask_idx_list0_base[mask_idx]=mask_idx;
    }while((mask_idx++)!=mask_idx_max);
    dyspoissometer_uint_idx_list_sort(mask_idx_list0_base, mask_idx_list1_base, mask_idx_max, mask_list_base, mask_list1_base);
    mask_idx=0;
    mibr=mask_idx_max;
    if(mask_idx_max){
      mask=mask_list_base[0];
      mask_idx++;
      do{
        mask_old=mask;
        mask=mask_list_base[mask_idx];
        if(mask==mask_old){
          ibr=(DYSPOISSOMETER_UINT)(mask_idx_list0_base[mask_idx]-1);
          mibr=MIN(ibr, mibr);
        }
      }while((mask_idx++)!=mask_idx_max);
    }
    status=0;
    *mibr_base=mibr;
  }
  dyspoissometer_free(mask_list1_base);
  dyspoissometer_free(mask_idx_list1_base);
  dyspoissometer_free(mask_idx_list0_base);
  return status;
}

DYSPOISSOMETER_NUMBER
dyspoissometer_skew_from_mibr_get(DYSPOISSOMETER_UINT mask_max, DYSPOISSOMETER_UINT mibr){
/*
Return the skew (K) for a given mask span and MIBR.

See also: http://dyspoissonism.blogspot.com/2016/06/mibr-randomness-metric-of-last-resort.html .

In:

  mask_max is as defined in dyspoissometer_mibr_get().

  mibr is the MIBR whose skew to evaluate, on [0, mask_max].

Out:

  Returns the MIBR skew.
*/
  DYSPOISSOMETER_NUMBER skew;

  skew=1.0f;
  if(mask_max){
    skew=EXP(LOG_SUM(mask_max)-LOG_SUM(mask_max-mibr)-(LOG_N_PLUS_1(mask_max)*mibr));
    if(skew<=0.0f){
      skew=0.0f;
    }else if(1.0f<skew){
      skew=1.0f;
    }
  }
  return skew;
}

DYSPOISSOMETER_UINT
dyspoissometer_mibr_from_skew_get(DYSPOISSOMETER_UINT mask_max, DYSPOISSOMETER_NUMBER skew){
/*
Return the maximum MIBR whose skew is at least skew, or mask_max if no such MIBR exists.

See also: http://dyspoissonism.blogspot.com/2016/06/mibr-randomness-metric-of-last-resort.html .

In:

  mask_max is as defined in dyspoissometer_mibr_get().

Out:

  Returns the MIBR as specified in the summary.
*/
  DYSPOISSOMETER_UINT mibr;
  DYSPOISSOMETER_UINT mibr_max;
  DYSPOISSOMETER_UINT mibr_min;
  DYSPOISSOMETER_NUMBER skew_mibr;

  mibr_max=mask_max;
  if(mask_max){
    mibr_min=0;
    while(mibr_max!=mibr_min){
      mibr=(DYSPOISSOMETER_UINT)(mibr_max-((mibr_max-mibr_min)>>1));
      skew_mibr=dyspoissometer_skew_from_mibr_get(mask_max, mibr);
      if(skew_mibr<skew){
        mibr_max=(DYSPOISSOMETER_UINT)(mibr-1);
      }else{
        mibr_min=mibr;
      }
    }
  }
  return mibr_max;
}
