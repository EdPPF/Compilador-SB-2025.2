#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>

using namespace std;

vector<string> split(string &line) {
    vector<string> args;
    size_t cur = 0;
    size_t next = line.find(' ');

    while (next != string::npos) {
        args.push_back(line.substr(cur, next - cur));
        cur = next + 1;
        next = line.find(' ', cur);
    }

    args.push_back(line.substr(cur));
    return args;
}





// Pre-Processing
void removeComment(string &line) {
    size_t cur = line.find(';');
    
    if (cur != string::npos) {
        line = line.substr(0, cur);
    }
}

void removeSpace(string &line) {
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

    while (j < i) {
        char x = line[j];

        if (isspace(x)) {
            if (!usedSpace) {
                newString += x;
                usedSpace = true;
            }
        } else {
            if ((x == ':' || x == ',') && !newString.empty() && newString.back() == ' ') {
                newString.pop_back();
            } else if (newString.back() == ':' && !isspace(x)) {
                newString += ' ';
            }

            newString += x;
            usedSpace = false;
        }

        j++;
    }
    
    line = newString;
}

void processLines(ifstream &file, vector<string> &tmp) {
    string line;
    string curLine;

    bool previousNeedsBody = false;
    
    while (getline(file, line)) {
        removeComment(line);
        removeSpace(line);

        if (!line.empty()) {
            bool needsBody = !line.empty() && line.back() == ':';
            bool hasLabel = needsBody || line.find(':') != string::npos;

            if (previousNeedsBody && !hasLabel) {
                curLine += " " + line;
            } else {
                if (!curLine.empty()) {
                    tmp.push_back(curLine);
                }

                curLine = line;
            }
            
            previousNeedsBody = needsBody;
        }
    }

    if (!curLine.empty()) {
        tmp.push_back(curLine);
    }
}





// Macros
void findMacro(string &line, pair<vector<string>, pair<string, string>> &macro, bool &inMacro, int &num) {
    bool end = line == "ENDMACRO";
    
    if (inMacro) {
        if (end) {
            inMacro = false;
            num += 1;
        } else {
            macro.first.push_back(line);
        }
    } else {
        size_t cur = line.find("MACRO");
        
        if (!end && cur != string::npos) {
            vector<string> args = split(line);

            macro.first.push_back(line.substr(0, cur - 2));

            if (args.size() > 3) {
                args[2].pop_back();
                macro.second = make_pair(args[2], args[3]);
            } else if (args.size() > 2) {  
                macro.second = make_pair(args[2], NULL);
            } else {
                macro.second = make_pair(NULL, NULL);
            }
            
            inMacro = true;
        }
    }
}

void processMacros(vector<string> &tmp, pair<vector<string>, pair<string, string>> &firstMacro, pair<vector<string>, pair<string, string>> &secondMacro) {
    int num = 0;
    bool inMacro = false;

    for (auto& line : tmp) {
        if (num == 0) {
            findMacro(line, firstMacro, inMacro, num);
        } else {
            findMacro(line, secondMacro, inMacro, num);
        }
    }
}

void replaceArgs(vector<string> &newTmp, vector<string> &args, pair<vector<string>, pair<string, string>> &macro) {
    for (int i = 1; i < macro.first.size(); i++) {
        string newLine = macro.first[i];
        string newArg;
        size_t pos;

        if (args.size() > 1) {
            if (args[1].back() == ',') {
                newArg = args[1];
                newArg.pop_back();
            } else {
                newArg = args[1];
            }
            
            pos = newLine.find(" " + macro.second.first + ",");
            
            if (pos != string::npos) {
                newLine.replace(pos + 1, macro.second.first.length(), newArg);
            } else {
                pos = newLine.find(" " + macro.second.first);
                
                if (pos != string::npos && pos + macro.second.first.length() + 1 == newLine.length()) {
                    newLine.replace(pos + 1, macro.second.first.length(), newArg);        
                }
            }
        }

        if (args.size() > 2) {
            if (args[2].back() == ',') {
                newArg = args[2];
                newArg.pop_back();
            } else {
                newArg = args[2];
            }
            
            pos = newLine.find(" " + macro.second.second + ",");
            
            if (pos != string::npos) {
                newLine.replace(pos + 1, macro.second.second.length(), newArg);
            } else {
                pos = newLine.find(" " + macro.second.second);
                
                if (pos != string::npos && pos + macro.second.second.length() + 1 == newLine.length()) {
                    newLine.replace(pos + 1, macro.second.second.length(), newArg);        
                }
            }
        }

        newTmp.push_back(newLine);
    }
}

vector<string> extendMacros(vector<string> &tmp, pair<vector<string>, pair<string, string>> &firstMacro, pair<vector<string>, pair<string, string>> &secondMacro) {
    vector<string> newTmp;

    bool firstMacroExists = !firstMacro.first.empty();
    bool secondMacroExists = !secondMacro.first.empty();

    for (int i = 0; i < tmp.size(); i++) {
        string line = tmp[i];
        vector<string> args = split(line);

        if (firstMacroExists && (args[0] == firstMacro.first[0] || args[0] == firstMacro.first[0] + ':')) {
            if (args[1] != ("MACRO")) {
                replaceArgs(newTmp, args, firstMacro);
            } else {
                i += firstMacro.first.size();
            }
        } else if (secondMacroExists && (args[0] == secondMacro.first[0] || args[0] == secondMacro.first[0] + ":")) {
            if (args[1] != ("MACRO")) {
                replaceArgs(newTmp, args, secondMacro);
            } else {
                i += secondMacro.first.size();
            }
        } else {
            newTmp.push_back(line);
        }
    }
    
    return newTmp;
}





// Compilation
void printBinary(vector<string> &args, fstream &o1, map<string, pair<int, bool>> &labels, vector<int> &binary, int &addr, int opCode) {
    if (addr > 0) {
        o1 << " ";
    }
    
    if (args.size() == 1) {
        o1 << opCode;
        binary.push_back(opCode);
        
    } else if (args.size() == 2) {    
        int labelAddr;

        if (!labels.count(args[1])) {
            labels[args[1]] = make_pair(addr + 1, false);
            labelAddr = -1;
        } else {
            labelAddr = labels[args[1]].first;
        }
        
        o1 << opCode << " " << labelAddr;
        binary.push_back(opCode);
        binary.push_back(labelAddr);

    } else if (args.size() == 3) {
        int labelAddr1;
        int labelAddr2;
        
        args[1].pop_back();

        if (!labels.count(args[1])) {
            labels[args[1]] = make_pair(addr + 1, false);
            labelAddr1 = -1;
        } else {
            labelAddr1 = labels[args[1]].first;
        }

        if (!labels.count(args[2])) {
            labels[args[2]] = make_pair(addr + 2, false);
            labelAddr2 = -1;
        } else {
            labelAddr2 = labels[args[2]].first;
        }
        
        o1 << opCode << " " << labelAddr1 << " " << labelAddr2;
        binary.push_back(opCode);
        binary.push_back(labelAddr1);
        binary.push_back(labelAddr2);
    }
}

void printError(fstream &pre, string cmd) {
    pre << "\tErro Sintático: Número errado de argumentos para " << cmd << "." << endl;
}

void printSpace(vector<string> &args, fstream &o1, vector<int> &binary, int &addr) {
    if (addr > 0) {
        o1 << " ";
    }
    
    if(args.size() == 1) {
        o1 << 0;
        binary.push_back(0);

        addr += 1;
    } else if (args.size() == 2) {
        int con = stoi(args[1]);

        for (int i = 0; i < con; i++) {
            o1 << 0;
            binary.push_back(0);

            if (i < con - 1) {
                o1 << " ";
            }
        }
        
        addr += con;
    }
}

void printConst(vector<string> &args, fstream &o1, vector<int> &binary, int &addr) {
    if (addr > 0) {
        o1 << " ";
    }
    
    int con = stoi(args[1]);

    o1 << con;
    binary.push_back(con);
}

void getInstructions(string &line, fstream &o1, fstream &pre, map<string, pair<int, bool>> &labels, vector<int> &binary, int &addr, bool overLabel) {
    vector<string> args = split(line);

    if (!overLabel) {
       pre << line << endl; 
    }
    
    if (args[0].back() == ':') {

        args[0].pop_back();

        if (overLabel) {
            pre << "\tErro Sintático: Mais de um rótulo na mesma linha." << endl;
            return;
        }

        if (isdigit(line[0])) {
            pre << "\tErro Léxico: Rótulo possui caracteres inválidos." << endl;
            return;
        } else {
            for (int i = 0; i < args[0].length(); i++) {
                if(!isalnum(line[i]) && line[i] != '_') {
                    pre << "\tErro Léxico: Rótulo possui caracteres inválidos." << endl;
                    return;
                }
            }
        }
    
        if (labels.count(args[0])) {
            if (labels[args[0]].second) {
                pre << "\tErro Semântico: Rótulo '"  << args[0] << "' já foi definido." << endl;
                return;
            }
            
            labels[args[0]].second = true;
            
            int nextAddr;
            
            for (int curAddr = labels[args[0]].first; curAddr != -1; curAddr = nextAddr) {
                nextAddr = binary[curAddr];
                binary[curAddr] = addr;  
            }
        }
        
        labels[args[0]] = make_pair(addr, true);
        
        line.erase(0, args[0].length() + 1);
        removeSpace(line);
        
        getInstructions(line, o1, pre, labels, binary, addr, true);

    } else if (args[0] == "ADD") {
        if(args.size() == 2) {
            printBinary(args, o1, labels, binary, addr, 1);
        } else {
            printError(pre, "ADD");
        }
        addr += 2;
    } else if (args[0] == "SUB") {
        if(args.size() == 2) {
            printBinary(args, o1, labels, binary, addr, 2);
        } else {
            printError(pre, "SUB");
        }
        addr += 2;
    } else if (args[0] == "MULT") {
        if(args.size() == 2) {
            printBinary(args, o1, labels, binary, addr, 3);
        } else {
            printError(pre, "MULT");
        }
        addr += 2;
    } else if (args[0] == "DIV") {
        if(args.size() == 2) {
            printBinary(args, o1, labels, binary, addr, 4);
        } else {
            printError(pre, "DIV");
        }
        addr += 2;
    } else if (args[0] == "JMP") {
        if(args.size() == 2) {
            printBinary(args, o1, labels, binary, addr, 5);
        } else {
            printError(pre, "JMP");
        }
        addr += 2;
    } else if (args[0] == "JMPN") {
        if(args.size() == 2) {
            printBinary(args, o1, labels, binary, addr, 6);
        } else {
            printError(pre, "JMPN");
        }
        addr += 2;
    } else if (args[0] == "JMPP") {
        if(args.size() == 2) {
            printBinary(args, o1, labels, binary, addr, 7);
        } else {
            printError(pre, "JMPP");
        }
        addr += 2;
    } else if (args[0] == "JMPZ") {
        if(args.size() == 2) {
            printBinary(args, o1, labels, binary, addr, 8);
        } else {
            printError(pre, "JMPZ");
        }
        addr += 2;
    } else if (args[0] == "COPY") {
        if(args.size() == 3) {
            printBinary(args, o1, labels, binary, addr, 9);
        } else {
            printError(pre, "COPY");
        }
        addr += 3;
    } else if (args[0] == "LOAD") {
        if(args.size() == 2) {
            printBinary(args, o1, labels, binary, addr, 10);
        } else {
            printError(pre, "LOAD");
        }
        addr += 2;
    } else if (args[0] == "STORE") {
        if(args.size() == 2) {
            printBinary(args, o1, labels, binary, addr, 11);
        } else {
            printError(pre, "STORE");
        }
        addr += 2;
    } else if (args[0] == "INPUT") {
        if(args.size() == 2) {
            printBinary(args, o1, labels, binary, addr, 12);
        } else {
            printError(pre, "INPUT");
        }
        addr += 2;
    } else if (args[0] == "OUTPUT") {
        if(args.size() == 2) {
            printBinary(args, o1, labels, binary, addr, 13);
        } else {
            printError(pre, "OUTPUT");
        }
        addr += 2;
    } else if (args[0] == "STOP") {
        if(args.size() == 1) {
            printBinary(args, o1, labels, binary, addr, 14);
        } else {
            printError(pre, "STOP");
        }
        addr += 1;
    } else if (args[0] == "SPACE") {
        if(args.size() == 1) {
            printSpace(args, o1, binary, addr);
        } else if (args.size() == 2) {
            printSpace(args, o1, binary, addr);
        } else {
            printError(pre, "SPACE");
        }
    } else if (args[0] == "CONST") {
        if(args.size() == 2) {
            printConst(args, o1, binary, addr);
        } else {
            printError(pre, "CONST");
        }
        addr += 1;
    } else {
        pre << "\tErro Léxico: Instrução Inexistente." << endl;
    }
}

void compile(vector<string> &tmp, fstream &pre, fstream &o1, fstream &o2) {
    vector<int> binary;
    map<string, pair<int, bool>> labels;

    int addr = 0;
    
    for (string &line : tmp) {
        getInstructions(line, o1, pre, labels, binary, addr, false);
    }

    for (int bin = 0; bin < binary.size(); bin++) {
        o2 << binary[bin];
        
        if (bin < binary.size() - 1) {
            o2 << " ";
        }
    }
    
    bool firstMissing = true;
    
    for (auto &label : labels) {
        if (!label.second.second) {
            if (firstMissing) {
                pre << endl;
                firstMissing = false;
            }

            pre << "Erro Semântico: Rótulo '" << label.first << "' não foi declarado." << endl;
        }
    }
}





// Main
int main() {
    ifstream file("arquivo.asm");
    
    fstream pre("arquivo.pre", ios::in | ios::out | ios::trunc);
    fstream o1("arquivo.o1", ios::in | ios::out | ios::trunc);
    fstream o2("arquivo.o2", ios::in | ios::out | ios::trunc);
    
    if (!file || !pre || !o1 || !o2) {
        return 1;
    }

    vector<string> tmp;
    pair<vector<string>, pair<string, string>> firstMacro;
    pair<vector<string>, pair<string, string>> secondMacro;

    processLines(file, tmp);

    file.close();
    
    processMacros(tmp, firstMacro, secondMacro);
    tmp = extendMacros(tmp, firstMacro, secondMacro);
    tmp = extendMacros(tmp, firstMacro, secondMacro);
    
    compile(tmp, pre, o1, o2);

    pre.close();
    o1.close();
    o2.close();

    return 0;
}