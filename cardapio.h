#ifndef CARDAPIO_H
#define CARDAPIO_H

#define MAX_NOME 50
#define MAX_DESCRICAO 100
#define MAX_SECAO 30
#define ITENS_POR_PAGINA 10

typedef struct ItemCardapio {
    char nome[MAX_NOME];
    char descricao[MAX_DESCRICAO];
    float valor;
    char secao[MAX_SECAO];
    struct ItemCardapio* proximo;
} ItemCardapio;

// Funções para gerenciar o cardápio
void adicionarItemCardapio(ItemCardapio** cardapio, const char* nome, const char* descricao, float valor, const char* secao);
void liberarCardapio(ItemCardapio* cardapio);
void listarSecoesDisponiveis(ItemCardapio* cardapio);
void exibirCardapioPaginado(ItemCardapio* cardapio);

// Funções de persistência
void salvarCardapio(ItemCardapio* cardapio, const char* nomeArquivo);
ItemCardapio* carregarCardapio(const char* nomeArquivo);

#endif