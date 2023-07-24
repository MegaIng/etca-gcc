
#define IN_TARGET_CODE 1
#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "backend.h"
#include "target.h"
#include "rtl.h"
#include "tree.h"
#include "stringpool.h"
#include "attribs.h"
#include "df.h"
#include "regs.h"
#include "memmodel.h"
#include "emit-rtl.h"
#include "diagnostic-core.h"
#include "output.h"
#include "stor-layout.h"
#include "varasm.h"
#include "calls.h"
#include "expr.h"
#include "builtins.h"

#include "target-def.h"

static rtx
etca_function_arg (cumulative_args_t cum_v, const function_arg_info &arg)
{
    CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);

    if (*cum <= ETCA_R2)
	return gen_rtx_REG (arg.mode, *cum);
    else
	return NULL_RTX;
}

static void
etca_function_arg_advance (cumulative_args_t cum_v,
			    const function_arg_info &arg ATTRIBUTE_UNUSED)
{
    CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);

    *cum = (*cum <= ETCA_R2
	    ? *cum + 1
	    : *cum);
}


static rtx
etca_function_value (const_tree valtype,
		     const_tree fntype_or_decl ATTRIBUTE_UNUSED,
		     bool outgoing ATTRIBUTE_UNUSED)
{
    return gen_rtx_REG (TYPE_MODE (valtype), ETCA_R0);
}
static bool
etca_function_value_regno_p (const unsigned int regno)
{
    return regno == 0;
}

static bool
etca_reg_ok_for_base_p (const_rtx reg, bool strict_p)
{
    int regno = REGNO (reg);

    if (strict_p)
	return REGNO_OK_FOR_BASE_P (regno)
	       || REGNO_OK_FOR_BASE_P (reg_renumber[regno]);
    else
	return !HARD_REGISTER_NUM_P (regno)
	       || REGNO_OK_FOR_BASE_P (regno);
}


static bool
etca_legitimate_address_p (machine_mode mode ATTRIBUTE_UNUSED,
			    rtx x, bool strict_p,
			    addr_space_t as)
{
    gcc_assert (ADDR_SPACE_GENERIC_P (as));

    if (REG_P (x) && etca_reg_ok_for_base_p (x, strict_p))
	return true;
    if (GET_CODE (x) == SYMBOL_REF
	|| GET_CODE (x) == LABEL_REF
	|| GET_CODE (x) == CONST)
	return true;
    return false;
}


static void
etca_print_operand (FILE *file, rtx x, int code)
{
    rtx operand = x;
    if (!(code == 0 || code == 'h' || code == 'x' || code == 'd' || code == 'q')) {
	debug_rtx (x);
	output_operand_lossage ("invalid operand modifier code: '%c'", code);
	return;
    }

    /* Print an operand as without a modifier letter.  */
    switch (GET_CODE (operand))
    {
	case REG: {
	    if (REGNO(operand) > ETCA_R15)
		internal_error("internal error: bad register: %d", REGNO(operand));
	    const char* reg_name = reg_names[REGNO(operand)];
	    /* reg_name is always 3 chars long (plus 0 term)
	     * first is either % for a real register or ? for a virtual one
	     * second is either always a letter
	     * third is either a letter or a digit.
	     *
	     * Depending on whether the third is a letter, the modifier
	     * goes either between or after the name.
	     * */
	    if (code == 0) {
		fprintf(file, "%s", reg_name);
	    } else if ('0' <= reg_name[2] && reg_name[2] < '9') {
		fprintf(file, "%c%c%c%c", reg_name[0], reg_name[1], code, reg_name[2]);
	    } else {
		fprintf(file, "%s%c", reg_name, code);
	    }

	    return;
	}
	case MEM:
	    output_address (GET_MODE (XEXP (operand, 0)), XEXP (operand, 0));
	    return;

	default:
	    if (CONSTANT_P (operand))
	    {
		output_addr_const (file, operand);
		return;
	    }

	    debug_rtx (operand);
	    output_operand_lossage ("unexpected operand");
	    return;
    }
}


static void
etca_print_operand_address (FILE *file, machine_mode, rtx x)
{
    switch (GET_CODE (x))
    {
	case REG:
	    fprintf (file, "[%s]", reg_names[REGNO (x)]);
	    break;

	case PLUS:
	    switch (GET_CODE (XEXP (x, 1)))
	    {
		case CONST_INT:
		    fprintf (file, "[%s + %ld]",
			     reg_names[REGNO (XEXP (x, 0))], INTVAL(XEXP (x, 1)));
		    break;
		case SYMBOL_REF:
		    fprintf (file, "[%s + ", reg_names[REGNO (XEXP (x, 0))]);
		    output_addr_const (file, XEXP (x, 1));
		    fprintf (file, "]");
		    break;
		    /*
		case CONST:
		{
		    rtx plus = XEXP (XEXP (x, 1), 0);
		    if (GET_CODE (XEXP (plus, 0)) == SYMBOL_REF
			&& CONST_INT_P (XEXP (plus, 1)))
		    {
			output_addr_const(file, XEXP (plus, 0));
			fprintf (file,"+%ld(%s)", INTVAL (XEXP (plus, 1)),
				 reg_names[REGNO (XEXP (x, 0))]);
		    }
		    else
			abort();
		}
		    break;
		     */
		default:
		    abort();
	    }
	    break;

	default:
	    output_addr_const (file, x);
	    break;
    }
}

/* Return the fixed registers used for condition codes.  */

static bool
etca_fixed_condition_code_regs (unsigned int *p1, unsigned int *p2)
{
    *p1 = ETCA_CC;
    *p2 = INVALID_REGNUM;
    return false; /* The pass that get's enabled probably does nothing for us. */
}

/* We need to override this because otherwise GCC thinks the CC registers is split up.
 * This is because GET_MODE_SIZE(CCmode) is hardcoded for MODE_CC to be 4, but our
 * registers are only 2 units.
 * */
static unsigned int
etca_hard_regno_nregs (unsigned int regno, machine_mode mode)
{
    if (regno < ETCA_PC)
	return CEIL (GET_MODE_SIZE (mode), UNITS_PER_WORD);
    return 1;
}



/* Per-function machine data.  */
struct GTY(()) machine_function
{
    /* various flags for how to treat the function */
    uint32_t func_type;
    /* Number of bytes saved on the stack for callee saved registers.  */
    uint32_t callee_saved_reg_size;
    /* Number of bytes saved on the stack for local variables.  */
    uint32_t local_vars_size;
};

/* Zero initialization is OK for all current fields.  */

static struct machine_function *
etca_init_machine_status (void)
{
    return ggc_cleared_alloc<machine_function> ();
}

static uint32_t
etca_compute_function_type (void)
{
    uint32_t res = ETCA_FT_NORMAL | ETCA_FT_CC_UNKNOWN;
    tree a;
    tree attr;

    attr = DECL_ATTRIBUTES (current_function_decl);
    a = lookup_attribute ("naked", attr);
    if (a != NULL_TREE) {
	res |= ETCA_FT_NAKED;
    }
    return res;
}


static uint32_t
etca_get_function_type (void) {
    if (cfun->machine->func_type == ETCA_FT_UNKNOWN) {
	cfun->machine->func_type = etca_compute_function_type ();
    }
    return cfun->machine->func_type;
}

/* Compute the size of the local area and the size to be adjusted by the
 * prologue and epilogue.  */

static void
etca_compute_frame (void)
{
    /* For aligning the local variables.  */
    int stack_alignment = STACK_BOUNDARY / BITS_PER_UNIT;
    int padding_locals;
    int regno;

    /* Padding needed for each element of the frame.  */
    cfun->machine->local_vars_size = get_frame_size ();
    cfun->machine->func_type = etca_compute_function_type ();

    /* Align to the stack alignment.  */
    padding_locals = cfun->machine->local_vars_size % stack_alignment;
    if (padding_locals) {
	padding_locals = stack_alignment - padding_locals;
    }

    cfun->machine->local_vars_size += padding_locals;

    cfun->machine->callee_saved_reg_size = 0;

    /* Save callee-saved registers.  */
    for (regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++) {
	if (df_regs_ever_live_p(regno) && (!call_used_or_fixed_reg_p(regno))) {
	    cfun->machine->callee_saved_reg_size += 2;
	}
    }
}


void
etca_init_expanders (void) {
    /* Arrange to initialize and mark the machine per-function status.  */
    init_machine_status = etca_init_machine_status;
}

int
etca_initial_elimination_offset (int from, int to)
{
    int ret;

    if ((from) == ARG_POINTER_REGNUM && (to) == HARD_FRAME_POINTER_REGNUM)
    {
	ret = 2;
    } else if ((from) == FRAME_POINTER_REGNUM && (to) == HARD_FRAME_POINTER_REGNUM)
    {
	ret = cfun->machine->callee_saved_reg_size;
    }  else if ((from) == HARD_FRAME_POINTER_REGNUM && (to) == STACK_POINTER_REGNUM)
    {
	ret = -(cfun->machine->callee_saved_reg_size + cfun->machine->local_vars_size);
    } else {
	abort();
    }
    return ret;
}
#define MUST_SAVE_FRAME_POINTER	 (df_regs_ever_live_p (HARD_FRAME_POINTER_REGNUM)  || frame_pointer_needed)


void
etca_expand_prologue (void) {
    int regno;
    rtx insn;
    uint32_t ft = etca_get_function_type();
    if (IS_NAKED(ft)) {
	return;
    }
    if (MUST_SAVE_FRAME_POINTER) {
	/* Store base old base pointer */
	insn = emit_insn (gen_pushhi1 (gen_rtx_REG (Pmode, ETCA_BP)));
	RTX_FRAME_RELATED_P (insn) = 1;
	/* Create the new base pointer*/
	RTX_FRAME_RELATED_P (insn) = 1;
	insn = emit_insn (gen_movhi (gen_rtx_REG (Pmode, ETCA_BP), gen_rtx_REG (Pmode, ETCA_SP)));
    }
    for (int i = 0; i <= ETCA_R15; i++) {
	if (i == ETCA_SP || i == ETCA_BP) { continue; }
	if (df_regs_ever_live_p(i) && !call_used_or_fixed_reg_p(i)) {
	    insn = emit_insn (gen_pushhi1 (gen_rtx_REG (Pmode, i)));
	    RTX_FRAME_RELATED_P (insn) = 1;
	}
    }
    /* Allocated memory for the local variables */
    if (cfun->machine->local_vars_size) {
	insn = emit_insn (gen_subhi3 (
		gen_rtx_REG (Pmode, ETCA_SP),
		gen_rtx_REG (Pmode, ETCA_SP),
		gen_int_mode (cfun->machine->local_vars_size, Pmode)));
	RTX_FRAME_RELATED_P (insn) = 1;
    }
}


void
etca_expand_epilogue ()
{
    rtx insn;
    uint32_t ft = etca_get_function_type();
    if (IS_NAKED(ft)) {
	/* We don't even have a return instruction in this case. That's the job of the programmer now */
	return;
    }
    if(cfun->machine->local_vars_size) {
	insn = emit_insn (gen_addhi3 (
		gen_rtx_REG (Pmode, ETCA_SP),
		gen_rtx_REG (Pmode, ETCA_SP),
		gen_int_mode (cfun->machine->local_vars_size, Pmode)));
	RTX_FRAME_RELATED_P (insn) = 1;
    }
    for (int i = ETCA_R15; i >= 0; i--) {
	if (i == ETCA_SP || i == ETCA_BP) { continue; }
	if (df_regs_ever_live_p(i) && !call_used_or_fixed_reg_p(i)) {
	    insn = emit_insn (gen_pophi1 (gen_rtx_REG (Pmode, i)));
	    RTX_FRAME_RELATED_P (insn) = 1;
	}
    }
    if(MUST_SAVE_FRAME_POINTER) {
	insn = emit_insn (gen_pophi1 (gen_rtx_REG (Pmode, ETCA_BP)));
	RTX_FRAME_RELATED_P (insn) = 1;
    }
    emit_jump_insn (gen_etca_returner ());
}


/* Handle an attribute requiring a FUNCTION_DECL;
   arguments as in struct attribute_spec.handler.  */
static tree
etca_handle_fndecl_attribute (tree *node, tree name, tree args ATTRIBUTE_UNUSED,
			     int flags ATTRIBUTE_UNUSED, bool *no_add_attrs)
{
    if (TREE_CODE (*node) != FUNCTION_DECL)
    {
	warning (OPT_Wattributes, "%qE attribute only applies to functions",
		 name);
	*no_add_attrs = true;
    }

    return NULL_TREE;
}

/* Table of machine attributes.  */
static const struct attribute_spec etca_attribute_table[] = {
	/* { name, min_len, max_len, decl_req, type_req, fn_type_req,
	     affects_type_identity, handler, exclude } */
	{ "naked",        0, 0, true,  false, false, false,
	  etca_handle_fndecl_attribute, NULL },
	{ NULL,        0, 0, false,  false, false, false,
		NULL, NULL },
};


#undef  TARGET_FUNCTION_ARG
#define TARGET_FUNCTION_ARG		etca_function_arg
#undef  TARGET_FUNCTION_ARG_ADVANCE
#define TARGET_FUNCTION_ARG_ADVANCE	etca_function_arg_advance
#undef  TARGET_FUNCTION_VALUE
#define TARGET_FUNCTION_VALUE 		etca_function_value
#undef  TARGET_FUNCTION_VALUE_REGNO_P
#define TARGET_FUNCTION_VALUE_REGNO_P 	etca_function_value_regno_p

#undef TARGET_COMPUTE_FRAME_LAYOUT
#define TARGET_COMPUTE_FRAME_LAYOUT	etca_compute_frame

#undef  TARGET_FIXED_CONDITION_CODE_REGS
#define TARGET_FIXED_CONDITION_CODE_REGS	etca_fixed_condition_code_regs
#undef  TARGET_FLAGS_REGNUM
#define TARGET_FLAGS_REGNUM  ETCA_CC
#undef  TARGET_HARD_REGNO_NREGS
#define TARGET_HARD_REGNO_NREGS etca_hard_regno_nregs

#undef  TARGET_ADDR_SPACE_LEGITIMATE_ADDRESS_P
#define TARGET_ADDR_SPACE_LEGITIMATE_ADDRESS_P	etca_legitimate_address_p

#undef  TARGET_ATTRIBUTE_TABLE
#define TARGET_ATTRIBUTE_TABLE etca_attribute_table

#undef  TARGET_PRINT_OPERAND
#define TARGET_PRINT_OPERAND etca_print_operand
#undef  TARGET_PRINT_OPERAND_ADDRESS
#define TARGET_PRINT_OPERAND_ADDRESS etca_print_operand_address

struct gcc_target targetm = TARGET_INITIALIZER;

#include "gt-etca.h"