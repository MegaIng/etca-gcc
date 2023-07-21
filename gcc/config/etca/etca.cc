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

    *cum = (*cum < ETCA_R2
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


#undef  TARGET_FUNCTION_ARG
#define TARGET_FUNCTION_ARG		etca_function_arg
#undef  TARGET_FUNCTION_ARG_ADVANCE
#define TARGET_FUNCTION_ARG_ADVANCE	etca_function_arg_advance
#undef  TARGET_FUNCTION_VALUE
#define TARGET_FUNCTION_VALUE 		etca_function_value
#undef  TARGET_FUNCTION_VALUE_REGNO_P
#define TARGET_FUNCTION_VALUE_REGNO_P 	etca_function_value_regno_p
#undef  TARGET_ADDR_SPACE_LEGITIMATE_ADDRESS_P
#define TARGET_ADDR_SPACE_LEGITIMATE_ADDRESS_P	etca_legitimate_address_p

struct gcc_target targetm = TARGET_INITIALIZER;