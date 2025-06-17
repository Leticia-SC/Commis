#ifndef ESTOQUE_H
#define ESTOQUE_H

typedef struct ItemEstoque {
    char nome[50];                    // Nome do item (ex: "Pizza Margherita")
    int quantidade;                   // Quantidade disponível
    float preco;                      // Preço unitário
    struct ItemEstoque* proximo;     // Ponteiro para o próximo item na lista
} ItemEstoque;

// Funções para manipulação do estoque
ItemEstoque* criarItem(const char* nome, int quantidade, float preco);
void adicionarItem(ItemEstoque** estoque, ItemEstoque* novoItem, int mostrarMensagem);
void atualizarQuantidade(ItemEstoque* estoque, const char* nome, int novaQuantidade);
ItemEstoque* buscarItem(ItemEstoque* estoque, const char* nome);
void listarEstoque(ItemEstoque* estoque);
void inserirItem(ItemEstoque** estoque, const char* nome, int quantidade, float preco);
void liberarEstoque(ItemEstoque* estoque);
int salvarEstoque(ItemEstoque* estoque, const char* nomeArquivo);
ItemEstoque* carregarEstoque(const char* nomeArquivo);
void exportarEstoqueParaCSV(ItemEstoque* estoque, const char* nomeArquivo);

#endif // ESTOQUE_H
