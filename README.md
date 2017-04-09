# README

## Set-up instructions
`cd` into JOOSA-src and compile joos for your system using `make all`.  
Modify `./joosc` to contain the absolute path to jasmin.jar.  
Add `export PEEPDIR=path_to_this_directory` to your `bash_profile`.

## To Run
`cd` into a folder containing java files, e.g. `cd PeepholeBenchmarks/bench01`.  
Run `make all` for un-optimized code or `make opt` for optimized code.
