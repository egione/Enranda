/*
Enranda
Copyright 2015 Tigerspike Ltd
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
Timestamp Interface for X86 and X64 Architectures
*/
TYPEDEF_START
   uint64_t timestamp;
   uint64_t temp0;
   uint64_t temp1;
   uint64_t temp2;
TYPEDEF_END(timestamp_cpu_t)

#ifdef _32_
  extern void timestamp_cpu_get(timestamp_cpu_t *timestamp_cpu_base) __attribute__((fastcall));
  extern void timestamp_x4_cpu_get(timestamp_cpu_t *timestamp_cpu_base) __attribute__((fastcall));
#else
  extern void timestamp_cpu_get(timestamp_cpu_t *timestamp_cpu_base);
  extern void timestamp_x4_cpu_get(timestamp_cpu_t *timestamp_cpu_base);
#endif
