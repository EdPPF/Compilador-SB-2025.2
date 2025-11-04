M1:     MACRO   &x
        ADD     &x
        ENDMACRO
M2:     MACRO   &x, &y
        M1      &x
        M1      &y
        ENDMACRO
        M2      L1, L2
L1:     CONST   5
L2:     CONST   10