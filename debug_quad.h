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
  #define DEBUG_QUAD(name_base, value) debug_quad(name_base, value)
  #define DEBUG_QUAD_LIST(name_base, quad_count, quad_list_base) debug_quad_list(name_base, quad_count, quad_list_base)
#else
  #define DEBUG_QUAD(name_base, value)
  #define DEBUG_QUAD_LIST(name_base, quad_count, quad_list_base)
#endif
