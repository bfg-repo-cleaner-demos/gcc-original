/* { dg-do run } */
/* { dg-options "-O2 -ftree-vectorize -fdump-tree-vect-all -fno-unroll-loops --save-temps -fno-inline" } */

#define FTYPE float
#define OP >=
#define INV_OP <

#include "vect-fcm.x"

/* { dg-final { scan-tree-dump-times "vectorized 1 loops" 3 "vect" } } */
/* { dg-final { scan-assembler-times "fcmge\\tv\[0-9\]+\.\[24\]s, v\[0-9\]+\.\[24\]s, v\[0-9\]+\.\[24\]s" 2 } } */
/* { dg-final { scan-assembler-times "fcmge\\tv\[0-9\]+\.\[24\]s, v\[0-9\]+\.\[24\]s, 0" 1 } } */
/* { dg-final { cleanup-tree-dump "vect" } } */
/* { dg-final { cleanup-saved-temps } } */
