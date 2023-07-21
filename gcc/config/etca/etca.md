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

;; SImode

(define_insn "movsi"
 [(set (match_operand:SI 0 "register_operand" "")
	   (match_operand:SI 1 "general_operand" ""))]
  ""
  "mov %0, %1")

;; -------------------------------------------------------------------------
;; Jump instructions
;; -------------------------------------------------------------------------

(define_insn "indirect_jump"
 [(set (pc) (match_operand:SI 0 "nonimmediate_operand" "r"))]
  ""
  "jmp %0")

(define_insn "jump"
  [(set (pc)
	(label_ref (match_operand 0 "" "")))]
  ""
  "jmp %l0%#")

