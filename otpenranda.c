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
One-Time-Pad Maker with Enranda
*/
#include "flag.h"
#include "flag_timestamp.h"
#include "flag_enranda.h"
#include "flag_otpenranda.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constant.h"
#include "enranda.h"
#include "enranda_xtrn.h"

int
main(int argc, char *argv[]){
  u8 digit0;
  u8 digit1;
  u8 digit2;
  enranda_t *enranda_base;
  char *filename_base;
  FILE *handle;
  u8 log2_valid_status;
  u64 random;
  u64 *random_u64_list_base;
  u8 *random_u8_list_base;
  u64 random_size;
  u8 size_log2;
  char *size_log2_base;
  u8 status;
  u32 transfer_idx;
  u32 transfer_idx_max;
  u32 transfer_size;
  u64 transfer_size_actual;

  status=1;
  enranda_base=enranda_init(0, 0);
  random_u64_list_base=(u64 *)(malloc((size_t)(ENRANDA_ENTROPY_SIZE)));
  random_u8_list_base=(u8 *)(malloc((size_t)(ENRANDA_ENTROPY_SIZE)));
  do{
    if(!(enranda_base&&random_u64_list_base&&random_u8_list_base)){
      printf("ERROR: Initialization failed!\n");
      break;
    }
    if((argc!=2)&&(argc!=3)){
      printf("One-Time-Pad Maker with Enranda\nCopyright 2016 Russell Leidich\nhttp://enranda.blogspot.com\n");
      printf("build_id_in_hex=%02X\n\n", OTPENRANDA_BUILD_ID);
      printf("Create a power-of-2 block of true random bytes from Enranda.\n\n");
      printf("Syntax:\n\notpenranda file_size_log2 [filename_to_overwrite]\n\n");
      printf("Omit filename_to_overwrite in order to print out the requested number of random\nbytes as hexadecimal.\n\n");
      break;
    }
    size_log2_base=argv[1];
    digit0=(u8)(size_log2_base[0]);
    digit1=0;
    log2_valid_status=0;
    if(digit0){
      digit0=(u8)(digit0-'0');
      size_log2=digit0;
      if(digit0<=9){
        digit1=(u8)(size_log2_base[1]);
        log2_valid_status=1;
        if(digit1){
          log2_valid_status=0;
          digit1=(u8)(digit1-'0');
          size_log2=(u8)((size_log2*10)+digit1);
          if(digit1<=9){
            digit2=(u8)(size_log2_base[2]);
            if(!digit2){
              log2_valid_status=1;
            }
          }
        }
      }
    }
    if((!log2_valid_status)||(U64_BIT_MAX<=size_log2)){
      printf("ERROR: Invalid file_size_log2!\n");
      break;
    }
    handle=NULL;
    if(argc==3){
      filename_base=argv[2];
      handle=fopen(filename_base, "wb");
      if(!handle){
        printf("ERROR: Cannot open that file for writing!\n");
        break;
      }
    }
    status=0;
    random_size=1;
    random_size<<=size_log2;
    do{
      transfer_size=ENRANDA_ENTROPY_SIZE;
      if(random_size<transfer_size){
        transfer_size=(u32)(random_size);
      }
      if(U64_SIZE_LOG2<=size_log2){
        transfer_idx_max=(transfer_size>>U64_SIZE_LOG2)-1;
        enranda_entropy_u64_list_get(enranda_base, 0, transfer_idx_max, random_u64_list_base);
      }else{
        transfer_idx_max=transfer_size-1;
        enranda_entropy_u8_list_get(enranda_base, 0, transfer_idx_max, random_u8_list_base);
      }
      if(argc==3){
        if(U64_SIZE_LOG2<=size_log2){
          transfer_size_actual=(u64)(fwrite(random_u64_list_base, (size_t)(U8_SIZE), (size_t)(transfer_size), handle));
        }else{
          transfer_size_actual=(u64)(fwrite(random_u8_list_base, (size_t)(U8_SIZE), (size_t)(transfer_size), handle));
        }
        if(transfer_size!=transfer_size_actual){
          status=1;
          printf("ERROR: File write failed!\n");
          break;
        }
      }else{
        transfer_idx=0;
        if(U64_SIZE_LOG2<=size_log2){
          transfer_idx_max=(transfer_size>>U64_SIZE_LOG2)-1;
          do{
            random=random_u64_list_base[transfer_idx];
            printf("%08X%08X", (u32)(random>>U32_BITS), (u32)(random));
            if((transfer_idx&3)==3){
              printf("\n");
            }
          }while((transfer_idx++)!=transfer_idx_max);
        }else{
          transfer_idx_max=transfer_size-1;
          do{
            printf("%02X", random_u8_list_base[transfer_idx]);
            if((transfer_idx&0x1F)==0x1F){
              printf("\n");
            }
          }while((transfer_idx++)!=transfer_idx_max);
        }
      }
      random_size-=transfer_size;
    }while(random_size);
    if(argc==3){
      if(status){
        fclose(handle);
        break;
      }
      status=!!fclose(handle);
      if(status){
        status=1;
        printf("ERROR: File closure failed!\n");
        break;
      }
    }else{
      printf("\n");
    }
  }while(0);
  free(random_u8_list_base);
  free(random_u64_list_base);
  enranda_free(enranda_base);
  return status;
}
