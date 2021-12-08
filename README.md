# lc3-vm

Blog article:

https://www.andreinc.net/2021/12/01/writing-a-simple-vm-in-less-than-125-lines-of-c

A LC3 virtual machine implementation in a few lines of C code.

Code is experimental. Things are not final.

The machine should be able to run simple LC3 programs.

Compile the sample program:

`gcc -Wall -std=c11 simple_program.c`

Compile the `vm`:

`gcc -Wall -std=c11 vm.c vm_dbg.c`

Run the program:

`./a.out`


