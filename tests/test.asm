; Teste este acho que tá nos slides
;ROTULO1: INPUT N1      ; Le um numero
;         COPY  N1, N2
;ROTULO2: STOP
;N1:      SPACE
;N2:      SPACE

M1: 
MACRO %x
ADD %x
K: SUB L
ENDMACRO
M2: 
MACRO %x
M1 %x
ENDMACRO
TREE: M1 A
STORE L1 + 3
B: CONST 10
A: CONST 5
L1: SPACE 5
COPY A, B