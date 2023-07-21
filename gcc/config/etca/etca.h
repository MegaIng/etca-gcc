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
#define PARM_BOUNDARY 32
#define STACK_BOUNDARY 8

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
   "?cc"            }

/* Registers */
#define ETCA_R0		0
#define ETCA_R1		1
#define ETCA_R2		2
#define ETCA_R3		3
#define ETCA_R4		4
#define ETCA_BP		5
#define ETCA_SP		6
#define ETCA_LN		7
#define ETCA_R8		8
#define ETCA_R9		9
#define ETCA_R10	10
#define ETCA_R11	11
#define ETCA_R12	12
#define ETCA_R13	13
#define ETCA_R14	14
#define ETCA_R15	15
#define ETCA_CC		16

#define FIRST_PSEUDO_REGISTER 17
#define FIXED_REGISTERS  /*  a0  a1  a2  s0 */   { 0, 0, 0, 0, \
                         /*  s1  bp  sp  ln */     0, 0, 1, 1, \
                         /*  t0  t1  t2  t3 */     0, 0, 0, 0, \
                         /*  t4  s2  s3  s4 */     0, 0, 0, 0, \
			 /*  cc             */     1          }

#define CALL_REALLY_USED_REGISTERS \
                         /*  a0  a1  a2  s0 */   { 1, 1, 1, 0, \
                         /*  s1  bp  sp  ln */     0, 0, 0, 0, \
                         /*  t0  t1  t2  t3 */     1, 1, 1, 1, \
                         /*  t4  s2  s3  s4 */     1, 0, 0, 0, \
			 /*  cc             */     0          }

/* Register Classes */

enum reg_class
{
    NO_REGS,
    GENERAL_REGS,
    CC_REGS,
    ALL_REGS,
    LIM_REG_CLASSES
};

#define N_REG_CLASSES (int) LIM_REG_CLASSES

#define REG_CLASS_NAMES {\
    "NO_REGS", \
    "GENERAL_REGS", \
    "CC_REGS", \
    "ALL_REGS" }

#define REG_CLASS_CONTENTS {\
    0x00000, \
    0x0FFFF,  /* GENERAL_REGS*/ \
    0x10000,  /* CC_REGS */ \
    0x1FFFF }

#define REGNO_REG_CLASS(R) ((R == ETCA_CC) ? CC_REGS : GENERAL_REGS)

#define BASE_REG_CLASS GENERAL_REGS
#define INDEX_REG_CLASS GENERAL_REGS

/* MISC */

#define MOVE_MAX 2

#define Pmode SImode

#define FUNCTION_MODE QImode



/* STACK AND CALLING */

#define STACK_GROWS_DOWNWARD 1

#define STACK_POINTER_REGNUM ETCA_SP
#define FRAME_POINTER_REGNUM ETCA_BP
#define ARG_POINTER_REGNUM FRAME_POINTER_REGNUM

#define FIRST_PARM_OFFSET(F) 12

#define FUNCTION_ARG_REGNO_P(regno) (regno <= ETCA_R2)


#define ELIMINABLE_REGS	{{ FIRST_PSEUDO_REGISTER, FIRST_PSEUDO_REGISTER }}
#define INITIAL_ELIMINATION_OFFSET(FROM, TO, OFFSET)	0


#define TRAMPOLINE_SIZE  (abort (), 0)

/* Passing Arguments in Registers */

/* A C type for declaring a variable that is used as the first
   argument of `FUNCTION_ARG' and other related values.  */
#define CUMULATIVE_ARGS unsigned int
#define INIT_CUMULATIVE_ARGS(CUM,FNTYPE,LIBNAME,FNDECL,N_NAMED_ARGS) \
  (CUM = ETCA_R0)

#define LIBCALL_VALUE(mode) (gen_rtx_REG (mode, ETCA_R0))

/* Addressing Modes */

#define MAX_REGS_PER_ADDRESS 2
#define CASE_VECTOR_MODE SImode

#define HARD_REGNO_OK_FOR_BASE_P(NUM) ((unsigned) (NUM) < ETCA_CC )

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
