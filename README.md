# CPP-MIPS-Simulator
* MIPS simulator implemented with C++.
* Translates and simulates MIPS machine language.
* TO RUN: sim.exe x.obj
* Register changes after each instruction are stored in log.txt
* Final evaluation is printed

# MIPS Instruction Supported
* addiu
* addu
* and
* beq
* bne
* div
* j
* lw
* mfhi
* mflo
* mult
* or
* slt
* subu
* sw
* syscall

# MIPS Directives Supported
* .text
* .data
* .word (w1,...,wn)
* .space n

# Error Checking
* Verifies Opcode or function           "Invalid Opcode/Function Line: x"
* Verifies if the system call is valid  "Invalid Syscall"
* Checks division by 0                  "Cannot divide by 0 on PC x"
* Checks for valid jump address         "Invalid Jump address at PC x"
* Checks for valid branch address       "Invalid Branch address at PC x"
* Checks for valid data address         "Invalid data address at PC x"
* Checks for valid register             "Invalid register"
