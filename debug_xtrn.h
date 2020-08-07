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
#ifdef DEBUG
  extern void debug_allocation_check(void);
  extern void debug_bitmap(char *name_base, u64 bit_count, u64 bit_idx_min, ULONG *chunk_list_base);
  extern void *debug_calloc_paranoid(ULONG size);
  extern void debug_double(char *name_base, double value);
  extern void debug_double_list(char *name_base, ULONG double_count, double *double_list_base);
  extern void debug_exit(int status);
  extern void debug_float(char *name_base, float value);
  extern void debug_float_list(char *name_base, ULONG float_count, float *float_list_base);
  extern void debug_free_paranoid(void *base);
  extern void debug_line(ULONG line_idx);
  extern void debug_list(char *name_base, ULONG chunk_count, u8 *chunk_list_base, u8 chunk_size_log2);
  extern void debug_list_custom(char *name_base, ULONG chunk_count, u8 *chunk_list_base, u8 chunk_size_log2, u8 compilable_status);
  extern void *debug_malloc_paranoid(ULONG size);
  extern void debug_memory_leak_report(void *base);
  extern void debug_print(char *string_base);
  extern void debug_print_flush(void);
  extern void debug_print_if(u8 status, char *string_base);
  extern void debug_ptr(char *name_base, void *base);
  extern void *debug_realloc_paranoid(void *base, ULONG size);
  extern void debug_u16(char *name_base, u16 value);
  extern void debug_u24(char *name_base, u32 value);
  extern void debug_u32(char *name_base, u32 value);
  extern void debug_u64(char *name_base, u64 value);
  extern void debug_u8(char *name_base, u8 value);
#endif
