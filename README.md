# 🌳 Sistema de Arquivos como Árvore

## 👥 Integrantes
- Artur Mesquita Jeager (188354)
- João Pedro de Oliveira de Carvalho (188402)
- Octávio Francisco Petry Bortoluzzi (188380)

---

## ⚙️ Especificações do Projeto:
- Plataforma: Linux
    - Notebook Octavio Linux → Ubuntu 24.04.2 LTS
- Linguagem: C++ (padrão C++17)
  
---

## 🧪 Instruções de compilação/execução.
### ✅ Pré-requisitos
- Sistema: GNU/Linux
- Compilador: `g++` com suporte a __C++17__ (ou superior)
Para instalar o g++, execute (caso ainda não tenha):
```bash
sudo apt update
sudo apt install g++
```


### 📂 1. Salvando o Código
Salve o código-fonte em um arquivo, por exemplo:
```bash
ArvoreArq.cpp
```


### 🧱 2. Compilando o Código
No terminal, acesse a pasta onde está o arquivo e compile com:
```bash
g++ -std=c++17 -Wall -o ArvoreArq ArvoreArq.cpp
```
Explicando os parâmetros:
- `-std=c++17` → ativa a versão C++17 (necessária para `<filesystem>`)
- `-Wall` → mostra todos os avisos úteis
- `-o gerenciador` → define o nome do executável como `ArvoreArq`

### 🚀 3. Executando o Programa
- Para usar o diretório atual:
```bash
./ArvoreArq
```
- Para usar um diretório específico:
```bash
./ArvoreArq /home/octavio-petry/Downloads
```

---

## 🎯 Exemplos de Execução

### 🌱 ArvoreArq v1.0:
```bash
./ArvoreArq /home/octavio-petry/Downloads
Carregando estrutura de arquivos a partir de: "/home/octavio-petry/Downloads"

=== MENU ===
1. Exibir árvore completa
2. Sair
Escolha uma opção: 1
/home/octavio-petry/Downloads (8 filhos, 114780457 bytes)
Downloads (8 filhos, 114780457 bytes)
    ├── google-chrome-stable_current_amd64.deb (114483440 bytes)
    ├── mercado (17024 bytes)
    ├── fontes_threads.zip (4511 bytes)
    ├── AbastecimentoMercado.pdf (56620 bytes)
    ├── AppRevisaoProva (5 filhos, 113011 bytes)
    │   ├── manifest.mf (85 bytes)
    │   ├── test (0 filhos, 0 bytes)
    │   ├── build.xml (3630 bytes)
    │   ├── nbproject (4 filhos, 104801 bytes)
    │   │   ├── project.properties (3080 bytes)
    │   │   ├── build-impl.xml (100721 bytes)
    │   │   ├── project.xml (523 bytes)
    │   │   └── genfiles.properties (477 bytes)
    │   └── src (2 filhos, 4495 bytes)
    │       ├── br (1 filhos, 2360 bytes)
    │       │   └── upf (1 filhos, 2360 bytes)
    │       │       └── aluno999 (1 filhos, 2360 bytes)
    │       │           └── dominio (1 filhos, 2360 bytes)
    │       │               └── Aluguel.java (2360 bytes)
    │       └── apprevisaoprova (1 filhos, 2135 bytes)
    │           └── AppRevisaoProva.java (2135 bytes)
    ├── AppRevisaoProva.zip (19013 bytes)
    ├── Processamento em Tempo Real de Dados Sísmicos.pdf (79824 bytes)
    └── fontes_threads (8 filhos, 7014 bytes)
        ├── gastatempo.c (721 bytes)
        ├── somareduz.c (970 bytes)
        ├── duasthreads.c (679 bytes)
        ├── umathread.c (447 bytes)
        ├── cria3threads.c (1117 bytes)
        ├── threadsret.c (739 bytes)
        ├── acelera.c (1587 bytes)
        └── threadscargs.c (754 bytes)
```

