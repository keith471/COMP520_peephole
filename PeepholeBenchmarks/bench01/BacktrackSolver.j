.class public BacktrackSolver

.super SudokuSolver

.method public <init>()V
  .limit locals 1
  .limit stack 1
  aload_0
  invokenonvirtual SudokuSolver/<init>()V
  return
.end method

.method public getVal(II)I
  .limit locals 8
  .limit stack 2
  aload_0
  getfield SudokuSolver/grid Ljava/util/Vector;
  iload_1
  invokevirtual java/util/Vector/elementAt(I)Ljava/lang/Object;
  checkcast java/util/Vector
  iload_2
  invokevirtual java/util/Vector/elementAt(I)Ljava/lang/Object;
  checkcast java/lang/Integer
  invokevirtual java/lang/Integer/intValue()I
  ireturn
.end method

.method public setVal(III)V
  .limit locals 9
  .limit stack 3
  aload_0
  getfield SudokuSolver/grid Ljava/util/Vector;
  iload_1
  invokevirtual java/util/Vector/elementAt(I)Ljava/lang/Object;
  checkcast java/util/Vector
  astore 7
  new java/lang/Integer
  dup
  iload_3
  invokenonvirtual java/lang/Integer/<init>(I)V
  astore 6
  aload 7
  aload 6
  iload_2
  invokevirtual java/util/Vector/setElementAt(Ljava/lang/Object;I)V
  return
.end method

.method public checkRow(II)Z
  .limit locals 4
  .limit stack 3
  iconst_0
  istore_3
  start_0:
  iload_3
  ldc 9
  if_icmplt true_2
  goto stop_1
  true_2:
  aload_0
  iload_1
  iload_3
  invokevirtual BacktrackSolver/getVal(II)I
  iload_2
  if_icmpeq optlabel_7
  goto stop_4
  ifeq stop_4
  optlabel_7:
  iconst_0
  ireturn
  stop_4:
  iinc 3 1
  goto start_0
  stop_1:
  iconst_1
  ireturn
.end method

.method public checkCol(II)Z
  .limit locals 4
  .limit stack 3
  iconst_0
  istore_3
  start_0:
  iload_3
  ldc 9
  if_icmplt true_2
  goto stop_1
  true_2:
  aload_0
  iload_3
  iload_1
  invokevirtual BacktrackSolver/getVal(II)I
  iload_2
  if_icmpeq optlabel_7
  goto stop_4
  ifeq stop_4
  optlabel_7:
  iconst_0
  ireturn
  stop_4:
  iinc 3 1
  goto start_0
  stop_1:
  iconst_1
  ireturn
.end method

.method public checkBox(III)Z
  .limit locals 6
  .limit stack 4
  iload_1
  iconst_3
  idiv
  iconst_3
  imul
  istore_1
  iload_2
  iconst_3
  idiv
  iconst_3
  imul
  istore_2
  iconst_0
  istore 4
  start_0:
  iload 4
  iconst_3
  if_icmplt true_2
  goto stop_1
  true_2:
  iconst_0
  istore 5
  start_4:
  iload 5
  iconst_3
  if_icmplt true_6
  goto stop_5
  true_6:
  aload_0
  iload_1
  iload 4
  iadd
  iload_2
  iload 5
  iadd
  invokevirtual BacktrackSolver/getVal(II)I
  iload_3
  if_icmpeq optlabel_11
  goto stop_8
  ifeq stop_8
  optlabel_11:
  iconst_0
  ireturn
  stop_8:
  iinc 5 1
  goto start_4
  stop_5:
  iinc 4 1
  goto start_0
  stop_1:
  iconst_1
  ireturn
.end method

.method public checkFinish()Z
  .limit locals 3
  .limit stack 3
  iconst_0
  istore_1
  start_0:
  iload_1
  ldc 9
  if_icmplt true_2
  goto stop_1
  true_2:
  iconst_0
  istore_2
  start_4:
  iload_2
  ldc 9
  if_icmplt true_6
  goto stop_5
  true_6:
  aload_0
  iload_1
  iload_2
  invokevirtual BacktrackSolver/getVal(II)I
  iconst_0
  if_icmpeq optlabel_11
  goto stop_8
  ifeq stop_8
  optlabel_11:
  iconst_0
  ireturn
  stop_8:
  iinc 2 1
  goto start_4
  stop_5:
  iinc 1 1
  goto start_0
  stop_1:
  iconst_1
  ireturn
.end method

.method public solveCell(II)V
  .limit locals 4
  .limit stack 4
  aload_0
  invokevirtual BacktrackSolver/checkFinish()Z
  ifeq stop_0
  return
  stop_0:
  aload_0
  iload_1
  iload_2
  invokevirtual BacktrackSolver/getVal(II)I
  iconst_0
  if_icmpne true_3
  goto else_1
  true_3:
  aload_0
  iload_1
  iload_2
  invokevirtual BacktrackSolver/next(II)V
  goto stop_2
  else_1:
  iconst_1
  istore_3
  start_5:
  iload_3
  ldc 10
  if_icmplt true_7
  goto stop_6
  true_7:
  aload_0
  iload_1
  iload_3
  invokevirtual BacktrackSolver/checkRow(II)Z
  dup
  ifeq false_11
  pop
  aload_0
  iload_2
  iload_3
  invokevirtual BacktrackSolver/checkCol(II)Z
  false_11:
  dup
  ifeq false_10
  pop
  aload_0
  iload_1
  iload_2
  iload_3
  invokevirtual BacktrackSolver/checkBox(III)Z
  false_10:
  ifeq stop_9
  aload_0
  invokevirtual BacktrackSolver/checkFinish()Z
  ifeq true_13
  goto stop_12
  true_13:
  aload_0
  iload_1
  iload_2
  iload_3
  invokevirtual BacktrackSolver/setVal(III)V
  stop_12:
  aload_0
  iload_1
  iload_2
  invokevirtual BacktrackSolver/next(II)V
  stop_9:
  iinc 3 1
  goto start_5
  stop_6:
  aload_0
  invokevirtual BacktrackSolver/checkFinish()Z
  ifeq true_16
  goto stop_2
  true_16:
  aload_0
  iload_1
  iload_2
  iconst_0
  invokevirtual BacktrackSolver/setVal(III)V
  stop_2:
  return
.end method

.method public next(II)V
  .limit locals 3
  .limit stack 4
  iload_2
  ldc 8
  if_icmplt true_2
  goto else_0
  true_2:
  aload_0
  iload_1
  iload_2
  iconst_1
  iadd
  invokevirtual BacktrackSolver/solveCell(II)V
  goto stop_1
  else_0:
  aload_0
  iload_1
  iconst_1
  iadd
  iconst_0
  invokevirtual BacktrackSolver/solveCell(II)V
  stop_1:
  return
.end method

.method public solve()V
  .limit locals 1
  .limit stack 3
  aload_0
  iconst_0
  iconst_0
  invokevirtual BacktrackSolver/solveCell(II)V
  return
.end method

