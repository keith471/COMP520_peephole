.class public Benchmark

.super java/lang/Object

.method public <init>()V
  .limit locals 1
  .limit stack 1
  aload_0
  invokenonvirtual java/lang/Object/<init>()V
  return
.end method

.method public static main([Ljava/lang/String;)V
  .limit locals 4
  .limit stack 2
  new joos/lib/JoosIO
  dup
  invokenonvirtual joos/lib/JoosIO/<init>()V
  astore_1
  aload_1
  invokevirtual joos/lib/JoosIO/readLine()Ljava/lang/String;
  astore_2
  aload_2
  ldc "backtrack"
  if_acmpeq true_2
  goto else_0
  true_2:
  new BacktrackSolver
  dup
  invokenonvirtual BacktrackSolver/<init>()V
  astore_3
  goto stop_1
  else_0:
  aload_2
  ldc "bruteforce"
  if_acmpeq true_6
  goto else_4
  true_6:
  new BacktrackSolver
  dup
  invokenonvirtual BacktrackSolver/<init>()V
  astore_3
  goto stop_1
  else_4:
  new BacktrackSolver
  dup
  invokenonvirtual BacktrackSolver/<init>()V
  astore_3
  stop_1:
  aload_3
  aload_1
  invokevirtual SudokuSolver/parse(Ljoos/lib/JoosIO;)V
  aload_3
  invokevirtual SudokuSolver/solve()V
  aload_3
  aload_1
  invokevirtual SudokuSolver/print(Ljoos/lib/JoosIO;)V
  return
.end method

