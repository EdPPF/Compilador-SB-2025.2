        INPUT   Y
        LOAD    VAL
        ADD     Y
        STORE   Y + 2
        JMPP    MOD_B
L1:     STOP
VAL:    CONST   5
MOD_B:  OUTPUT  Y
        OUTPUT  VAL
        OUTPUT  Y + 2
        JMP     L1
Y:      SPACE   3
