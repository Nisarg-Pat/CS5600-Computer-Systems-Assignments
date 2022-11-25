# int rc = 0;
# int main() {
#   rc = printf("Hello World!");
#   return 0;
# }

.globl main
.data
  rc: .word 0   # Global variable:  int rc = 0;
  str: .word str_data # Global:  char **str = &str_data; (not used)
  str_data: .asciiz "Hello World!" # Global:  char *str_data = "Hello World!";
.text
main:
	la $a0, str_data
	move $s0, $ra # Operating system put return addr of main in $ra
		      # Save it, or jal print_mystring will step on it
	jal print_mystring  # Call print_mystring(str_data)
	move $ra, $s0 # Restore the $ra that was put there by operating system
        sw $v0, rc($zero) # $v0 is return value of print_mystring; store in rc

        li $v0, 10     # system call 10 is exit()
        li $a0, 0      # setting return code of program to 0 (success)
	syscall	       # Equivalent to "return 0;" statement in main.
    
print_mystring:
	# argument to print_str syscall already in $a0; Else do:  move $a0, ...
	li $v0, 4  # The print_str system call is number 4 in the table.
	syscall    # Some syscalls return result in $v0; print_str has no result
	li $v0, 0  # 0 means success; Set return value of print_mystring in $v0
	jr $ra
