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
Demo
*/
#include "flag.h"
#include "flag_dyspoissometer.h"
#include "flag_timestamp.h"
#include "flag_enranda.h"
#include <math.h>
#ifdef DYSPOISSOMETER_NUMBER_QUAD
  #include <quadmath.h>
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <quadmath.h>
#include "constant.h"
#include "debug.h"
#include "debug_xtrn.h"
#ifdef DYSPOISSOMETER_NUMBER_QUAD
  #include "debug_quad.h"
  #include "debug_quad_xtrn.h"
#endif
#include "dyspoissometer.h"
#include "dyspoissometer_xtrn.h"
#include "enranda.h"
#include "enranda_xtrn.h"
#include "timestamp_xtrn.h"

#define DEMO_BUILD_ID (DYSPOISSOMETER_BUILD_ID+ENRANDA_BUILD_ID)

u8
demo_bimobius_u16_list_get(u32 u16_idx_max, u16 *u16_list_base){
/*
Generate the (binary) terms of the bimobius function.
*/
  u8 bimobius_bit;
  u16 bimobius_u16;
  u8 bit_idx;
  u8 continue_status;
  u32 dividend;
  u32 divisor;
  u32 n;
  u32 prime_factor_count;
  u32 prime_idx;
  u32 prime_idx_max;
  u32 prime_idx_max_max;
  u32 *prime_list_base;
  u32 *prime_list_base_new;
  u64 prime_list_size;
  u32 remainder;
  u8 status;
  u32 u16_idx;

  status=1;
  prime_list_size=U32_SIZE;
  prime_list_base=(u32 *)(malloc((size_t)(prime_list_size)));
  prime_idx_max=0;
  prime_idx_max_max=0;
  if(prime_list_base){
    continue_status=1;
    prime_list_base[0]=3;
    u16_idx=0;
    bit_idx=3;
    bimobius_u16=6;
    n=5;
    do{
      prime_factor_count=0;
      dividend=n;
      if(!(dividend&1)){
        prime_factor_count++;
        dividend>>=1;
      }
      prime_idx=0;
      do{
        divisor=prime_list_base[prime_idx];
        remainder=dividend%divisor;
        if(!remainder){
          dividend/=divisor;
          remainder=dividend%divisor;
          if(remainder){
            prime_factor_count++;
            if(dividend==1){
              break;
            }
          }else{
            prime_factor_count=0;
            break;
          }
        }
        if(dividend<(divisor*divisor)){
          prime_factor_count++;
          break;
        }
      }while((prime_idx++)!=prime_idx_max);
      if(prime_factor_count==1){
        if(prime_idx_max==prime_idx_max_max){
          prime_idx_max_max=(prime_idx_max_max<<1)+1;
          prime_list_size<<=1;
          prime_list_base_new=(u32 *)(malloc((size_t)(prime_list_size)));
          if(prime_list_base_new){
            prime_list_size>>=1;
            memcpy(prime_list_base_new, prime_list_base, (size_t)(prime_list_size));
            prime_list_size<<=1;
            free(prime_list_base);
            prime_list_base=prime_list_base_new;
          }else{
            free(prime_list_base);
            prime_list_base=NULL;
            break;
          }
        }
        prime_idx_max++;
        prime_list_base[prime_idx_max]=n;
      }
      bimobius_bit=prime_factor_count&1;
      if(prime_factor_count){
        bimobius_u16=(u16)(bimobius_u16+((u16)(bimobius_bit)<<bit_idx));
        bit_idx++;
        if(bit_idx==U16_BITS){
          u16_list_base[u16_idx]=bimobius_u16;
          if(u16_idx!=u16_idx_max){
            u16_idx++;
            bit_idx=0;
            bimobius_u16=0;
          }else{
            continue_status=0;
          }
        }
      }
      n++;
      n=(u32)(n+!(n&3));
    }while(continue_status);
    if(prime_list_base){
      status=0;
      free(prime_list_base);
    }
  }
  return status;
}

void
demo_print_out_of_memory(void){
  DEBUG_PRINT("\nERROR: Out of memory!\n");
  return;
}

int
main (void){
  u32 bimobius_bit;
  u8 bimobius_bit_idx;
  u32 bimobius_idx;
  u16 *bimobius_u16_list_base;
  u64 call_count;
  enranda_t *enranda_base;
  u8 entropy_not_ready_status;
  u32 i;
  u32 j;
  DYSPOISSOMETER_NUMBER kernel_density;
  DYSPOISSOMETER_NUMBER kernel_density_expected;
  DYSPOISSOMETER_NUMBER kernel_density_sum;
  DYSPOISSOMETER_UINT kernel_size;
  DYSPOISSOMETER_NUMBER kernel_skew;
  DYSPOISSOMETER_NUMBER logfreedom;
  DYSPOISSOMETER_UINT *mask_list_base;
  u16 *random_u16_list_base;
  u32 *random_u32_list_base;
  u64 random_u64_list_base[7];
  u8 random_u8_list_base[16];
  u8 status;
  u64 timestamp_delta;
  u64 timestamp_end;
  u64 timestamp_start;

  status=1;
  DEBUG_PRINT("Enranda Demo\nCopyright 2016 Russell Leidich\nhttp://enranda.blogspot.com\n");
  DEBUG_U32("build_id_in_hex", DEMO_BUILD_ID);
  DEBUG_PRINT("\nFollow along with main() in demo.c, so you can learn how to use Enranda.\nThe math details are explained at the webpage linked above.\n\n");
  DEBUG_PRINT("Before doing anything else, we need to call enranda_init() to ensure that\nall required features, bug fixes, and performance improvements are present.\n\n");
  enranda_base=enranda_init(0, 0);
  bimobius_u16_list_base=NULL;
  random_u16_list_base=NULL;
  random_u32_list_base=NULL;
  do{
    if(!enranda_base){
      DEBUG_PRINT("ERROR: enranda_init() failed!\n");
      break;
    }
    bimobius_u16_list_base=(u16 *)(DEBUG_MALLOC_PARANOID((U16_SPAN<<U16_SIZE_LOG2)*2));
    random_u16_list_base=(u16 *)(DEBUG_MALLOC_PARANOID(U16_SPAN<<U16_SIZE_LOG2));
    random_u32_list_base=(u32 *)(DEBUG_MALLOC_PARANOID(1234567<<U32_SIZE_LOG2));
    if(!bimobius_u16_list_base&&random_u16_list_base&&random_u32_list_base){
      demo_print_out_of_memory();
      break;
    }
/*
We don't need these memset()s, except to prove that the randomness ain't coming from memory contents!
*/
    memset(bimobius_u16_list_base, 0, (size_t)(U16_SPAN<<U16_SIZE_LOG2));
    memset(random_u8_list_base, 0, (size_t)(16));
    memset(random_u16_list_base, 0, (size_t)(U16_SPAN<<U16_SIZE_LOG2));
    DEBUG_PRINT("OK that worked. So let's make some noise! Start simple. Let's generate 16\nrandom bytes using enranda_entropy_u8_list_get(). We'll store them to\n*random_u8_list_base, which is a preallocated array. Here we go...\n\n");
    enranda_entropy_u8_list_get(enranda_base, 0, 16-1, random_u8_list_base);
    DEBUG_LIST("random_u8_list_base", 16, random_u8_list_base, U8_SIZE_LOG2);
    DEBUG_PRINT("\nBy the way, we actually generated (2^19) bits of entropy behind the scenes,\nwhich is Enranda's private granularity. As explained on the webpage, Enranda\noutputs less entropy than it inputs, so there is no pseudorandomness involved,\napart from trapdooring big entropy into small entropy. But is the result\nreally convincingly random? With only 16 bytes, who knows...\n\n");
    DEBUG_PRINT("To help answer this question, we can create (2^16) samples, each of which 16\nbits in size. If it's truly random, the logfreedom of the result should be\nequally likely to be less than or equal to the median logfreedom, as greater\nthan or equal to it. dyspoissometer_logfreedom_median_get() will provide us\nwith the median logfreedom. Unfortunately, it takes too long for demo purposes,\nbut if you run it yourself, you'll find that the median is close to\n7.267915800940217E+05; the exact median is combinatorially hard to find, but\npolynomially easy to approximate (and more accurately so than we require).\n\n");
    DEBUG_PRINT("By the way, logfreedom is most useful as a randomness test when the mask count\n(2^16, in this case) equals the mask span (which it does). So let's get 10 such\nrandom mask sets, and measure the logfreedom of each:\n\n");
    DEBUG_PRINT("logfreedom_median=7.267915800940217E+05\n");
    status=0;
    i=0;
    do{
      timestamp_start=timestamp_get();
      enranda_entropy_u16_list_get(enranda_base, 0, U16_MAX, random_u16_list_base);
      timestamp_end=timestamp_get();
      logfreedom=dyspoissometer_u16_list_logfreedom_get(U16_MAX, 0, U16_MAX, random_u16_list_base);
      if(logfreedom<0.0f){
        status=1;
        demo_print_out_of_memory();
        break;
      }
      DEBUG_NUMBER("logfreedom", logfreedom);
    }while((i++)!=9);
    if(status){
      demo_print_out_of_memory();
      break;
    }
    timestamp_delta=timestamp_end-timestamp_start;
    DEBUG_U64("timestamp_delta", timestamp_delta);
    DEBUG_PRINT("\nAs to performance, this ^ hexadecimal value is the number of CPU clock ticks\nthat it took to generate (2^19) bits of entropy. Based on CPU frequency, you\ncan accurately predict how long it would take to generate some larger number of\nbits; lesser populations take the same amount of time.\n\n");
    DEBUG_PRINT("Granted, 10 samples is not enough to really see whether we're straddling the\nmedian with equal probability to the left and right, but you can easily run\nthis demo several times and tabulate the results for yourself (or just modify\nthe code). In any event, it should be obvious from the results above that\nEnranda outputs very hard randomness which dances around the median, just like\nan ideal TRNG would. A weak TRNG would tend to stay below the median, and\nperhaps move up to it over time, or alternatively remain stuck at maximum\nlogfreedom all the time because it was contrived to do so pseudorandomly.\n\n");
    DEBUG_PRINT("But how does this logfreedom compare to that of a \"good\" PRNG? For some\ninsights on this matter, we turn to the Mobius function.\n\n");
    DEBUG_PRINT("The Mobius function is defined as follows, for all positive integers N:\n\n");
    DEBUG_PRINT("  +1 for N=1\n");
    DEBUG_PRINT("  -1 for N=2\n");
    DEBUG_PRINT("  else 0 if N contains a prime factor with an exponent of at least 2\n");
    DEBUG_PRINT("  else +1 if N contains an even number of prime factors\n");
    DEBUG_PRINT("  else -1\n\n");
    DEBUG_PRINT("According to Wikipedia, the first 23 values starting with N=1 are:\n\n");
    DEBUG_PRINT("{1, -1, -1, 0, -1, 1, -1, 0, 0, 1, -1, 0, -1, 1, 1, 0, -1, 0, -1, 0, 1, 1, -1}\n\n");
    DEBUG_PRINT("Informally, the famous Riemann Hypothesis says that Mobius is random.\nSpecifically, +/-1 occur with asymptotically equal probability; however, there\nis a dearth of zeroes. Thus we have in Mobius a ternary PRNG in which the\nmiddle value exhibits bias. Because of this bias, we must instead turn to what\nI call the \"bimobius\" (binary Mobius) function as a randomness standard,\nwhich is replicated in both demo.c and mathematica.txt for your convenience.\n\n");
    DEBUG_PRINT("To understand bimobius, we first remove the zeroes from the Mobius function:\n\n");
    DEBUG_PRINT("{1, -1, -1, -1, 1, -1, 1, -1, -1, 1, 1, -1, -1, 1, 1, -1}\n\n");
    DEBUG_PRINT("Then we convert +/-1 to their sign bits -- zero and one, respectively. Hang on\na moment while I generate a big blob of bimobius...\n\n");
    status=demo_bimobius_u16_list_get((U16_SPAN*2)-1, bimobius_u16_list_base);
    if(status){
      demo_print_out_of_memory();
      break;
    }
    status=1;
    printf("{");
    i=0;
    do{
      bimobius_idx=i>>U16_BITS;
      bimobius_bit_idx=i&U16_BIT_MAX;
      bimobius_bit=(bimobius_u16_list_base[bimobius_idx]>>bimobius_bit_idx)&1;
      printf("%d", bimobius_bit);
      if(i!=15){
        printf(", ");
      }
    }while((i++)!=15);
    DEBUG_PRINT("}\n\n");
    DEBUG_PRINT("Done! By the way, the above text was generated by demo_bimobius_u16_list_get().\n\nIt may also be of interest to you that if we have N terms of Mobius, then this\nwill asymptotically reduce to (6N/(pi^2)) terms of bimobius. The reason relates\nto the fact that the bimobius function is essentially a mapping of the\nleast-common-multiples of coprimes to their number of prime factors mod 2.\n(6/(pi^2)) is the probability that 2 random positive integers will be coprime.\n\n");
    DEBUG_PRINT("So what's the logfreedom of a bitstring containing (2^20) ((2^16) samples of 16\nbits each) sequential bimobius bits? Let's see...\n\n");
    logfreedom=dyspoissometer_u16_list_logfreedom_get(U16_MAX, 0, U16_MAX, bimobius_u16_list_base);
    if(logfreedom<0.0f){
      demo_print_out_of_memory();
      break;
    }
    DEBUG_NUMBER("bimobius_logfreedom", logfreedom);
    DEBUG_PRINT("\nAs you should be able to see from above, Enranda's logfreedom is similar to\nthat of bimobius. (Of course, bimobius has infinite domain, so \"similar\" is the\nbest we can say.) Now, because bimobius is actually more random than Mobius for\nthe reasons stated above, Enranda is thus noisier than Mobius, which is in turn\nmore random than any polynomial boolean function if the Riemann Hypothesis is\ntrue (http://arxiv.org/pdf/1103.4991v4.pdf).\n\n");
    DEBUG_PRINT("Now we should take some time to explain how to use Enranda. It all starts with\nenranda_init(), a call to which you can see in main(). (Watch out for a NULL\nreturn value!) Then you have 2 options for generating entropy, which you can\nchange anytime: (1) low-latency, low-bandwith call-by-call accrual, until\n(2^19) bits are ready to issue or (2) high-bandwidth single-call accrual,\nwherein entropy is generated (2^19) bits at a time until the caller's\nbuffer has been filled. Each option buffers pregenerated entropy for future\ncalls, so once (2^19) bits are generated, they can be served out as needed,\nwhereupon they are automatically refreshed when depleted. For the first\noption, see enranda_entropy_accrue(); for the other, see\nenranda_entropy_u16/u32/u64/u8_list_get().\n\n");
    DEBUG_PRINT("Let's practice calling enranda_entropy_accrue(), which you can follow along\nwith in main(). The whole point of this function is to allow a caller to\naccrue a bit or so of entropy with each call, which keeps latency low\nbut allows the caller to accrue randomness while idle, for example, while\nwaiting for a network connection. (Just make sure not to call it so often that\nyour process stays awake when it should actually go to sleep and save on energy\nconsuption; ultimately option #2 may be better, depending on the situation.)\nHere we go:\n\n");
/*
Normally, DO NOT waste time calling enranda_rewind(). We do so here because the entropy buffer is already mostly full, on account of the bytes we just generated above, and we're trying to demo a realistic accrual process.
*/
    enranda_rewind(enranda_base);
    call_count=0;
    do{
/*
Do some work, e.g. update the screen or process a mouse movement. Then, when you're idle, call enranda_entropy_accrue() with (fill_status==0). In practice, you would never code a tight loop like this (although it's safe to do so) because in that case, enranda_entropy_u16/u32/u64/u8_list_get() would provide much greater throughput.
*/
      entropy_not_ready_status=enranda_entropy_accrue(enranda_base, 0);
      call_count++;
    }while(entropy_not_ready_status);
    DEBUG_U64("call_count", call_count);
    DEBUG_PRINT("\nenranda_entropy_accrue() finally returned zero, after call_count accrual calls.\nIn other words, it took that many calls to produce (2^20) bits of protoentropy,\nwhich will provide for (2^19) bits of output entropy. Typically, each call\nprovides about 3 bits of protoentropy, which is why option #2 is faster if you\ncan tolerate the latency required to fill the entire protoentropy buffer.\n\n");
    DEBUG_PRINT("Now it's time to call enranda_entropy_u16/u32/u64/u8_list_get() to get a string\nof entropy out of the (2^19) random bits that we just generated. Let's get it\nin u64 form because that's the most efficicient. We'll send them to\n*random_u64_list_base. But first, we'll zero that memory (totally unnecessary)\njust so you can see the entropy appear:\n\n");
    memset(random_u64_list_base, 0, (size_t)(7<<U64_SIZE_LOG2));
    DEBUG_LIST("random_u64_list_base", 7, (u8 *)(random_u64_list_base), U64_SIZE_LOG2);
    DEBUG_PRINT("\nOK now let's fill it with 3 (u64)s of entropy, starting at index 2 (just\nbecause it makes for a good demo):\n\n");
    enranda_entropy_u64_list_get(enranda_base, 2, 3-1, random_u64_list_base);
    DEBUG_LIST("random_u64_list_base", 7, (u8 *)(random_u64_list_base), U64_SIZE_LOG2);
    DEBUG_PRINT("\nNow let's say that for some crazy reason, we need 1234567 (u32)s of entropy\nimmediately. We can call enranda_entropy_u32_list_get() to fill a buffer in\nmemory, regardless of the previous output -- if any -- from\nenranda_entropy_accrue(). Enranda will internally generate entropy in units of\n(2^19) bits, as always, but the caller need not care about that. Here we go:\n\n");
    timestamp_start=timestamp_get();
    enranda_entropy_u32_list_get(enranda_base, 0, 1234567-1, random_u32_list_base);
    timestamp_end=timestamp_get();
    timestamp_delta=timestamp_end-timestamp_start;
    DEBUG_U64("timestamp_delta", timestamp_delta);
    DEBUG_PRINT("\nDone! That ^ is how many CPU ticks it took for (1234567*32)=39506144 bits (a\nfew more, actually, if we account for the block size).\n\n");
    DEBUG_PRINT("For sake of brevity, here are the first 4 (u32)s:\n\n");
    DEBUG_LIST("random_u32_list_base", 4, (u8 *)(random_u32_list_base), U32_SIZE_LOG2);
    DEBUG_PRINT("\n...and the last 4:\n\n");
    DEBUG_LIST("random_u32_list_base[1234563]", 4, (u8 *)(&random_u32_list_base[1234563]), U32_SIZE_LOG2);
    DEBUG_PRINT("\nNow let's look at Enranda's mean order-(2^16) kernel density, as measured over\n1000 samples (of (2^16) 16-bit random values, per sample). Kernel density is\nexplained at:\n\n");
    DEBUG_PRINT("http://cognomicon.blogspot.com/2014/12/the-kernel-density-randomness-metric.html\n\n");
    DEBUG_PRINT("This might take a few minutes because kernel density is extremely noisy and\ntherefore requires longterm averaging. I will display 10 intermediate results\nso you can see the progress. But first, I will compute the expected kernel\ndensity, to which Enranda should in theory converge...\n\n");
    mask_list_base=dyspoissometer_uint_list_malloc_zero(U16_MAX);
    if(!mask_list_base){
      demo_print_out_of_memory();
      break;
    }
    kernel_density_expected=dyspoissometer_kernel_density_expected_slow_get(U16_MAX);
    DEBUG_NUMBER("kernel_density_expected", kernel_density_expected);
    i=0;
    kernel_density_sum=0.0f;
    do{
      enranda_entropy_u16_list_get(enranda_base, 0, U16_MAX, random_u16_list_base);
      for(j=0; j<=U16_MAX; j++){
        mask_list_base[j]=random_u16_list_base[j];
      }
      kernel_size=dyspoissometer_kernel_size_get(U16_MAX, mask_list_base);
      if(kernel_size){
        kernel_density_sum+=dyspoissometer_kernel_density_get(U16_MAX, kernel_size);
        if(i&&(!(i%100))){
          kernel_density=kernel_density_sum/(i+1);
          DEBUG_NUMBER("kernel_density_mean", kernel_density);
        }
      }
    }while(((i++)!=(1000-1))&&kernel_size);
    if(!kernel_size){
      demo_print_out_of_memory();
      break;
    }
    kernel_density=kernel_density_sum/1000;
    DEBUG_NUMBER("kernel_density_mean", kernel_density);
    DEBUG_PRINT("\nTypically, you should see kernel_density_mean oscillating within about 5% of\nkernel_density_expected, or less at later values. We can measure the quality of\ntheir mutual compliance with kernel skew, as discussed at:\n\n");
    DEBUG_PRINT("http://cognomicon.blogspot.com/2014/12/the-kernel-density-randomness-metric.html\n\n");
    kernel_skew=dyspoissometer_kernel_skew_get(kernel_density, kernel_density_expected);
    DEBUG_NUMBER("enranda_kernel_skew", kernel_skew);
    DEBUG_PRINT("\nThis ^ value is always on [0.0, 1.0]. It should be as close to 0.5 as\npossible, although lesser and greater values are actually asymmetrical in\ntheir meaning. In truth, kernel skew is like dyspoissonism: it's useful for\ncomparing 2 mask lists, but is not particularly informative in and of itself.\n\n");
    DEBUG_PRINT("Therefore, for sake of comparison, here is the kernel skew of the first (2^20)\nbits of bimobius:\n\n");
    for(j=0; j<=U16_MAX; j++){
      mask_list_base[j]=bimobius_u16_list_base[j];
    }
    kernel_size=dyspoissometer_kernel_size_get(U16_MAX, mask_list_base);
    if(!kernel_size){
      demo_print_out_of_memory();
      break;
    }
    kernel_density=dyspoissometer_kernel_density_get(U16_MAX, kernel_size);
    kernel_skew=dyspoissometer_kernel_skew_get(kernel_density, kernel_density_expected);
    DEBUG_NUMBER("bimobius_kernel_skew", kernel_skew);
    DEBUG_PRINT("\nHmm... bimobius doesn't look very random. Let's try the _next_ (2^20) bits...\n\n");
    for(j=0; j<=U16_MAX; j++){
      mask_list_base[j]=bimobius_u16_list_base[j+U16_SPAN];
    }
    kernel_size=dyspoissometer_kernel_size_get(U16_MAX, mask_list_base);
    if(!kernel_size){
      demo_print_out_of_memory();
      break;
    }
    kernel_density=dyspoissometer_kernel_density_get(U16_MAX, kernel_size);
    kernel_skew=dyspoissometer_kernel_skew_get(kernel_density, kernel_density_expected);
    DEBUG_NUMBER("bimobius_kernel_skew_next", kernel_skew);
    DEBUG_PRINT("\nThat's a bit better, but still quite skewed away from randomness. Maybe the\nprime numbers are more orderly than we thought, which would not be suprising\nbecause computing bimobius is (merely) subexponentially expensive.\n\n");
    dyspoissometer_free(mask_list_base);
    DEBUG_PRINT("This concludes the demo. Now deallocate *enranda_base using enranda_free()...\n\n");
    enranda_base=enranda_free(enranda_base);
    DEBUG_PRINT("Done! Have fun using Enranda. And don't forget to try out otpenranda,\ntimedeltaprofile, and timedeltasave as well (see README.txt).\n\n");
    DEBUG_PRINT("Scroll up or redirect to a file in order to see what you missed!\n\n");
    status=0;
  }while(0);
  DEBUG_FREE_PARANOID(random_u32_list_base);
  DEBUG_FREE_PARANOID(random_u16_list_base);
  DEBUG_FREE_PARANOID(bimobius_u16_list_base);
  DEBUG_ALLOCATION_CHECK();
  return status;
}
