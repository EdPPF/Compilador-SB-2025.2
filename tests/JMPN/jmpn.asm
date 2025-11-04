; Teste específico para a instrução JMPN
        LOAD    VAL_5       ; ACC = 5
        SUB     VAL_10      ; ACC = 5 - 10 = -5
        
        JMPP    FALHA_P     ; Não deve pular (ACC é negativo)
        JMPN    SUCESSO_N   ; DEVE pular (ACC é negativo)
        JMP     FALHA_G     ; Não deve executar

FALHA_P:
        STOP                ; Ponto de falha 1
FALHA_G:
        STOP                ; Ponto de falha 2

SUCESSO_N:
        OUTPUT  VAL_5       ; Se chegou aqui, JMPN funcionou
        STOP

VAL_5:  CONST   5
VAL_10: CONST   10

; Resultado esperado: .pre, .o1 e .o2 gerados. O resultado do programa é imprimir 5.