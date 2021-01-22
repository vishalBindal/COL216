.data
mt: .word 0:9 # 0-O, 1-X, 2-Blank  
msg: .asciiz "***********\n Welcome to Tireless Tic Tac Toe\n ***********\n"
msg1: .asciiz "Player 1's name?\n"
msg2: .asciiz "Player 2's name?\n"
inprompt: .asciiz "Next turn: "
buffer1: .space 20
buffer2: .space 20
zero: .asciiz "O"
cross: .asciiz "X"
tab: .asciiz "\t"
blank: .asciiz " "
newline: .asciiz "\n"
vertical: .asciiz " | "
# horizontal: .asciiz "___________\n"
horizontal: .asciiz "-----------\n"
invinp: .asciiz "Invalid input\n"
winmsg: .asciiz "...And the winner is "
drawmsg: .asciiz "The game ends in a draw!\n"
newgame: .asciiz "Do you want a new game?(Yes - 1/No - 0)\n"
# s3-user1, s4-user2
.text
.globl main
.ent main
main:
    subu $sp, $sp, 4
    sw $ra, ($sp)
    li $v0, 4
    la $a0, msg
    syscall
    la $a0, msg1
    syscall
    li $v0, 8
    la $a0, buffer1
    li $a1, 20
    move $s3, $a0 
    syscall

    li $v0, 4
    la $a0, msg2
    syscall
    li $v0, 8
    la $a0, buffer2
    li $a1, 20
    move $s4, $a0
    syscall

    jal initialise


    #s0 - program counter
    li $s0, 0
    inploop:
        jal input
        bne $s0, 9, inploop

    jal draw

    lw $ra, ($sp)
    addu $sp, $sp, 4
    jr $ra
.end main

.globl initialise
.ent initialise

# We initialise the tic tac toe with blank in all indices
initialise:
    li $t0, 2 # 2 denotes blank
    li $t1, 0 # counter
    li $t2, 0 #address counter
    loopinit:
        sw $t0, mt($t2)
        addi $t1, $t1, 1
        addi $t2, $t2, 4
        bne $t1, 9, loopinit
    jr $ra
.end initialise

.globl print
.ent print

# s5-symbol, s6-counter, s7-address
print:
    subu $sp, $sp, 4
    sw $ra, ($sp)

    li $v0, 4
    li $s6, 0
    li $s7, 0
    loop:
        la $a0, tab
        syscall

        la $a0, blank
        syscall

        lw $s5, mt($s7)
        jal prints5
        
        la $a0, vertical
        syscall
        
        addi $s7, $s7, 4
        
        lw $s5, mt($s7)
        jal prints5
        
        la $a0, vertical
        syscall
        
        addi $s7, $s7, 4
        
        lw $s5, mt($s7)
        jal prints5
        
        addi $s7, $s7, 4 
        
        la $a0, newline
        syscall
        beq $s6, 2, afterprint
        la $a0, tab
        syscall

        la $a0, horizontal
        syscall
        
        addi $s6, $s6, 1
        bne $s6, 3, loop
    afterprint:
    lw $ra, ($sp)
    addu $sp, $sp, 4
    jr $ra
.end print

.globl prints5
.ent prints5
prints5:
    li $v0, 4
        bne $s5, 0, else1
        la $a0, zero
        syscall
        j after
    else1: 
        bne $s5, 1, else2
        la $a0, cross
        syscall
        j after
    else2:
        la $a0, blank
        syscall
    after:
        jr $ra
.end prints5

.globl input
.ent input
input:
    subu $sp, $sp, 4
    sw $ra, ($sp)

    user1:
    li $v0, 4
    la $a0, inprompt
    syscall

    move $a0, $s3 # contains name of user1
    syscall

    li $v0, 5
    syscall
    move $t3, $v0 # rows
    # checking valid values
    li $t7, 0
    blt $t3, $t7, invalid1
    li $t7, 2
    bgt $t3, $t7, invalid1

    li $v0, 5
    syscall
    move $t4, $v0 #columns
    # checking valid values
    li $t7, 0
    blt $t4, $t7, invalid1
    li $t7, 2
    bgt $t4, $t7, invalid1

    li $t5, 3
    mul $t3, $t5, $t3
    addu $t3, $t3, $t4
    move $s2, $t3
    li $t5, 4
    mul $t3, $t3, $t5 

    lw $t6, mt($t3) # $t3 stores req address
    li $t7, 2
    bne $t6, $t7, invalid1 #If our req index was not blank before
    li $t7, 0
    blt $t6, $t7, invalid1 #If number < 0 entered
    li $t7, 2
    bgt $t6, $t7, invalid1 #If number > 2 entered
    j user1after

    # For invalid input
    invalid1:
    li $v0, 4
    la $a0, invinp
    syscall
    j user1

    user1after:
    li $t5, 0
    sw $t5, mt($t3)

    jal print
    jal check

    addi $s0, $s0, 1
    bne $s0, 9, user2
    jal draw
    # user 2(X)
    user2:
    la $a0, inprompt
    syscall

    li $v0, 4
    move $a0, $s4
    syscall

    li $v0, 5
    syscall
    move $t3, $v0 # rows
    li $t7, 0
    blt $t3, $t7, invalid2
    li $t7, 2
    bgt $t3, $t7, invalid2
    
    li $v0, 5
    syscall
    move $t4, $v0 #columns
    li $t7, 0
    blt $t4, $t7, invalid2
    li $t7, 2
    bgt $t4, $t7, invalid2

    li $t5, 3
    mul $t3, $t5, $t3
    addu $t3, $t3, $t4
    move $s2, $t3
    li $t5, 4
    mul $t3, $t3, $t5 
    
    lw $t6, mt($t3)
    li $t7, 2
    bne $t6, $t7, invalid2
    
    j user2after
    invalid2:
    li $v0, 4
    la $a0, invinp
    syscall
    j user2

    user2after:
    li $t5, 1
    sw $t5, mt($t3)

    jal print
    jal check

    addi $s0, $s0, 1
    lw $ra, ($sp)
    addu $sp, $sp, 4
    jr $ra

#assert: $s2 contains last changed index
check:
#t0-row, t1-column, t2-counter, t3-3, t4-4, t5-value at addr, t6-addr, t7-last changed value
    li $t3, 3
    li $t4, 4 
    div $t0, $s2, $t3 # row
    rem $t1, $s2, $t3 # column
    
    mul $t6, $s2, $t4 #address
    lw $t7, mt($t6) # value at address $s2
    
    # row check
    li $t2, 0
    mul $t0, $t0, $t3
    looprow:
        mul $t6, $t0, $t4 #address
        lw $t5, mt($t6) # value at address
        bne $t5, $t7, endrow
        addi $t0, $t0, 1
        addi $t2, $t2, 1
        bne $t2, 3, looprow 
    jal winfun
   endrow:

   # column check
    li $t2, 0
    loopcol:
        mul $t6, $t1, $t4 #address
        lw $t5, mt($t6) # value at address
        bne $t5, $t7, endcol
        addi $t1, $t1, 3
        addi $t2, $t2, 1
        bne $t2, 3, loopcol
    jal winfun
    endcol:
    
    # diagonal check
    li $t0, 16 # index 4 check
    lw $t1, mt($t0)
    bne $t1, $t7, endcheck
    
    li $t0, 0 # index 0 check
    lw $t1, mt($t0)
    bne $t1, $t7, diagonal2

    li $t0, 32 # index 8 check
    lw $t1, mt($t0)
    bne $t1, $t7, diagonal2

    jal winfun
    diagonal2:
    li $t0, 8 # index 2 check
    lw $t1, mt($t0)
    bne $t1, $t7, endcheck

    li $t0, 24 # index 6 check
    lw $t1, mt($t0)
    bne $t1, $t7, endcheck

    jal winfun
    endcheck:
        jr $ra

.globl winfun
.ent winfun

#function called when a player wins
winfun:
    li $v0, 4
    la $a0, winmsg
    syscall

    li $t0, 0 # $t0 stores the symbol of user1
    li $t4, 4
    mul $s2, $s2, $t4 #$s2 now stores the address req
    lw $t7, mt($s2)

    bne $t7, $t0, user2wins
    move $a0, $s3
    syscall
    j exitall
    user2wins:
    move $a0, $s4
    syscall
 
    exitall:

    la $a0 , newgame
    syscall

    li $v0, 5
    syscall

    li $t0 , 0
    li $t1 , 1

    beq $v0 , $t1 , newg 
    #exit
    li $v0, 10
    syscall

    newg:
        jal main
    
.end winfun

# function called when game ends in a draw
.globl draw
.ent draw
draw:
    li $v0, 4
    la $a0, drawmsg
    syscall

    la $a0 , newgame
    syscall

    li $v0, 5
    syscall

    li $t0 , 0
    li $t1 , 1

    beq $v0 , $t1 , newg1 
    #exit
    li $v0, 10
    syscall

    newg1:
        jal main
.end draw