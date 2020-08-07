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
extern u8 enranda_entropy_accrue(enranda_t *enranda_base, u8 fill_status);
extern void enranda_entropy_u16_list_get(enranda_t *enranda_base, ULONG entropy_idx_min, ULONG entropy_u16_count_minus_1, u16 *entropy_u16_list_base);
extern void enranda_entropy_u32_list_get(enranda_t *enranda_base, ULONG entropy_idx_min, ULONG entropy_u32_count_minus_1, u32 *entropy_u32_list_base);
extern void enranda_entropy_u64_list_get(enranda_t *enranda_base, ULONG entropy_idx_min, ULONG entropy_u64_count_minus_1, u64 *entropy_u64_list_base);
extern void enranda_entropy_u8_list_get(enranda_t *enranda_base, ULONG entropy_idx_min, ULONG entropy_u8_count_minus_1, u8 *entropy_u8_list_base);
extern void *enranda_free(void *base);
extern void enranda_rewind(enranda_t *enranda_base);
extern enranda_t *enranda_init(u32 build_break_count, u32 build_feature_count);
