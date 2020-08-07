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
Timedelta Profile with Dyspoissometer
*/
#include "flag.h"
#include "flag_dyspoissometer.h"
#include "flag_timestamp.h"
#include "flag_timedeltaprofile.h"
#include <math.h>
#ifdef DYSPOISSOMETER_NUMBER_QUAD
  #include <quadmath.h>
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constant.h"
#include "dyspoissometer.h"
#include "dyspoissometer_xtrn.h"
#include "timestamp_xtrn.h"

#define MODE_DYSPOISSONISM 0
#define MODE_DYSPOISSONISM_HEX 1
#define MODE_MEAN 2
#define MODE_MEAN_HEX 3
#define MODE_HISTOGRAM 4
#define MODE_HISTOGRAM_HEX 5
#define TIMEDELTA_COUNT_LOG2_MAX 48

void
timedeltaprofile_newline_printf(void){
  printf("\n");
  return;
}

void
timedeltaprofile_double_printf(double value){
  printf("%+-1.15E", (double)(value));
  return;
}

void
timedeltaprofile_u64_printf(u64 value){
  printf("%08X%08X", (u32)(value>>32), (u32)(value));
  return;
}

int
main(int argc, char *argv[]){
  u8 continuous_status;
  u8 csv_status;
  char *digit_list_base;
  u8 digit0;
  u8 digit1;
  u8 digit2;
  DYSPOISSOMETER_NUMBER dyspoissonism;
  u64 freq;
  u64 *freq_list0_base;
  u64 *freq_list1_base;
  u8 log2_valid_status;
  DYSPOISSOMETER_NUMBER logfreedom;
  u64 mask_idx_max;
  u8 mode;
  u8 msb;
  u64 pop;
  DYSPOISSOMETER_NUMBER pop_normalized;
  u8 status;
  u16 time;
  u16 timedelta;
  u64 timedelta_count;
  u8 timedelta_count_log2;
  u64 timedelta_counter;
  u64 timedelta_log2_pop_list_base[TIMEDELTA_COUNT_LOG2_MAX+1];
  DYSPOISSOMETER_NUMBER timedelta_mean;
  u64 timedelta_sum;

  freq_list0_base=NULL;
  freq_list1_base=NULL;
  do{
    status=1;
    if(argc!=4){
      printf("Timedelta Profile with Dyspoissometer\nCopyright 2016 Russell Leidich\nhttp://enranda.blogspot.com\n");
      printf("build_id_in_hex=%02X\n\n", TIMEDELTAPROFILE_BUILD_ID);
      printf("Perform realtime statistical analysis of the low 16 bits of the differences\nbetween successive timestamps. See README.txt for important warnings.\n\n");
      printf("Syntax:\n\n");
      printf("timedeltaprofile mode continuous timedelta_count_log2\n\n");
      printf("where:\n\n");
      printf("mode tells what to display:\n\n");
      printf("  0: Dyspoissonism of timedelta -- watch it drop under heavy load (more timing\n     entropy)!\n  1: Like 0, but expressed as a hex fraction where (2^63) is one\n  2: Mean of timedelta\n  3: Like 2, but rounded down and expressed in hex\n  4: Normalized histogram of log2(timedelta), for MSB=(0 through\n     timedelta_count_log2)\n  5: Like 4, but expressed as a hex fraction where (2^63) is one\n\n");
      printf("continuous is:\n\n");
      printf("  0: Quit after a single iteration\n  1: Same as 0, but separate multiple values with commas (CSV format)\n  2: Continue output with timedelta_count_log2 timedeltas between updates until\n     Ctrl+C is pressed\n  3: Same as 2, but CSV format\n\n");
      printf("timedelta_count_log2 is the log of the number of timedeltas to sample between\noutputs. Low values produce inaccurate results, not the least of which because\nthe act of running this program temporarily changes the timedelta histogram of\nyour system. 24 is a reasonable starting value; allowed values are on [16, 48].\n\n");
      printf("Notes:\n\n");
      printf("All of the above can only be used for comparison purposes provided that\ntimedelta_count_log2 is held constant. Floating-point is internally quad\nprecision regardless of display mode, but at most double precision is\ndisplayed.\n\n");
      break;
    }
    status=dyspoissometer_init(0, 0);
    if(status){
      printf("Dyspoissometer source code is outdated!\n");
      break;
    }
    status=1;
    digit_list_base=argv[1];
    mode=(u8)(digit_list_base[0]-'0');
    if((MODE_HISTOGRAM_HEX<mode)||(digit_list_base[1])){
      printf("Invalid mode!\n");
      break;
    }
    digit_list_base=argv[2];
    continuous_status=(u8)(digit_list_base[0]-'0');
    if((3<continuous_status)||(digit_list_base[1])){
      printf("Invalid continuous flag!\n");
      break;
    }
    csv_status=(u8)(continuous_status&1);
    continuous_status=(u8)(continuous_status>>1);
    digit_list_base=argv[3];
    digit0=(u8)(digit_list_base[0]);
    digit1=0;
    log2_valid_status=0;
    if(digit0){
      digit0=(u8)(digit0-'0');
      timedelta_count_log2=digit0;
      if(digit0<=9){
        digit1=(u8)(digit_list_base[1]);
        log2_valid_status=1;
        if(digit1){
          log2_valid_status=0;
          digit1=(u8)(digit1-'0');
          timedelta_count_log2=(u8)((timedelta_count_log2*10)+digit1);
          if(digit1<=9){
            digit2=(u8)(digit_list_base[2]);
            if(!digit2){
              log2_valid_status=1;
            }
          }
        }
      }
    }
    if(!log2_valid_status){
      printf("ERROR: Invalid timedelta_count_log2!\n");
      break;
    }
    if(timedelta_count_log2<U16_BITS){
      printf("ERROR: timedelta_count_log2 is too small to be useful!\n");
      break;
    }
    if(TIMEDELTA_COUNT_LOG2_MAX<timedelta_count_log2){
      printf("ERROR: timedelta_count_log2 is intractably large!\n");
      break;
    }
    timedelta_count=1;
    timedelta_count<<=timedelta_count_log2;
    freq_list0_base=dyspoissometer_uint_list_malloc(U16_MAX);
    if(!freq_list0_base){
      printf("ERROR: Out of memory!\n");
      break;
    }
    if((mode==MODE_DYSPOISSONISM)||(mode==MODE_DYSPOISSONISM_HEX)){
      freq_list1_base=dyspoissometer_uint_list_malloc(U16_MAX);
      if(!freq_list1_base){
        printf("ERROR: Out of memory!\n");
        break;
      }
    }
    status=0;
    do{
      dyspoissometer_uint_list_zero(U16_MAX, freq_list0_base);
      timedelta_counter=timedelta_count;
      time=0;
      do{
        timedelta=time;
        time=(u16)(timestamp_get());
        timedelta=(u16)(time-timedelta);
        freq_list0_base[timedelta]++;
        timedelta_counter--;
      }while(timedelta_counter);
      switch(mode){
      case MODE_DYSPOISSONISM:
      case MODE_DYSPOISSONISM_HEX:
        logfreedom=dyspoissometer_logfreedom_sparse_get(freq_list0_base, freq_list1_base, U16_MAX);
        mask_idx_max=timedelta_count-1;
        dyspoissonism=dyspoissometer_dyspoissonism_get(logfreedom, mask_idx_max, U16_MAX);
        if(mode==MODE_DYSPOISSONISM){
          timedeltaprofile_double_printf((double)(dyspoissonism));
        }else{
          timedeltaprofile_u64_printf((u64)(dyspoissonism*(U64_SPAN_HALF)));
        }
        break;
      case MODE_MEAN:
      case MODE_MEAN_HEX:
        timedelta_sum=0;
        timedelta=0;
        do{
/*
This won't overflow because (TIMEDELTA_COUNT_LOG2_MAX==48).
*/
          timedelta_sum+=timedelta*freq_list0_base[timedelta];
        }while((timedelta++)!=U16_MAX);
        timedelta_mean=timedelta_sum;
        timedelta_mean/=timedelta_count;
        if(mode==MODE_MEAN){
          timedeltaprofile_double_printf((double)(timedelta_mean));
        }else{
          timedeltaprofile_u64_printf((u64)(timedelta_mean));
        }
        break;
      case MODE_HISTOGRAM:
      case MODE_HISTOGRAM_HEX:
        msb=0;
        do{
          timedelta_log2_pop_list_base[msb]=0;
        }while((msb++)!=TIMEDELTA_COUNT_LOG2_MAX);
        timedelta=0;
        do{
          freq=freq_list0_base[timedelta];
          msb=0;
          while(1<(DYSPOISSOMETER_UINT)(freq>>msb)){
            msb++;
          }
          timedelta_log2_pop_list_base[msb]+=freq;
        }while((timedelta++)!=U16_MAX);
        msb=0;
        do{
          if(mode==MODE_HISTOGRAM){
            pop_normalized=timedelta_log2_pop_list_base[msb];
            pop_normalized/=timedelta_count;
            printf("%02d: ", msb);
            timedeltaprofile_double_printf((double)(pop_normalized));
          }else{
            pop=timedelta_log2_pop_list_base[msb];
            pop<<=U64_BIT_MAX-timedelta_count_log2;
            printf("%02d: ", msb);
            timedeltaprofile_u64_printf(pop);
          }
          if(csv_status&&(msb!=timedelta_count_log2)){
            printf(", ");
          }else{
            timedeltaprofile_newline_printf();
          }
        }while((msb++)!=timedelta_count_log2);
        if(!csv_status){
          timedeltaprofile_newline_printf();
        }
        break;
      }
      switch(mode){
      case MODE_DYSPOISSONISM:
      case MODE_DYSPOISSONISM_HEX:
      case MODE_MEAN:
      case MODE_MEAN_HEX:
        if(!csv_status){
          timedeltaprofile_newline_printf();
        }else{
          printf(", ");
        }
        break;
      }
      fflush(stdout);
    }while(continuous_status);
  }while(0);
  dyspoissometer_free(freq_list1_base);
  dyspoissometer_free(freq_list0_base);
  return status;
}
