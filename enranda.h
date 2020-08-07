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
Enranda Local Data Structure
*/
#define ENRANDA_ENTROPY_SIZE (1U<<ENRANDA_ENTROPY_SIZE_LOG2)
#define ENRANDA_ENTROPY_SIZE_LOG2 16U
#define ENRANDA_ENTROPY_U16_COUNT (1U<<(ENRANDA_ENTROPY_SIZE_LOG2-U16_SIZE_LOG2))
#define ENRANDA_ENTROPY_U32_COUNT (1U<<(ENRANDA_ENTROPY_SIZE_LOG2-U32_SIZE_LOG2))
#define ENRANDA_ENTROPY_U64_COUNT (1U<<(ENRANDA_ENTROPY_SIZE_LOG2-U64_SIZE_LOG2))
#define ENRANDA_ENTROPY_U16_IDX_MAX (ENRANDA_ENTROPY_U16_COUNT-1U)
#define ENRANDA_ENTROPY_U32_IDX_MAX (ENRANDA_ENTROPY_U32_COUNT-1U)
#define ENRANDA_ENTROPY_U64_IDX_MAX (ENRANDA_ENTROPY_U64_COUNT-1U)
#define ENRANDA_ENTROPY_U8_IDX_MAX (ENRANDA_ENTROPY_SIZE-1U)
#define ENRANDA_PHASE_ACCRUE 0U
#define ENRANDA_PHASE_TRAPDOOR 1U

TYPEDEF_START
  u16 sequence_hash_count_list[U16_SPAN];
  u16 sequence_hash_list[U16_SPAN];
  u16 unique_list[U16_SPAN];
  u16 history_hash;
  u16 sequence_hash;
  u16 sequence_hash_idx;
  u16 time;
  u16 unique_idx;
  u8 phase;
TYPEDEF_END(enranda_t)
