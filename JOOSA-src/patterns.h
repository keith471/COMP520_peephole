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
 * Checks if this code is jumped to by any labels
 */
int is_label_destination(CODE *c) {
  int i;
  for (i = 0; i < currentlabelstablesize; i++) {
    if (currentlabels[i].position == c)
      return 1;
  }
  return 0;
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
      is_ifeq(next(destination(l1)), &l2)) {
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
      !is_label_destination(next(*c)) &&
      storeInd == loadInd &&
      is_last_value_load(next(*c))) {
    return replace(c, 2, NULL);
  }
  else if (is_istore(*c, &storeInd) &&
      is_iload(next(*c), &storeInd) &&
      !is_label_destination(next(*c)) &&
      storeInd == loadInd &&
      is_last_value_load(next(*c))) {
    return replace(c, 2, NULL);
  }
  return 0;
}

void init_patterns(void) {
  ADD_PATTERN(simplify_multiplication_right);
  ADD_PATTERN(simplify_astore);
  ADD_PATTERN(positive_increment);
  ADD_PATTERN(simplify_goto_goto);
  ADD_PATTERN(simplify_istore);
  ADD_PATTERN(simplify_istore_0_double_branch);
  ADD_PATTERN(remove_superfluous_storeloads);
}

