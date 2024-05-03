WORK_RAM_ADDR = 0x2000000
MAX_DATA = 100
MAX_ADDR_SPACE = MAX_DATA * 4

mov r0, WORK_RAM_ADDR
mov r1, MAX_DATA

_load_into_memory:
    str r1, [r0], #4

    subs r1, #1
    bne _load_into_memory

mov r0, WORK_RAM_ADDR
b _bubble_sort

_swap:
    str r3, [r0, r1]
    str r4, [r0, r2]

_bubble_sort:
    mov r1, #4

    _loop_through_data:
        sub r2, r1, #4

        ldr r3, [r0, r2]
        ldr r4, [r0, r1]

        cmp r3, r4
        bhi _swap

        adds r1, #4
        cmp r1, MAX_ADDR_SPACE
        bne _loop_through_data
    
    b _done

_done:
    mov r7, #200