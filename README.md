# Trab 1: Compilador (+Pré-Processador)

## Instalando ferramentas para desenvolvimento C++

Apenas siga o [tutorial da Microsoft](https://code.visualstudio.com/docs/cpp/config-mingw). Se estiver usando algo diferente do VSCode, é necessário procurar outro turorial.

## Especificação

Deve ser entregue o código fonte em C ou C++ sem uso de biblioteca externa, porém sem restrição a nenhum padrão especifico.
Deve ser entregue um arquivo separado TXT com nome do grupo ate 3 alunos e instruções de como rodar no GCC ou CODEBLOCKS em Linux ou Windows.
Todos os arquivos de entrada e saída devem estar na mesma pasta do arquivo executável.

Fazer um compilador que deve ser chamado para executar da seguinte forma: ./compilador arquivo.asm

O compilador deve dar três saídas: uma com o mesmo nome do arquivo mudando a extensão para .pre e as outras mudando a extensão para .o1 e .o2 respectivamente, todas em modo texto.

### 1. Pré-processamento

O compilador deve aceitar macros seguindo a sintaxe do exemplo abaixo (considere que terá no máximo DUAS macros por programa testado contra seu compilador).
Cada Macro pode receber no máximo 2 argumentos e uma macro pode chamar outra macro.
Não haverá checagem erros no pré-processamento nem uso de EQU e IF. Porém SPACE pode receber argumento o que significa que existe a possibilidade de linhas do LABEL+(numero).
A saída do após .pre deve apresentar o código após expandir as macros.
Não é necessário existir uma estrutura MNT e MDT, porém é recomendado que tenha.

Exemplo de Macro:

```
TROCA: MACRO
COPY A, TEMP
COPY B, A
COPY TEMP, B
ENDMACRO
. . . ; parte sem macros 2
TROCA
. . . ; parte sem macros 3
TROCA
```

Exemplo de Macro com parâmetros:

```
TROCA: MACRO &A, &B, &T
COPY &A, &T
COPY &B, &A
COPY &T, &B
ENDMACRO
. . .
TROCA X, Y, Z
. . .
TROCA R, S, T
```

### 2. Compilador

Fazer o algoritmo de *passagem única* com a lista de pendencias feita no próprio código.
Mostrar o código inteiro com as listas de pendencias SEM corrigir as pendencias na saída .o1.
A saída final do código compilado é no arquivo .o2.

A saída deve ser em uma única linha SEM enter e com espaços. Os espaços reservados com SPACE devem ser colocados como 0 (não xx).
Exemplo:

`5 9 8 12 2 12 3 12 1 12 14 0`

Deve ser capaz de ignorar qualquer espaço, enter ou tabulação desnecessária. Deve ser capaz de aceitar enter depois de um rotulo do tipo:

```
ROT:
Add N1
```

O compilador deve aceitar maiúsculas e minúsculas (case insensitive)e diretivas SPACE e CONST com argumentos.
Deve indicar erros, marcando a linha no arquivo .pre e o tipo (sintático, semântico ou léxico). Alista de erros é:

- Rotulo declarado duas vezes em lugares diferentes
- Dois rótulos na mesma linha
- Rotulo não declarado
- Instrução com número de parâmetros errado
- Instrução inexistente
- Erros léxicos (label não pode começar por número e o único caracter especial que pode ter é o “_”).

A sintaxe deve ser igual ao assembly inventado (por exemplo, usando MULT para multiplicação).

# Fluxo do Projeto:

O programa opera em três estágios principais, desde o código fonte em assembly até o código objeto final:

- Pré-processamento: O compilador lê o arquivo .asm e processa as definições de macro (MACRO/ENDMACRO).
As chamadas de macro no código são substituídas pelo código correspondente (com até dois argumentos).
O resultado é um novo arquivo de texto com a extensão .pre, que contém o código assembly "puro", com todas as macros já expandidas;

- Compilação (Passagem Única): O arquivo .pre é lido uma única vez para ser traduzido.
Durante esta passagem, o código é convertido para seu formato numérico. Quando um rótulo é utilizado antes de ser definido, ele é adicionado a uma "lista de pendências" interna.
O programa gera um arquivo de saída intermediário, com extensão .o1, que exibe o código traduzido junto com a lista de pendências, sem as correções;

> Erros: Durante a fase de compilação, o programa analisa o código do arquivo .pre em busca de erros léxicos, sintáticos ou semânticos. Se um erro for encontrado, sua linha e tipo são reportados ao usuário.

- Resolução de Pendências: Após a passagem única, o programa utiliza a tabela de símbolos completa para resolver as pendências, corrigindo os endereços no código objeto gerado.
O código objeto final é salvo no arquivo com extensão .o2.
Esta saída final é formatada em uma única linha, com os valores numéricos separados por espaços.

# Rodando Testes:

Compile o programa com um comando como:

`g++ .\src\compilador.cpp .\src\Tabelas.cpp -o compilador.exe -std=c++17 -Wall`

- `-Wall`: Habilita todos os warnings do compilador.

Execute o executável criado:

`.\compilador.exe .\tests\teste.asm`