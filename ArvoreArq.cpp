//O QUE FALTA ATÉ O MOMENTO
    //EXIBIR O MAIOR TAMANHO E SEU ARQUIVO/PASTA
    //PASSAR PARA HTML

// Inclusão das bibliotecas necessárias
#include <iostream>       // Para entrada e saída padrão
#include <vector>         // Para usar vetor
#include <string>         // Para manipulação de strings
#include <filesystem>     // Para manipular o sistema de arquivos (disponível no C++17+)
#include <iomanip>        // Para formatações

using namespace std;
namespace fs = filesystem;     // Alias para facilitar o uso de std::filesystem

// Estrutura que representa um nó na árvore de arquivos
/* @uintmax_t foi utilizado nesse contexto porque é um tido de dado inteiro sem sinal, ou seja apenas numeros positivos,
   que é usado para armazenar valores muito grandes, o que é útil para representar tamanhos de arquivos, por que ele é util
   nesse caso? pois como um arquivo ou diretório não pode ter tamanho negativo, o uso de uintmax_t é apropriado. */
struct Node {
    string nome;                     // Nome do arquivo ou diretório
    string caminho;                  // Caminho completo até o arquivo/pasta
    bool eh_pasta;                   // Indica se é uma pasta
    uintmax_t tamanho;               // Tamanho do arquivo ou soma dos arquivos internos (se for pasta)
    vector<Node*> filhos;            // Lista de arquivos/pastas contendo ponteiros para os nós filhos (caso seja pasta)
};

// Função recursiva que constrói a árvore de arquivos
// @Node* é um ponteiro para um nó da árvore, que permite a construção de uma estrutura de dados dinâmica
Node* construir_arvore(const fs::path& caminho) {
    Node* no = new Node();                            // Aloca memória para um novo nó
    no->nome = caminho.filename().string();           // Define o nome do nó (somente o nome final)
    no->caminho = caminho.string();                   // Armazena o caminho completo
    no->eh_pasta = fs::is_directory(caminho);         // Verifica a partir do caminho se é pasta ou não
    no->tamanho = 0;                                  // Inicializa o tamanho

    // IF - Se o nó for uma pasta, percorre os arquivos e subpastas dentro dela
    // ELSE - Se for um arquivo, o tamanho será obtido diretamente
    if (no->eh_pasta) {
        // Percorre todos os elementos do diretório
        for (const auto& entry : fs::directory_iterator(caminho)) {
            // Considera apenas arquivos regulares e subpastas(ignorar links, dispositivos, sockets, etc...)
            if (fs::is_regular_file(entry) || fs::is_directory(entry)) {
                Node* filho = construir_arvore(entry.path()); // Cria recursivamente o filho
                no->filhos.push_back(filho);                  // Adiciona o filho ao vetor de filhos
                no->tamanho += filho->tamanho;                // Soma o tamanho acumulado dos filhos
            }
        }
    } else {
        no->tamanho = fs::file_size(caminho);  // Obtém o tamanho do arquivo
    }

    return no; // Retorna o ponteiro para o nó criado
}

// Função recursiva que exibe a árvore com indentação e símbolos gráficos
/* @prefixo: string, inicialmente vazia, usada para desenhar os "galhos" da árvore, e pode ser modificada
   a cada recursiva*/
/* @eh_ultimo: bool, que define se este nó é o último filho do seu pai (para usar └── ou ├──), ele é modificado
   a cada recursiva pelo ultimo_filho, que verifica se o nó atual é o último filho do pai.*/
void exibir_arvore(Node* no, const string& prefixo = "", bool eh_ultimo = true) {
    cout << prefixo; // Printa o prefixo

    // Se o prefixo não estiver vazio entra no IF
    // Caso for o último nó, usa └──, caso contrário usa ├──
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
            case 1:                         // Caso 1: Exibe a árvore completa
                cout << "\n"                // Exibe o caminho, quantidade de filhos e tamanho total para melhor entendimento da visualização
                     << raiz->caminho 
                     << " (" << raiz->filhos.size() 
                     << " filhos, " << raiz->tamanho 
                     << " bytes)" << endl;
                exibir_arvore(raiz);        // Exibe a árvore completa a partir da raiz
                cout << endl;
                break;                
            
            case 2:                          // Caso 2: Sair do programa
                cout << "Encerrando...\n";   // Finaliza o programa
                break;
            default:                         // Caso inválido: Opção fora do menu          
                cout << "Opção inválida.\n"; // Opção fora do menu
        }

    } while (opcao != 2); // Continua até o usuário escolher "Sair" digitando 2
}


void shoBigFile()
{ // exibe arquivos que possuem o maior tamanho
}

void shoOverXfile()
{ // exibe arquivos com tamanho maior que X
}

void shoWideDir()
{ // exibe folder com maior numero de arquivos (direto)
}

void shoExtFile()
{ // exibe arquivos com extensão especifica
    string extInp;
    cin >> extInp;
    
}

void shoEmptyDir()
{ // exibe folder vazio
}

int main()
{
    return 0;
}


// Função principal do programa
int main(int argc, char* argv[]) {
    // Define o diretório inicial com base nos argumentos da linha de comando
    // Se um caminho for passado como argumento, usa esse caminho; caso contrário, usa o diretório atual definido por fs::current_path()
    fs::path caminho_inicial = (argc > 1) ? fs::path(argv[1]) : fs::current_path();

    // Valida se o caminho existe no sistema de arquivos e se é um diretório(e não um arquivo comum)
    // Se não for, exibe uma mensagem de erro e encerra o programa
    /* @cerr é usado para exibir mensagens de erro no console, ajudando a diferençar entre mensagens normais e de erro,
       tambem pode ser usado o cout, porem como queremos mostrar um erro é uma boa prático deixar 'cerr'*/
    if (!fs::exists(caminho_inicial) || !fs::is_directory(caminho_inicial)) {
        cerr << "Diretório inválido: " << caminho_inicial << endl;
        return 1;
    }

    cout << "Carregando estrutura de arquivos a partir de: " << caminho_inicial << "\n";

    // Constrói a árvore de arquivos e pastas a partir do caminho inicial e armazena o ponteiro na raiz
    Node* raiz = construir_arvore(caminho_inicial);

    // Inicia/chama o menu interativo com a raiz da árvore
    // O menu permite ao usuário interagir com a árvore, exibindo-a ou saindo do programa
    menu(raiz);

    // Libera a memória usada pela árvore ao final do programa em uma função recursiva
    // Isso é importante para evitar vazamentos de memória
    deletar_arvore(raiz);

    return 0;
}
