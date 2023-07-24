
(include "predicates.md")

;; -------------------------------------------------------------------------
;; Register Constants
;; -------------------------------------------------------------------------

(define_constants
[	(ETCA_R0	0)
	(ETCA_R1	1)
	(ETCA_R2	2)
	(ETCA_R3	3)
	(ETCA_R4	4)
	(ETCA_BP	5)   ; Hard Base/Frame Pointer
	(ETCA_SP	6)   ; Stack Pointer
	(ETCA_LN	7)   ; Link register
	(ETCA_R8	8)
	(ETCA_R9	9)
	(ETCA_R10	10)
	(ETCA_R11	11)
	(ETCA_R12	12)
	(ETCA_R13	13)
	(ETCA_R14	14)
	(ETCA_R15	15)
	(ETCA_ARG	16)  ; Argument Pointer, will always be eliminated
	(ETCA_VFP	17)  ; Virtual Frame Pointer, will always be eliminated
	(ETCA_PC	18)  ; Program Counter
	(ETCA_CC	19)  ; Condition Code registers, i.e. Flags
])


;; -------------------------------------------------------------------------
;; Mode Iterator
;; -------------------------------------------------------------------------

(define_mode_iterator SS [QI HI SI DI])
(define_mode_attr x [(QI "h") (HI "x") (SI "d") (DI "q")])


;; -------------------------------------------------------------------------
;; Nop instructions
;; -------------------------------------------------------------------------

(define_insn "nop"
  [(const_int 0)]
  ""
  "nop")

;; -------------------------------------------------------------------------
;; Stack Operations
;; -------------------------------------------------------------------------



(define_insn "push<mode>1"
  [(set (mem:SS (pre_dec:SS (reg:SS ETCA_SP)))
        (match_operand:SS 0 "etca_arithmetic_operand" "ri"))]
  ""
  "push<x>\t%<x>0")

(define_insn "pop<mode>1"
  [(set (mem:SS (post_inc:SS (reg:SS ETCA_SP)))
        (match_operand:SS 0 "etca_arithmetic_operand" "ri"))]
  ""
  "pop<x>\t%<x>0")

;; -------------------------------------------------------------------------
;; Move instructions
;; -------------------------------------------------------------------------


(define_insn "*mov<mode>"
 [(set (match_operand:SS 0 "nonimmediate_operand" "=rm,r")
	   (match_operand:SS 1 "etca_general_movsrc_operand" "r,mi"))]
  ""
  "mov\\t\\t%<x>0, %<x>1")

(define_expand "mov<mode>"
   [(set (match_operand:SS 0 "general_operand" "")
 	     (match_operand:SS 1 "general_operand" ""))]
   ""
  "
{
  /* If this is a store, force the value into a register.  */
  if (! (reload_in_progress || reload_completed))
  {
    if (MEM_P (operands[0]))
    {
      operands[1] = force_reg (<SS:MODE>mode, operands[1]);
      if (MEM_P (XEXP (operands[0], 0)))
        operands[0] = gen_rtx_MEM (<SS:MODE>mode, force_reg (<SS:MODE>mode, XEXP (operands[0], 0)));
    }
    else
      if (MEM_P (operands[1])
          && MEM_P (XEXP (operands[1], 0)))
        operands[1] = gen_rtx_MEM (<SS:MODE>mode, force_reg (<SS:MODE>mode, XEXP (operands[1], 0)));
  }
}")

;; -------------------------------------------------------------------------
;; Arithmetic instructions
;; -------------------------------------------------------------------------

(define_insn "add<mode>3"
  [(set (match_operand:SS 0 "register_operand" "=r")
	  (plus:SS
	   (match_operand:SS 1 "register_operand" "%0")
	   (match_operand:SS 2 "etca_arithmetic_operand" "ri")))]
  ""
  "add<x>\\t%<x>0, %<x>2"
)

(define_insn "sub<mode>3"
  [(set (match_operand:SS 0 "register_operand" "=r,r")
	  (minus:SS
	   (match_operand:SS 1 "etca_arithmetic_operand" "0,ri")
	   (match_operand:SS 2 "etca_arithmetic_operand" "ri,0")))]
  ""
  "@
  sub<x>\\t%<x>0, %<x>2
  rsub<x>\\t%<x>0, %<x>2"
)

(define_insn "ior<mode>3"
  [(set (match_operand:SS 0 "register_operand" "=r")
	  (ior:SS
	   (match_operand:SS 1 "register_operand" "%0")
	   (match_operand:SS 2 "etca_arithmetic_operand" "ri")))]
  ""
  "or<x>\\t%<x>0, %<x>2"
)

(define_insn "xor<mode>3"
  [(set (match_operand:SS 0 "register_operand" "=r")
	  (xor:SS
	   (match_operand:SS 1 "register_operand" "%0")
	   (match_operand:SS 2 "etca_arithmetic_operand" "ri")))]
  ""
  "xor<x>\\t%<x>0, %<x>2"
)

(define_insn "and<mode>3"
  [(set (match_operand:SS 0 "register_operand" "=r")
	  (and:SS
	   (match_operand:SS 1 "register_operand" "%0")
	   (match_operand:SS 2 "etca_arithmetic_operand" "ri")))]
  ""
  "and<x>\\t%<x>0, %<x>2"
)


;; -------------------------------------------------------------------------
;; Function body
;; -------------------------------------------------------------------------

(define_expand "prologue"
  [(const_int 0)]
  ""
  "
{
  etca_expand_prologue ();
  DONE;
}
")
(define_expand "epilogue"
  [(return)]
  ""
  "
{
  etca_expand_epilogue ();
  DONE;
}
")


(define_insn "etca_returner"
  [(return)]
  "reload_completed"
  "ret")


;; -------------------------------------------------------------------------
;; Jump instructions
;; -------------------------------------------------------------------------

(define_insn "indirect_jump"
 [(set (pc) (match_operand:HI 0 "nonimmediate_operand" "r"))]
  ""
  "jmp\\t\\t%0")

(define_expand "jump"
  [(set (pc)
	(label_ref (match_operand 0 "" "")))]
  "")

(define_insn "*jump"
  [(set (pc)
	(label_ref (match_operand 0 "" "")))]
  ""
  "jmp\\t\\t%l0")

(define_expand "call"
  [(parallel [(call (match_operand:QI 0 "memory_operand" "")
                    (match_operand 1 "general_operand" ""))
              (clobber (reg:SI ETCA_LN))])]
  ""
{
  gcc_assert (MEM_P (operands[0]));
})


(define_insn "*call"
  [(call (mem:QI (match_operand:HI
		  0 "nonmemory_operand" "i,r"))
	 (match_operand 1 "" ""))
   (clobber (reg:SI ETCA_LN))]
  ""
  "call\\t%0")

(define_expand "call_value"
  [(parallel [(set (match_operand 0 "" "")
                    (call (match_operand:QI 1 "memory_operand" "")
                     (match_operand 2 "" "")))
              (clobber (reg:SI ETCA_LN))])]
  ""
{
  gcc_assert (MEM_P (operands[1]));
})

(define_insn "*call_value"
  [(set (match_operand 0 "register_operand" "=r")
	(call (mem:QI (match_operand:HI
		       1 "immediate_operand" "i"))
	      (match_operand 2 "" "")))
   (clobber (reg:SI ETCA_LN))]
  ""
  "call\\t%1")


;; -------------------------------------------------------------------------
;; Conditions
;; -------------------------------------------------------------------------

(define_expand "cbranch<mode>4"
  [(set (reg:CC_NZCV ETCA_CC)
        (compare:CC_NZCV
         (match_operand:SS 1 "general_operand")
         (match_operand:SS 2 "general_operand")))
   (set (pc)
        (if_then_else (match_operator 0 "comparison_operator"
                       [(reg:CC_NZCV ETCA_CC) (const_int 0)])
                      (label_ref (match_operand 3))
                      (pc)))]
  ""
  "
  if (GET_CODE (operands[1]) != REG)
	operands[1] = force_reg (<MODE>mode, operands[1]);
  if (GET_CODE (operands[2]) != REG)
	operands[2] = force_reg (<MODE>mode, operands[2]);
  ")

(define_insn "*comp<mode>"
  [(set (reg:CC_NZCV ETCA_CC)
        (compare:CC_NZCV
         (match_operand:SS 0 "register_operand" "r")
         (match_operand:SS 1 "etca_arithmetic_operand" "ri")))]
  ""
  "comp<x>	%<x>0, %<x>1")



(define_code_iterator cond [ne eq lt ltu gt gtu ge le geu leu])
(define_code_attr asm_cond [(ne "ne") (eq "eq") (lt "lt") (ltu "b")
		      (gt "gt") (gtu "a") (ge "ge") (le "le")
		      (geu "ae") (leu "be") ])



(define_insn "*branch<code>"
  [(set (pc)
	(if_then_else (cond (reg:CC_NZCV ETCA_CC)
			            (const_int 0))
		          (match_operand 0 "" "")
		          (pc)))]
  ""
  "j<asm_cond>		%0")
