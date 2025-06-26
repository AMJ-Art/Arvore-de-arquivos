// Inclusão das bibliotecas necessárias
#include <iostream>       // Para entrada e saída padrão
#include <vector>         // Para usar vetor
#include <string>         // Para manipulação de strings
#include <filesystem>     // Para manipular o sistema de arquivos (disponível no C++17+)
#include <iomanip>        // Para formatações
#include <fstream>        // Para manipulação de fluxo de dados em arquivos

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

//Constroi a arvore em forma de string, para depois passar para função de criar arquivo HTML
string gerarArvoreHtml(Node *no, const string& prefixo = "", bool eh_ultimo = true){
    string html;

    html += prefixo; //Adiciona o prefixo por causa da identação

    if(!prefixo.empty()){
        html += eh_ultimo ? "└──" : "├──"; //Adiciona os simbolos
    }

    if(no->eh_pasta){
        html += no->nome + " (" + to_string(no->filhos.size()) + " filhos, " + to_string(no->tamanho) + " bytes)\n";
    } else {
        html += no->nome + " (" + to_string(no->tamanho) + " bytes)\n";
    }


    if (no->eh_pasta) {
        for (size_t i = 0; i < no->filhos.size(); ++i) {      //Percorre todos os filhos
            bool ultimo_filho = (i == no->filhos.size() - 1); //Identifica o ultimo filho
            string novo_prefixo = prefixo + (eh_ultimo ? "    " : "│   "); //Constroi o prefixo
            html += gerarArvoreHtml(no->filhos[i], novo_prefixo, ultimo_filho); //Chama a recursividade
        }
    }

    return html;
}

//Salva o arquivo HTML
void salvarArvoreHtml(Node* raiz, const string& caminhoArquivo = "arvore.html") {

    ofstream htmlFile(caminhoArquivo); //Abre o arquivo, permitindo o fluxo de dados

    if (!htmlFile.is_open()) { //Se não abrir...
        cerr << "Erro ao criar o arquivo HTML!" << endl; //cerr imprime uma mensagem de erro
        return;
    }

    // Cabeçalho HTML com estilo monoespaçado (para manter alinhamento)
    // R(de Raw) passa a string bruta, na exata mesma forma que foi enviada
    htmlFile << R"(
<!DOCTYPE html>
<html>
<head>
    <title>Árvore de Diretórios</title>
    <style>
        body { font-family: monospace; margin: 20px; white-space: pre; }
    </style>
</head>
<body>
)";

    // Conteúdo da árvore (texto puro, sem tags HTML)
    htmlFile << gerarArvoreHtml(raiz);

    // Fecha o HTML
    htmlFile << R"(
</body>
</html>
)";

    htmlFile.close(); //Fecha arquivo
    cout << "HTML criado com sucesso!" << endl;
}

void shoExtFile(Node *no, string extInp)
{ // exibe arquivos com extensão especifica

    if (no->nome.size() >= extInp.size() && std::equal(no->nome.end() - extInp.size(), no->nome.end(), extInp.begin())) //detecta caso o arquivo termine com a extensão alvo
    {
        cout << no->caminho << " (" << no->tamanho << " bytes)" <<"\n"; //print
    }
    if (no->eh_pasta) // caso seja uma pasta repete o processo para os filhos
    {
        for (size_t i = 0; i < no->filhos.size(); ++i) // itera pelos filhos
        {
            shoExtFile(no->filhos[i], extInp); // verifica recursivamente o arquivo atual
        }
    }
    return;
}
uintmax_t getmaxsize(Node* no, uintmax_t maxsize)
{ // acha o maior tamanho

    if (!no->eh_pasta) { // detecta arquivols
        if (no->tamanho > maxsize) maxsize = no->tamanho; // se o arquivo é maior que o tamanho maximo atual atualiza o valor para o tamanho do arquivo
    }
    for (size_t i = 0; i < no->filhos.size(); ++i) // itera pelos filhos
    {
        maxsize = getmaxsize(no->filhos[i], maxsize); // recursivamente busca pela arvore o maior valor e armazena ele em maxsize
    }
    return maxsize; // devolve maxsize
}

void printBIG(Node* no, uintmax_t maxsz){
   // print dos arquivos com maior tamanho
    if(no->tamanho==maxsz && !no->eh_pasta){ // verifica se o arquivo possui tamanho maximo e é arquivo
        cout << no->caminho << " (" << no->tamanho << " bytes)" <<"\n"; // print do caminho
    }
    for (size_t i = 0; i < no->filhos.size(); ++i) // itera pelos filhos
    {
     printBIG(no->filhos[i], maxsz);   // recursivamente move pela arvore, printando arquivos validos
    }
}

void shoBigFile(Node* no)
{ // exibe arquivos que possuem o maior tamanho
    uintmax_t maxsz = getmaxsize(no, 0); // armazena tamanho maximo da arvore
    printBIG(no, maxsz); // printa arquivos validos
}

void shoOverXfile(Node* no, uintmax_t tgSz)
{ // exibe arquivos com tamanho maior que X
    if (!no->eh_pasta && no->tamanho > tgSz){ // detecta se o arquivo é arquivo e maior que o tamanho alvo
        cout << "\n" << no->caminho << " (" << no->tamanho << " bytes)" <<"\n"; // printa caminho
    }
    if (no->eh_pasta) // se é pasta
    {
        for (size_t i = 0; i < no->filhos.size(); ++i) // itera por filhos
        {
            shoOverXfile(no->filhos[i], tgSz); // recursivamente verifica os valores da arvore
        }
    }
}

size_t gtWide(Node* no, size_t maxW){
    if (no->eh_pasta) { // verifica se é pasta
        if (no->filhos.size() > maxW) maxW = no->filhos.size(); // se o numero de filhos é maior que o maximo anterior atualiza o maximo
    }
    for (size_t i = 0; i < no->filhos.size(); ++i) // itera pelos filhos
    {
        maxW = gtWide(no->filhos[i], maxW); // recursivamente verifica a arvore
    }
    return maxW; // devolve o maximo
}

void printWide(Node* no, size_t mLar){
    
    if (no->eh_pasta && no->filhos.size() == mLar) // verifica se a pasta possui largura maxima
    {
        cout << no->caminho << " (" << no->filhos.size() << " filhos, " << no->tamanho << " bytes)\n"; // printa caminho
    }
    
    if (no->eh_pasta)
    {
        for (size_t i = 0; i < no->filhos.size(); ++i) // verifica filhos
        {
            printWide(no->filhos[i], mLar); // recursivamente verifica os arquivos da arvore por diretorios compativeis
        }
    }
}

void shoWideDir(Node* no)
{ // exibe folder com maior numero de arquivos (direto)
    size_t mLar;
    mLar = gtWide(no, 0); // acha o valor maximo de largura entre as pastas
    printWide(no, mLar); // devolve as pastas com largura maxima
    
}
void shoEmpDir(Node* no){
    if (no->eh_pasta && no->filhos.size() == 0) // detecta se é uma pasta vazia
    {
        cout << no->caminho << "\n"; //printa caminho
    }
    if (no->eh_pasta)
    {
        for (size_t i = 0; i < no->filhos.size(); ++i) // itera pelos filhos
        {
            shoEmpDir(no->filhos[i]); // recursivamente busca pela arvore por diretorios compativeis
        }
    }
}


void subMenu(Node* root){ // menu de busca especial
uintmax_t tgSz;
string trg;
int opt;
do {
    
    //Exibe o submenu search
    cout << "\n=== SEARCH MENU ===\n";
    cout << "1. Exibir arquivos com extensão específica\n";
    cout << "2. Exibir maior(es) arquivo(s)\n";
    cout << "3. Exibir pasta(s) vazia(s)\n";
    cout << "4. Exibir pasta(s) com mais arquivos \n";
    cout << "5. Exibir arquivos maiores que determinado tamanho\n";
    cout << "0. Sair\n";
    cout << "Escolha uma opção: ";

    // Validação para evitar erro caso o usuário digite letras
    if (!(cin >> opt)) {
        cin.clear();              // Limpa o estado de erro do cin
        cin.ignore(10000, '\n');  // Descarta o restante da linha
        cout << "Entrada inválida. Tente novamente.\n";
        continue;
    }

    switch (opt) { // submenu

        case 1:
            cout << "Target extension: ";
            cin >> trg; //armazena alvo
            cout << "Exibindo arquivos com extensão (" << trg << "): \n";
            shoExtFile(root, trg);
            break;
        case 2:
            cout << "Exibindo maior(es) arquivo(s):\n";
            shoBigFile(root);
            break;
        case 3:
            cout << "Exibindo pasta(as) vazia(as):\n";
            shoEmpDir(root);
            break;
        case 4:
            cout << "Exibindo pasta(as) com mais arquivos:\n";
            shoWideDir(root);
            break;
        case 5:
            cout << "Target size: ";
            cin >> tgSz; // armazena tamanho alvo
            if(tgSz < 0) { // Verifica se o tamanho é negativo
                cerr << "Tamanho inválido. Deve ser um número positivo.\n";
                break;
            }
            cout << "Exibindo arquivos maiores que " << tgSz << " bytes:\n";
            shoOverXfile(root, tgSz);
            break;
        case 0:                          // Caso 0: Sair do programa
            cout << "Encerrando...\n";   // Finaliza o programa
            break;

        default:                         // Caso inválido: Opção fora do menu
            cout << "Opção inválida.\n"; // Opção fora do menu
    }

} while (opt != 0); // Continua até o usuário escolher "Sair" digitando 0
}
// Função que exibe o menu e processa as escolhas do usuário
void menu(Node *raiz)
{
    int opcao;

    do
    {
        // Exibe o menu
        cout << "\n=== MENU ===\n";
        cout << "1. Exibir árvore completa\n";
        cout << "2. Exportar arvore para HTML\n";
        cout << "3. Pesquisas\n";
        cout << "0. Sair\n";
        cout << "Escolha uma opção: ";

        // Validação para evitar erro caso o usuário digite letras
        if (!(cin >> opcao))
        {
            cin.clear();             // Limpa o estado de erro do cin
            cin.ignore(10000, '\n'); // Descarta o restante da linha
            cout << "Entrada inválida. Tente novamente.\n";
            continue;
        }

        switch (opcao)
        {
        case 1:          // Caso 1: Exibe a árvore completa
            cout << "\n" // Exibe o caminho, quantidade de filhos e tamanho total para melhor entendimento da visualização
                 << raiz->caminho
                 << " (" << raiz->filhos.size()
                 << " filhos, " << raiz->tamanho
                 << " bytes)" << endl;
            exibir_arvore(raiz); // Exibe a árvore completa a partir da raiz
            cout << endl;
            break;

        case 2:
            cout << "Criando HTML..." << "\n"
                 << endl;
            salvarArvoreHtml(raiz);
            break;

        case 3:
            subMenu(raiz);
            break;

        case 0:                        // Caso 0: Sair do programa
            cout << "Encerrando...\n"; // Finaliza o programa
            break;

        default:                         // Caso inválido: Opção fora do menu
            cout << "Opção inválida.\n"; // Opção fora do menu
        }

    } while (opcao != 0); // Continua até o usuário escolher "Sair" digitando 0
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
