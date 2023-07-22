
(include "predicates.md")

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
;; Jump instructions
;; -------------------------------------------------------------------------

(define_insn "indirect_jump"
 [(set (pc) (match_operand:HI 0 "nonimmediate_operand" "r"))]
  ""
  "jmp\\t%0")

(define_insn "jump"
  [(set (pc)
	(label_ref (match_operand 0 "" "")))]
  ""
  "jmp\\t%l0%#")

(define_expand "call"
  [(call (match_operand:QI 0 "memory_operand" "")
		(match_operand 1 "general_operand" ""))]
  ""
{
  gcc_assert (MEM_P (operands[0]));
})


(define_insn "*call"
  [(call (mem:QI (match_operand:HI
		  0 "nonmemory_operand" "i,r"))
	 (match_operand 1 "" ""))]
  ""
  "call\\t%0")

(define_expand "call_value"
  [(set (match_operand 0 "" "")
		(call (match_operand:QI 1 "memory_operand" "")
		 (match_operand 2 "" "")))]
  ""
{
  gcc_assert (MEM_P (operands[1]));
})

(define_insn "*call_value"
  [(set (match_operand 0 "register_operand" "=r")
	(call (mem:QI (match_operand:HI
		       1 "immediate_operand" "i"))
	      (match_operand 2 "" "")))]
  ""
  "call\\t%1")