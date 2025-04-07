; file this is an input1 file!
.entry LIST
.extern W

mcro APPLE
add    #2   ,   W 
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

; i am a comment that divides the instructions with the data

STR: .string "abcd"
LIST: .data 6, -9
 .data -100
K: .data 31
.extern L3 