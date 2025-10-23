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

void fixSpacing(string &line) {
    string newString;
    size_t i = line.size();
    size_t j = 0;

    bool usedSpace = false;
    
    while (i > 0 && isspace(line[i - 1])) {
        i--;
    }

    while (j < i && isspace(line[j])) {
        j++;
    }

    size_t labelPos = line.find(':');

    while (j < i) {
        char x = line[j];

        if (isspace(x) && (!usedSpace || (labelPos != string::npos && j < labelPos))) {
            newString += x;
            usedSpace = true;
        } else {
            if (!newString.empty() && x == ',' && newString.back() == ' ') {
                newString.pop_back();
            } else if (!newString.empty() && x == '+' && newString.back() != ' ') {
                newString += ' ';
            } else if (!newString.empty() && (newString.back() == ':' || newString.back() == ',' || newString.back() == '+') && !isspace(x)) {
                newString += ' ';
            }

            newString += x;
            usedSpace = false;
        }

        j++;
    }
    
    line = newString;
}

// Expansão de Macros
void findMacro(LinhaProcessada &parsedLine) {
    if (!macroTable.currentMacro.empty()) {
        if (parsedLine.instrucao == "ENDMACRO") {
            macroTable.currentMacro = string();
        } else {
            macroTable.table[macroTable.currentMacro].body.push_back(parsedLine);
        }
    } else {
        if (parsedLine.instrucao == "MACRO") {
            Macro macro = {parsedLine.operandos, vector<LinhaProcessada>()};

            macroTable.table[parsedLine.rotulo] = macro;
            macroTable.currentMacro = parsedLine.rotulo;
        } else {
            memFile.push_back(parsedLine);
        }
    }
}

void replaceArgs(LinhaProcessada &parsedLine) {
    Macro macro = macroTable.table[parsedLine.instrucao];
    
    for (LinhaProcessada macroLine : macro.body) {

        for (int i = 0; i < macroLine.operandos.size(); i++) {
            string &arg1 = macroLine.operandos[i];
            for (int j = 0; j < macro.args.size(); j++) {
                string arg2 = macro.args[j];

                if (arg1 == arg2) {
                    arg1 = parsedLine.operandos[j];
                }
            }
        }

        if (macroTable.table.find(macroLine.instrucao) != macroTable.table.end()) {
            replaceArgs(macroLine);
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
        findMacro(parsedLine);
    }
    
}

// Processamento Principal
void processLines() {
    string line; 
    string previousLine;

    bool previousNeedsBody = false;
    
    while (getline(arq, line)) {
        removeComment(line);
        fixSpacing(line);

        if (!line.empty()) {
            bool needsBody = line.back() == ':';
            bool hasLabel = needsBody || line.find(':') != string::npos;

            if (previousNeedsBody) {
                if (!hasLabel) {
                    line = previousLine + " " + line;
                } else if (!previousLine.empty()) {
                    extendMacros(previousLine);
                } 
            }

            if (!needsBody) {
                extendMacros(line);
            }
            
            previousNeedsBody = needsBody;
            previousLine = line;
        }
    }
}
