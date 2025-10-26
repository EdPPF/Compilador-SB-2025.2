#include "Tabelas.h"

std::map<std::string, InfoInstrucao> tabelaInstrucoes;

void inicializarTabelas() {
    // Formato 1: Opcode (0 operandos)
    tabelaInstrucoes["STOP"]   = {14, 1, {0}}; 

    // Formato 2: Opcode + 1 Endereço (1 operando)
    tabelaInstrucoes["ADD"]    = {1, 2, {1}};
    tabelaInstrucoes["SUB"]    = {2, 2, {1}};
    tabelaInstrucoes["MULT"]   = {3, 2, {1}};
    tabelaInstrucoes["DIV"]    = {4, 2, {1}};
    tabelaInstrucoes["JMP"]    = {5, 2, {1}};
    tabelaInstrucoes["JMPN"]   = {6, 2, {1}};
    tabelaInstrucoes["JMPP"]   = {7, 2, {1}};
    tabelaInstrucoes["JMPZ"]   = {8, 2, {1}};
    tabelaInstrucoes["LOAD"]   = {10, 2, {1, 2}};
    tabelaInstrucoes["STORE"]  = {11, 2, {1, 2}};
    tabelaInstrucoes["INPUT"]  = {12, 2, {1, 2}};
    tabelaInstrucoes["OUTPUT"] = {13, 2, {1, 2}};

    // Formato 3: Opcode + 2 Endereços (2 operandos)
    tabelaInstrucoes["COPY"]   = {9, 3, {2}};

    // Diretivas
    tabelaInstrucoes["CONST"] = {-1, 1, {1}};
    tabelaInstrucoes["SPACE"] = {-1, 1, {0, 1}};
}