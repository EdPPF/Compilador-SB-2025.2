#include "main.h"
#include "Parser.h"
#include "Pre_Processador.h"
#include <iostream>

using namespace std;

MacroTable macroTable;

// Tratamento de linhas
void removeComment(string &line) {
    size_t cur = line.find(';');
    
    if (cur != std::string::npos) {
        line = line.substr(0, cur);
    }
}

// Expansão de Macros
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

void replaceArgs(LinhaProcessada &parsedLine) {
    Macro macro = macroTable.table[parsedLine.instrucao];
    
    for (string macroLine : macro.body) {

        LinhaProcessada processedMacro = parseLinha(macroLine);

        for (int i = 0; i < processedMacro.operandos.size(); i++) {
            string &arg1 = processedMacro.operandos[i];
            for (int j = 0; j < macro.args.size(); j++) {
                string arg2 = macro.args[j];

                if (arg1 == arg2) {
                    size_t pos = macroLine.find(" " + arg1 + " ");
                    if (pos != string::npos) {
                        macroLine.replace(pos, arg1.length() + 2, parsedLine.operandos[j]);
                    }
                    pos = macroLine.find(" " + arg1);
                    if (pos != string::npos && pos == macroLine.length() - arg1.length() - 1) {
                        macroLine.replace(pos, arg1.length() + 1, parsedLine.operandos[j]);
                    }
                    pos = macroLine.find(" " + arg1 + ",");
                    if (pos != string::npos) {
                        macroLine.replace(pos, arg1.length() + 2, parsedLine.operandos[j]);
                    }
                    pos = macroLine.find(" " + arg1 + "+");
                    if (pos != string::npos) {
                        macroLine.replace(pos, arg1.length() + 2, parsedLine.operandos[j]);
                    }
                }
            }
        }

        processedMacro = parseLinha(macroLine);

        if (macroTable.table.find(processedMacro.instrucao) != macroTable.table.end()) {
            replaceArgs(processedMacro);
            continue;
        }

        memFile.push_back(macroLine);
    }
}

void extendMacros(string &line) {
    LinhaProcessada parsedLine = parseLinha(line);

    if (macroTable.table.find(parsedLine.instrucao) != macroTable.table.end() && macroTable.currentMacro.empty()) {
        replaceArgs(parsedLine);
    } else {
        findMacro(parsedLine, line);
    }
    
}

// Processamento Principal
void processLines() {
    string line; 
    string previousLine;

    bool previousNeedsBody = false;
    
    while (getline(arq, line)) {
        removeComment(line);

        if (!line.empty()) {
            extendMacros(line);
        }
    }
}
