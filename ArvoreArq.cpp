//O QUE FALTA ATÉ O MOMENTO
    //EXIBIR O MAIOR TAMANHO E SEU ARQUIVO/PASTA
    //PASSAR PARA HTML

// Inclusão das bibliotecas necessárias
#include <iostream>       // Para entrada e saída padrão (cout, cin)
#include <vector>         // Para usar vetor (std::vector)
#include <string>         // Para manipulação de strings (std::string)
#include <filesystem>     // Para manipular o sistema de arquivos (disponível no C++17+)
#include <iomanip>        // Para futuras formatações (não usado diretamente aqui)

using namespace std;
namespace fs = filesystem;     // Alias para facilitar o uso de std::filesystem

// Estrutura que representa um nó na árvore de arquivos
struct Node {
    string nome;                     // Nome do arquivo ou diretório
    string caminho;                  // Caminho completo até o arquivo/pasta
    bool eh_pasta;                   // Indica se é uma pasta
    uintmax_t tamanho;               // Tamanho do arquivo ou soma dos arquivos internos (se for pasta)
    vector<Node*> filhos;            // Lista de arquivos/pastas contendo ponteiros para os nós filhos (caso seja pasta)
};

// Função recursiva que constrói a árvore de arquivos
Node* construir_arvore(const fs::path& caminho) {
    Node* no = new Node();                            // Aloca memória para um novo nó
    no->nome = caminho.filename().string();           // Define o nome do nó (somente o nome final)
    no->caminho = caminho.string();                   // Armazena o caminho completo
    no->eh_pasta = fs::is_directory(caminho);         // Verifica se é pasta
    no->tamanho = 0;                                  // Inicializa o tamanho

    if (no->eh_pasta) {
        // Percorre todos os elementos do diretório
        for (const auto& entry : fs::directory_iterator(caminho)) {
            // Considera apenas arquivos regulares e subpastas
            if (fs::is_regular_file(entry) || fs::is_directory(entry)) {
                Node* filho = construir_arvore(entry.path()); // Cria recursivamente o filho
                no->filhos.push_back(filho);                  // Adiciona o filho ao vetor de filhos
                no->tamanho += filho->tamanho;                // Soma o tamanho acumulado dos filhos
            }
        }
    } else {
        no->tamanho = fs::file_size(caminho);         // Obtém o tamanho do arquivo
    }

    return no; // Retorna o ponteiro para o nó criado
}

// Função recursiva que exibe a árvore com indentação e símbolos gráficos
// prefixo: string, inicialmente vazia, usada para desenhar os "galhos" da árvore
// eh_ultimo: define se este nó é o último filho do seu pai (para usar └── ou ├──)
void exibir_arvore(Node* no, const string& prefixo = "", bool eh_ultimo = true) {
    cout << prefixo; // Printa o prefixo

    // Se não estiver vazio, imprimimos o "galho" └── ou ├──
    if (!prefixo.empty()) {
        cout << (eh_ultimo ? "└── " : "├── "); // Escolhe o símbolo adequado
    }

    //Se o nó for uma pasta entra no if para iniciar o print da arvore
    if (no->eh_pasta) {
        // Começa mostrando o número de filhos e o tamanho acumulado
        cout << no->nome << " (" << no->filhos.size() << " filhos, " << no->tamanho << " bytes)" << endl;

        // Percorre todos os filhos da pasta recursivamente
        for (size_t i = 0; i < no->filhos.size(); ++i) {
            // Verifica se este é o último filho (para ajustar o prefixo e o símbolo └──)
            bool ultimo_filho = (i == no->filhos.size() - 1);
            
            // Chama recursivamente com prefixo ajustado:
            // Se este nó é o último, o prefixo recebe "    "
            // Caso contrário, o prefixo recebe "│   " para manter a linha vertical
            exibir_arvore(no->filhos[i], prefixo + (eh_ultimo ? "    " : "│   "), ultimo_filho);
        }
    } else {
        // Se for arquivo, apenas exibe o nome e o tamanho
        cout << no->nome << " (" << no->tamanho << " bytes)" << endl;
    }
}

// Função recursiva para liberar a memória de todos os nós da árvore
void deletar_arvore(Node* no) {
    for (Node* filho : no->filhos) {
        deletar_arvore(filho); // Libera os filhos recursivamente(se repete várias vezes)
    }
    delete no; // Libera o próprio nó
}

// Função que exibe o menu e processa as escolhas do usuário
void menu(Node* raiz) {
    int opcao;
    do {
        //Exibe o menu
        cout << "\n=== MENU ===\n";
        cout << "1. Exibir árvore completa\n";
        cout << "2. Sair\n";
        cout << "Escolha uma opção: ";

        // Validação para evitar erro caso o usuário digite letras
        if (!(cin >> opcao)) {
            cin.clear();              // Limpa o estado de erro do cin
            cin.ignore(10000, '\n');  // Descarta o restante da linha
            cout << "Entrada inválida. Tente novamente.\n";
            continue;
        }

        switch (opcao) {
            case 1:
                // Exibe a raiz e chama a função que mostra a árvore
                cout << "\n"
                     << raiz->caminho << " (" 
                     << raiz->filhos.size() << " filhos, " 
                     << raiz->tamanho << " bytes)" << endl;
                exibir_arvore(raiz);
                cout << endl;
                break;
            case 2:
                cout << "Encerrando...\n";   // Finaliza o programa
                break;
            default:
                cout << "Opção inválida.\n"; // Opção fora do menu
        }

    } while (opcao != 2); // Continua até o usuário escolher "Sair" digitando 2
}

// Função principal do programa
int main(int argc, char* argv[]) {
    // Define o diretório inicial com base nos argumentos da linha de comando
    fs::path caminho_inicial = (argc > 1) ? fs::path(argv[1]) : fs::current_path();

    // Valida se o caminho existe e é um diretório
    if (!fs::exists(caminho_inicial) || !fs::is_directory(caminho_inicial)) {
        cerr << "Diretório inválido: " << caminho_inicial << endl;
        return 1;
    }

    cout << "Carregando estrutura de arquivos a partir de: " << caminho_inicial << "\n";

    // Constrói a árvore de arquivos e pastas a partir do caminho inicial
    Node* raiz = construir_arvore(caminho_inicial);

    // Inicia/chama o menu interativo
    menu(raiz);

    // Libera a memória usada pela árvore ao final do programa
    deletar_arvore(raiz);

    return 0;
}

