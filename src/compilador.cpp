#include "Parser.h"
#include "main.h"
#include "Tabelas.h"
#include <algorithm>
#include <filesystem>
#include <iostream>

using namespace std;

int lineCount = 0;
int addrCount = 0;
bool currentError = false;
map<string, InfoSimbolo> symbolTable;
map<int, vector<string>> errorList;

/** 
 * Índices: 0 - Valor/Pendências (.o2) | 1 - Indicador de Label (.o1) | 2 - Offset | 3 - Linha do .pre
 * 
 * Veja que, apesar de ser um vetor de inteiros, o ÚNICO índice que trata das pendências
 * é o '0' para que a solicitação de lista de pendência em código seja satisfeita. Os outros
 * índices são apenas para facilitar outros processos e podiam muito bem serem armazenados
 * em uma outra estrutura. 
 * 
 * Para que não hajam dúvidas: '1' tem o mesmo valor que '0' mas que não é alterado de acordo 
 *                                 com pendências. Usado no arquivo .o1.
 *                             '2' é o offset para instruções INPUT/LOAD que, originalmente, 
 *                                 seria armazenado no próprio arquivo .o2 para depois ser 
 *                                 somado.
 *                             '3' armazena a linha do arquivo .pre que o endereço indica. É
 *                                 usado para mostrar as linhas dos erros.
 */
vector<vector<int>> memObj;  

/**
 * @brief Reporta um erro de compilação, grava a mensagem no arquivo .pre e sinaliza que um erro ocorreu.
 */
void callError(LinhaProcessada &cmd, int errorID, string label=string()) {
    houveErroDeCompilacao = true; // Seta a flag de erro
    currentError = true;

    if (errorList.find(lineCount) == errorList.end()) {
        errorList[lineCount] = vector<string>();
    }

    if (errorID == 0) {
        errorList[lineCount].push_back("Erro Sintatico na linha (" + to_string(lineCount + 1) + "): Dois rotulos na mesma linha.");
    } else if (errorID == 1) {
        errorList[lineCount].push_back("Erro Lexico na linha (" + to_string(lineCount + 1) + "): Rotulo '" + label + "' com caracteres invalidos.");          
    } else if (errorID == 2) {
        errorList[lineCount].push_back("Erro Semantico na linha (" + to_string(lineCount + 1) + "): Rotulo '" + label + "' declarado duas vezes em lugares diferentes.");
    } else if (errorID == 3) {
        errorList[lineCount].push_back("Erro Sintatico na linha (" + to_string(lineCount + 1) + "): Instrucao '" + cmd.instrucao + "' inexistente."); 
    } else if (errorID == 4) {
        errorList[lineCount].push_back("Erro Sintatico na linha (" + to_string(lineCount + 1) + "): Instrucao '" + cmd.instrucao + "' com numero de parametros errado.");    
    }
}

/**
 * @brief Escreve o código objeto não resolvido (com pendências) no arquivo .o1 e armazena no vetor memObj.
 */
void decodeInstruction(LinhaProcessada &cmd, InfoInstrucao &info) {
    // Diretivas
    if (cmd.instrucao == "CONST") {
        memObj.push_back({stoi(cmd.operandos[0]), stoi(cmd.operandos[0]), 0, lineCount});
        addrCount++;

        return;
    }
    if (cmd.instrucao == "SPACE") {
        if (cmd.operandos.empty()) {
            memObj.push_back({0, 0, 0, lineCount});
            addrCount++;

            return;
        } else {
            int count = stoi(cmd.operandos[0]);

            for (int i = 0; i < count; i++) {
                memObj.push_back({0, 0, 0, lineCount});
            }

            addrCount += count;
            return;
        }
    }

    // Instruções
    memObj.push_back({info.opcode, info.opcode, 0, lineCount});
    addrCount++;

    if (cmd.instrucao == "COPY" || cmd.operandos.size() == 1) {
        for (string &op : cmd.operandos){ 

            if (symbolTable.find(op) == symbolTable.end()) {
                symbolTable[op] = {-1, false};
            }

            memObj.push_back({symbolTable[op].endereco, symbolTable[op].endereco, 0, lineCount});

            if (!symbolTable[op].definido) {
                symbolTable[op] = {addrCount, false};  
            }

            addrCount++;
        }
    } else if (cmd.instrucao == "STORE" || cmd.instrucao == "LOAD" || cmd.instrucao == "INPUT" || cmd.instrucao == "OUTPUT") {
        string op1 = cmd.operandos[0];
        string op2 = cmd.operandos[1];

        if (symbolTable.find(op1) == symbolTable.end()) {
            symbolTable[op1] = {-1, false};
        }

        if (!symbolTable[op1].definido) {
            memObj.push_back({symbolTable[op1].endereco, symbolTable[op1].endereco, stoi(op2), lineCount});
            symbolTable[op1] = {addrCount, false};  
        } else {
            memObj.push_back({symbolTable[op1].endereco + stoi(op2), symbolTable[op1].endereco + stoi(op2), 0, lineCount});
        }

        addrCount++;
    }
}

/**
 * @brief Valida uma instrução, seus operandos, e chama writeO1 para gerar o código.
 */
void checkInstruction(LinhaProcessada &cmd, string &line) {
    InfoInstrucao info;

    if (tabelaInstrucoes.find(cmd.instrucao) == tabelaInstrucoes.end()) {
        callError(cmd, 3);
    } else {
        info = tabelaInstrucoes[cmd.instrucao];

        if (find(info.operandos.begin(), info.operandos.end(), cmd.operandos.size()) == info.operandos.end()) {
            callError(cmd, 4);
        } else if (!currentError || !houveErroDeCompilacao){
            decodeInstruction(cmd, info);
        }
    }
}

/**
 * @brief Verifica erros semânticos (rótulos pendentes) e escreve o código objeto final resolvido no arquivo .o2.
 */
void writeObject() {
    for (const auto& entry : symbolTable) {
        if (!entry.second.definido) {

            for (int i = entry.second.endereco; i > 0; i = memObj[i][0]) {
                int x = memObj[i][3];

                if (errorList.find(x) == errorList.end()) {
                    errorList[x] = vector<string>();
                }

                errorList[x].push_back("Erro Semantico na linha (" + to_string(x + 1) + "): Rotulo '" + entry.first + "' nao declarado.");
            }
            houveErroDeCompilacao = true; // Seta a flag de erro
        }
    }

    // Escreve o arquivo .o2
    if (!houveErroDeCompilacao) {
        for (size_t i = 0; i < memObj.size(); i++) {
            if (i > 0) {
                o1 << " ";
                o2 << " ";
            }
            o1 << memObj[i][1];
            o2 << memObj[i][0];
        }
    } else {
        for(const auto& entry : errorList) {
            for (const auto& error : entry.second) {
                cout << endl << error;
            }
        }
    }
}

/**
 * @brief Executa o algoritmo de compilação de passagem única, populando a Tabela de Símbolos e resolvendo pendências.
 */
void compile() {
    for (lineCount; lineCount < memFile.size(); lineCount++) {
        string line = memFile[lineCount];
        LinhaProcessada cmd = parseLinha(line);

        currentError = false;

        pre << line << endl;

        if (cmd.rotulo.size() > 1) {
            callError(cmd, 0);
        }

        if (!cmd.rotulo.empty()) {
            for (auto &rotulo : cmd.rotulo) {
                bool labelError = false;

                if (isdigit(rotulo[0]) || any_of(rotulo.begin(), rotulo.end(), [](auto x) {return !isalnum(x) && x != '_';})) {
                    callError(cmd, 1, rotulo);
                    labelError = true;
                }

                if (symbolTable.find(rotulo) != symbolTable.end()) {
                    if (symbolTable[rotulo].definido) {
                        callError(cmd, 2, rotulo);
                        labelError = true;
                    } else {
                        int tmp;

                        for (int i = symbolTable[rotulo].endereco; i > 0; i = tmp) {
                            tmp = memObj[i][0];
                            memObj[i][0] = addrCount + memObj[i][2];
                        }
                    }
                } 

                if (!labelError) {
                    symbolTable[rotulo] = {addrCount, true};
                }
            }
        }

        if (!cmd.instrucao.empty()) {
            checkInstruction(cmd, line);
        }
    }

    writeObject();
}