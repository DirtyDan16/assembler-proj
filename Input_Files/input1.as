; file ps.as
.entry LIST
.extern W

mcro APPLE
add    #2   ,   SUS
cmp #5,#-1
mcroend

APPLE

MAIN: add r3, LIST
LOOP: prn #48
 lea W, r6
 inc r6
 mov r3, K
 sub r1, r4
 bne END
 cmp K, #-6
 bne &END
 dec W
.entry MAIN
 jmp &LOOP
 add L3, L3
END: stop

APPLE

STR: .string "abcd"
LIST: .data 6, -9
 .data -100
K: .data 31
.extern L3 