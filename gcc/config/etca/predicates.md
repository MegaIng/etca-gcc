(define_predicate "etca_general_movsrc_operand"
  (match_code "mem,const_int,reg,subreg,symbol_ref,label_ref,const")
{
  /* Any (MEM LABEL_REF) is OK.  That is a pc-relative load.  */
  if (MEM_P (op) && GET_CODE (XEXP (op, 0)) == LABEL_REF)
    return 1;

  if (MEM_P (op)
      && GET_CODE (XEXP (op, 0)) == PLUS
      && GET_CODE (XEXP (XEXP (op, 0), 0)) == REG
      && GET_CODE (XEXP (XEXP (op, 0), 1)) == CONST_INT
      && IN_RANGE (INTVAL (XEXP (XEXP (op, 0), 1)), -32768, 32767))
    return 1;

  return general_operand (op, mode);
})

(define_predicate "etca_arithmetic_operand_signed"
  (match_code "const_int,reg,subreg,const")
{
  if (CONST_INT_P (op)) {
  	return IN_RANGE(INTVAL(op), -16, 15);
  }
  return general_operand (op, mode);
})

(define_predicate "etca_arithmetic_operand_unsigned"
  (match_code "const_int,reg,subreg,const")
{
  if (CONST_INT_P (op)) {
  	return IN_RANGE(INTVAL(op), 0, 32);
  }
  return general_operand (op, mode);
})


(define_predicate "etca_and_immediate_operand"
  (match_code "const_int, reg")
{
  if (!CONST_INT_P (op))
    return 0;
  HOST_WIDE_INT val = INTVAL(op);

  if (GET_MODE_BITSIZE (mode) <= HOST_BITS_PER_INT
      && val == (HOST_WIDE_INT)GET_MODE_MASK(mode))
    return 1;

  if (IN_RANGE(val, -16, 15))
    return 1;

  if (REG_P(op)) {
    return 1;
  }

  return 0;
})

(define_predicate "etca_cmp_immediate_operand"
  (match_code "const_int, reg")
{
  if (CONST_INT_P (op)) {
  	return IN_RANGE(INTVAL(op), -16, 15);
  }
  if (REG_P(op)) {
    return 1;
  }
  return 0;
})