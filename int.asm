.orig x1200
    ;push
    stw r1, r6, #-1
    stw r2, r6, #-2
    stw r3, r6, #-3
    stw r4, r6, #-4


    ;clear regs
    and r1, r1, #0
    and r2, r2, #0
    and r3, r3, #0
    and r4, r4, #0

    ;r1 gets ptbr
    lea r1 ptbr
    ldw r1, r1, #0

    ;r2 gets value we or with
    lea r2 cmp
    ldw r2, r2, #0

    ;r3 gets our max value of page table
    lea r3 max
    ldw r3, r3, #0


    ;main loop
loop ldw r4, r1, #0
    and r4, r4, r2
    stw r4, r1, #0

    add r1, r1, #2
    add r3, r3, #-1
    brp loop


    ;pop
    ldw r4, r6, #-4
    ldw r3, r6, #-3
    ldw r2, r6, #-2
    ldw r1, r6, #-1

    rti


ptbr  .fill x1000
cmp   .fill xFFFE
max   .fill #128

.END
