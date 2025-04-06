; Broken Code Bananza

.entry This1Fails
; Label starts with a number - illegal

.extern Banana

mcro UmbraShift
    mov r3, r4
    add r1, r2
    cmp #8, r5
mcroend

UmbraShift

Gate99:
    prn &MissingLabel
    ; Relative addressing to an undefined symbol

    mov #3, #9
    ; Illegal: both operands are immediate

    stop #1
    ; stop cannot take an operand

    sub #6, Gate99

Fork:
    lea &UmbraShift, r6
    ; 'lea' cannot use relative addressing

    dec &Gate99
    ; 'dec' does not allow relative addressing

    jmp 123Invalid
    ; Jumping to an illegal label

.dataLine: .data 1, 2, banana
; 'banana' is not a numeric constant

.codez: .string apples
; Missing quotes around the string

UmbraShift: .string "shadow"
; Duplicate label - already used as a macro name

Zebra: .extern Orange
; Cannot define and extern the same label

.entry Zebra
; Redundant .entry for external label