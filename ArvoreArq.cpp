//O QUE FALTA ATÉ O MOMENTO
    //EXIBIR O MAIOR TAMANHO E SEU ARQUIVO/PASTA
    //PASSAR PARA HTML

// Inclusão das bibliotecas necessárias
#include <iostream>       // Para entrada e saída padrão (cout, cin)
#include <vector>         // Para usar vetor (std::vector)
#include <string>         // Para manipulação de strings
#include <filesystem>     // Para manipular o sistema de arquivos (disponível no C++17+)
#include <memory>         // Para ponteiros inteligentes (shared_ptr)
#include <iomanip>        // Para formatação de saída (não usado ainda, mas pode ser útil)

using namespace std;            // Evita escrever std:: o tempo todo
namespace fs = filesystem; // Alias para facilitar o uso de std::filesystem

// Estrutura que representa um nó na árvore de arquivos
struct Node {
    string nome;                          // Nome do arquivo ou pasta
    string caminho;                       // Caminho completo até o arquivo/pasta
    bool eh_pasta;                        // Flag indicando se é uma pasta
    uintmax_t tamanho;                    // Tamanho do arquivo (ou total acumulado se for pasta)
    vector<shared_ptr<Node>> filhos;     // Vetor de filhos (subpastas ou arquivos dentro da pasta)
};

// Função recursiva que constrói a árvore de arquivos/pastas a partir de um caminho inicial
shared_ptr<Node> construir_arvore(const fs::path& caminho) {
    auto no = make_shared<Node>();                   // Cria um novo nó usando ponteiro inteligente
    no->nome = caminho.filename().string();          // Pega apenas o nome (sem o caminho)
    no->caminho = caminho.string();                  // Pega o caminho completo como string
    no->eh_pasta = fs::is_directory(caminho);        // Verifica se é uma pasta
    no->tamanho = 0;                                 // Inicializa o tamanho

    if (no->eh_pasta) {                              // Se for uma pasta:
        for (const auto& entry : fs::directory_iterator(caminho)) {
            // Para cada item dentro da pasta (recursivamente)
            if (fs::is_regular_file(entry) || fs::is_directory(entry)) {
                auto filho = construir_arvore(entry.path()); // Chama recursivamente
                no->filhos.push_back(filho);                 // Adiciona à lista de filhos
                no->tamanho += filho->tamanho;               // Soma tamanho dos filhos
            }
        }
    } else {
        no->tamanho = fs::file_size(caminho); // Se for arquivo, pega o tamanho diretamente
    }

    return no; // Retorna o nó construído
}

//ATUALIZAR ESSA FUNÇÃO POSTERIORMENTE
// Função recursiva para exibir a árvore formatada na tela
void exibir_arvore(const shared_ptr<Node>& no, int nivel = 0) {
    string indent(nivel * 4, ' '); // Indentação baseada na profundidade (nível da árvore)

    if (no->eh_pasta) {
        // Exibe nome da pasta, número de filhos e tamanho total acumulado
        cout << indent << no->nome << " (" << no->filhos.size()
             << " filhos, " << no->tamanho << " bytes)" << endl;

        // Exibe todos os filhos recursivamente, aumentando o nível (indentação)
        for (const auto& filho : no->filhos) {
            exibir_arvore(filho, nivel + 1);
        }
    } else {
        // Exibe nome e tamanho do arquivo
        cout << indent << no->nome << " (" << no->tamanho << " bytes)" << endl;
    }
}

// Menu interativo para o usuário escolher opções
void menu(const shared_ptr<Node>& raiz) {
    int opcao;
    do {
        // Exibe o menu
        cout << "\n=== MENU ===\n";
        cout << "1. Exibir árvore completa\n";
        cout << "2. Sair\n";
        cout << "Escolha uma opção: ";
        cin >> opcao; // Lê a escolha do usuário

        switch (opcao) {
            case 1:
                exibir_arvore(raiz); // Chama função para exibir árvore
                break;
            case 2:
                cout << "Encerrando...\n"; // Finaliza o programa
                break;
            default:
                cout << "Opção inválida.\n"; // Opção fora do menu
        }

    } while (opcao != 2); // Continua exibindo o menu até escolher "Sair"
}

// Função principal do programa
int main(int argc, char* argv[]) {
    // Determina o caminho inicial (passado por argumento ou o diretório atual)
    fs::path caminho_inicial = (argc > 1) ? fs::path(argv[1]) : fs::current_path();

    // Verifica se o caminho existe e é um diretório
    if (!fs::exists(caminho_inicial) || !fs::is_directory(caminho_inicial)) {
        cerr << "Diretório inválido: " << caminho_inicial << endl;
        return 1; // Encerra com erro
    }

    // Mensagem de carregamento
    cout << "Carregando estrutura de arquivos a partir de: " << caminho_inicial << "\n";

    // Constrói a árvore a partir do diretório
    auto raiz = construir_arvore(caminho_inicial);

    // Chama o menu interativo
    menu(raiz);

    return 0; // Fim do programa
}
