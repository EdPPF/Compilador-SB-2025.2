#include "Parser.h"
#include "Pre_processor.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>

LinhaProcessada parseLinha(const std::string& linha) {
    LinhaProcessada resultado; // Inicializa a struct
    std::string linhaLimpa = linha; // Cópia da linha

    removeSpaceLeft(linhaLimpa);

    // Rótulo é tudo que vem antes de ':', stringstream 'elimina'
    // espaços no rótulo (charactere inválido)
    size_t colonPos = linhaLimpa.find(':');
    if (colonPos != string::npos) {
        resultado.rotulo = linhaLimpa.substr(0, colonPos);
    }

    if (colonPos + 1 > linhaLimpa.size()) {
        return resultado;
    }

    linhaLimpa = linhaLimpa.substr(colonPos + 1);

    // Usar stringstream para dividir a linha em "tokens" (palavras)
    std::stringstream ss(linhaLimpa);
    std::string token;
    std::vector<std::string> tokens;
    while (ss >> token) {
        tokens.push_back(token);
    }

    if (tokens.empty()) {
        return resultado; // Retorna objeto vazio se a linha for vazia ou só comentário
    }

    // Identificar os componentes
    // int tokenIndex = 0;
    size_t tokenIndex = 0;

    // A instrução/diretiva é o próximo token
    if (tokenIndex < tokens.size()) {
        std::string instrucao = tokens[tokenIndex];

        if (instrucao.back() == ',') {
            instrucao.pop_back();
        }
        resultado.instrucao = instrucao;
        tokenIndex++;
    }

    // O resto são operandos
    while (tokenIndex < tokens.size()) {
        std::string operando = tokens[tokenIndex];
        
        if (operando != "+") {
            // Remove vírgulas, se houver
            if (operando.back() == ',') {
                operando.pop_back();
            }
            
            resultado.operandos.push_back(operando);
        }

        tokenIndex++;
    }

    return resultado;
}