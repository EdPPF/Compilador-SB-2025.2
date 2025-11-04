#ifndef PRE_PROCESSADOR_H
#define PRE_PROCESSADOR_H

#include <map>

using namespace std;

/**
 * Estrutura para armazenar uma Macro.
 */
struct Macro {
    vector<string> args;
    vector<string> body;
};

/** 
 * Tabela de Macros, combinando MDT e MNT.
 */
struct MacroTable {
    map<string, Macro> table;
    string currentMacro;
};

/**
 * @brief Remove comentários, ajusta rótulos e expande macros.
 */
void processLines();

#endif // PRE_PROCESSOR_H