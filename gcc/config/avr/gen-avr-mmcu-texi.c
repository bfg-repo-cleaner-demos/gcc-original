/* Copyright (C) 2012-2013 Free Software Foundation, Inc.
   Contributed by Georg-Johann Lay (avr@gjlay.de)

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.
   
   GCC is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

#include <stdlib.h>
#include <stdio.h>

#define IN_GEN_AVR_MMCU_TEXI

#include "avr-arch.h"
#include "avr-devices.c"

static const char*
mcu_name[sizeof avr_mcu_types / sizeof avr_mcu_types[0]];

static int letter (char c)
{
  return c >= 'a' && c <= 'z';
}

static int digit (char c)
{
  return c >= '0' && c <= '9';
}

static int
comparator (const void *va, const void *vb)
{
  const char *a = *(const char* const*) va;
  const char *b = *(const char* const*) vb;

  while (*a && *b)
    {
      /* Make letters smaller than digits so that `atmega16a' follows
         `atmega16' without `atmega161' etc. between them.  */
      
      if (letter (*a) && digit (*b))
        return -1;

      if (digit (*a) && letter (*b))
        return 1;

      if (*a != *b)
        return *a - *b;
      
      a++;
      b++;
    }

  return *a - *b;
} 

static void
print_mcus (size_t n_mcus)
{
  size_t i;
    
  if (!n_mcus)
    return;
    
  qsort (mcu_name, n_mcus, sizeof (char*), comparator);

  printf ("@*@var{mcu}@tie{}=");

  for (i = 0; i < n_mcus; i++)
    printf (" @code{%s}%s", mcu_name[i], i == n_mcus-1 ? ".\n\n" : ",");
}

int main (void)
{
  enum avr_arch arch = ARCH_UNKNOWN;
  size_t i, n_mcus = 0;
  const struct mcu_type_s *mcu;

  printf ("@c Copyright (C) 2012-2013 Free Software Foundation, Inc.\n");
  printf ("@c This is part of the GCC manual.\n");
  printf ("@c For copying conditions, see the file "
          "gcc/doc/include/fdl.texi.\n\n");

  printf ("@c This file is generated automatically using\n");
  printf ("@c gcc/config/avr/gen-avr-mmcu-texi.c from:\n");
  printf ("@c    gcc/config/avr/avr-arch.h\n");
  printf ("@c    gcc/config/avr/avr-devices.c\n");
  printf ("@c    gcc/config/avr/avr-mcus.def\n\n");

  printf ("@c Please do not edit manually.\n\n");

  printf ("@table @code\n\n");

  for (mcu = avr_mcu_types; mcu->name; mcu++)
    {
      if (mcu->macro == NULL)
        {
          arch = mcu->arch;

          /* Start a new architecture:  Flush the MCUs collected so far.  */

          print_mcus (n_mcus);
          n_mcus = 0;

          for (i = 0; i < sizeof (avr_texinfo) / sizeof (*avr_texinfo); i++)
            if (arch == avr_texinfo[i].arch)
              printf ("@item %s\n%s\n", mcu->name, avr_texinfo[i].texinfo);
        }
      else if (arch == (enum avr_arch) mcu->arch)
        {
          mcu_name[n_mcus++] = mcu->name;
        }
    }

  print_mcus (n_mcus);
  printf ("@end table\n");

  return EXIT_SUCCESS;
}
