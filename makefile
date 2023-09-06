default:
	gcc -std=c99 -g3 lc3bsim5.c -o simulate


# change "test.asm" to the name of your assembly file and "test.obj" to the name of the object file you want to create
run: default
	./simulate ucode5 pagetable.obj add.obj data.obj except_pagefault.obj except_prot.obj except_unaligned.obj except_unknown.obj int.obj vector_table.obj

clean:
	rm simulate