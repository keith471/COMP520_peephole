# README

## Set-up instructions
Modify `./joosc` to contain the absolute path to jasmin.jar.
Add `export PEEPDIR=path_to_this_file` to your `bash_profile`.

## To Run
`cd` into a folder containing java files.
Run `$PEEPDIR/joosc *.java` for un-optimized code or `$PEEPDIR/joosc -O *.java` for optimized code.
