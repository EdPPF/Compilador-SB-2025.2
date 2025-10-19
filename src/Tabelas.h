#ifndef TABELAS_H
#define TABELAS_H

#include <string>
#include <map>

// TABELA DE INSTRUÇÕES (TI)
/**
 * Armazena as informações de uma instrução do assembly inventado.
 */
struct InfoInstrucao {
    int opcode;
    int tamanho; // Tamanho da palavra em memória
    int operandos;
};

// definida em Tabelas.cpp
extern std::map<std::string, InfoInstrucao> tabelaInstrucoes;

// TABELA DE SIMBOLOS (TS)
/**
 * Armazena as informações de um símbolo (rótulo, variável, etc.).
 */
struct InfoSimbolo {
    int endereco;
    bool definido;
};
// TS pe criada na lógica principal do compilador

void inicializarTabelas();

#endif // TABELAS_H