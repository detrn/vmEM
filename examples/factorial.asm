INIT:

IRMOVW $5, %RAX		; as n 	
IRMOVW $1, %RCX		; as current factor 	
IRMOVW $1, %RDX		; result of the current factor added 
IRMOVW $1, %R8		; constant 1  
IRMOVW $0, %R9		; constant 0 
IRMOVW $65534, %RSP	; stack pointer initialized to be able to use ret and call 

LOOP:
    RRMOVW %RCX, %R10   ; verifies if we reached the end (current factor - n > 0) 
    SUBW %RAX, %R10
    JG FINISH
    RRMOVW %RDX, %RBX	; copy of rdx 
    CALL MULTIPLY	; multiply(rbx,r10) return in rsi 	
    RRMOVW %RSI, %RDX
    ADDW %R8, %RCX
    JMP LOOP

MULTIPLY:
    RRMOVW %RCX, %R11	;copy of rcx 
    RRMOVW %R9, %RSI	; rsi = 0 

MULTIPLY_LOOP:
    RRMOVW %R11, %R10	; r10 = rcx 
    SUBW %R9, %R10	; rcx - 0 
    JE MULTIPLY_DONE	; rcx == 0 goto done 

    ADDW %RBX, %RSI	; rsi += rbx   
    SUBW %R8, %R11	; rcx--
    JMP MULTIPLY_LOOP	

MULTIPLY_DONE:
    RET

FINISH:
    RRMOVW %RDX, %RAX
    HALT
