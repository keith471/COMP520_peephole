/*
 * JOOS is Copyright (C) 1997 Laurie Hendren & Michael I. Schwartzbach
 *
 * Reproduction of all or part of this software is permitted for
 * educational or research use on condition that this copyright notice is
 * included in any copy. This software comes with no warranty of any
 * kind. In no event will the authors be liable for any damages resulting from
 * use of this software.
 *
 * email: hendren@cs.mcgill.ca, mis@brics.dk
 */

/*
 * HELPERS
 */

/*
 * Checks if the given load (returns false if not load) is the last
 * before another store of the same index. If there are any jump
 * operations return false since we cannot conclude they wont jump
 * somewhere that uses a load of the same index
 */
int is_last_value_load(CODE *c) {
  int loadInd, storeInd, testLoadInd, labelBuf;
  if (is_aload(c, &loadInd)) {
    /* a load stuff */
    CODE *cur = next(c);
    /* while cur isn't null and isn't an astore with same value */
    while(cur && !(is_astore(cur, &storeInd) && storeInd == loadInd)) {
      if (is_aload(cur, &testLoadInd) && testLoadInd == loadInd)
        return 0;
      if (uses_label(cur, &labelBuf))
        return 0;
      cur = next(cur);
    }
    /* If we didn't find a corresponding load then this must be last load */
    return 1;
  }
  else if (is_iload(c, &loadInd)) {
    /* i load stuff */
    CODE *cur = next(c);
    /* while cur isn't null and isn't an astore with same value */
    while(cur && !(is_istore(cur, &storeInd) && storeInd == loadInd)) {
      if (is_iload(cur, &testLoadInd) && testLoadInd == loadInd)
        return 0;
      if (uses_label(cur, &labelBuf))
        return 0;
      cur = next(cur);
    }
    /* If we didn't find a corresponding load then this must be last load */
    return 1;
  }
  else {
    return 0;
  }
}

/*
 * PATTERNS
 */

/* iload x        iload x        iload x
 * ldc 0          ldc 1          ldc 2
 * imul           imul           imul
 * ------>        ------>        ------>
 * ldc 0          iload x        iload x
 *                               dup
 *                               iadd
 */

int simplify_multiplication_right(CODE **c)
{ int x,k;
  if (is_iload(*c,&x) &&
      is_ldc_int(next(*c),&k) &&
      is_imul(next(next(*c)))) {
     if (k==0) return replace(c,3,makeCODEldc_int(0,NULL));
     else if (k==1) return replace(c,3,makeCODEiload(x,NULL));
     else if (k==2) return replace(c,3,makeCODEiload(x,
                                       makeCODEdup(
                                       makeCODEiadd(NULL))));
     return 0;
  }
  return 0;
}

/*
 * Might as well do the same as the above but for the left
 */
int simplify_multiplication_left(CODE **c) {
    int x, k;
    if (is_ldc_int(*c, &x) && is_iload(next(*c), &k) && is_imul(next(next(*c)))) {
        if (x == 0) return replace(c, 3, makeCODEldc_int(0, NULL));
        else if (x == 1) return replace(c, 3, makeCODEiload(k, NULL));
        else if (x == 2) return replace(c, 3, makeCODEiload(k, makeCODEdup(makeCODEiadd(NULL))));
        return 0;
    }
    return 0;
}

/*
 * Similar for addition
 * iload x
 * ldc 0
 * iadd
 * ------->
 * iload x
 */
int simplify_addition_right(CODE **c) {
    int x, k;
    if (is_iload(*c, &x) &&
            is_ldc_int(next(*c), &k) &&
            is_iadd(next(next(*c))) &&
            !is_dup(next(next(next(*c))))) {
        if (k == 0) return replace(c, 3, makeCODEiload(x, NULL));
        return 0;
    }
    return 0;
}

int simplify_addition_left(CODE **c) {
    int x, k;
    if (is_ldc_int(*c, &x) &&
            is_iload(next(*c), &k) &&
            is_iadd(next(next(*c)))) {
        if (x == 0) return replace(c, 3, makeCODEiload(k, NULL));
        return 0;
    }
    return 0;
}

/*
 * And subtraction
 * iload x
 * ldc 0
 * isub
 * ------->
 * iload x
 */
int simplify_subtraction_right(CODE **c) {
    int x, k;
    if (is_iload(*c, &x) &&
            is_ldc_int(next(*c), &k) &&
            is_isub(next(next(*c)))) {
        if (k == 0) return replace(c, 3, makeCODEiload(x, NULL));
        return 0;
    }
    return 0;
}

int simplify_subtraction_left(CODE **c) {
    int x, k;
    if (is_ldc_int(*c, &x) &&
            is_iload(next(*c), &k) &&
            is_isub(next(next(*c)))) {
        if (x == 0) return replace(c, 3, makeCODEiload(k, makeCODEineg(NULL)));
        return 0;
    }
    return 0;
}

/*
 * And division
 * iload x      iload x
 * ldc 1        ldc 2
 * idiv         idiv
 * ------->     ------->
 * iload x      iload x
 *              ldc 1
 *              ishl
 */
int simplify_division_right(CODE **c) {
    int x, k;
    if (is_iload(*c, &x) &&
            is_ldc_int(next(*c), &k) &&
            is_idiv(next(next(*c)))) {
        if (k == 1) return replace(c, 3, makeCODEiload(x, NULL));
        return 0;
    }
    return 0;
}

/*
 * ldc 0
 * iload x
 * idiv
 * ------->
 * iload x
 */
int simplify_division_left(CODE **c) {
    int x, k;
    if (is_ldc_int(*c, &x) &&
            is_iload(next(*c), &k) &&
            is_idiv(next(next(*c)))) {
        if (x == 0) return replace(c, 3, makeCODEiload(k, NULL));
        return 0;
    }
    return 0;
}

/*
 * And finally modulo
 * iload x
 * ldc 1
 * irem
 * ------->
 * ldc 0
 */
int simplify_modulo_right(CODE **c) {
    int x, k;
    if (is_iload(*c, &x) &&
            is_ldc_int(next(*c), &k) &&
            is_irem(next(next(*c)))) {
        if (k == 1) return replace(c, 3, makeCODEldc_int(0, NULL));
        return 0;
    }
    return 0;
}

/* dup
 * astore x
 * pop
 * -------->
 * astore x
 */
int simplify_astore(CODE **c)
{ int x;
  if (is_dup(*c) &&
      is_astore(next(*c),&x) &&
      is_pop(next(next(*c)))) {
     return replace(c,3,makeCODEastore(x,NULL));
  }
  return 0;
}

/* iload x
 * ldc k   (0<=k<=127)
 * iadd
 * istore x
 * --------->
 * iinc x k
 */
int positive_increment(CODE **c)
{ int x,y,k;
  if (is_iload(*c,&x) &&
      is_ldc_int(next(*c),&k) &&
      is_iadd(next(next(*c))) &&
      is_istore(next(next(next(*c))),&y) &&
      x==y && 0<=k && k<=127) {
     return replace(c,4,makeCODEiinc(x,k,NULL));
 } else if (is_ldc_int(*c,&x) &&
     is_iload(next(*c),&k) &&
     is_iadd(next(next(*c))) &&
     is_istore(next(next(next(*c))),&y) &&
     k==y && 0<=x && x<=127) {
    return replace(c,4,makeCODEiinc(k,x,NULL));
 }
  return 0;
}

/* goto L1
 * ...
 * L1:
 * goto L2
 * ...
 * L2:
 * --------->
 * goto L2
 * ...
 * L1:    (reference count reduced by 1)
 * goto L2
 * ...
 * L2:    (reference count increased by 1)
 */
int simplify_goto_goto(CODE **c)
{ int l1,l2;
  if (is_goto(*c,&l1) && is_goto(next(destination(l1)),&l2) && l1>l2) {
     droplabel(l1);
     copylabel(l2);
     return replace(c,1,makeCODEgoto(l2,NULL));
  }
  return 0;
}

/* dup
 * istore_x
 * pop
 ----------->
 * istore_x
 *
 * Clearly this is sound. The original duplicates something on the stack
 * then immediately throws it away. This removes the 2 unnecessary instructions.
 */

int simplify_istore(CODE **c) {
  int x;
  if (is_dup(*c) &&
      is_istore(next(*c), &x) &&
      is_pop(next(next(*c)))) {
    return replace(c, 3, makeCODEistore(x, NULL));
  }
  return 0;
}

/* iconst_0
 * goto L1
 * ...
 * L1:
 * ifeq L2
 * ...
 * L2:
 * ----------->
 * goto L2
 * ...
 * L1
 * ifeq L2
 * ...
 * L2:
 *
 * In the above case, we always branch from L1 to L2 because we put a 0 on
 * the stack right before go to L1. Thus its better to just branch to L2. We
 * don't remove the ifeq L2 line in L1 because it might be needed by other callers.
 */
int simplify_istore_0_double_branch(CODE **c) {
  int x, l1, l2;
  if (is_ldc_int(*c, &x) &&
      x == 0 &&
      is_goto(next(*c), &l1) &&
      is_ifeq(next(destination(l1)), &l2) &&
      l1 > l2) {
    droplabel(l1);
    copylabel(l2);
    return replace(c, 2, makeCODEgoto(l2, NULL));
  }
  return 0;
}

/* store_x
 * load_x
 * ...
 * no more load_x before another store_x
 * no labels jumping to load_x
 * no more jump operations before another store_x
 * (We could generalize this rule with more work to account for jumps
 * that don't violate the soundness but for now that's too complex)
 * --------->
 * remove store/load combo
 *
 * sound because store/load removes then adds back to the stack. Since
 * we no longer need to use that value and there's no jumps, there's nothing
 * that relies on this store, and therefore we can just remove it.
 */
int remove_superfluous_storeloads(CODE **c)
{
  int storeInd, loadInd;
  if (is_astore(*c, &storeInd) &&
      is_aload(next(*c), &loadInd) &&
      storeInd == loadInd &&
      is_last_value_load(next(*c))) {
    return replace(c, 2, NULL);
  }
  else if (is_istore(*c, &storeInd) &&
      is_iload(next(*c), &storeInd) &&
      storeInd == loadInd &&
      is_last_value_load(next(*c))) {
    return replace(c, 2, NULL);
  }
  return 0;
}

/* iconst_x | ldc x
 * imul
 * iconst_x | ldc x
 * idiv
 * --------------->
 * none
 *
 * additional conditions:
 * > lines 2,3,4 are not the target of a label
 *
 * this is sound because if you integer mul by the same number as you
 * divid by, it will just end up at the same number. However the reverse
 * is not true because integer div of 5/2 is 2 then 2 *2 = 4.
 */
int remove_pointless_mul_div(CODE **c) {
  int constantVal1, constantVal2;
  if (is_ldc_int(*c, &constantVal1) &&
      is_imul(next(*c)) &&
      is_ldc_int(next(next(*c)), &constantVal2) &&
      is_idiv(next(next(next(*c)))) &&
      constantVal1 == constantVal2) {
    return replace(c, 4, NULL);
  }
  return 0;
}

/* ldc x
 * ldc x
 * idiv
 * ------->
 * ldc 1

 * iload x
 * iload x
 * idiv
 * -------->
 * ldc 1
 *
 * Simply replaces instances of dividing a thing by itself by putting
 * a 1 on the stack.
 */
int remove_self_div(CODE **c) {
  int x, y;
  if(is_ldc_int(*c, &x) &&
     is_ldc_int(next(*c), &y) &&
     is_idiv(next(next(*c))) &&
     x == y) {
    return replace(c, 3, makeCODEldc_int(1, NULL));
  } else if (
     is_iload(*c, &x) &&
     is_iload(next(*c), &y) &&
     is_idiv(next(next(*c))) &&
     x == y) {
    return replace(c, 3, makeCODEldc_int(1, NULL));
  }
  return 0;
}

/* nop
 * ------->
 *
 * Remove pointlness nop's which sometimes sneak in.
 */
int remove_nop(CODE **c) {
  if(is_nop(*c)) {
    return replace(c, 1, NULL);
  }
  return 0;
}

/* iconst_x | ldc x
 * iadd
 * iconst_x | ldc x
 * isub
 * --------------->
 * none
 *
 * additional conditions:
 * > lines 2,3,4 are not the target of a label
 *
 * this is sound because if you integer add by the same number as you
 * sub by, it will just end up at the same number.
 */
int remove_pointless_add_sub(CODE **c) {
  int constantVal1, constantVal2;
  if (is_ldc_int(*c, &constantVal1) &&
      is_iadd(next(*c)) &&
      is_ldc_int(next(next(*c)), &constantVal2) &&
      is_isub(next(next(next(*c)))) &&
      constantVal1 == constantVal2) {
    return replace(c, 4, NULL);
  }
  return 0;
}

/* iconst_x | ldc x
 * isub
 * iconst_x | ldc x
 * iadd
 * --------------->
 * none
 *
 * additional conditions:
 * > lines 2,3,4 are not the target of a label
 *
 * this is sound because if you integer sub by the same number as you
 * add by, it will just end up at the same number.
 */
int remove_pointless_sub_add(CODE **c) {
  int constantVal1, constantVal2;
  if (is_ldc_int(*c, &constantVal1) &&
      is_isub(next(*c)) &&
      is_ldc_int(next(next(*c)), &constantVal2) &&
      is_iadd(next(next(next(*c)))) &&
      constantVal1 == constantVal2) {
    return replace(c, 4, NULL);
  }
  return 0;
}

/* any line that is the destination of a label
 * remove dead labels
 * sound for obvious reasons
 */
int remove_dead_label(CODE **c) {
  /* First test for iconst_1, label, ifeq label2
   * we can remove this soundly because if label is dead
   * then there is no possible way to jump from ifeq
   */
  int ldcVal, labelVal, temp;
  if (is_ldc_int(*c, &ldcVal) &&
      ldcVal == 1 &&
      is_label(next(*c), &labelVal) &&
      is_ifeq(next(next(*c)), &temp) &&
      deadlabel(labelVal)) {
    droplabel(temp);
    return replace(c, 3, NULL);
  }
  /* otherwise if it's simply a dead label we just delete it
   * sound because no patterns increase a dead labels count.
   */
  else if (is_label(*c, &labelVal) &&
    deadlabel(labelVal)) {
    return replace(c, 1, NULL);
  }
  return 0;
}

/*
 * goto l1
 * ...
 * l1:
 * l2:
 * ------->
 * goto l2
 * ...
 * l2
 */
int remove_unnecessary_label(CODE **c) {
    int l1, l2;
    if (is_goto(*c, &l1) && uniquelabel(l1) && is_label(next(destination(l1)), &l2) && l1 > l2) {
        droplabel(l1);
        copylabel(l2);
        return replace(c, 1, makeCODEgoto(l2, NULL));
    }
    return 0;
}

/*
 * helper for remove_unnecessary_label_traversal
 *
 * traverses through code of the following form, returning the label true_1:
 * true_3:
 * dup
 * ifne true_2
 * ...
 * true_2:
 * dup
 * ifne true_1
 * ...
 * true_1:
 * ifeq stop_0
 * optlabel_x:
 */
int get_true_label(int l) {
    int stopLabel;
    int retLabel;
    int nextLabel = l;
    while (is_dup(next(destination(nextLabel))) &&
            is_ifne(next(next(destination(nextLabel))), &nextLabel)) {
    }

    /* nextLabel is true_1 */
    if (is_ifeq(next(destination(nextLabel)), &stopLabel) && is_label(next(next(destination(nextLabel))), &retLabel)) {
        return retLabel;
    } else {
        return -1;
    }
}

/*
 *
 */
int remove_unnecessary_label_traversal(CODE **c) {
    int x;
    int l1, l2, l3;
    if ((is_if_icmpeq(*c, &l1) ||
            is_if_icmpne(*c, &l1) ||
            is_if_icmpge(*c, &l1) ||
            is_if_icmple(*c, &l1) ||
            is_if_icmpgt(*c, &l1) ||
            is_if_icmplt(*c, &l1)) &&
            is_ldc_int(next(destination(l1)), &x) &&
            x == 1 &&
            is_label(next(next(destination(l1))), &l2) &&
            is_dup(next(next(next(destination(l1))))) &&
            is_ifne(next(next(next(next(destination(l1))))), &l3)) {
        /* get the label to update this label with */
        int trueLabel = get_true_label(l3);
        if (trueLabel != -1) {
            if (is_if_icmpeq(*c, &l1)) {
                replace(c, 1, makeCODEif_icmpeq(trueLabel, NULL));
            } else if (is_if_icmpne(*c, &l1)) {
                replace(c, 1, makeCODEif_icmpne(trueLabel, NULL));
            } else if (is_if_icmpge(*c, &l1)) {
                replace(c, 1, makeCODEif_icmpge(trueLabel, NULL));
            } else if (is_if_icmple(*c, &l1)) {
                replace(c, 1, makeCODEif_icmple(trueLabel, NULL));
            } else if (is_if_icmpgt(*c, &l1)) {
                replace(c, 1, makeCODEif_icmpgt(trueLabel, NULL));
            } else {
                replace(c, 1, makeCODEif_icmplt(trueLabel, NULL));
            }
            copylabel(trueLabel);
            /* if l1 is unique, drop it and drop its following line iconst_1 */
            if (uniquelabel(l1)) {
                droplabel(l1);
                destination(l1)->next = next(destination(l1))->next;
            }
            return 1;
        }
    }
    return 0;
}

/*
 * iconst_0
 * dup
 * ifne true_3
 * pop
 */
int remove_useless_branch(CODE **c) {
    int x;
    int l;
    if (is_ldc_int(*c, &x) &&
            x == 0 &&
            is_dup(next(*c)) &&
            is_ifne(next(next(*c)), &l) &&
            is_pop(next(next(next(*c))))) {
        droplabel(l);
        return replace(c, 4, NULL);
    }
    return 0;
}

/*
 * if_icmpeq true_10
 * ...
 * true_10:
 * iconst_1
 * true_1:
 * ifeq stop_0
 * ----------------->
 * if_icmpeq true_x
 * ...
 * true_1:
 * ifeq stop_0
 * true_x:
 */
int simplify_end_of_conditional(CODE **c) {
    int x;
    int l1, l2, l3;
    if ((is_if_icmpeq(*c, &l1) ||
            is_if_icmpne(*c, &l1) ||
            is_if_icmpge(*c, &l1) ||
            is_if_icmple(*c, &l1) ||
            is_if_icmpgt(*c, &l1) ||
            is_if_icmplt(*c, &l1)) &&
            is_ldc_int(next(destination(l1)), &x) &&
            x == 1 &&
            is_label(next(next(destination(l1))), &l2) &&
            is_ifeq(next(next(next(destination(l1)))), &l3)) {
        /* make a new label */
        int nextLabel = next_label();
        CODE* newLabel = makeCODElabel(nextLabel, next(next(next(next(destination(l1))))));
        /* record the label in the labels table */
        INSERTnewlabel(nextLabel, "optlabel", newLabel, 1);
        /* have c point to the new label instead of true_10 */
        if (is_if_icmpeq(*c, &l1)) {
            replace(c, 1, makeCODEif_icmpeq(nextLabel, NULL));
        } else if (is_if_icmpne(*c, &l1)) {
            replace(c, 1, makeCODEif_icmpne(nextLabel, NULL));
        } else if (is_if_icmpge(*c, &l1)) {
            replace(c, 1, makeCODEif_icmpge(nextLabel, NULL));
        } else if (is_if_icmple(*c, &l1)) {
            replace(c, 1, makeCODEif_icmple(nextLabel, NULL));
        } else if (is_if_icmpgt(*c, &l1)) {
            replace(c, 1, makeCODEif_icmpgt(nextLabel, NULL));
        } else {
            replace(c, 1, makeCODEif_icmplt(nextLabel, NULL));
        }
        /* add the new label after ifeq stop_0 */
        next(next(next(destination(l1))))->next = newLabel;

        /* other optimizations...
         * if true_10 is unique, drop it and drop its following line iconst_1
         */
        if (uniquelabel(l1)) {
            droplabel(l1);
            destination(l1)->next = next(destination(l1))->next;
        }
        return 1;
    }
    return 0;
}

/*
 * after the above simplifications pertaining to conditionals, we might end up with
 * goto label
 * label:
 * In this case, we can clearly remove the goto
 */
int remove_unnecessary_goto(CODE **c) {
    int l1, l2;
    if (is_goto(*c, &l1) && is_label(next(*c), &l2) && l1 == l2) {
        return kill_line(c);
    }
    return 0;
}

void init_patterns(void) {
  ADD_PATTERN(simplify_multiplication_right);
  ADD_PATTERN(simplify_multiplication_left);
  ADD_PATTERN(simplify_addition_right);
  ADD_PATTERN(simplify_addition_left);
  ADD_PATTERN(simplify_subtraction_right);
  ADD_PATTERN(simplify_subtraction_left);
  ADD_PATTERN(simplify_division_right);
  ADD_PATTERN(simplify_division_left);
  ADD_PATTERN(simplify_modulo_right);
  ADD_PATTERN(simplify_astore);
  ADD_PATTERN(simplify_istore);
  ADD_PATTERN(positive_increment);
  ADD_PATTERN(simplify_goto_goto);
  ADD_PATTERN(simplify_istore_0_double_branch);
  ADD_PATTERN(remove_superfluous_storeloads);
  ADD_PATTERN(remove_pointless_mul_div);
  ADD_PATTERN(remove_pointless_add_sub);
  ADD_PATTERN(remove_pointless_sub_add);
  ADD_PATTERN(remove_dead_label);
  ADD_PATTERN(remove_self_div);
  ADD_PATTERN(remove_nop);
  ADD_PATTERN(remove_unnecessary_label);
  ADD_PATTERN(simplify_end_of_conditional);
  ADD_PATTERN(remove_unnecessary_label_traversal); /* must come after simplify_end_of_conditional */
  ADD_PATTERN(remove_unnecessary_goto);
  ADD_PATTERN(remove_useless_branch);
}
