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
Timedelta Stream Saver

NOTE: Compile with little or no optimization (-O0) in order to minimize autopseudorandomness when reading the timestamp in a tight loop.
*/
#include "flag.h"
#include "flag_timestamp.h"
#include "flag_timedeltasave.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constant.h"
#include "timestamp_xtrn.h"

int
main(int argc, char *argv[]){
  char *decimal_base;
  u8 digit0;
  u8 digit1;
  char *filename_base;
  FILE *handle;
  u8 mode;
  u8 status;
  ULONG timedelta_count;
  u8 timedelta_count_log2;
  ULONG timedelta_list_size;
  u8 timedelta_sample_size;
  u8 timedelta_sample_size_x2;
  u8 *timedelta_u8_list_base;
  u64 timedelta;
  u64 timestamp;
  ULONG timestamp_count;
  ULONG timestamp_list_size;
  u64 transfer_size_actual;
  ULONG u8_idx;

  status=1;
  timedelta_u8_list_base=NULL;
  do{
    if(argc!=4){
      printf("Timedelta Stream Saver\nCopyright 2016 Russell Leidich\nhttp://enranda.blogspot.com\n");
      printf("build_id_in_hex=%02X\n\n", TIMEDELTASAVE_BUILD_ID);
      printf("Save a power-of-2 block of N-byte successive timedeltas.\n\n");
      printf("Syntax:\n\ntimedeltasave mode timedelta_count_log2 filename_to_overwrite\n\n");
      printf("where:\n\n");
      printf("mode is the number of bytes per timedelta, less one, on [0, 7]. Each timedelta\nis the low (mode+1) byte(s) of the differnce between 2 successive timestamp\nreads, but the first read is discarded due to plausible branch misprediction\nin the read loop. Thus the first timedelta is issued following 3 timestamp\nreads, the next after a total of 4, etc.\n\n");
      printf("timedelta_count_log2 is the log2 of the number of timedeltas to write to the\nindicated file. You can then perform a statistical analysis with tools such as\ndyspoissofile from the Dyspoissometer toolkit, or search for timedeltas in\nunusual ranges which may indicate the presence of malware. In any event no file\nIO shall take place until all timedeltas have been sampled, so as to avoid\nmistaking my own IO effects for timedelta anomalies.\n\n");
      printf("filename_to_overwrite is the output filename.\n\n");
      break;
    }
    decimal_base=argv[1];
    mode=(u8)(decimal_base[0]-'0');
    if((U64_BYTE_MAX<mode)||decimal_base[1]){
      printf("ERROR: Invalid mode!\n");
      break;
    }
    timedelta_sample_size=(u8)(mode+1);
    timedelta_sample_size_x2=(u8)(timedelta_sample_size<<1);
    decimal_base=argv[2];
    digit0=(u8)(decimal_base[0]-'0');
    timedelta_count_log2=U8_MAX;
    if(digit0<=9){
      digit1=(u8)(decimal_base[1]);
      if(!digit1){
        timedelta_count_log2=digit0;
      }else{
        digit1=(u8)(digit1-'0');
        if((digit1<=9)&&(!decimal_base[2])){
          timedelta_count_log2=(u8)((digit0*10)+digit1);
        }
      }
    }
    timedelta_count=1;
    timedelta_count<<=timedelta_count_log2;
    timestamp_count=timedelta_count+2;
    timestamp_list_size=timestamp_count*timedelta_sample_size;    
    if((!timedelta_count)||((timestamp_list_size/timedelta_sample_size)!=timestamp_count)){
      printf("ERROR: Invalid timedelta_count_log2!\n");
      break;
    }
    timedelta_list_size=timestamp_list_size-timedelta_sample_size_x2;
    timedelta_u8_list_base=(u8 *)(malloc((size_t)(timestamp_list_size)));
    if(!timedelta_u8_list_base){
      printf("ERROR: Out of memory!\n");
      break;
    }
    handle=NULL;
    filename_base=argv[3];
    handle=fopen(filename_base, "wb");
    if(!handle){
      printf("ERROR: Cannot open that file for writing!\n");
      break;
    }
    printf("Reading timestamps into memory...\n");
    fflush(stdout);
    u8_idx=0;
    do{
      timestamp=timestamp_get();
      memcpy(&timedelta_u8_list_base[u8_idx], &timestamp, (size_t)(timedelta_sample_size));
      u8_idx+=timedelta_sample_size;
    }while(u8_idx!=timestamp_list_size);
    printf("Converting timestamps to timedeltas...\n");
    fflush(stdout);
    timedelta=0;
    memcpy(&timedelta, &timedelta_u8_list_base[timedelta_sample_size], (size_t)(timedelta_sample_size));
    u8_idx=timedelta_sample_size_x2;
    timestamp=0;
    do{
      memcpy(&timestamp, &timedelta_u8_list_base[u8_idx], (size_t)(timedelta_sample_size));
      timedelta=timestamp-timedelta;
      memcpy(&timedelta_u8_list_base[u8_idx-timedelta_sample_size_x2], &timedelta, (size_t)(timedelta_sample_size));
      timedelta=timestamp;
      u8_idx+=timedelta_sample_size;
    }while(u8_idx!=timestamp_list_size);
    status=0;
    printf("Saving to %s...\n", filename_base);
    fflush(stdout);
    transfer_size_actual=(u64)(fwrite(timedelta_u8_list_base, (size_t)(U8_SIZE), (size_t)(timedelta_list_size), handle));
    if(timedelta_list_size!=transfer_size_actual){
      status=1;
      printf("ERROR: File write failed!\n");
      fclose(handle);
      break;
    }
    status=!!fclose(handle);
    if(status){
      printf("ERROR: File closure failed!\n");
      break;
    }else{
      printf("\n");
    }
  }while(0);
  if(timedelta_u8_list_base){
    free(timedelta_u8_list_base);
  }
  return status;
}
