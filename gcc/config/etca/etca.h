//
// Created by MegaIng on 2023-07-20.
//

#ifndef GCC_ETCA_H
#define GCC_ETCA_H

 /* Memory model Specification */

#define BITS_BIG_ENDIAN 0
#define BYTES_BIG_ENDIAN 0
#define WORDS_BIG_ENDIAN 0

#define UNITS_PER_WORD 2

#define DEFAULT_SIGNED_CHAR 1

#define SHORT_TYPE_SIZE 16
#define INT_TYPE_SIZE 16
#define LONG_TYPE_SIZE 32
#define LONG_LONG_TYPE_SIZE 64

#define FLOAT_TYPE_SIZE 32
#define DOUBLE_TYPE_SIZE 64
#define LONG_DOUBLE_TYPE_SIZE 64

/* Stack alignment */
#define PARM_BOUNDARY 16
#define STACK_BOUNDARY 16

/* Function entry point alignment */
#define FUNCTION_BOUNDARY 8

#define BIGGEST_ALIGNMENT 64

/* This should depend on if the UMA feature is enabled or not*/
#define STRICT_ALIGNMENT 1


/* Maybe we should use numeric names instead? */
#define REGISTER_NAMES {	  \
   "%a0", "%a1",  "%a2",  "%s0",  \
   "%s1", "%bp",  "%sp",  "%ln",  \
   "%t0", "%t1",  "%t2",  "%t3",  \
   "%t4", "%s2",  "%s3",  "%s4",  \
   "?ag", "?fp",  "?pc",  "?cc",  }

/* Registers */
/* Names are defined in etca.md*/

#define FIRST_PSEUDO_REGISTER 20
#define FIXED_REGISTERS  /*  a0  a1  a2  s0 */   { 0, 0, 0, 0, \
                         /*  s1  bp  sp  ln */     0, 0, 1, 1, \
                         /*  t0  t1  t2  t3 */     0, 0, 0, 0, \
                         /*  t4  s2  s3  s4 */     0, 0, 0, 0, \
			 /*  ag  fp  pc  cc */     1, 1, 1, 1, }

#define CALL_REALLY_USED_REGISTERS \
                         /*  a0  a1  a2  s0 */   { 1, 1, 1, 0, \
                         /*  s1  bp  sp  ln */     0, 0, 0, 0, \
                         /*  t0  t1  t2  t3 */     1, 1, 1, 1, \
                         /*  t4  s2  s3  s4 */     1, 0, 0, 0, \
			 /*  ag  fp  pc  cc */     0, 0, 0, 0, }

/* Register Classes */

enum reg_class
{
    NO_REGS,
    GENERAL_REGS,
    SPECIAL_REGS,
    CC_REGS,
    ALL_REGS,
    LIM_REG_CLASSES
};

#define N_REG_CLASSES (int) LIM_REG_CLASSES

#define REG_CLASS_NAMES {\
    "NO_REGS", \
    "GENERAL_REGS", \
    "SPECIAL_REGS", \
    "CC_REGS", \
    "ALL_REGS" }

#define REG_CLASS_CONTENTS {\
    0x00000, \
    0x3FFFF,  /* GENERAL_REGS*/ \
    0x40000,  /* SPECIAL_REGS */ \
    0x80000,  /* CC_REGS */ \
    0xFFFFF }

#define REGNO_REG_CLASS(R) ((R <= ETCA_VFP) ? GENERAL_REGS : ((R == ETCA_CC)? CC_REGS : SPECIAL_REGS))

#define BASE_REG_CLASS GENERAL_REGS
#define INDEX_REG_CLASS GENERAL_REGS

/* MISC */

#define MOVE_MAX 2

#define Pmode HImode

#define FUNCTION_MODE QImode

#define AVOID_CCMODE_COPIES 1


/* STACK AND CALLING */

/* To allow initialization of `cfun->machine`*/
#define INIT_EXPANDERS  etca_init_expanders ()

#define STACK_GROWS_DOWNWARD 1
#define FRAME_GROWS_DOWNWARD 1
#define STACK_ALIGNMENT_NEEDED 0

#define STACK_POINTER_REGNUM ETCA_SP
#define FRAME_POINTER_REGNUM ETCA_VFP
#define HARD_FRAME_POINTER_REGNUM ETCA_BP
#define ARG_POINTER_REGNUM ETCA_ARG

#define FIRST_PARM_OFFSET(F) 0

#define FUNCTION_ARG_REGNO_P(regno) (regno <= ETCA_R2)


#define ELIMINABLE_REGS	{ \
    { FRAME_POINTER_REGNUM, HARD_FRAME_POINTER_REGNUM }, \
    { ARG_POINTER_REGNUM, HARD_FRAME_POINTER_REGNUM }, \
    { HARD_FRAME_POINTER_REGNUM, STACK_POINTER_REGNUM }, \
}

#define INITIAL_ELIMINATION_OFFSET(FROM, TO, OFFSET)			\
  do {									\
    (OFFSET) = etca_initial_elimination_offset ((FROM), (TO));		\
  } while (0)
#define TRAMPOLINE_SIZE  (abort (), 0)

/* func types to be used in machine_function.func_type*/
#define ETCA_FT_UNKNOWN		0x0000
#define ETCA_FT_NORMAL		0x0001
#define ETCA_FT_MASK		0x0001

#define ETCA_FT_CC_UNKNOWN	(0b00 << 1)
#define ETCA_FT_CC_CC16		(0b01 << 1)
#define ETCA_FT_CC_CC32		(0b10 << 1)
#define ETCA_FT_CC_CC64		(0b11 << 1)
#define ETCA_FT_CC_MASK		(0b11 << 1)

#define ETCA_FT_NAKED		(1 << 3)

#define EXTRACT_FT_CC(FT)	(FT & ETCA_FT_CC_MASK)
#define DECIDE_FT_CC(FT)	(FT & ETCA_FT_CC_MASK)

#define IS_NAKED(FT)	((FT & ETCA_FT_NAKED) != 0)

/* Passing Arguments in Registers */

/* A C type for declaring a variable that is used as the first
   argument of `FUNCTION_ARG' and other related values.  */
#define CUMULATIVE_ARGS unsigned int
#define INIT_CUMULATIVE_ARGS(CUM,FNTYPE,LIBNAME,FNDECL,N_NAMED_ARGS) \
  (CUM = ETCA_R0)

#define LIBCALL_VALUE(mode) (gen_rtx_REG (mode, ETCA_R0))

/* Addressing Modes */

#define MAX_REGS_PER_ADDRESS 2
#define CASE_VECTOR_MODE HImode

#define HARD_REGNO_OK_FOR_BASE_P(NUM) ((unsigned) (NUM) <= ETCA_VFP )

#ifdef REG_OK_STRICT
#define REGNO_OK_FOR_BASE_P(NUM)		 \
  (HARD_REGNO_OK_FOR_BASE_P(NUM) 		 \
   || HARD_REGNO_OK_FOR_BASE_P(reg_renumber[(NUM)]))
#else
#define REGNO_OK_FOR_BASE_P(NUM)		 \
  ((NUM) >= FIRST_PSEUDO_REGISTER || HARD_REGNO_OK_FOR_BASE_P(NUM))
#endif

#define HARD_REGNO_OK_FOR_INDEX_P(NUM) HARD_REGNO_OK_FOR_BASE_P(NUM)
#define REGNO_OK_FOR_INDEX_P(NUM)	REGNO_OK_FOR_BASE_P(NUM)

/* Costs */

#define SLOW_BYTE_ACCESS 0  /* Says that byte accesses are not more expensive than word accesses */

/* Profiler */

#define FUNCTION_PROFILER(FILE,LABELNO) (abort (), 0)

/* Assembly Output */

/*
#undef  ASM_SPEC
#define ASM_SPEC "%{!mel:-EB} %{mel:-EL}"
*/

#define ASM_COMMENT_START ";"
#define ASM_APP_ON ""
#define ASM_APP_OFF ""


#define FILE_ASM_OP     "\t.file\n"

#define TEXT_SECTION_ASM_OP  "\t.text"
#define DATA_SECTION_ASM_OP  "\t.data"

#define ASM_OUTPUT_ALIGN(STREAM,POWER) \
	fprintf (STREAM, "\t.p2align\t%d\n", POWER);

#define GLOBAL_ASM_OP "\t.global\t"

/* Run-time Target Specification */

/* TODO: We Should include the requested minimum target in these definitions, but I am not sure how*/
#define TARGET_CPU_CPP_BUILTINS() \
  { \
    builtin_define ("__ETCA__");          \
  }


#endif //GCC_ETCA_H
