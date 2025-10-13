#include <iostream>
#include <string>
#include <vector>

// Protótipo das funções principais
/**
 * @brief Função de pré-processamento.
 * Stub para a função de pré-processamento.
 */
void executarPreProcessamento(const std::string& arquivo, const std::string& arqPre);
/**
 * @brief Função de compilação.
 * Stub para a função de compilação.
 */
void executarCompilacao(const std::string& arquivo, const std::string& arqO1, const std::string& arqO2);

int main(int argc, char* argv[]) {
    // Argumentos de linha de comando
    if (argc != 2) {
        std::cerr << "Erro: NNumero incorreto de argumentos.\n" << std::endl;
        std::cerr << "Uso: ./compilador <arquivo.asm>\n" << std::endl;
        return 1;
    }

    std::string nomeArquivoAsm = argv[1];
    std::string baseNomeArquivo;

    // Remove a extensão do arquivo
    size_t pos = nomeArquivoAsm.find_first_of('.');
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


void executarPreProcessamento(const std::string& arquivo, const std::string& arqPre) {
    std::cout << "\nPre-processamento..." << std::endl;
    std::cout << "  (Expansao de macros e tudo mais)" << std::endl;
    std::cout << "  Lendo de '" << arquivo << "' e escrevendo em '" << arqPre << "'" << std::endl;
}

void executarCompilacao(const std::string& arqPre, const std::string& arqO1, const std::string& arqO2) {
    std::cout << "\nCompilacao..." << std::endl;
    std::cout << "  (Logica da passagem unica)" << std::endl;
    std::cout << "  Lendo de '" << arqPre << "' e escrevendo em '" << arqO1 << "' e '" << arqO2 << "'" << std::endl;
}