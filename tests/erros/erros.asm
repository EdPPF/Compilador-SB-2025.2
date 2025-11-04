; Teste de vários erros
; O compilador deve reportar TODOS os erros abaixo no terminal e não parar no primeiro.

VALOR:      CONST   10

; Erro Léxico: Label não pode começar com número 
1INICIO:    CONST   1

; Erro Léxico: Label com char inválido (só '_' é permitido) 
TAXA$:      CONST   5

; Erro Sintático: Dois rótulos na linha 
LABEL1: LABEL2: CONST 2

; Erro Sintático: Instrução Inexistente 
            LOADS   VALOR

; Erro Sintático: Número errado de operandos (ADD espera 1) 
            ADD     VALOR, VALOR

; Erro Sintático: Número errado de operandos (STOP espera 0) 
            STOP    1

; Erro Semântico: Rótulo duplicado 
VALOR:      CONST   20

; Erro Semântico: Rótulo não declarado (será pego no fim) 
            LOAD    FANTASMA
            
            STOP

; Esperado:
; Console: O compilador deve rodar até o fim e indicar que a compilação falhou.
; .pre: Deve conter o código-fonte processado E uma lista de TODOS os 8 erros, cada um em uma linha.
; .o1: DELETADO (ou vazio).
; .o2: DELETADO (ou vazio).