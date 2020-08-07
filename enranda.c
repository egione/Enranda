/*
Enranda
Copyright 2016 Russell Leidich
http://enranda.blogspot.com

This collection of files constitutes the Enranda Library. (This is a
library in the abstact sense; it's not intended to compile to a ".lib"
file.)

The Enranda Library is free software: you can redistribute it and/or
modify it under the terms of the GNU Limited General Public License as
published by the Free Software Foundation, version 3.

The Enranda Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Limited General Public License version 3 for more details.

You should have received a copy of the GNU Limited General Public
License version 3 along with the Enranda Library (filename
"COPYING"). If not, see http://www.gnu.org/licenses/ .
*/
/*
Enranda True Random Number Generator
*/
#include "flag.h"
#include "flag_timestamp.h"
#include "flag_enranda.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "constant.h"
#include "debug.h"
#include "debug_xtrn.h"
#include "enranda.h"
#include "timestamp_xtrn.h"

u8
enranda_entropy_accrue(enranda_t *enranda_base, u8 fill_status){
/*
Accrue a bit or so (on average) of protoentropy, then return to the caller with minimum latency; or accrue protoentropy until the internal list is full.

In:

  enranda_base is the return value of enranda_init().

  fill_status is zero to return after a single timestamp read, else one to loop until the internal protoentropy list is full.

Out:

  Guaranteed to return zero if (fill_status==1). Else: returns zero if protoentropy is ready for trapdooring (into bona fide entropy) and subsequent output via enranda_entropy_u16/u32/u64/u8_list_get(), else one.

  DO NOT attempt to extract entropy directly from *enranda_base, which is in the form of protoentropy, which by definition is diffusely entropic and therefore unsafe.
*/
  u16 history_hash;
  u8 not_ready_status;
  u16 sequence_hash;
  u16 sequence_hash_count;
  u16 *sequence_hash_count_list_base;
  u16 sequence_hash_idx;
  u16 *sequence_hash_list_base;
  u16 sequence_hash_old;
  u16 time;
  u16 timedelta;
  u8 timestamp_count;
  u64 timestamp_x4;
  u16 unique_idx;
  u16 *unique_list_base;
  u16 unique0;
  u16 unique1;
/*
Make sure that we're actually in the accrual phase. If not, then the caller must have become confused and called us excessively, which is entirely possible in multithreaded scenarios.
*/
  not_ready_status=0;
  if(enranda_base->phase==ENRANDA_PHASE_ACCRUE){
/*
We are in the accrual phase. Assume that protoentropy is not ready for trapdooring.
*/
    not_ready_status=1;
    history_hash=enranda_base->history_hash;
    sequence_hash=enranda_base->sequence_hash;
    sequence_hash_idx=enranda_base->sequence_hash_idx;
    time=enranda_base->time;
    unique_idx=enranda_base->unique_idx;
    sequence_hash_list_base=&enranda_base->sequence_hash_list[0];
    sequence_hash_count_list_base=&enranda_base->sequence_hash_count_list[0];
    unique_list_base=&enranda_base->unique_list[0];
    timestamp_count=0;
    do{
/*
If we're in "fill" mode, then read 4 timestamps at a time, else one.
*/
      timedelta=time;
      if(fill_status){
        if(!timestamp_count){
/*
You might think that we should get gobs of timestamps at once for maximum performance. Counterintuitively, the optimum is around 4. The reason seems to have to do with the fact that we want to spend most of the time executing high-entropy tasks like this memory-obsessed loop, not low-entropy ones, like reading the timestamp counter; but this must be balanced against the overhead of timestamp_get(). Hence 4.
*/
          timestamp_x4=timestamp_x4_get();
          timestamp_count=4;
        }
        time=(u16)(timestamp_x4);
        timestamp_x4>>=U16_BITS;
        timestamp_count--;
      }else{
        time=(u16)(timestamp_get());
      }
/*
Get timedelta, which is the low 16 bits of: (the most recent timestamp) minus (the previous timestamp). It's theoretically possible that it would always be zero -- in particular, if the CPU clock is throttled by a factor of at least (2^16). This could plausibly occur on CPUs operating in the terahertz range. But in such case, implicitly, we're extremely idle and in no urgent need of entropy anyway. Fundamentally, utilizing the entire 64 bits of timedelta would be more hassle than its entropy is worth.
*/
      timedelta=(u16)(time-timedelta);
/*
Accrue a "corkscrew hash" of the timedelta sequence. Just like its metal counterpart, a corkscrew hash rotates on each iteration in order to make it as sensitive as possible to the order of particular (timedelta)s, yet symmetric with respect to each iteration in that all (timedelta)s influence all bits of the corkscrew hash with equal weight. We rotate (right) by 3 because it's empirically optimal, but any odd value would be reasonable.
*/
      sequence_hash=(u16)((sequence_hash>>3)+(sequence_hash<<(U16_BITS-3))+timedelta);
/*
Load sequence_hash_count, which is the number of times that we've seen this particular sequence_hash within the last (2^16) timedelta reads.
*/
      sequence_hash_count=sequence_hash_count_list_base[sequence_hash];
      sequence_hash_count++;
/*
If sequence_hash_count has wrapped, then our statistics will get totally warped, in which case, ignore this timedelta (but continue to accrue sequence_hash). (Yes, this is actually a plausible scenario: a quiescent, largely serialized CPU could in theory generate (2^16) identical timedeltas in a row.)
*/
      if(sequence_hash_count){
/*
Replace the oldest ((2^16) (sequence_hash)es ago) sequence_hash from the ring list at sequence_hash_list_base with its new value. Contemporaneously decrement the population of the old hash and increment the population of the new one.
*/
        sequence_hash_old=sequence_hash_list_base[sequence_hash_idx];
        sequence_hash_count_list_base[sequence_hash]=sequence_hash_count;
        sequence_hash_count_list_base[sequence_hash_old]--;
        sequence_hash_list_base[sequence_hash_idx]=sequence_hash;
        sequence_hash_idx++;
/*
If we have seen this sequence_hash in the last (2^16) such hashes, then ignore it because it's assumed to be predictable using timing models of the physical machine. But otherwise, we have a strong justification for assuming that it's worth at least 16 bits of entropy. (I realize that this is a discontinuity, but at some point, I think it's entirely possible to draw a line in the sand, beyond which even pseudorandom timestamps are no longer predictable by an attacker; (2^16) _unique_ _sequences_ of timestamps seems well beyond that line, and furthermore, trying to capture fewer entropy bits at a time is an enormously complex game which frankly isn't worth the modest bandwidth improvement.) Yes, an attacker could hijack the timestamp reporting mechanism, but in that case, he has total machine control anyway; the scenario we need to prevent is an unprivileged attacker predicting the timedelta behavior of this function.
*/
        if(sequence_hash_count==1){
/*
Compute history_hash, which is a corkscrew hash of the entire history of (sequence_hash)es. But this time we rotate by 1 in order to minimize resonance with sequence_hash. Contemporaneously, load unique0 and unique1, which are unique among all values at unique_list_list, each being one of (2^16) different (u16)s. Granted, there is a tiny chance that they happen to be identical, but this doesn't hurt anything. Swap their positions with respect to unique_list_base, thereby creating permutative information. Note that such information is only protoentropy instead of actual entropy because permutations are not uniformally distributed.
*/
          unique0=unique_list_base[unique_idx];
          history_hash=(u16)((history_hash>>1)+(history_hash<<(U16_BITS-1))+sequence_hash);
          unique1=unique_list_base[history_hash];
          sequence_hash=0;
          unique_list_base[history_hash]=unique0;
          unique_list_base[unique_idx]=unique1;
          unique_idx++;
          if(!unique_idx){
/*
Every u16 at unique_list_base has been randomly swapped with another one. So we've randomly selected one of ((2^16)!) possible permutations. Time to trapdoor the permutative protoentropy into actual entropy.
*/
            fill_status=0;
            not_ready_status=0;
            enranda_base->phase=ENRANDA_PHASE_TRAPDOOR;
          }
        }
      }
    }while(fill_status);
    enranda_base->unique_idx=unique_idx;
    enranda_base->time=time;
    enranda_base->sequence_hash_idx=sequence_hash_idx;
    enranda_base->sequence_hash=sequence_hash;
    enranda_base->history_hash=history_hash;
  }
  return not_ready_status;
}

void
enranda_entropy_uint_list_get(enranda_t *enranda_base, ULONG entropy_idx_min, ULONG entropy_uint_count_minus_1, u16 *entropy_u16_list_base, u32 *entropy_u32_list_base, u64 *entropy_u64_list_base, u8 *entropy_u8_list_base){
/*
Output entropy from Enranda's private data structure to a subregion of a list owned by the caller. This function is private because callers should use enranda_entropy_u16/u32/u64/u8_list_get() instead.

In:

  If enranda_entropy_accrue() has not yet returned zero, then this function will take a lot longer because it must first finish filling the internal protoentropy list. It is nonetheless unnecessary (and in some scenarios, less efficient) to call that function before this one.

  enranda_base is the return value of enranda_init().

  entropy_idx_min offsets entropy_u16/u32/u64/u8_list_base.

  entropy_uint_count_minus_1 is one less than the number of (uint)s of entropy to write to entropy_u16/u32/u64/u8_list_base.

  At most one of the following may be non(NULL):

    u16_list_base is the base of a writable but undefined list of (entropy_uint_count_minus_1+1) (u16)s.

    u32_list_base is the base of a writable but undefined list of (entropy_uint_count_minus_1+1) (u32)s.
    
    u64_list_base is the base of a writable but undefined list of (entropy_uint_count_minus_1+1) (u64)s.

    u8_list_base is the base of a writable but undefined list of (entropy_uint_count_minus_1+1) (u8)s.
    
Out:

  If more entropy is required, the caller can call this function or enranda_entropy_accrue() next, just as before. Internal entropy may or may not be exhausted; if not, the latter will return zero. Otherwise, call enranda_free().

  *entropy_u16/u32/u64/u8_list_base has been overwritten with entropy in the region defined on In. The source of the entropy is the sum of the lower and upper halves of a u16 permutation. (The permutation contains every possible u16 exactly once a in random order as determined by enranda_entropy_accrue().) Carry propagation terminates no more often than every 16 bits. The effect is to produce entropy which, although constrained in the sense that not every possible binary state is reachable, is nontheless indistinct from noise in practice. For example, an output of (2^15) zero (u16)s is sometimes impossible (depending on Enranda's internal list alignment and the requested output granularity), but ((2^15)-1) such zeroes is always possible, which is clearly an indetectable difference. Addition is of critical importance, as neither subtraction nor xoring is capable of producing any zeroes at all.
*/
  ULONG entropy_uint_count;
  u16 output_idx;
  u32 output_u16_count;
  u32 output_uint_count;
  u16 trapdoor;
  u32 trapdoor_u32;
  u64 trapdoor_u64;
  u16 *unique_list_base;
  u16 unique0;
  u16 unique1;
  u32 unique0_u32;
  u32 unique1_u32;
  u64 unique0_u64;
  u64 unique1_u64;

  unique_list_base=&enranda_base->unique_list[0];
  entropy_uint_count=entropy_uint_count_minus_1+1;
  do{
/*
Ensure that we have accrued sufficient protoentropy.
*/
    if(enranda_base->phase==ENRANDA_PHASE_ACCRUE){
      enranda_entropy_accrue(enranda_base, 1);
    }
/*
The number of entropy (u16)s that we can output is at most half the size of the protoentropy (permutation) because its upper and lower haves will be added together in-place. We need to subtract the number of (u16)s already output since the last time we filled the protoentropy list (for example, in the if() above)). The difference is guaranteed to be nonzero on account of the refill check at the bottom of this function. Note that when (enranda_base->phase==ENRANDA_PHASE_ACCRUE), unique_idx indexes the protoentropy (on [0, U16_MAX]), but when (enranda_base->phase==ENRANDA_PHASE_TRAPDOOR), it indexes the entropy (on [0, U16_SPAN_HALF-1]).
*/
    output_idx=enranda_base->unique_idx;
    output_u16_count=(u32)(U16_SPAN_HALF-output_idx);
    if(entropy_u8_list_base){
/*
The caller has requested u8 granularity, which is rather pathetic but sometimes necessary. Convert output_u16_count to u8 units.
*/
      output_uint_count=output_u16_count<<U16_SIZE_LOG2;
/*
If less entropy is requested than is available, adjust the number of (u8)s to output accordingly.
*/
      if(entropy_uint_count<output_uint_count){
        output_uint_count=(u32)(entropy_uint_count);
/*
Because the number of (u8)s of entropy available is always even, the only way in which we could output an odd number thereof is if requested to do so by the caller. Check for that here.
*/
        if(entropy_uint_count&1){
          unique0=unique_list_base[output_idx];
          unique1=unique_list_base[output_idx+U16_SPAN_HALF];
          entropy_uint_count--;
/*
Use an entire u16 of entropy to issue the extra (odd count) u8 requested. Doing so allows us to maintain u16 granularity, at the expense of poor performance in a rare corner case. But it's more than that: we cannot cause carry discontinuities more often than every 16 bits, per Out.
*/
          output_uint_count--;
          output_idx++;
          trapdoor=(u16)(unique0+unique1);
          entropy_u8_list_base[entropy_idx_min]=(u8)(trapdoor);
          entropy_idx_min++;
        }
      }
/*
We have an even number of (u8)s left to issue. Do so.
*/
      if(output_uint_count){
        entropy_uint_count-=output_uint_count;
/*
entropy_idx_min indexes the caller's output list. Precompensate it negatively so we can increment it while waiting for the occasional cache stall upon *unique_list_base access.
*/
        entropy_idx_min-=U16_SIZE;
        do{
/*
Load 2 unique values, one from each half of the permutation.
*/
          unique0=unique_list_base[output_idx];
          unique1=unique_list_base[output_idx+U16_SPAN_HALF];
          entropy_idx_min+=U16_SIZE;
          output_uint_count-=U16_SIZE;
          output_idx++;
/*
Trapdoor the unique values by finding their sum, then output it as entropy. Maintain carry continuity for 16 bits, as required by Out.
*/
          trapdoor=(u16)(unique0+unique1);
          entropy_u8_list_base[entropy_idx_min]=(u8)(trapdoor);
          entropy_u8_list_base[entropy_idx_min+1]=(u8)(trapdoor>>U8_BITS);
        }while(output_uint_count);
        entropy_idx_min+=U16_SIZE;
      }
    }else if(entropy_u16_list_base){
/*
Do the same as above, with these differences: (1) there is no chance that the caller requested an odd number of (u8)s and (2) the uint count is identical to the u16 count because that's the granularity in this case.
*/
      if(entropy_uint_count<output_u16_count){
        output_u16_count=(u32)(entropy_uint_count);
      }
      entropy_uint_count-=output_u16_count;
      entropy_idx_min--;
      do{
        unique0=unique_list_base[output_idx];
        unique1=unique_list_base[output_idx+U16_SPAN_HALF];
        entropy_idx_min++;
        output_u16_count--;
        output_idx++;
        trapdoor=(u16)(unique0+unique1);
        entropy_u16_list_base[entropy_idx_min]=trapdoor;
      }while(output_u16_count);
      entropy_idx_min++;
    }else if(entropy_u32_list_base){
/*
Do the same as above, after converting the requested u32 count to a u16 count. (Hopefully the compiler is smart enough to consolidate multiple sequential u16 reads.) It's possible that we have only one u16 left, in which case set output_idx to its postterminal value, thereby forcing reaccrual at the bottom of this loop.
*/
      output_uint_count=output_u16_count>>(U32_SIZE_LOG2-U16_SIZE_LOG2);
      if(entropy_uint_count<output_uint_count){
        output_uint_count=(u32)(entropy_uint_count);
      }
      if(output_uint_count){
        entropy_uint_count-=output_uint_count;
        entropy_idx_min--;
        do{
          unique0_u32=((u32)(unique_list_base[output_idx+1])<<U16_BITS)|unique_list_base[output_idx];
          unique1_u32=((u32)(unique_list_base[output_idx+U16_SPAN_HALF+1])<<U16_BITS)|unique_list_base[output_idx+U16_SPAN_HALF];
          entropy_idx_min++;
          output_uint_count--;
          output_idx=(u16)(output_idx+2);
          trapdoor_u32=unique0_u32+unique1_u32;
          entropy_u32_list_base[entropy_idx_min]=trapdoor_u32;
        }while(output_uint_count);
        entropy_idx_min++;
      }else{
        output_idx=U16_SPAN_HALF;
      }
    }else{
/*
Do the same as above, after converting the requested u64 count to a u16 count.
*/
      output_uint_count=output_u16_count>>(U64_SIZE_LOG2-U16_SIZE_LOG2);
      if(entropy_uint_count<output_uint_count){
        output_uint_count=(u32)(entropy_uint_count);
      }
      if(output_uint_count){
        entropy_uint_count-=output_uint_count;
        entropy_idx_min--;
        do{
          unique0_u64=((u64)(unique_list_base[output_idx+3])<<48)|((u64)(unique_list_base[output_idx+2])<<U32_BITS)|(((u32)(unique_list_base[output_idx+1])<<U16_BITS)|unique_list_base[output_idx]);
          unique1_u64=((u64)(unique_list_base[output_idx+U16_SPAN_HALF+3])<<48)|((u64)(unique_list_base[output_idx+U16_SPAN_HALF+2])<<U32_BITS)|(((u32)(unique_list_base[output_idx+U16_SPAN_HALF+1])<<U16_BITS)|unique_list_base[output_idx+U16_SPAN_HALF]);
          entropy_idx_min++;
          output_uint_count--;
          output_idx=(u16)(output_idx+4);
          trapdoor_u64=unique0_u64+unique1_u64;
          entropy_u64_list_base[entropy_idx_min]=trapdoor_u64;
        }while(output_uint_count);
        entropy_idx_min++;
      }else{
        output_idx=U16_SPAN_HALF;
      }
    }
    if(output_idx==U16_SPAN_HALF){
/*
Protoentropy has been exhausted, so we cannot produce any more entropy. Revert to accrual, which will completely refill the protoentropy list, either on the next loop or the next call to this function or enranda_entropy_accrue().
*/
      enranda_base->phase=ENRANDA_PHASE_ACCRUE;
      output_idx=0;
    }
    enranda_base->unique_idx=output_idx;
  }while(entropy_uint_count);
  return;
}

void
enranda_entropy_u16_list_get(enranda_t *enranda_base, ULONG entropy_idx_min, ULONG entropy_u16_count_minus_1, u16 *entropy_u16_list_base){
/*
Output a list of entropy (u16)s.

In:

  enranda_base is the return value of enranda_init().

  entropy_idx_min offsets entropy_u16_list_base.

  entropy_u16_count_minus_1 is one less than the number of (u16)s of entropy to write to entropy_u16_list_base.

Out:

  See enranda_entropy_uint_list_get():Out.
*/
  enranda_entropy_uint_list_get(enranda_base, entropy_idx_min, entropy_u16_count_minus_1, entropy_u16_list_base, NULL, NULL, NULL);
  return;
}

void
enranda_entropy_u32_list_get(enranda_t *enranda_base, ULONG entropy_idx_min, ULONG entropy_u32_count_minus_1, u32 *entropy_u32_list_base){
/*
Output a list of entropy (u32)s.

In:

  enranda_base is the return value of enranda_init().

  entropy_idx_min offsets entropy_u32_list_base.

  entropy_u32_count_minus_1 is one less than the number of (u32)s of entropy to write to entropy_u32_list_base.

Out:

  See enranda_entropy_uint_list_get():Out.
*/
  enranda_entropy_uint_list_get(enranda_base, entropy_idx_min, entropy_u32_count_minus_1, NULL, entropy_u32_list_base, NULL, NULL);
  return;
}

void
enranda_entropy_u64_list_get(enranda_t *enranda_base, ULONG entropy_idx_min, ULONG entropy_u64_count_minus_1, u64 *entropy_u64_list_base){
/*
Output a list of entropy (u64)s.

In:

  enranda_base is the return value of enranda_init().

  entropy_idx_min offsets entropy_u64_list_base.

  entropy_u64_count_minus_1 is one less than the number of (u16)s of entropy to write to entropy_u64_list_base.

Out:

  See enranda_entropy_uint_list_get():Out.
*/
  enranda_entropy_uint_list_get(enranda_base, entropy_idx_min, entropy_u64_count_minus_1, NULL, NULL, entropy_u64_list_base, NULL);
  return;
}

void
enranda_entropy_u8_list_get(enranda_t *enranda_base, ULONG entropy_idx_min, ULONG entropy_u8_count_minus_1, u8 *entropy_u8_list_base){
/*
Output a list of entropy (u8)s.

In:

  enranda_base is the return value of enranda_init().

  entropy_idx_min offsets entropy_u8_list_base.

  entropy_u8_count_minus_1 is one less than the number of (u8)s of entropy to write to entropy_u8_list_base.

Out:

  See enranda_entropy_uint_list_get():Out.
*/
  enranda_entropy_uint_list_get(enranda_base, entropy_idx_min, entropy_u8_count_minus_1, NULL, NULL, NULL, entropy_u8_list_base);
  return;
}

void *
enranda_free(void *base){
/*
To maximize portability and debuggability, this is the only place where Enranda frees memory.

In:

  base is the return value of enranda_init() or malloc(). May be NULL.

Out:

  Returns NULL so that the caller can easily maintain the good practice of NULLing out invalid pointers.

  *enranda_base is freed.
*/
  DEBUG_FREE_PARANOID(base);
  return NULL;
}

void
enranda_rewind(enranda_t *enranda_base){
/*
Reset Enranda. Apart from one-time memory initialization, this is unnecessary and wastes time, but may be useful for testing purposes. enranda_entropy_u16/u32/u64/u8_int_list_get() automatically performs fast and secure cleanup between entropy outputs, so there is no reason in practice for a published application to call this function.
*/
  u16 idx;
  u16 *sequence_hash_count_list_base;
  u16 *sequence_hash_list_base;
  u16 *unique_list_base;

  sequence_hash_count_list_base=&enranda_base->sequence_hash_count_list[0];
  sequence_hash_list_base=&enranda_base->sequence_hash_list[0];
  unique_list_base=&enranda_base->unique_list[0];
  idx=0;
  do{
/*
Assume that every possible u16 sequence hash has occurred at some point in the last (2^16) such hashes. This is a guard against jumping to the conclusion that a popular and predictable sequence constitutes a novel event.
*/
    sequence_hash_count_list_base[idx]=1;
/*
Consistent with the previous line, fill the ring list of sequence hashes with one occurrence of each possible u16 hash.
*/
    sequence_hash_list_base[idx]=idx;
/*
Create a permutation involving integers on [0, U16_MAX]. A unique rearrangement of this permutation will constitute the protoentropy used to produce entropy.
*/
    unique_list_base[idx]=idx;
  }while((idx++)!=U16_MAX);
  enranda_base->history_hash=0;
  enranda_base->sequence_hash=0;
  enranda_base->sequence_hash_idx=0;
/*
Set time to zero instead of the actual timestamp. We do this because (1) it would be unwise to risk doing an OS call during a rewind operation which is conventionally limited to memory transactions and (2) storing the actual timestamp would _increase_ the predictability of the first sequence_hash to be discovered by enranda_entropy_accrue().
*/
  enranda_base->time=0;
  enranda_base->unique_idx=0;
  enranda_base->phase=ENRANDA_PHASE_ACCRUE;
  return;
}

enranda_t *
enranda_init(u32 build_break_count, u32 build_feature_count){
/*
Verify that the source code is sufficiently updated and initialize persistent storage.

To maximize portability and debuggability, this is the only place where Enranda allocates memory.

In:

  build_break_count is the caller's most recent knowledge of ENRANDA_BUILD_BREAK_COUNT, which will fail if the caller is unaware of all critical updates.

  build_feature_count is the caller's most recent knowledge of ENRANDA_BUILD_FEATURE_COUNT, which will fail if this library is not up to date with the caller's expectations.

Out:

  Returns NULL if (build_break_count!=ENRANDA_BUILD_BREAK_COUNT); or (build_feature_count>ENRANDA_BUILD_FEATURE_COUNT); or if we failed to allocate memory. Otherwise, returns the base of Enranda's private data structure, in which case, pass it to enranda_entropy_accrue() or enranda_entropy_u16/u32/u64/u8_list_get(). After use, free it with enranda_free().
*/
  enranda_t *enranda_base;

  enranda_base=NULL;
  if((build_break_count==ENRANDA_BUILD_BREAK_COUNT)&&(build_feature_count<=ENRANDA_BUILD_FEATURE_COUNT)){
    enranda_base=(enranda_t *)(DEBUG_MALLOC_PARANOID(sizeof(enranda_t)));
    if(enranda_base){
      enranda_rewind(enranda_base);
    }
  }
  return enranda_base;
}
