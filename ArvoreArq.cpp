#include <iostream>

using namespace std;

struct Node {
    string nome;     // Nome da arqv/pasta atual
    string caminho; //Armazena o camiho das pastas
    bool eh_pasta; //V ou F
    long tamanho; // em bytes
    vector<Node*> filhos;
};

int main(){

    return 0;
}
