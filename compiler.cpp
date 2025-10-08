#include <iostream>
#include <fstream>
#include <map>
#include <utility>
#include <string>

using namespace std;

void removeComment(string &line) {
    size_t cur = line.find(';');
    
    if (cur != string::npos) {
        line = line.substr(0, cur);
    }
}

void removeSpaceBefore(string &line) {
    size_t i = 0;
    
    while (i < line.size() && isspace(line[i])) {
        i++;
    }
    
    line = line.substr(i);
}

void removeSpaceAfter(string &line) {
    size_t i = line.size();
    
    while (i > 0 && isspace(line[i - 1])) {
        i--;
    }
    
    line = line.substr(0, i);
}

void removeSpaceBetween(string &line) {
    string newString = "";
    bool usedSpace = false;

    for (char x : line) {
        if (isspace(x)) {
            if (!usedSpace) {
                newString += ' ';
                usedSpace = true;
            }
        } else {
            newString += x;
            usedSpace = false;
        }
    }

    line = newString;
}

void processLines(ifstream &file, fstream &tmp) {
    string line;
    string previousLine;
    bool previousNeedsBody = false;
    
    while (getline(file, line)) {
        removeComment(line);
        removeSpaceBefore(line);
        removeSpaceAfter(line);
        removeSpaceBetween(line);

        if (!line.empty()) {
            bool hasLabel = line.find(':') != string::npos;
            bool needsBody = line.back() == ':';

            if (previousNeedsBody && !hasLabel) {
                previousLine += " " + line;
                line = previousLine;
            }

            if (previousNeedsBody && hasLabel) {
                tmp << previousLine << endl;
            }

            if (!needsBody) {
                tmp << line << endl;
            }

            previousLine = line;
            previousNeedsBody = needsBody;
        }
    }
}

bool findMacro(string &line, string macro[]) {
    size_t cur = line.find("MACRO");
    
    if (cur != string::npos) {
        macro[0] = line.substr(0, line.find(':'));
        return true;
    }
    
    return false;
}

bool hasMacroStart(const string &line) {
    return line.find("MACRO") != string::npos;
}

bool hasMacroEnd(string &line) {
    return line.find("ENDMACRO") != string::npos;
}

void addToMacro(string &line, string macro[]) {
    int i = 0;
    
    while (!macro[i].empty()) {
        i++;
    }
    
    macro[i] = line;
}

void processMacros(fstream &tmp, string firstMacro[], string secondMacro[]) {
    string line;
    bool result = false;
    bool num = false;

    while (getline(tmp, line)) {
        if (result) {
            if (!num) {
                if (!hasMacroEnd(line)) {
                    addToMacro(line, firstMacro);
                } else {
                    num = 1;
                    result = false;
                }
            } else {
                if (!hasMacroEnd(line)) {
                    addToMacro(line, secondMacro);
                } else {
                    result = false;
                }
            }
        } else {
            if (!num) {
                result = findMacro(line, firstMacro);
            } else {
                result = findMacro(line, secondMacro);
            }
        }
    }
}

void extendMacros(fstream &tmp1, fstream &tmp2, string firstMacro[], string secondMacro[]) {
    string line;
    
    while (getline(tmp1, line)) {
        if (!firstMacro[0].empty() && line.find(firstMacro[0]) != string::npos && line.find("MACRO") == string::npos) {
            for (int i = 1; i < 32 && !firstMacro[i].empty(); i++) {
                tmp2 << firstMacro[i] << endl;
            }
            
        } else if (!secondMacro[0].empty() && line.find(secondMacro[0]) != string::npos && line.find("MACRO") == string::npos) {
            for (int i = 1; i < 32 && !secondMacro[i].empty(); i++) {
                tmp2 << secondMacro[i] << endl;
            }
            
        } else {
            tmp2 << line << endl;
        }
    }
}

void removeMacros(fstream &tmp1, fstream &tmp2) {
    string line;
    bool result = true;
    
    while (getline(tmp1, line)) {
        if (result) {
            if (hasMacroEnd(line)) {
                result = true;
            } else if (hasMacroStart(line)) {
                result = false;
            } else {
                tmp2 << line << endl;
            }
        } else {
            if (hasMacroEnd(line)) {
                result = true;
            }
        }
    }
}

int argumentsNum(string &line) {
    size_t i = 0;
    int count = 1;
    
    for (size_t i = 0; i < line.size(); i++) {
        if (isspace(line[i])) {
            count++;
        }
    }
    
    return count;
}

string getArgument(string &line, int num) {
    string label = "";
    
    for (size_t i = 0; i < line.size() && num >= 0; i++) {
        if (isspace(line[i])) {
            num--;
        } else if (num == 0) {
            label += line[i];
        }
    }   
    
    return label;
}

void printBinary(string &line, fstream &o1, map<string, pair<int, bool>> &labels, int &addr, int binary[], string opCode, int numArgs) {
    if (addr > 0) {
        o1 << " ";
    }
    
    if (numArgs == 1) {
        o1 << opCode;
        binary[addr] = 14;
        
    } else if (numArgs == 2) {    
        string label = getArgument(line, 1);

        if (!labels.count(label)) {
            labels[label] = make_pair(-1, false);
        }
        
        o1 << opCode << " " << labels[label].first;
        binary[addr] = stoi(opCode);
        binary[addr + 1] = labels[label].first;
            
        if (!labels[label].second) {
            labels[label] = make_pair(addr + 1, false);
        }
    } else if (numArgs == 3) {
        string label1 = getArgument(line, 1);
        string label2 = getArgument(line, 2);
        
        if (!labels.count(label1)) {
            labels[label1] = make_pair(-1, false);
        }
        if (!labels.count(label2)) {
            labels[label2] = make_pair(-1, false);
        }
        
        o1 << opCode << " " << labels[label1].first << " " << labels[label2].first;
        binary[addr] = stoi(opCode);
        binary[addr + 1] = labels[label1].first;
        binary[addr + 2] = labels[label2].first;
            
        if (!labels[label1].second) {
            labels[label1] = make_pair(addr + 1, false);
        }
        if (!labels[label2].second) {
            labels[label2] = make_pair(addr + 2, false);
        }
    }
}

void printError(fstream &pre, fstream &o1, int &addr, int binary[], string opCode, string cmd) {
    if (addr > 0) {
        o1 << " ";
    }
    
    o1 << opCode << " " << "-1";
    binary[addr] = stoi(opCode);
    binary[addr + 1] = -1;
    
    pre << "\tErro Sintático: Número errado de argumentos para " << cmd << "." << endl;
}

void printSpace(string &line, fstream &o1, int &addr, int binary[], int numArgs) {
    if (addr > 0) {
        o1 << " ";
    }
    
    if(numArgs == 1) {
        o1 << "0";
        binary[addr] = 0;
        
        addr += 1;
    } else if (numArgs == 2) {
        string con = getArgument(line, 1);

        for (int i = 0; i < stoi(con); i++) {
            o1 << "0";
            binary[addr + i] = 0;

            if (i < stoi(con) - 1) {
                o1 << " ";
            }
        }
        
        addr += stoi(con);
    }
}

void printErrorDirective(fstream &pre, string cmd) {
    pre << "\tErro Sintático: Número errado de argumentos para " << cmd << "." << endl;
}

void printConst(string &line, fstream &o1, int &addr, int binary[]) {
    if (addr > 0) {
        o1 << " ";
    }
    
    string con = getArgument(line, 1);

    o1 << con;
    binary[addr] = stoi(con);
}

void getInstructions(string line, fstream &o1, fstream &pre, map<string, pair<int, bool>> &labels, int &addr, int binary[], bool overLabel) {
    int numArguments = argumentsNum(line);
    int labelPos = line.find(':');
    
    if (!overLabel) {
       pre << line << endl; 
    }
    
    if (labelPos != string::npos) {
        if (overLabel) {
            pre << "\tErro Sintático: Mais de um rótulo na mesma linha." << endl;
        }
        
        string label = "";
        
        for (int i = 0; i < labelPos; i++) {
            if ((i == 0 && isdigit(line[i])) || (!isalpha(line[i]) && !isdigit(line[i]) && line[i] != '_')) {
                pre << "\tErro Léxico: Rótulo possui caracteres inválidos." << endl;
                label = "";
                break;
            } else {
                label += line[i];
            }
        }
    
        if (labels.count(label)) {
            if (labels[label].second) {
                pre << "\tErro Semântico: Rótulo '"  << label << "' já foi definido." << endl;
            }
            
            labels[label].second = true;
            int j = labels[label].first;
            
            while (j != -1) {
                int k = binary[j];
                binary[j] = addr;  
                j = k;
            }

        } else {
            labels[label] = make_pair(addr, true);
        }
        
        string newLine = line.substr(labelPos + 1, line.size() - labelPos - 1);
        removeSpaceBefore(newLine);
        
        getInstructions(newLine, o1, pre, labels, addr, binary, true);
    } else if(line.substr(0, 4) == "ADD " || line.substr(0, 3) == "ADD") {
        if(numArguments == 2) {
            printBinary(line, o1, labels, addr, binary, "1", 2);
        } else {
            printError(pre, o1, addr, binary, "1", "ADD");
        }
        addr += 2;
    } else if (line.substr(0, 4) == "SUB " || line.substr(0, 3) == "SUB") {
        if(numArguments == 2) {
            printBinary(line, o1, labels, addr, binary, "2", 2);
        } else {
            printError(pre, o1, addr, binary, "2", "SUB");
        }
        addr += 2;
    } else if (line.substr(0, 5) == "MULT " || line.substr(0, 4) == "MULT") {
        if(numArguments == 2) {
            printBinary(line, o1, labels, addr, binary, "3", 2);
        } else {
            printError(pre, o1, addr, binary, "3", "MULT");
        }
        addr += 2;
    } else if (line.substr(0, 4) == "DIV " || line.substr(0, 3) == "DIV") {
        if(numArguments == 2) {
            printBinary(line, o1, labels, addr, binary, "4", 2);
        } else {
            printError(pre, o1, addr, binary, "4", "DIV");
        }
        addr += 2;
    } else if (line.substr(0, 4) == "JMP " || line.substr(0, 3) == "JMP") {
        if(numArguments == 2) {
            printBinary(line, o1, labels, addr, binary, "5", 2);
        } else {
            printError(pre, o1, addr, binary, "5", "JMP");
        }
        addr += 2;
    } else if (line.substr(0, 5) == "JMPN " || line.substr(0, 4) == "JMPN") {
        if(numArguments == 2) {
            printBinary(line, o1, labels, addr, binary, "6", 2);
        } else {
            printError(pre, o1, addr, binary, "6", "JMPN");
        }
        addr += 2;
    } else if (line.substr(0, 5) == "JMPP " || line.substr(0, 4) == "JMPP") {
        if(numArguments == 2) {
            printBinary(line, o1, labels, addr, binary, "7", 2);
        } else {
            printError(pre, o1, addr, binary, "7", "JMPP");
        }
        addr += 2;
    } else if (line.substr(0, 5) == "JMPZ " || line.substr(0, 4) == "JMPZ") {
        if(numArguments == 2) {
            printBinary(line, o1, labels, addr, binary, "8", 2);
        } else {
            printError(pre, o1, addr, binary, "8", "JMPZ");
        }
        addr += 2;
    } else if (line.substr(0, 4) == "COPY" || line.substr(0, 4) == "COPY") {
        if(numArguments == 2) {
            printBinary(line, o1, labels, addr, binary, "9", 3);
        } else {
            printError(pre, o1, addr, binary, "9", "COPY");
        }
        addr += 3;
    } else if (line.substr(0, 5) == "LOAD " || line.substr(0, 4) == "LOAD") {
        if(numArguments == 2) {
            printBinary(line, o1, labels, addr, binary, "10", 2);
        } else {
            printError(pre, o1, addr, binary, "10", "LOAD");
        }
        addr += 2;
    } else if (line.substr(0, 6) == "STORE " || line.substr(0, 5) == "STORE") {
        if(numArguments == 2) {
            printBinary(line, o1, labels, addr, binary, "11", 2);
        } else {
            printError(pre, o1, addr, binary, "11", "STORE");
        }
        addr += 2;
    } else if (line.substr(0, 6) == "INPUT " || line.substr(0, 5) == "INPUT") {
        if(numArguments == 2) {
            printBinary(line, o1, labels, addr, binary, "12", 2);
        } else {
            printError(pre, o1, addr, binary, "12", "INPUT");
        }
        addr += 2;
    } else if (line.substr(0, 7) == "OUTPUT " || line.substr(0, 6) == "OUTPUT") {
        if(numArguments == 2) {
            printBinary(line, o1, labels, addr, binary, "13", 2);
        } else {
            printError(pre, o1, addr, binary, "13", "OUTPUT");
        }
        addr += 2;
    } else if (line.substr(0, 5) == "STOP " || line.substr(0, 4) == "STOP") {
        if(numArguments == 2) {
            printBinary(line, o1, labels, addr, binary, "14", 1);
        } else {
            printError(pre, o1, addr, binary, "14", "STOP");
        }
        addr += 1;
    } else if (line.substr(0, 6) == "SPACE " || line.substr(0, 5) == "SPACE") {
        if(numArguments == 1) {
            printSpace(line, o1, addr, binary, 1);
        } else if (numArguments == 2) {
            printSpace(line, o1, addr, binary, 2);
        } else {
            printErrorDirective(pre, "SPACE");
        }
    } else if (line.substr(0, 6) == "CONST " || line.substr(0, 5) == "CONST") {
        if(numArguments == 2) {
            printConst(line, o1, addr, binary);
        } else {
            printErrorDirective(pre, "CONST");
        }
        addr += 1;
    }
    
}

void compile(fstream &file, fstream &pre, fstream &o1, fstream &o2) {
    map<string, pair<int, bool>> labels;
    string line;

    int addr = 0;
    int binary[1024];
    
    while(getline(file, line)) {
        getInstructions(line, o1, pre, labels, addr, binary, false);
    }
    
    o1.close();
    
    for (int bin = addr; bin > 0; bin--) {
        o2 << binary[addr - bin];
        
        if (bin > 1) {
            o2 << " ";
        }
    }
    
    o2.close();
    int count = 0;
    
    for (auto label : labels) {
        if (!label.second.second) {
            if (count == 0) {
                pre << endl;
                count++;
            }
            pre << "Erro Semântico: Rótulo '" << label.first << "' não foi declarado.";
            
            if (count < labels.size() - 1) {
                pre << endl;
            }
        }
    }
    
    pre.close();
}

int main() {
    ifstream file("arquivo.asm");
    fstream tmp1("tmp1.tmp", ios::in | ios::out | ios::trunc);
    fstream tmp2("tmp2.tmp", ios::in | ios::out | ios::trunc);
    fstream tmp3("tmp3.tmp", ios::in | ios::out | ios::trunc);
    fstream tmp4("tmp4.tmp", ios::in | ios::out | ios::trunc);
    
    fstream pre("arquivo.pre", ios::in | ios::out | ios::trunc);
    fstream o1("arquivo.o1", ios::in | ios::out | ios::trunc);
    fstream o2("arquivo.o2", ios::in | ios::out | ios::trunc);
    
    if (!file || !tmp1 || !tmp2 || !tmp3 || !tmp4 || !pre || !o1 || !o2) {
        return 1;
    }

    processLines(file, tmp1);

    file.close();
    tmp1.clear();
    tmp1.seekg(0);

    string firstMacro[32];
    string secondMacro[32];
    
    processMacros(tmp1, firstMacro, secondMacro);

    tmp1.clear();
    tmp1.seekg(0);
    
    extendMacros(tmp1, tmp2, firstMacro, secondMacro);
    
    tmp1.close();
    tmp2.clear();
    tmp2.seekg(0);
    
    extendMacros(tmp2, tmp3, firstMacro, secondMacro);

    tmp2.close();
    tmp3.clear();
    tmp3.seekg(0);
    
    removeMacros(tmp3, tmp4);

    tmp3.close();
    tmp4.clear();
    tmp4.seekg(0);
    
    compile(tmp4, pre, o1, o2);

    return 0;
}
