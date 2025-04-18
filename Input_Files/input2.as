
.entry START
.extern X

; Macro for performing a series of operations
mcro MULTIPLY
    add r1, r1   
    add r1, r2
    cmp r1, r4
mcroend

MULTIPLY

START: mov r1, r5
    lea X, r7
    dec r7
    jmp &PROCESS
    prn #123
    sub r6, r1

PROCESS: mov r3, r2
    add r6, r7
    cmp r1, #4
    bne NEXT
    inc r2
    jmp &END

MULTIPLY

NEXT: mov r4, r1
    lea X, r3
    cmp r4, #-5
    bne &PROCESS
    dec r3

END: stop

; Data Section
NUMS: .data 15, -6, 33
VAL:  .data 7
    .data -1
    .string "hello"