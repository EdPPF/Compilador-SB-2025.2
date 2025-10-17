#ifndef PRE_PROCESSOR_H
#define PRE_PROCESSOR_H

#include <string>
#include <vector>

using namespace std;

/**
 * Estrutura para armazenar uma Macro.
 */
struct Macro {
    string label;
    vector<string> args;
    vector<string> body;
};

/**
 * @brief Remove comentários e ajusta rótulos à instruções.
 *
 * @param arq Nome do arquivo .arm.
 * @param tmp Vetor para armazenamento das linhas processadas.
 */
void concatLines(const string &arq, vector<string> &tmp);

/**
 * @brief Identifica, armazena e inicializa Macros.
 *
 * @param pre Nome do arquivo .pre.
 * @param tmp Vetor com as linhas armazenadas.
 */
void extendMacros(string &pre, vector<string> &tmp);

/**
 * @brief Remove os espaços à esquerda da linha.
 *
 * @param line Linha a ser alterada.
 */
void removeSpaceLeft(string &line);

#endif // PRE_PROCESSOR_H