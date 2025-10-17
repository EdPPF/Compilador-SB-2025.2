#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>

/**
 * Estrutura para armazenar os componentes de uma linha de código ASM após ser processado pelo parser.
 */
struct LinhaProcessada {
    std::string rotulo;
    std::string instrucao;
    std::vector<std::string> operandos;
};

/**
 * @brief Processa uma única linha de código ASM.
 * 
 * A função limpa a linha (remove comentários, espaços extras) e a divide em
 * seus componentes lógicos: rótulo, instrução e operandos.
 *
 * @param linha A string contendo a linha de código a ser processada.
 * @return Um objeto LinhaProcessada com os componentes da linha.
 */
LinhaProcessada parseLinha(const std::string& linha);

#endif // PARSER_H