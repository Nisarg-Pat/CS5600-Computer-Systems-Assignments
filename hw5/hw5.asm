 .globl main

.data
    prog_text: .asciiz "This program counts the number of occurences of each digit,\nand of each white space character, and each other character.\n\n"
    TEXT: .asciiz "TEXT:\n"
    text: .asciiz "This is a sample text.  The Khoury office is 201 WVH,\nand the university address is 360 Huntington Ave., Boston, MA 02115.\n"
    digit_string: .asciiz "digits ="
    space_string: .asciiz ", white space = "
    other_string: .asciiz ", other = "

    .align 2
    ndigit: .space 40
    nwhite: .word 0
    nother: .word 0
    c: .word 0
    i: .word 0

    null: .byte '\0'
    newline: .byte '\n'
    tab: .byte '\t'
    space: .byte ' '
    zero: .byte '0'
    nine: .byte '9'

.text
    main:
    # int c, i
        li $t0, 0       #t0 = i
        li $t1, 0       #t1 = c
        li $t2, 10      #end value of the first loop
        la $t3, 0       #off
        li $t4, 0

    #nwhite = nother = 0;
        sw $t0, nwhite
        sw $t0, nother

    #for (i = 0; i < 10; ++i)
    #    ndigit[i] = 0;
    loop_init:
        sw $t4, ndigit($t3)
        addi $t3, $t3, 4
        addi $t0, $t0, 1
        blt $t0, $t2, loop_init

    #i=0;
        li $t0, 0
        lb $t2, null
        la $t3, ndigit
        la $t5, text

    #while ((c = text[i++]) != '\0')
    loop_chars:
        lb $t6, ($t5)

        beq $t6, $t2, end_loop_chars

        #if (c >= '0' && c <= '9')
        lb $t9, zero
        sge $t7, $t6, $t9
        lb $t9, nine
        sle $t8, $t6, $t9
        and $t7, $t7, $t8
        beq $t7, 1, if

        # else if (c == ' ' || c == '\n' || c == '\t')
        lb $t9, space
        seq $t7, $t6, $t9
        lb $t9, newline
        seq $t8, $t6, $t9
        or $t7, $t7, $t8
        lb $t9, tab
        seq $t8, $t6, $t9
        or $t7, $t7, $t8
        beq $t7, 1, elseif
        j else

    if: #++ndigit[c-'0'];
        lb $t9, zero
        sub $t7, $t6, $t9
        la $t3, ndigit

        add $a0, $t7, 0
        li $a1, 4
        jal my_mult
        add $t3, $t3, $v0

        lw $t7, ($t3)
        add $t7, $t7, 1
        sw $t7, ($t3)

        j endif

    elseif: #++nwhite;
        la $t3, nwhite

        lw $t7, ($t3)
        add $t7, $t7, 1
        sw $t7, ($t3)

        j endif

    else: #++nother;
        la $t3, nother

        lw $t7, ($t3)
        add $t7, $t7, 1
        sw $t7, ($t3)

        j endif

    endif:
        addi $t5, $t5, 1
        j loop_chars

    end_loop_chars:

    #printf("This program counts the number of occurences of each digit,\n"
    #       "and of each white space character, and each other character.\n\n");
        la $a0, prog_text
        move $s0, $ra
        jal print_mystring
        move $ra, $s0

    #printf("TEXT:\n%s\n", text);
        la $a0, TEXT
        move $s0, $ra
        jal print_mystring
        move $ra, $s0

        la $a0, text
        move $s0, $ra
        jal print_mystring
        move $ra, $s0

        lb $a0, newline
        move $s0, $ra
        jal print_mychar
        move $ra, $s0

    #printf("digits =");
        la $a0, digit_string
        move $s0, $ra
        jal print_mystring
        move $ra, $s0

#for (i = 0; i < 10; ++i)
    # printf(" %d", ndigit[i]);
        la $t0, ndigit
        li $t1, 10

    print_digit_loop:
        lb $a0, space
        move $s0, $ra
        jal print_mychar
        move $ra, $s0

        lw $a0, ($t0)
        move $s0, $ra
        jal print_myint
        move $ra, $s0

        add $t0, $t0, 4
        sub $t1, $t1, 1
        bgt $t1, 0, print_digit_loop

    #printf(", white space = %d, other = %d\n", nwhite, nother);
        la $a0, space_string
        move $s0, $ra
        jal print_mystring
        move $ra, $s0

        la $t0, nwhite
        lw $a0, ($t0)
        move $s0, $ra
        jal print_myint
        move $ra, $s0

        la $a0, other_string
        move $s0, $ra
        jal print_mystring
        move $ra, $s0

	la $t0, nother
        lw $a0, ($t0)
        move $s0, $ra
        jal print_myint
        move $ra, $s0

        lb $a0, newline
        move $s0, $ra
        jal print_mychar
        move $ra, $s0

    #return 0;
        li $v0, 10
        li $a0, 0
        syscall

    print_mystring:
        # argument to print_str syscall already in $a0; Else do:  move $a0, ...
        li $v0, 4  # The print_str system call is number 4 in the table.
        syscall    # Some syscalls return result in $v0; print_str has no result
        li $v0, 0  # 0 means success; Set return value of print_mystring in $v0
        jr $ra

    print_mychar:
        # argument to print_str syscall already in $a0; Else do:  move $a0, ...
        li $v0, 11  # The print_char system call is number 11 in the table.
        syscall
        li $v0, 0
        jr $ra

    print_myint:
        # argument to print_str syscall already in $a0; Else do:  move $a0, ...
        li $v0, 1  # The print_int system call is number 1 in the table.
        syscall
        li $v0, 0
        jr $ra

    my_mult: #Function to multiply a1 * a2 and return into v0
        li $v0, 0
        beq $a1, 0, endmult
        multloop:
            add $v0, $v0, $a0
            sub $a1, $a1, 1
            bne $a1, 0, multloop

        endmult:
            jr $ra
