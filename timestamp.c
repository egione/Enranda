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
CPU-Architecture-Agnostic Timestamp Reading Interface
*/
#include "flag.h"
#include <stdint.h>
#include "constant.h"
#include "debug.h"
#include "debug_xtrn.h"
#if defined(ABSTIME)
  #include <mach/mach_time.h>
#elif defined(TIMESTAMP_X86_X64)
  #include "timestamp_x86_x64.h"
/*
#elif defined(YOUR_CPU_ARCHITECTURE)
  #include "timestamp_your_cpu_architecture.h"
*/
#else
  #error "No timestamp source defined!"
#endif

u64
timestamp_get(void){
/*
Return the timestamp on the current core. Architectures providing the ABSTIME interface essentially prevent us from doing this in assembly language because they require an OS call, but on the other hand tend to be very rich in entropy due to being mobile devices for the most part, as opposed to servers which might find themselves in more quiescent execution environments most of the time.

Out:

  Returns the CPU timestamp.
*/
/*
timestamp_cpu_t is a CPU-architecture-specific data structure defined in timestamp_your_cpu_architecture.h containing (1) temporary storage for the target assembly language function and (2) the timestamp itself, which could be any size of at least 32 bits. Architectures using ABSTIME are already handled below and do not require this data structure.
*/
  #ifndef ABSTIME
    timestamp_cpu_t timestamp_cpu;
  #endif
  u64 timestamp;

  #ifndef ABSTIME
/*
timestamp_cpu_get() fills in the fields of timestamp_cpu for your particular CPU architecture.
*/
    timestamp_cpu_get(&timestamp_cpu);
    timestamp=(u64)(timestamp_cpu.timestamp);
  #else
/*
Ideally you need to rewrite this in assembly language (similarly to timestamp_x86_x64.asm) but it might not pass muster with the compiler or the OS.
*/
    timestamp=(u64)(mach_absolute_time());
  #endif
  return timestamp;
}

u64
timestamp_x4_get(void){
/*
Return a list of 4 successive u16 timestamps on the current core. (Empirically, 4 is roughly optimal for maximizing the rate of entropy accrual, provided that it's done in assembly language; the OS call version is just for compatilibilty but offers negligible performance advantage.) Otherwise this function behaves much like timestamp_get().

Out:

  Returns a u64 containing 4 successive timestamps, with the first (but not necessarily the least) in the low u16.
*/
  #ifndef ABSTIME
    u64 timestamp_x4;

    timestamp_cpu_t timestamp_cpu;

    timestamp_x4_cpu_get(&timestamp_cpu);
    timestamp_x4=timestamp_cpu.timestamp;
  #else
    u8 timestamp_count;
    u64 timestamp_x4;

    timestamp_count=4;
    timestamp_x4=0;
    do{
      timestamp_x4>>U16_BITS;
      timestamp_x4|=(u64)(mach_absolute_time())<<48;
    }while(--timestamp_count);
  #endif
  return timestamp_x4;
}
