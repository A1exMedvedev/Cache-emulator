sb x31, 0xFF0(x2)
sb x31, 0xFF0(x3) 
sb x31, 0xFF0(x4)   
sb x31, 0xFF0(x5)  
sb x31, 0xFF0(x6)
addi x8, x8, 0x001
bne x7 x8 -20
sb x31, 0xFF0(x6)
sb x31, 0xFF0(x6)
sb x31, 0xFF0(x6)
addi x8, x8, 0x001
addi x8, x8, 0x001
addi x8, x8, 0x001
addi x8, x8, 0x001
addi x8, x8, 0x001
addi x8, x8, 0x001
jal x16, 32
addi x8, x8, 0x001
addi x8, x8, 0x001
jal x16, 64
ebreak