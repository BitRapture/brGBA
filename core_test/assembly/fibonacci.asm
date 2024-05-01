MAX_ITERATIONS = 47

movs r0, MAX_ITERATIONS
beq _done

mov r1, #0
mov r2, #0
mov r3, #1

_fibonacci_loop:
    add r1, r2, r3
    mov r3, r2
    mov r2, r1

    subs r0, #1
    bne _fibonacci_loop

_done:
    mov r4, #0x200