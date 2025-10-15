#include "Parser.h"
#include <sstream>
#include <algorithm>
#include <cctype>

LinhaProcessada parseLinha(const std::string& linha) {
    LinhaProcessada resultado; // Inicializa a struct
    std::string linhaLimpa = linha; // Cópia da linha

    // Remove comentários (tudo após ';')
    size_t posComentario = linhaLimpa.find(';');
    if (posComentario != std::string::npos) {
        linhaLimpa = linhaLimpa.substr(0, posComentario);
    }

    // Converte toda a linha para maiúsculas para ser case-insensitive
    // (mudar essa lógica se acharem necessário)
    std::transform(linhaLimpa.begin(), linhaLimpa.end(), linhaLimpa.begin(),
                   [](unsigned char c){ return std::toupper(c); });

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

    // O rótulo é o primeiro token e termina com ':'
    if (tokens[0].back() == ':') {
        resultado.rotulo = tokens[0].substr(0, tokens[0].length() - 1);
        tokenIndex++; // Avança para o próximo token
    }

    // A instrução/diretiva é o próximo token
    if (tokenIndex < tokens.size()) {
        resultado.instrucao = tokens[tokenIndex];
        tokenIndex++;
    }

    // O resto são operandos
    while (tokenIndex < tokens.size()) {
        std::string operando = tokens[tokenIndex];
        // Remove vírgulas, se houver
        if (operando.back() == ',') {
            operando.pop_back();
        }
        resultado.operandos.push_back(operando);
        tokenIndex++;
    }

    return resultado;
}