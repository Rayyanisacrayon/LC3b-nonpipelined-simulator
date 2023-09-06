.orig x3000

    
    and r0, r0, #0
    add r0, r0, #10
    add r0, r0, #10

    ;give r1 c014
    lea r1 dest
    ldw r1 r1 #0
    ;give r2 c000
    lea r2 src
    ldw r2 r2 #0
    and r3 r3 #0

loop    ldb r4 r2 #0
    add r3 r3 r4

    add r2 r2 #1

    add r0 r0 #-1
    brp loop

    stw r3 r1 #0
    jmp r3

    halt

src .fill xC000
dest .fill xC014
.end