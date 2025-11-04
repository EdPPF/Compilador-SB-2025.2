        INPUT   A
        INPUT   B
        JMP     MOD_B
L1:     OUTPUT  R + 1
        STOP    
A:      SPACE   1
B:      SPACE   1
MOD_B:  LOAD    A
        MULT    B
        STORE   R
        DIV     DOIS
        STORE   R + 1
        JMP     L1
R:      SPACE   2
DOIS:   CONST   2