#include "Parser.h"
#include "main.h"
#include "Tabelas.h"
#include <algorithm>
#include <filesystem>

using namespace std;

int addrCount = 0;
map<string, InfoSimbolo> symbolTable;
vector<pair<int, int>> memObj;

/**
 * @brief Escreve o código objeto não resolvido (com pendências) no arquivo .o1 e armazena no vetor memObj.
 */
void writeObject(LinhaProcessada &cmd, InfoInstrucao &info) {
    if (addrCount > 0) {
        o1 << " ";
    }

    // Diretivas
    if (cmd.instrucao == "CONST") {
        o1 << cmd.operandos[0];
        memObj.push_back(make_pair(stoi(cmd.operandos[0]), 0));
        addrCount++;
        return;
    }
    if (cmd.instrucao == "SPACE") {
        if (cmd.operandos.empty()) {
            o1 << 0;
            memObj.push_back(make_pair(0, 0));

            addrCount++;
            return;
        } else {
            o1 << 0;
            memObj.push_back(make_pair(0, 0));

            int count = stoi(cmd.operandos[0]);
            for (int i = 0; i < count - 1; i++) {
                o1 << " " << 0;
                memObj.push_back(make_pair(0, 0));
            }

            addrCount += count;
            return;
        }
    }

    // Instruções
    o1 << info.opcode;
    memObj.push_back(make_pair(info.opcode, 0));

    addrCount++;

    if (cmd.instrucao == "COPY" || cmd.operandos.size() == 1) {
        for (size_t i = 0; i < cmd.operandos.size(); i++){ 
            string op = cmd.operandos[i];

            if (symbolTable.find(op) == symbolTable.end()) {
                symbolTable[op] = {-1, false};
            }

            memObj.push_back(make_pair(symbolTable[op].endereco, 0));
            o1 << " " << symbolTable[op].endereco;

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
            memObj.push_back(make_pair(symbolTable[op1].endereco, stoi(op2)));
            o1 << " " << symbolTable[op1].endereco;
            symbolTable[op1] = {addrCount, false};  
        } else {
            memObj.push_back(make_pair(symbolTable[op1].endereco + stoi(op2), 0));
            o1 << " " << symbolTable[op1].endereco + stoi(op2);
        }

        addrCount++;
    }
}

/**
 * @brief Reporta um erro de compilação, grava a mensagem no arquivo .pre e sinaliza que um erro ocorreu.
 */
void callError(LinhaProcessada &cmd, int &lineCount, int errorID) {
    houveErroDeCompilacao = true; // Seta a flag de erro

    if (errorID == 0) {
        pre << endl << "Erro Semântico na linha (" + to_string(lineCount + 1) + "): Rótulo '" << cmd.rotulo << "' declarado duas vezes em lugares diferentes.";    
    } else if (errorID == 1) {
        pre << endl << "Erro Sintático na linha (" + to_string(lineCount + 1) + "): Dois rótulos na mesma linha.";    
    } else if (errorID == 2) {
        pre << endl << "Erro Sintático na linha (" + to_string(lineCount + 1) + "): Instrução '" << cmd.instrucao << "' com número de parâmetros errado.";    
    } else if (errorID == 3) {
        pre << endl << "Erro Sintático na linha (" + to_string(lineCount + 1) + "): Instrução '" << cmd.instrucao << "' inexistente.";    
    } else if (errorID == 4) {
        pre << endl << "Erro Léxico na linha (" + to_string(lineCount + 1) + "): Rótulo '" << cmd.rotulo << "' com caracteres inválidos.";    
    }
}

/**
 * @brief Valida uma instrução, seus operandos, e chama writeObject para gerar o código.
 */
int decodeInstruction(LinhaProcessada &cmd, string &line, int &lineCount) {
    if (cmd.instrucao.back() == ':') {
        callError(cmd, lineCount, 1);
        return 1; // Sinaliza erro, compile vai continuar para a próxima linha
    }
    if (tabelaInstrucoes.find(cmd.instrucao) == tabelaInstrucoes.end()) {
        callError(cmd, lineCount, 3);
        return 1;
    }

    InfoInstrucao info = tabelaInstrucoes[cmd.instrucao];
    if (find(info.operandos.begin(), info.operandos.end(), cmd.operandos.size()) == info.operandos.end()) {
        callError(cmd, lineCount, 2);
        return 1;
    }

    for (string op : cmd.operandos) {
        if (op.back() == ':') {
            callError(cmd, lineCount, 1);
            return 1;
        }
    }

    pre << line << endl;
    writeObject(cmd, info);
    return 0;
}

/**
 * @brief Verifica erros semânticos (rótulos pendentes) e escreve o código objeto final resolvido no arquivo .o2.
 */
int writeO2() {
    for (const auto& entry : symbolTable) {
        if (!entry.second.definido) {
            pre << endl << "Erro Semântico: Rótulo '" << entry.first << "' não declarado.";
            houveErroDeCompilacao = true; // Seta a flag de erro
        }
    }

    if (houveErroDeCompilacao) {
        return 1; // Indica que houve erro na compilação
    }

    // Escreve o arquivo .o2
    for (size_t i = 0; i < memObj.size(); i++) {
        int bin = memObj[i].first;
        if (i > 0) {
            o2 << " ";
        }
        o2 << bin;
    }

    return 0;
}

/**
 * @brief Executa o algoritmo de compilação de passagem única, populando a Tabela de Símbolos e resolvendo pendências.
 */
int compile() {
    int lineCount = 0;

    for (lineCount; lineCount < memFile.size(); lineCount++) {
        string line = memFile[lineCount];
        LinhaProcessada cmd = parseLinha(line);
        
        if (!cmd.rotulo.empty()) {
            if (isdigit(cmd.rotulo[0]) || any_of(cmd.rotulo.begin(), cmd.rotulo.end(), [](auto x) {return !isalnum(x) && x != '_';})) {
                callError(cmd, lineCount, 4);
                continue;
            }

            if (symbolTable.find(cmd.rotulo) != symbolTable.end()) {
                if (symbolTable[cmd.rotulo].definido) {
                    callError(cmd, lineCount, 0);
                    continue;
                } else {
                    int tmp;
                    for (int i = symbolTable[cmd.rotulo].endereco; i > 0; i = tmp) {
                        tmp = memObj[i].first;
                        memObj[i].first = addrCount + memObj[i].second;
                    }
                }
            } 

            symbolTable[cmd.rotulo] = {addrCount, true};
        }

        if (cmd.instrucao.empty()) {
            pre << line << endl;
        } else if (decodeInstruction(cmd, line, lineCount)) {
            continue;
        }
    }

    return writeO2();
}