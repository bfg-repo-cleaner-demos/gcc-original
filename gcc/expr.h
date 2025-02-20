/* Definitions for code generation pass of GNU compiler.
   Copyright (C) 1987-2013 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#ifndef GCC_EXPR_H
#define GCC_EXPR_H

/* For inhibit_defer_pop */
#include "function.h"
/* For XEXP, GEN_INT, rtx_code */
#include "rtl.h"
/* For optimize_size */
#include "flags.h"
/* For host_integerp, tree_low_cst, fold_convert, size_binop, ssize_int,
   TREE_CODE, TYPE_SIZE, int_size_in_bytes,    */
#include "tree.h"
/* For GET_MODE_BITSIZE, word_mode */
#include "machmode.h"

/* This is the 4th arg to `expand_expr'.
   EXPAND_STACK_PARM means we are possibly expanding a call param onto
   the stack.
   EXPAND_SUM means it is ok to return a PLUS rtx or MULT rtx.
   EXPAND_INITIALIZER is similar but also record any labels on forced_labels.
   EXPAND_CONST_ADDRESS means it is ok to return a MEM whose address
    is a constant that is not a legitimate address.
   EXPAND_WRITE means we are only going to write to the resulting rtx.
   EXPAND_MEMORY means we are interested in a memory result, even if
    the memory is constant and we could have propagated a constant value.  */
enum expand_modifier {EXPAND_NORMAL = 0, EXPAND_STACK_PARM, EXPAND_SUM,
		      EXPAND_CONST_ADDRESS, EXPAND_INITIALIZER, EXPAND_WRITE,
		      EXPAND_MEMORY};

/* Prevent the compiler from deferring stack pops.  See
   inhibit_defer_pop for more information.  */
#define NO_DEFER_POP (inhibit_defer_pop += 1)

/* Allow the compiler to defer stack pops.  See inhibit_defer_pop for
   more information.  */
#define OK_DEFER_POP (inhibit_defer_pop -= 1)

enum direction {none, upward, downward};

/* Structure to record the size of a sequence of arguments
   as the sum of a tree-expression and a constant.  This structure is
   also used to store offsets from the stack, which might be negative,
   so the variable part must be ssizetype, not sizetype.  */

struct args_size
{
  HOST_WIDE_INT constant;
  tree var;
};

/* Package up various arg related fields of struct args for
   locate_and_pad_parm.  */
struct locate_and_pad_arg_data
{
  /* Size of this argument on the stack, rounded up for any padding it
     gets.  If REG_PARM_STACK_SPACE is defined, then register parms are
     counted here, otherwise they aren't.  */
  struct args_size size;
  /* Offset of this argument from beginning of stack-args.  */
  struct args_size offset;
  /* Offset to the start of the stack slot.  Different from OFFSET
     if this arg pads downward.  */
  struct args_size slot_offset;
  /* The amount that the stack pointer needs to be adjusted to
     force alignment for the next argument.  */
  struct args_size alignment_pad;
  /* Which way we should pad this arg.  */
  enum direction where_pad;
  /* slot_offset is at least this aligned.  */
  unsigned int boundary;
};

/* Add the value of the tree INC to the `struct args_size' TO.  */

#define ADD_PARM_SIZE(TO, INC)					\
do {								\
  tree inc = (INC);						\
  if (host_integerp (inc, 0))					\
    (TO).constant += tree_low_cst (inc, 0);			\
  else if ((TO).var == 0)					\
    (TO).var = fold_convert (ssizetype, inc);			\
  else								\
    (TO).var = size_binop (PLUS_EXPR, (TO).var,			\
			   fold_convert (ssizetype, inc));	\
} while (0)

#define SUB_PARM_SIZE(TO, DEC)					\
do {								\
  tree dec = (DEC);						\
  if (host_integerp (dec, 0))					\
    (TO).constant -= tree_low_cst (dec, 0);			\
  else if ((TO).var == 0)					\
    (TO).var = size_binop (MINUS_EXPR, ssize_int (0),		\
			   fold_convert (ssizetype, dec));	\
  else								\
    (TO).var = size_binop (MINUS_EXPR, (TO).var,		\
			   fold_convert (ssizetype, dec));	\
} while (0)

/* Convert the implicit sum in a `struct args_size' into a tree
   of type ssizetype.  */
#define ARGS_SIZE_TREE(SIZE)					\
((SIZE).var == 0 ? ssize_int ((SIZE).constant)			\
 : size_binop (PLUS_EXPR, fold_convert (ssizetype, (SIZE).var),	\
	       ssize_int ((SIZE).constant)))

/* Convert the implicit sum in a `struct args_size' into an rtx.  */
#define ARGS_SIZE_RTX(SIZE)					\
((SIZE).var == 0 ? GEN_INT ((SIZE).constant)			\
 : expand_normal (ARGS_SIZE_TREE (SIZE)))


/* This structure is used to pass around information about exploded
   unary, binary and trinary expressions between expand_expr_real_1 and
   friends.  */
typedef struct separate_ops
{
  enum tree_code code;
  location_t location;
  tree type;
  tree op0, op1, op2;
} *sepops;

/* Functions from optabs.c, commonly used, and without need for the optabs
   tables:  */

/* Passed to expand_simple_binop and expand_binop to say which options
   to try to use if the requested operation can't be open-coded on the
   requisite mode.  Either OPTAB_LIB or OPTAB_LIB_WIDEN says try using
   a library call.  Either OPTAB_WIDEN or OPTAB_LIB_WIDEN says try
   using a wider mode.  OPTAB_MUST_WIDEN says try widening and don't
   try anything else.  */

enum optab_methods
{
  OPTAB_DIRECT,
  OPTAB_LIB,
  OPTAB_WIDEN,
  OPTAB_LIB_WIDEN,
  OPTAB_MUST_WIDEN
};

/* Generate code for a simple binary or unary operation.  "Simple" in
   this case means "can be unambiguously described by a (mode, code)
   pair and mapped to a single optab."  */
extern rtx expand_simple_binop (enum machine_mode, enum rtx_code, rtx,
				rtx, rtx, int, enum optab_methods);
extern rtx expand_simple_unop (enum machine_mode, enum rtx_code, rtx, rtx,
			       int);

/* Report whether the machine description contains an insn which can
   perform the operation described by CODE and MODE.  */
extern int have_insn_for (enum rtx_code, enum machine_mode);

/* Emit code to make a call to a constant function or a library call.  */
extern void emit_libcall_block (rtx, rtx, rtx, rtx);

/* Create but don't emit one rtl instruction to perform certain operations.
   Modes must match; operands must meet the operation's predicates.
   Likewise for subtraction and for just copying.  */
extern rtx gen_add2_insn (rtx, rtx);
extern rtx gen_add3_insn (rtx, rtx, rtx);
extern rtx gen_sub2_insn (rtx, rtx);
extern rtx gen_sub3_insn (rtx, rtx, rtx);
extern rtx gen_move_insn (rtx, rtx);
extern int have_add2_insn (rtx, rtx);
extern int have_sub2_insn (rtx, rtx);

/* Emit a pair of rtl insns to compare two rtx's and to jump
   to a label if the comparison is true.  */
extern void emit_cmp_and_jump_insns (rtx, rtx, enum rtx_code, rtx,
				     enum machine_mode, int, rtx, int prob=-1);

/* Generate code to indirectly jump to a location given in the rtx LOC.  */
extern void emit_indirect_jump (rtx);

/* Generate a conditional trap instruction.  */
extern rtx gen_cond_trap (enum rtx_code, rtx, rtx, rtx);

#include "insn-config.h"

#ifdef HAVE_conditional_move
/* Emit a conditional move operation.  */
rtx emit_conditional_move (rtx, enum rtx_code, rtx, rtx, enum machine_mode,
			   rtx, rtx, enum machine_mode, int);

/* Return nonzero if the conditional move is supported.  */
int can_conditionally_move_p (enum machine_mode mode);

#endif
rtx emit_conditional_add (rtx, enum rtx_code, rtx, rtx, enum machine_mode,
			  rtx, rtx, enum machine_mode, int);

rtx expand_sync_operation (rtx, rtx, enum rtx_code);
rtx expand_sync_fetch_operation (rtx, rtx, enum rtx_code, bool, rtx);
rtx expand_sync_lock_test_and_set (rtx, rtx, rtx);

rtx expand_atomic_exchange (rtx, rtx, rtx, enum memmodel);
rtx expand_atomic_load (rtx, rtx, enum memmodel);
rtx expand_atomic_store (rtx, rtx, enum memmodel, bool);
rtx expand_atomic_fetch_op (rtx, rtx, rtx, enum rtx_code, enum memmodel, 
			      bool);
rtx expand_atomic_test_and_set (rtx, rtx, enum memmodel);
rtx expand_atomic_clear (rtx, enum memmodel);
void expand_atomic_thread_fence (enum memmodel);
void expand_atomic_signal_fence (enum memmodel);


/* Functions from expmed.c:  */

/* Arguments MODE, RTX: return an rtx for the negation of that value.
   May emit insns.  */
extern rtx negate_rtx (enum machine_mode, rtx);

/* Expand a logical AND operation.  */
extern rtx expand_and (enum machine_mode, rtx, rtx, rtx);

/* Emit a store-flag operation.  */
extern rtx emit_store_flag (rtx, enum rtx_code, rtx, rtx, enum machine_mode,
			    int, int);

/* Like emit_store_flag, but always succeeds.  */
extern rtx emit_store_flag_force (rtx, enum rtx_code, rtx, rtx,
				  enum machine_mode, int, int);

/* Choose a minimal N + 1 bit approximation to 1/D that can be used to
   replace division by D, and put the least significant N bits of the result
   in *MULTIPLIER_PTR and return the most significant bit.  */
extern unsigned HOST_WIDE_INT choose_multiplier (unsigned HOST_WIDE_INT, int,
						 int, unsigned HOST_WIDE_INT *,
						 int *, int *);

/* Functions from builtins.c:  */
extern rtx expand_builtin (tree, rtx, rtx, enum machine_mode, int);
extern tree std_build_builtin_va_list (void);
extern tree std_fn_abi_va_list (tree);
extern tree std_canonical_va_list_type (tree);

extern void std_expand_builtin_va_start (tree, rtx);
extern rtx default_expand_builtin (tree, rtx, rtx, enum machine_mode, int);
extern void expand_builtin_setjmp_setup (rtx, rtx);
extern void expand_builtin_setjmp_receiver (rtx);
extern rtx expand_builtin_saveregs (void);
extern void expand_builtin_trap (void);
extern rtx builtin_strncpy_read_str (void *, HOST_WIDE_INT, enum machine_mode);

/* Functions from expr.c:  */

/* This is run during target initialization to set up which modes can be
   used directly in memory and to initialize the block move optab.  */
extern void init_expr_target (void);

/* This is run at the start of compiling a function.  */
extern void init_expr (void);

/* Emit some rtl insns to move data between rtx's, converting machine modes.
   Both modes must be floating or both fixed.  */
extern void convert_move (rtx, rtx, int);

/* Convert an rtx to specified machine mode and return the result.  */
extern rtx convert_to_mode (enum machine_mode, rtx, int);

/* Convert an rtx to MODE from OLDMODE and return the result.  */
extern rtx convert_modes (enum machine_mode, enum machine_mode, rtx, int);

/* Emit code to move a block Y to a block X.  */

enum block_op_methods
{
  BLOCK_OP_NORMAL,
  BLOCK_OP_NO_LIBCALL,
  BLOCK_OP_CALL_PARM,
  /* Like BLOCK_OP_NORMAL, but the libcall can be tail call optimized.  */
  BLOCK_OP_TAILCALL
};

extern GTY(()) tree block_clear_fn;
extern void init_block_move_fn (const char *);
extern void init_block_clear_fn (const char *);

extern rtx emit_block_move (rtx, rtx, rtx, enum block_op_methods);
extern rtx emit_block_move_via_libcall (rtx, rtx, rtx, bool);
extern rtx emit_block_move_hints (rtx, rtx, rtx, enum block_op_methods,
			          unsigned int, HOST_WIDE_INT);
extern bool emit_storent_insn (rtx to, rtx from);

/* Copy all or part of a value X into registers starting at REGNO.
   The number of registers to be filled is NREGS.  */
extern void move_block_to_reg (int, rtx, int, enum machine_mode);

/* Copy all or part of a BLKmode value X out of registers starting at REGNO.
   The number of registers to be filled is NREGS.  */
extern void move_block_from_reg (int, rtx, int);

/* Generate a non-consecutive group of registers represented by a PARALLEL.  */
extern rtx gen_group_rtx (rtx);

/* Load a BLKmode value into non-consecutive registers represented by a
   PARALLEL.  */
extern void emit_group_load (rtx, rtx, tree, int);

/* Similarly, but load into new temporaries.  */
extern rtx emit_group_load_into_temps (rtx, rtx, tree, int);

/* Move a non-consecutive group of registers represented by a PARALLEL into
   a non-consecutive group of registers represented by a PARALLEL.  */
extern void emit_group_move (rtx, rtx);

/* Move a group of registers represented by a PARALLEL into pseudos.  */
extern rtx emit_group_move_into_temps (rtx);

/* Store a BLKmode value from non-consecutive registers represented by a
   PARALLEL.  */
extern void emit_group_store (rtx, rtx, tree, int);

extern rtx maybe_emit_group_store (rtx, tree);

/* Copy BLKmode object from a set of registers.  */
extern void copy_blkmode_from_reg (rtx, rtx, tree);

/* Mark REG as holding a parameter for the next CALL_INSN.
   Mode is TYPE_MODE of the non-promoted parameter, or VOIDmode.  */
extern void use_reg_mode (rtx *, rtx, enum machine_mode);

extern rtx copy_blkmode_to_reg (enum machine_mode, tree);

/* Mark REG as holding a parameter for the next CALL_INSN.  */
static inline void
use_reg (rtx *fusage, rtx reg)
{
  use_reg_mode (fusage, reg, VOIDmode);
}

/* Mark NREGS consecutive regs, starting at REGNO, as holding parameters
   for the next CALL_INSN.  */
extern void use_regs (rtx *, int, int);

/* Mark a PARALLEL as holding a parameter for the next CALL_INSN.  */
extern void use_group_regs (rtx *, rtx);

/* Write zeros through the storage of OBJECT.
   If OBJECT has BLKmode, SIZE is its length in bytes.  */
extern rtx clear_storage (rtx, rtx, enum block_op_methods);
extern rtx clear_storage_hints (rtx, rtx, enum block_op_methods,
			        unsigned int, HOST_WIDE_INT);
/* The same, but always output an library call.  */
rtx set_storage_via_libcall (rtx, rtx, rtx, bool);

/* Expand a setmem pattern; return true if successful.  */
extern bool set_storage_via_setmem (rtx, rtx, rtx, unsigned int,
				    unsigned int, HOST_WIDE_INT);

extern unsigned HOST_WIDE_INT move_by_pieces_ninsns (unsigned HOST_WIDE_INT,
						     unsigned int,
						     unsigned int);

/* Return nonzero if it is desirable to store LEN bytes generated by
   CONSTFUN with several move instructions by store_by_pieces
   function.  CONSTFUNDATA is a pointer which will be passed as argument
   in every CONSTFUN call.
   ALIGN is maximum alignment we can assume.
   MEMSETP is true if this is a real memset/bzero, not a copy
   of a const string.  */
extern int can_store_by_pieces (unsigned HOST_WIDE_INT,
				rtx (*) (void *, HOST_WIDE_INT,
					 enum machine_mode),
				void *, unsigned int, bool);

/* Generate several move instructions to store LEN bytes generated by
   CONSTFUN to block TO.  (A MEM rtx with BLKmode).  CONSTFUNDATA is a
   pointer which will be passed as argument in every CONSTFUN call.
   ALIGN is maximum alignment we can assume.
   MEMSETP is true if this is a real memset/bzero, not a copy.
   Returns TO + LEN.  */
extern rtx store_by_pieces (rtx, unsigned HOST_WIDE_INT,
			    rtx (*) (void *, HOST_WIDE_INT, enum machine_mode),
			    void *, unsigned int, bool, int);

/* Emit insns to set X from Y.  */
extern rtx emit_move_insn (rtx, rtx);

/* Emit insns to set X from Y, with no frills.  */
extern rtx emit_move_insn_1 (rtx, rtx);

extern rtx emit_move_complex_push (enum machine_mode, rtx, rtx);
extern rtx emit_move_complex_parts (rtx, rtx);

/* Push a block of length SIZE (perhaps variable)
   and return an rtx to address the beginning of the block.  */
extern rtx push_block (rtx, int, int);

/* Generate code to push something onto the stack, given its mode and type.  */
extern void emit_push_insn (rtx, enum machine_mode, tree, rtx, unsigned int,
			    int, rtx, int, rtx, rtx, int, rtx);

/* Expand an assignment that stores the value of FROM into TO.  */
extern void expand_assignment (tree, tree, bool);

/* Generate code for computing expression EXP,
   and storing the value into TARGET.
   If SUGGEST_REG is nonzero, copy the value through a register
   and return that register, if that is possible.  */
extern rtx store_expr (tree, rtx, int, bool);

/* Given an rtx that may include add and multiply operations,
   generate them as insns and return a pseudo-reg containing the value.
   Useful after calling expand_expr with 1 as sum_ok.  */
extern rtx force_operand (rtx, rtx);

/* Work horses for expand_expr.  */
extern rtx expand_expr_real (tree, rtx, enum machine_mode,
			     enum expand_modifier, rtx *);
extern rtx expand_expr_real_1 (tree, rtx, enum machine_mode,
			       enum expand_modifier, rtx *);
extern rtx expand_expr_real_2 (sepops, rtx, enum machine_mode,
			       enum expand_modifier);

/* Generate code for computing expression EXP.
   An rtx for the computed value is returned.  The value is never null.
   In the case of a void EXP, const0_rtx is returned.  */
static inline rtx
expand_expr (tree exp, rtx target, enum machine_mode mode,
	     enum expand_modifier modifier)
{
  return expand_expr_real (exp, target, mode, modifier, NULL);
}

static inline rtx
expand_normal (tree exp)
{
  return expand_expr_real (exp, NULL_RTX, VOIDmode, EXPAND_NORMAL, NULL);
}

/* At the start of a function, record that we have no previously-pushed
   arguments waiting to be popped.  */
extern void init_pending_stack_adjust (void);

/* Discard any pending stack adjustment.  */
extern void discard_pending_stack_adjust (void);

/* When exiting from function, if safe, clear out any pending stack adjust
   so the adjustment won't get done.  */
extern void clear_pending_stack_adjust (void);

/* Pop any previously-pushed arguments that have not been popped yet.  */
extern void do_pending_stack_adjust (void);

/* Return the tree node and offset if a given argument corresponds to
   a string constant.  */
extern tree string_constant (tree, tree *);

/* Generate code to evaluate EXP and jump to LABEL if the value is zero.  */
extern void jumpifnot (tree, rtx, int);
extern void jumpifnot_1 (enum tree_code, tree, tree, rtx, int);

/* Generate code to evaluate EXP and jump to LABEL if the value is nonzero.  */
extern void jumpif (tree, rtx, int);
extern void jumpif_1 (enum tree_code, tree, tree, rtx, int);

/* Generate code to evaluate EXP and jump to IF_FALSE_LABEL if
   the result is zero, or IF_TRUE_LABEL if the result is one.  */
extern void do_jump (tree, rtx, rtx, int);
extern void do_jump_1 (enum tree_code, tree, tree, rtx, rtx, int);

extern void do_compare_rtx_and_jump (rtx, rtx, enum rtx_code, int,
				     enum machine_mode, rtx, rtx, rtx, int);

/* Two different ways of generating switch statements.  */
extern int try_casesi (tree, tree, tree, tree, rtx, rtx, rtx, int);
extern int try_tablejump (tree, tree, tree, tree, rtx, rtx, int);

/* Functions from alias.c */
#include "alias.h"


/* rtl.h and tree.h were included.  */
/* Return an rtx for the size in bytes of the value of an expr.  */
extern rtx expr_size (tree);

/* Return a wide integer for the size in bytes of the value of EXP, or -1
   if the size can vary or is larger than an integer.  */
extern HOST_WIDE_INT int_expr_size (tree);

/* Return an rtx that refers to the value returned by a function
   in its original home.  This becomes invalid if any more code is emitted.  */
extern rtx hard_function_value (const_tree, const_tree, const_tree, int);

extern rtx prepare_call_address (tree, rtx, rtx, rtx *, int, int);

extern bool shift_return_value (enum machine_mode, bool, rtx);

extern rtx expand_call (tree, rtx, int);

extern void fixup_tail_calls (void);

#ifdef TREE_CODE
extern rtx expand_variable_shift (enum tree_code, enum machine_mode,
				  rtx, tree, rtx, int);
extern rtx expand_shift (enum tree_code, enum machine_mode, rtx, int, rtx,
			     int);
extern rtx expand_divmod (int, enum tree_code, enum machine_mode, rtx, rtx,
			  rtx, int);
#endif

extern void locate_and_pad_parm (enum machine_mode, tree, int, int, tree,
				 struct args_size *,
				 struct locate_and_pad_arg_data *);

/* Return the CODE_LABEL rtx for a LABEL_DECL, creating it if necessary.  */
extern rtx label_rtx (tree);

/* As label_rtx, but additionally the label is placed on the forced label
   list of its containing function (i.e. it is treated as reachable even
   if how is not obvious).  */
extern rtx force_label_rtx (tree);

/* Return an rtx like arg but sans any constant terms.
   Returns the original rtx if it has no constant terms.
   The constant terms are added and stored via a second arg.  */
extern rtx eliminate_constant_term (rtx, rtx *);

/* Convert arg to a valid memory address for specified machine mode that points
   to a specific named address space, by emitting insns to perform arithmetic
   if necessary.  */
extern rtx memory_address_addr_space (enum machine_mode, rtx, addr_space_t);

/* Like memory_address_addr_space, except assume the memory address points to
   the generic named address space.  */
#define memory_address(MODE,RTX) \
	memory_address_addr_space ((MODE), (RTX), ADDR_SPACE_GENERIC)

/* Return a memory reference like MEMREF, but with its mode changed
   to MODE and its address changed to ADDR.
   (VOIDmode means don't change the mode.
   NULL for ADDR means don't change the address.)  */
extern rtx change_address (rtx, enum machine_mode, rtx);

/* Return a memory reference like MEMREF, but with its mode changed
   to MODE and its address offset by OFFSET bytes.  */
#define adjust_address(MEMREF, MODE, OFFSET) \
  adjust_address_1 (MEMREF, MODE, OFFSET, 1, 1, 0, 0)

/* Likewise, but the reference is not required to be valid.  */
#define adjust_address_nv(MEMREF, MODE, OFFSET) \
  adjust_address_1 (MEMREF, MODE, OFFSET, 0, 1, 0, 0)

/* Return a memory reference like MEMREF, but with its mode changed
   to MODE and its address offset by OFFSET bytes.  Assume that it's
   for a bitfield and conservatively drop the underlying object if we
   cannot be sure to stay within its bounds.  */
#define adjust_bitfield_address(MEMREF, MODE, OFFSET) \
  adjust_address_1 (MEMREF, MODE, OFFSET, 1, 1, 1, 0)

/* As for adjust_bitfield_address, but specify that the width of
   BLKmode accesses is SIZE bytes.  */
#define adjust_bitfield_address_size(MEMREF, MODE, OFFSET, SIZE) \
  adjust_address_1 (MEMREF, MODE, OFFSET, 1, 1, 1, SIZE)

/* Likewise, but the reference is not required to be valid.  */
#define adjust_bitfield_address_nv(MEMREF, MODE, OFFSET) \
  adjust_address_1 (MEMREF, MODE, OFFSET, 0, 1, 1, 0)

/* Return a memory reference like MEMREF, but with its mode changed
   to MODE and its address changed to ADDR, which is assumed to be
   increased by OFFSET bytes from MEMREF.  */
#define adjust_automodify_address(MEMREF, MODE, ADDR, OFFSET) \
  adjust_automodify_address_1 (MEMREF, MODE, ADDR, OFFSET, 1)

/* Likewise, but the reference is not required to be valid.  */
#define adjust_automodify_address_nv(MEMREF, MODE, ADDR, OFFSET) \
  adjust_automodify_address_1 (MEMREF, MODE, ADDR, OFFSET, 0)

extern rtx adjust_address_1 (rtx, enum machine_mode, HOST_WIDE_INT, int, int,
			     int, HOST_WIDE_INT);
extern rtx adjust_automodify_address_1 (rtx, enum machine_mode, rtx,
					HOST_WIDE_INT, int);

/* Return a memory reference like MEMREF, but whose address is changed by
   adding OFFSET, an RTX, to it.  POW2 is the highest power of two factor
   known to be in OFFSET (possibly 1).  */
extern rtx offset_address (rtx, rtx, unsigned HOST_WIDE_INT);

/* Definitions from emit-rtl.c */
#include "emit-rtl.h"

/* Return a memory reference like MEMREF, but with its mode widened to
   MODE and adjusted by OFFSET.  */
extern rtx widen_memory_access (rtx, enum machine_mode, HOST_WIDE_INT);

/* Return a memory reference like MEMREF, but which is known to have a
   valid address.  */
extern rtx validize_mem (rtx);

extern rtx use_anchored_address (rtx);

/* Given REF, a MEM, and T, either the type of X or the expression
   corresponding to REF, set the memory attributes.  OBJECTP is nonzero
   if we are making a new object of this type.  */
extern void set_mem_attributes (rtx, tree, int);

/* Similar, except that BITPOS has not yet been applied to REF, so if
   we alter MEM_OFFSET according to T then we should subtract BITPOS
   expecting that it'll be added back in later.  */
extern void set_mem_attributes_minus_bitpos (rtx, tree, int, HOST_WIDE_INT);

/* Return OFFSET if XEXP (MEM, 0) - OFFSET is known to be ALIGN
   bits aligned for 0 <= OFFSET < ALIGN / BITS_PER_UNIT, or
   -1 if not known.  */
extern int get_mem_align_offset (rtx, unsigned int);

/* Assemble the static constant template for function entry trampolines.  */
extern rtx assemble_trampoline_template (void);

/* Copy given rtx to a new temp reg and return that.  */
extern rtx copy_to_reg (rtx);

/* Like copy_to_reg but always make the reg Pmode.  */
extern rtx copy_addr_to_reg (rtx);

/* Like copy_to_reg but always make the reg the specified mode MODE.  */
extern rtx copy_to_mode_reg (enum machine_mode, rtx);

/* Copy given rtx to given temp reg and return that.  */
extern rtx copy_to_suggested_reg (rtx, rtx, enum machine_mode);

/* Copy a value to a register if it isn't already a register.
   Args are mode (in case value is a constant) and the value.  */
extern rtx force_reg (enum machine_mode, rtx);

/* Return given rtx, copied into a new temp reg if it was in memory.  */
extern rtx force_not_mem (rtx);

/* Return mode and signedness to use when an argument or result in the
   given mode is promoted.  */
extern enum machine_mode promote_function_mode (const_tree, enum machine_mode, int *,
					        const_tree, int);

/* Return mode and signedness to use when an object in the given mode
   is promoted.  */
extern enum machine_mode promote_mode (const_tree, enum machine_mode, int *);

/* Return mode and signedness to use when object is promoted.  */
enum machine_mode promote_decl_mode (const_tree, int *);

/* Remove some bytes from the stack.  An rtx says how many.  */
extern void adjust_stack (rtx);

/* Add some bytes to the stack.  An rtx says how many.  */
extern void anti_adjust_stack (rtx);

/* Add some bytes to the stack while probing it.  An rtx says how many. */
extern void anti_adjust_stack_and_probe (rtx, bool);

/* This enum is used for the following two functions.  */
enum save_level {SAVE_BLOCK, SAVE_FUNCTION, SAVE_NONLOCAL};

/* Save the stack pointer at the specified level.  */
extern void emit_stack_save (enum save_level, rtx *);

/* Restore the stack pointer from a save area of the specified level.  */
extern void emit_stack_restore (enum save_level, rtx);

/* Invoke emit_stack_save for the nonlocal_goto_save_area.  */
extern void update_nonlocal_goto_save_area (void);

/* Allocate some space on the stack dynamically and return its address.  */
extern rtx allocate_dynamic_stack_space (rtx, unsigned, unsigned, bool);

/* Emit one stack probe at ADDRESS, an address within the stack.  */
extern void emit_stack_probe (rtx);

/* Probe a range of stack addresses from FIRST to FIRST+SIZE, inclusive.
   FIRST is a constant and size is a Pmode RTX.  These are offsets from
   the current stack pointer.  STACK_GROWS_DOWNWARD says whether to add
   or subtract them from the stack pointer.  */
extern void probe_stack_range (HOST_WIDE_INT, rtx);

/* Return an rtx that refers to the value returned by a library call
   in its original home.  This becomes invalid if any more code is emitted.  */
extern rtx hard_libcall_value (enum machine_mode, rtx);

extern void store_bit_field (rtx, unsigned HOST_WIDE_INT,
			     unsigned HOST_WIDE_INT,
			     unsigned HOST_WIDE_INT,
			     unsigned HOST_WIDE_INT,
			     enum machine_mode, rtx);
extern rtx extract_bit_field (rtx, unsigned HOST_WIDE_INT,
			      unsigned HOST_WIDE_INT, int, bool, rtx,
			      enum machine_mode, enum machine_mode);
extern rtx extract_low_bits (enum machine_mode, enum machine_mode, rtx);
extern rtx expand_mult (enum machine_mode, rtx, rtx, rtx, int);
extern rtx expand_mult_highpart_adjust (enum machine_mode, rtx, rtx, rtx, rtx, int);

extern rtx assemble_static_space (unsigned HOST_WIDE_INT);
extern int safe_from_p (const_rtx, tree, int);
extern bool split_comparison (enum rtx_code, enum machine_mode,
			      enum rtx_code *, enum rtx_code *);

/* Call this once to initialize the contents of the optabs
   appropriately for the current target machine.  */
extern void init_optabs (void);
extern void init_all_optabs (void);

/* Call this to initialize an optab function entry.  */
extern rtx init_one_libfunc (const char *);
extern rtx set_user_assembler_libfunc (const char *, const char *);

/* Build a decl for a libfunc named NAME. */
extern tree build_libfunc_function (const char *);

/* Get the personality libfunc for a function decl.  */
rtx get_personality_function (tree);


/* In stmt.c */

/* Expand a GIMPLE_SWITCH statement.  */
extern void expand_case (gimple);

/* Like expand_case but special-case for SJLJ exception dispatching.  */
extern void expand_sjlj_dispatch_table (rtx, vec<tree> );

#endif /* GCC_EXPR_H */
