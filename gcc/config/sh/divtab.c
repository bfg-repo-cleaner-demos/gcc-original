/* Copyright (C) 2003-2013 Free Software Foundation, Inc.

This file is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3, or (at your option) any
later version.

This file is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */


/* Calculate division table for SH5Media integer division
   Contributed by Joern Rennecke
   joern.rennecke@superh.com  */

#include <stdio.h>
#include <math.h>

#define BITS 5
#define N_ENTRIES (1 << BITS)
#define CUTOFF_BITS 20

#define BIAS (-330)

double max_defect = 0.;
double max_defect_x;

double min_defect = 1e9;
double min_defect_x;

double max_defect2 = 0.;
double max_defect2_x;

double min_defect2 = 0.;
double min_defect2_x;

double min_defect3 = 01e9;
double min_defect3_x;
int min_defect3_val;

double max_defect3 = 0.;
double max_defect3_x;
int max_defect3_val;

static double note_defect3 (int val, double d2, double y2d, double x)
{
  int cutoff_val = val >> CUTOFF_BITS;
  double cutoff;
  double defect;

  if (val < 0)
    cutoff_val++;
  cutoff = (cutoff_val * (1<<CUTOFF_BITS) - val) * y2d;
  defect = cutoff + val * d2;
  if (val < 0)
    defect = - defect;
  if (defect > max_defect3)
    {
      max_defect3 = defect;
      max_defect3_x = x;
      max_defect3_val = val;
    }
  if (defect < min_defect3)
    {
      min_defect3 = defect;
      min_defect3_x = x;
      min_defect3_val = val;
    }
}

/* This function assumes 32-bit integers.  */
static double
calc_defect (double x, int constant, int factor)
{
  double y0 = (constant - (int) floor ((x * factor * 64.))) / 16384.;
  double y1 = 2 * y0 -y0 * y0 * (x + BIAS / (1.*(1LL<<30)));
  double y2d0, y2d;
  int y2d1;
  double d, d2;

  y1 = floor (y1 * (1024 * 1024 * 1024)) / (1024 * 1024 * 1024);
  d = y1 - 1 / x;
  if (d > max_defect)
    {
      max_defect = d;
      max_defect_x = x;
    }
  if (d < min_defect)
    {
      min_defect = d;
      min_defect_x = x;
    }
  y2d0 = floor (y1 * x * (1LL << 60-16));
  y2d1 = (int) (long long) y2d0;
  y2d = - floor ((y1 - y0 / (1<<30-14)) * y2d1) / (1LL<<44);
  d2 = y1 + y2d - 1/x;
  if (d2 > max_defect2)
    {
      max_defect2 = d2;
      max_defect2_x = x;
    }
  if (d2 < min_defect2)
    {
      min_defect2 = d2;
      min_defect2_x = x;
    }
  /* zero times anything is trivially zero.  */
  note_defect3 ((1 << CUTOFF_BITS) - 1, d2, y2d, x);
  note_defect3 (1 << CUTOFF_BITS, d2, y2d, x);
  note_defect3 ((1U << 31) - (1 << CUTOFF_BITS), d2, y2d, x);
  note_defect3 ((1U << 31) - 1, d2, y2d, x);
  note_defect3 (-1, d2, y2d, x);
  note_defect3 (-(1 << CUTOFF_BITS), d2, y2d, x);
  note_defect3 ((1U << 31) - (1 << CUTOFF_BITS) + 1, d2, y2d, x);
  note_defect3 (-(1U << 31), d2, y2d, x);
  return d;
}

int
main ()
{
  int i;
  unsigned char factors[N_ENTRIES];
  short constants[N_ENTRIES];
  int steps = N_ENTRIES / 2;
  double step = 1. / steps;
  double eps30 = 1. / (1024 * 1024 * 1024);

  for (i = 0; i < N_ENTRIES; i++)
    {
      double x_low = (i < steps ? 1. : -3.) + i * step;
      double x_high = x_low + step - eps30;
      double x_med;
      int factor, constant;
      double low_defect, med_defect, high_defect, max_defect;

      factor = (1./x_low- 1./x_high) / step * 256. + 0.5;
      if (factor == 256)
	factor = 255;
      factors[i] = factor;
      /* Use minimum of error function for x_med.  */
      x_med = sqrt (256./factor);
      if (x_low < 0)
	x_med = - x_med;
      low_defect = 1. / x_low + x_low * factor / 256.;
      high_defect = 1. / x_high + x_high * factor / 256.;
      med_defect = 1. / x_med + x_med * factor / 256.;
      max_defect
	= ((low_defect > high_defect) ^ (x_med < 0)) ? low_defect : high_defect;
      constant = (med_defect + max_defect) * 0.5 * 16384. + 0.5;
      if (constant < -32768 || constant > 32767)
	abort ();
      constants[i] = constant;
      calc_defect (x_low, constant, factor);
      calc_defect (x_med, constant, factor);
      calc_defect (x_high, constant, factor);
    }
    printf ("/* This table has been generated by divtab.c .\n");
    printf ("Defects for bias %d:\n", BIAS);
    printf ("   Max defect: %e at %e\n", max_defect, max_defect_x);
    printf ("   Min defect: %e at %e\n", min_defect, min_defect_x);
    printf ("   Max 2nd step defect: %e at %e\n", max_defect2, max_defect2_x);
    printf ("   Min 2nd step defect: %e at %e\n", min_defect2, min_defect2_x);
    printf ("   Max div defect: %e at %d:%e\n", max_defect3, max_defect3_val, max_defect3_x);
    printf ("   Min div defect: %e at %d:%e\n", min_defect3, min_defect3_val, min_defect3_x);
    printf ("   Defect at 1: %e\n",
	    calc_defect (1., constants[0], factors[0]));
    printf ("   Defect at -2: %e */\n",
	    calc_defect (-2., constants[steps], factors[steps]));
    printf ("\t.section\t.rodata\n");
    printf ("\t.balign 2\n");
    printf ("/* negative division constants */\n");
    for (i = steps; i < 2 * steps; i++)
      printf ("\t.word\t%d\n", constants[i]);
    printf ("/* negative division factors */\n");
    for (i = steps; i < 2*steps; i++)
      printf ("\t.byte\t%d\n", factors[i]);
    printf ("\t.skip %d\n", steps);
    printf ("\t.global	GLOBAL(div_table):\n");
    printf ("GLOBAL(div_table):\n");
    printf ("\t.skip %d\n", steps);
    printf ("/* positive division factors */\n");
    for (i = 0; i < steps; i++)
      printf ("\t.byte\t%d\n", factors[i]);
    printf ("/* positive division constants */\n");
    for (i = 0; i < steps; i++)
      printf ("\t.word\t%d\n", constants[i]);
  exit (0);
}
