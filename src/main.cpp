#include <iostream>
#include "main.h"
#include "Pre_Processador.h"
#include "Parser.h"
#include "Tabelas.h"
#include "Compilador.h"

ifstream arq;
ofstream pre;
ofstream o1;
ofstream o2;

string nomeArquivoO1;
string nomeArquivoO2;

vector<string> memFile;

/**
 * @brief Função de pré-processamento, inclundo:
 *     Remoção de comentários,
 *     Extensão de Macros,
 *     Ajuste de rótulos.
 */
void executarPreProcessamento() {
    std::cout << "\nPre-processamento..." << std::endl;

    processLines();    
}

/**
 * @brief Função de compilação.
 */
int executarCompilacao() {
    std::cout << "\nCompilacao..." << std::endl;

    return compile();
}

/**
 * @brief Ponto de entrada principal.
 * 
 * Gerencia argumentos, arquivos e orquestra as fases de pré-processamento e compilação.
 */
int main(int argc, char* argv[]) {
    inicializarTabelas();

    // Argumentos de linha de comando
    if (argc != 2) {
        std::cerr << "Erro: Numero incorreto de argumentos.\n" << std::endl;
        std::cerr << "Uso: ./compilador <arquivo.asm>\n" << std::endl;
        return 1;
    }

    std::string nomeArquivoAsm = argv[1];
    std::string baseNomeArquivo;

    // Remove a extensão do arquivo
    size_t pos = nomeArquivoAsm.find_last_of('.');
    if (pos != std::string::npos) { // npos é o valor retornado quando não encontra
        baseNomeArquivo = nomeArquivoAsm.substr(0, pos);
    } else {
        baseNomeArquivo = nomeArquivoAsm; // Sem extensão
    }

    // Nomes dos arquivos de saída
    std::string nomeArquivoPre = baseNomeArquivo + ".pre";
    nomeArquivoO1 = baseNomeArquivo + ".o1";
    nomeArquivoO2 = baseNomeArquivo + ".o2";

    arq.open(nomeArquivoAsm);
    pre.open(nomeArquivoPre);
    o1.open(nomeArquivoO1);
    o2.open(nomeArquivoO2);

    if (!arq.is_open() || !pre.is_open() || !o1.is_open() || !o2.is_open()) {
        std::cerr << "Erro: Nao foi possivel abrir os arquivos necessarios para compilacao" << std::endl;
        return 1;
    } 

    std::cout << "Compilando o arquivo: " << nomeArquivoAsm << std::endl;
    std::cout << "Gerando arquivos:" << std::endl;
    std::cout << "  - Pre-processado: " << nomeArquivoPre << std::endl;
    std::cout << "  - Saida Intermediaria: " << nomeArquivoO1 << std::endl;
    std::cout << "  - Saida Final: " << nomeArquivoO2 << std::endl;

    std::vector<LinhaProcessada> memFile;

    // Chama as funções principais
    // 1. Pré-processamento
    executarPreProcessamento();
    // 2. Compilação
    if(!executarCompilacao()) {
        arq.close();
        pre.close();
        o1.close();
        o2.close();

        std::cout << "\n+-+- Fim da Compilacao -+-+\n" << std::endl;
    } else {
        std::cout << "\n+-+- Compilacao interrompida devido a um erro -+-+\n" << std::endl;
    }

    return 0;
}