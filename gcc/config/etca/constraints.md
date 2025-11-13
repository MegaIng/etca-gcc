;; ETCA constraints
;; Define the immediate constraint used by ETCA machine descriptions.

(define_constraint "N"
  "A signed 5-bit immediate (range -16..15)."
  (and (match_code "const_int")
       (match_test "ival >= -16 && ival <= 15")))
