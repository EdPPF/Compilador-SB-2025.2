#include "main.h"
#include "Parser.h"
#include "Pre_Processador.h"
#include <iostream>

using namespace std;

MacroTable macroTable;

/**
 * @brief Remove comentários (tudo após ';') de uma linha de código.
 */
void removeComment(string &line) {
    size_t cur = line.find(';');
    
    if (cur != std::string::npos) {
        line = line.substr(0, cur);
    }
}

/**
 * @brief Processa uma linha para identificar o início ou corpo de uma definição de macro.
 */
void findMacro(LinhaProcessada &parsedLine, string &line) {
    if (!macroTable.currentMacro.empty()) {
        if (parsedLine.instrucao == "ENDMACRO") {
            macroTable.currentMacro = string();
        } else {
            macroTable.table[macroTable.currentMacro].body.push_back(line);
        }
    } else {
        if (parsedLine.instrucao == "MACRO") {
            Macro macro = {parsedLine.operandos, vector<string>()};

            macroTable.table[parsedLine.rotulo] = macro;
            macroTable.currentMacro = parsedLine.rotulo;
        } else {
            memFile.push_back(line);
        }
    }
}

/**
 * @brief Substitui os argumentos de uma macro pelos argumentos reais fornecidos na chamada.
 */
void replaceArgs(LinhaProcessada &parsedLine) {
    Macro macro = macroTable.table[parsedLine.instrucao];
    
    for (size_t count = 0; count < macro.body.size(); count++) {
        string macroLine = macro.body[count];

        LinhaProcessada processedMacro = parseLinha(macroLine);

        for (size_t i = 0; i < processedMacro.operandos.size(); i++) {
            string &arg1 = processedMacro.operandos[i];
            for (size_t j = 0; j < macro.args.size(); j++) {
                string arg2 = macro.args[j];

                if (arg1 == arg2) {
                    size_t pos = macroLine.find(" " + arg1 + " ");
                    if (pos != string::npos) {
                        macroLine.replace(pos, arg1.length() + 2, " " + parsedLine.operandos[j] + " ");
                    }
                    pos = macroLine.find(" " + arg1);
                    if (pos != string::npos && pos == macroLine.length() - arg1.length() - 1) {
                        macroLine.replace(pos, arg1.length() + 1, " " + parsedLine.operandos[j]);
                    }
                    pos = macroLine.find(" " + arg1 + ",");
                    if (pos != string::npos) {
                        macroLine.replace(pos, arg1.length() + 2, " " + parsedLine.operandos[j] + ",");
                    }
                    pos = macroLine.find(" " + arg1 + "+");
                    if (pos != string::npos) {
                        macroLine.replace(pos, arg1.length() + 2, " " + parsedLine.operandos[j] + "+");
                    }
                }
            }
        }

        processedMacro = parseLinha(macroLine);

        if (macroTable.table.find(processedMacro.instrucao) != macroTable.table.end()) {
            replaceArgs(processedMacro);
            continue;
        }

        if (!parsedLine.rotulo.empty() && count == 0) {
            macroLine = parsedLine.rotulo + ": " + macroLine;
        }

        memFile.push_back(macroLine);
    }
}

/**
 * @brief Ponto de entrada para expansão de macros; decide se uma linha é uma chamada de macro ou uma definição.
 */
void extendMacros(string &line) {
    LinhaProcessada parsedLine = parseLinha(line);

    if (macroTable.table.find(parsedLine.instrucao) != macroTable.table.end() && macroTable.currentMacro.empty()) {
        replaceArgs(parsedLine);
    } else {
        findMacro(parsedLine, line);
    }
    
}

/**
 * @brief Lê o arquivo .asm linha a linha, removendo comentários e expandindo macros.
 */
void processLines() {
    string line; 

    while (getline(arq, line)) {
        removeComment(line);

        if (!line.empty()) {
            extendMacros(line);
        }
    }
}
