# Trab 1: Compilador (+Pré-Processador) - Eduardo Ferreira e Victor Fontes Cavalcante

## Instalando ferramentas para desenvolvimento C++

Apenas siga o [tutorial da Microsoft](https://code.visualstudio.com/docs/cpp/config-mingw). Se estiver usando algo diferente do VSCode, é necessário procurar outro tutorial.

## Especificação

Deve ser entregue o código fonte em C ou C++ sem uso de biblioteca externa, porém sem restrição a nenhum padrão especifico.
Deve ser entregue um arquivo separado .txt com nome do grupo de até 3 alunos e instruções de como rodar no GCC ou CODEBLOCKS em Linux ou Windows.
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

O compilador deve aceitar maiúsculas e minúsculas (case insensitive) e diretivas SPACE e CONST com argumentos.
Deve indicar erros, marcando a linha no arquivo .pre e o tipo (sintático, semântico ou léxico). A lista de erros é:

- Rotulo declarado duas vezes em lugares diferentes
- Dois rótulos na mesma linha
- Rotulo não declarado
- Instrução com número de parâmetros errado
- Instrução inexistente
- Erros léxicos (label não pode começar por número e o único caracter especial que pode ter é o “_”).

A sintaxe deve ser igual ao assembly inventado (por exemplo, usando MULT para multiplicação). NENHUM outro erro é tratado além dos especificados, logo erros na execução podem ocorrer se outros erros forem encontrados nos arquivos. No caso de detecção de um erro, ele é indicado na saída do terminal, os arquivos .o1 e .o2 são deletados, e a decodificação daquela instrução é interrompida, mas ela continua sendo analisada. Como dito, nenhum erro de pré-processamento é tratada, e o uso correto dos delimitadores ',' e '+' e dos inteiros para as instruções de INPUT/LOAD são inferidos.

# Compilando e Rodando

Para compilar o projeto, rode

```bash
g++ main.cpp Pre_Processador.cpp compilador.cpp Parser.cpp Tabelas.cpp -o compilador
```

dentro da pasta `/src`.

## Configurando Make no Windows

O projeto conta com um Makefile para facilitar a compilação opcionalmente.

No Windows, para ter acesso ao comando `make`, primeiro instale o [scoop](scoop.sh) (siga as instruções do site).
Depois, instale [o app make](https://scoop.sh/#/apps?q=make&id=c43ff861c0f1713336e5304d85334a29ffb86317) a partir do comando `scoop install main/make`.

Com isso, o comando ficará disponível, então basta rodar `make` a partir da raiz do projeto para compilar os arquivos em um executável.

## Executando Arquivos

Mova o .exe para onde desejar e, para executar o compilador, rode

```bash
./compilador.exe <caminho_do_arquivo>.asm
```

na mesma pasta do .exe, onde `<caminho_do_arquivo>` é o `path` para o arquivo .asm e onde os arquivos .pre, .o1 e .o2 serão salvos.