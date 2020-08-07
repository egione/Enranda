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
  #include "flag.h"
  #include <quadmath.h>
  #include <stdint.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include "constant.h"
  #include "debug_xtrn.h"
  #include "debug_quad_xtrn.h"

  void
  debug_quad(char *name_base, __float128 value){
/*
Print a quad-precision floating-point value.

In:

  *name_base is a text identifier for the output.

  value is the __float128 to print.
*/
    char ascii_quad_base[48];

    printf("%s=", name_base);
    quadmath_snprintf(ascii_quad_base, 43, "%+-1.34QE", value);
    printf("%s\n", ascii_quad_base);
    debug_print_flush();
    return;
  }

  void
  debug_quad_list(char *name_base, ULONG quad_count, __float128 *quad_list_base){
/*
Print a list of quad-precision floating-point values.

In:

  *name_base is a text identifier for the output.

  quad_count is the number of (__float128)s to print.

  *quad_list_base is a list of (__float128)s.
*/
    char ascii_quad_base[48];
    ULONG quad_idx;

    #ifdef _64_
      printf("%s[%08X%08X]={\n", name_base, (u32)(quad_count>>U32_BITS), (u32)(quad_count));
    #else
      printf("%s[%08X%08X]={\n", name_base, 0, quad_count);
    #endif
    for(quad_idx=0; quad_idx<quad_count; quad_idx++){
      quadmath_snprintf(ascii_quad_base, 43, "%+-1.34QE", quad_list_base[quad_idx]);
      printf("  %s\n", ascii_quad_base);
    }
    printf("}\n");
    debug_print_flush();
    return;
  }
#endif
