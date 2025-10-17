#include "Parser.h"
#include "Pre_processor.h"
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>

using namespace std;

// Tratamento de linhas
void removeSpaceLeft(string &line) {
    size_t i = 0;

    while (i < line.size() && isspace(line[i])) {
        i++;
    }

    line = line.substr(i);
}

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

void concatLines(const string &arq, vector<string> &tmp) {
    ifstream asmArq(arq);
    if (!asmArq.is_open()) {
        std::cerr << "Erro: Nao foi possivel abrir o arquivo .asm '" << arq << "'." << std::endl;
        return;
    }

    string line; 
    string previousLine;

    bool previousNeedsBody = false;
    
    while (getline(asmArq, line)) {
        removeComment(line);
        fixSpacing(line);

        if (!line.empty()) {
            bool needsBody = !line.empty() && line.back() == ':';
            bool hasLabel = needsBody || line.find(':') != std::string::npos;

            if (previousNeedsBody && !hasLabel) {
                previousLine += " " + line;
            } else {
                if (!previousLine.empty()) {
                    tmp.push_back(previousLine);
                }

                previousLine = line;
            }
            
            previousNeedsBody = needsBody;
        }
    }

    if (!previousLine.empty()) {
        tmp.push_back(previousLine);
    }
}

// Processamento de Macros
void findMacro(ofstream &file, string &line, LinhaProcessada &newLine, map<string, Macro> &macroTable, string &curMacro) {
    if (!curMacro.empty()) {
        if (line == "ENDMACRO") {
            curMacro = "";
        } else {
            macroTable[curMacro].body.push_back(line);
        }
    } else {
        bool flag = line.find(" MACRO ") != string::npos;
        if (!flag) {
            flag = line.size() >= 6 && line.compare(line.size() - 6, 6, " MACRO") == 0;
        }
        
        if (flag) {
            Macro macro = {newLine.rotulo, newLine.operandos, vector<string>()};

            macroTable[macro.label] = macro;
            curMacro = macro.label;
        } else {
            file << line << endl;
        }
    }
}

void replaceArgs(ofstream &file, string &label, vector<string> &args, map<string, Macro> &macroTable) {
    Macro macro = macroTable[label];

    for (string macroLine : macro.body) {

        LinhaProcessada newMacroLine = parseLinha(macroLine);

        for (int i = 0; i < newMacroLine.operandos.size(); i++) {
            string &arg1 = newMacroLine.operandos[i];
            for (int j = 0; j < macro.args.size(); j++) {
                string arg2 = macro.args[j];

                if (arg1 == arg2) {
                    arg1 = args[j];
                }
            }
        }

        if (macroTable.find(newMacroLine.instrucao) != macroTable.end()) {
            replaceArgs(file, newMacroLine.instrucao, newMacroLine.operandos, macroTable);
            continue;
        }

        if (!newMacroLine.rotulo.empty()) {
            file << newMacroLine.rotulo + ": ";
        }
        if (!newMacroLine.instrucao.empty()) {
            file << newMacroLine.instrucao;
        }
        for (string op : newMacroLine.operandos) {
            file << " " + op;
        }
        file << endl;
    }
}

void extendMacros(string &pre, vector<string> &tmp) {
    map<string, Macro> macroTable;

    ofstream preArq(pre);
    if (!preArq.is_open()) {
        std::cerr << "Erro: Nao foi possivel abrir o arquivo .pre '" << pre << "'." << std::endl;
        return;
    }

    string curMacro = "";
    
    for (string line : tmp) {
        LinhaProcessada newLine = parseLinha(line);

        if (macroTable.find(newLine.instrucao) != macroTable.end() && curMacro.empty()) {
            replaceArgs(preArq, newLine.instrucao, newLine.operandos, macroTable);
        } else {
            findMacro(preArq, line, newLine, macroTable, curMacro);
        }
    }
}
