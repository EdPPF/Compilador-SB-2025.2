; Teste de formatação, case-insensitive e espaçamento 
; O compilador deve tratar 'load' e 'LOAD' da mesma forma.

        INPUT   val_in      ; Leitura
    load    val_in      ; Instrução em minúsculo
        ADD     const_um    ; Instrução com tabulação

    store   RESULTADO       ; Instrução com espaços extras
    
    OUTPUT      RESULTADO
    stop                ; Instrução em minúsculo

val_in:     space
const_um:   CONST   1
RESULTADO:  space

; Resultado esperado:
; .pre gerado sem erros
; .o1 gerado
; .o2 gerado (mostrando que o case insensitive funciona)