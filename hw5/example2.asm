# int main() {
#   for (i = 0; i < 10; i++) {
#     printf("%d\n", i);
#   }
#   return 0;
# }

.globl main
.data
  newline: .asciiz "\n"
.text
main:
	li $t0, 0  # $t0 is i:  i = 0
	li $t1, 10
LOOP:
	move $a0, $t0  # Place argument for the print_int syscall is in $a0
	li $v0, 1      # The print_int system call is number 1 in the table.
	syscall

	la $a0, newline  # Place argument for the print_int syscall is in $a0
	li $v0, 4      # The print_str system call is number 4 in the table.
	syscall
	
	addi $t0, $t0, 1 # i++
	blt $t0, $t1, LOOP # if $t0 < $t1, goto LOOP

        li $v0, 10     # return 0:  system call 10 is exit()
        li $a0, 0      # setting return code of program to 0 (success)
	syscall	       # Equivalent to "return" statement in main.