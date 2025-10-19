#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "Tabelas.h"
#include "Pre_processor.h"
#include "Parser.h"

/**
 * @brief Função de pré-processamento, inclundo:
 *     - Remoção de comentários;
 *     - Extensão de Macros;
 *     - Ajuste de rótulos.
 * @param arquivo Arquivo fonte .arm.
 * @param arqPre Arquivo pré-processado .pre.
 */
void executarPreProcessamento(const std::string& arquivo, std::string& arqPre) {
    std::cout << "\nPre-processamento..." << std::endl;
    std::cout << "  Lendo de '" << arquivo << "' e escrevendo em '" << arqPre << "'" << std::endl;

    std::vector<std::string> tmp;

    concatLines(arquivo, tmp);
    extendMacros(arqPre, tmp);
    //extendMacros(arqPre, tmp);
}

/**
 * @brief Função de compilação.
 * Stub para a função de compilação.
 */
void executarCompilacao(const std::string& arquivo, const std::string& arqO1, const std::string& arqO2);

int main(int argc, char* argv[]) {
    inicializarTabelas();

    // Argumentos de linha de comando
    if (argc != 2) {
        std::cerr << "Erro: NNumero incorreto de argumentos.\n" << std::endl;
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
    std::string nomeArquivoO1 = baseNomeArquivo + ".o1";
    std::string nomeArquivoO2 = baseNomeArquivo + ".o2";

    std::cout << "Compilando o arquivo: " << nomeArquivoAsm << std::endl;
    std::cout << "Gerando arquivos:" << std::endl;
    std::cout << "  - Pre-processado: " << nomeArquivoPre << std::endl;
    std::cout << "  - Saida Intermediaria: " << nomeArquivoO1 << std::endl;
    std::cout << "  - Saida Final: " << nomeArquivoO2 << std::endl;

    // Chama as funções principais
    // 1. Pré-processamento
    executarPreProcessamento(nomeArquivoAsm, nomeArquivoPre);
    // 2. Compilação
    executarCompilacao(nomeArquivoPre, nomeArquivoO1, nomeArquivoO2);

    std::cout << "\n+-+- Fim da Compilacao -+-+\n" << std::endl;

    return 0;
}

void executarCompilacao(const std::string& arqPre, const std::string& arqO1, const std::string& arqO2) {
    std::cout << "\nCompilacao..." << std::endl;

    std::ifstream arquivo(arqPre);
    if (!arquivo.is_open()) {
        std::cerr << "Erro: Nao foi possivel abrir o arquivo pre-processado '" << arqPre << "'." << std::endl;
        return;
    }

    std::string linha;
    int numLinha = 1;
    while (std::getline(arquivo, linha)) {
        LinhaProcessada p = parseLinha(linha);

        // Teste para ver o resultado do parser
        if (!p.rotulo.empty() || !p.rotulo.empty()) { // Se não estiver vazia
            std::cout << "Linha " << numLinha << ": ";
            if (!p.rotulo.empty()) {
                std::cout << "Rotulo=['" << p.rotulo << "'] ";
            }
            if (!p.instrucao.empty()) {
                std::cout << "Instrucao=['" << p.instrucao << "'] ";
            }
            if (!p.operandos.empty()) {
                std::cout << "Operandos=[";
                for (size_t i = 0; i < p.operandos.size(); ++i) {
                    std::cout << "'" << p.operandos[i] << "'";
                    if (i < p.operandos.size() - 1) {
                        std::cout << ", ";
                    }
                }
                std::cout << "]";
            }
            std::cout << std::endl;
        }
        numLinha++;
    }
}