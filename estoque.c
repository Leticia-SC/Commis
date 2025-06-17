#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estoque.h"

ItemEstoque* criarItem(const char* nome, int quantidade, float preco) {
    ItemEstoque* novoItem = (ItemEstoque*)malloc(sizeof(ItemEstoque));
    if (novoItem == NULL) {
        printf("Erro ao alocar memória para item do estoque.\n");
        return NULL;
    }
    strncpy(novoItem->nome, nome, sizeof(novoItem->nome) - 1);
    novoItem->nome[sizeof(novoItem->nome) - 1] = '\0';
    novoItem->quantidade = quantidade;
    novoItem->preco = preco;
    novoItem->proximo = NULL;
    return novoItem;
}

void adicionarItem(ItemEstoque** estoque, ItemEstoque* novoItem, int mostrarMensagem) {
    if (*estoque == NULL) {
        *estoque = novoItem;
    } else {
        ItemEstoque* temp = *estoque;
        while (temp->proximo != NULL) {
            temp = temp->proximo;
        }
        temp->proximo = novoItem;
    }
    if (mostrarMensagem)
        printf("Item '%s' adicionado ao estoque.\n", novoItem->nome);
}

void atualizarQuantidade(ItemEstoque* estoque, const char* nome, int novaQuantidade) {
    ItemEstoque* temp = estoque;
    while (temp != NULL) {
        if (strcmp(temp->nome, nome) == 0) {
            temp->quantidade = novaQuantidade;
            printf("Quantidade de '%s' atualizada para %d.\n", nome, novaQuantidade);
            return;
        }
        temp = temp->proximo;
    }
    printf("Item '%s' não encontrado no estoque.\n", nome);
}

ItemEstoque* buscarItem(ItemEstoque* estoque, const char* nome) {
    ItemEstoque* temp = estoque;
    while (temp != NULL) {
        if (strcmp(temp->nome, nome) == 0) {
            return temp;  // Retorna o item encontrado
        }
        temp = temp->proximo;
    }
    return NULL;  // Item não encontrado
}

void listarEstoque(ItemEstoque* estoque) {
    if (estoque == NULL) {
        printf("Estoque vazio.\n");
        return;
    }
    printf("\n--- ESTOQUE ---\n");
    ItemEstoque* temp = estoque;
    while (temp != NULL) {
        printf("Item: %s | Quantidade: %d | Preco: R$ %.2f\n",
               temp->nome, temp->quantidade, temp->preco);
        temp = temp->proximo;
    }
}

int verificarDisponibilidade(ItemEstoque* estoque, const char* nome, int quantidadeDesejada) {
    ItemEstoque* item = buscarItem(estoque, nome);
    if (item == NULL) {
        printf("Item '%s' não cadastrado.\n", nome);
        return 0;  // Indisponível
    }
    if (item->quantidade >= quantidadeDesejada) {
        return 1;  // Disponível
    } else {
        printf("Quantidade insuficiente de '%s' (estoque: %d).\n", nome, item->quantidade);
        return 0;
    }
}

void inserirItem(ItemEstoque** estoque, const char* nome, int quantidade, float preco) {
    ItemEstoque* existente = buscarItem(*estoque, nome);
    if (existente != NULL) {
        atualizarQuantidade(*estoque, nome, existente->quantidade + quantidade);
    } else {
        ItemEstoque* novo = criarItem(nome, quantidade, preco);
        adicionarItem(estoque, novo, 1);
    }
}

void liberarEstoque(ItemEstoque* estoque) {
    while (estoque != NULL) {
        ItemEstoque* temp = estoque;
        estoque = estoque->proximo;
        free(temp);
    }
}

int salvarEstoque(ItemEstoque* estoque, const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo para salvar estoque.\n");
        return 0;
    }

    ItemEstoque* atual = estoque;
    while (atual != NULL) {
        fprintf(arquivo, "%s|%d|%.2f\n", atual->nome, atual->quantidade, atual->preco);
        atual = atual->proximo;
    }

    fclose(arquivo);
    return 1;
}

ItemEstoque* carregarEstoque(const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Arquivo de estoque não encontrado. Criando novo estoque.\n");
        return NULL;
    }

    ItemEstoque* estoque = NULL;
    char linha[200];
    while (fgets(linha, sizeof(linha), arquivo)) {
        char nome[50];
        int quantidade;
        float preco;

        // Remove o \n do final
        linha[strcspn(linha, "\n")] = 0;

        // Divide a linha nos separadores |
        char* token = strtok(linha, "|");
        if (token) strcpy(nome, token);
        
        token = strtok(NULL, "|");
        if (token) quantidade = atoi(token);
        
        token = strtok(NULL, "|");
        if (token) preco = atof(token);

        // Cria e adiciona o item
        ItemEstoque* novoItem = criarItem(nome, quantidade, preco);
        adicionarItem(&estoque, novoItem, 0);
    }

    fclose(arquivo);
    return estoque;
}

void exportarEstoqueParaCSV(ItemEstoque* estoque, const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao criar arquivo CSV do estoque.\n");
        return;
    }

    // Cabeçalho do CSV
    fprintf(arquivo, "Item,Quantidade,Preco Unitario\n");

    ItemEstoque* atual = estoque;
    while (atual != NULL) {
        fprintf(arquivo, "\"%s\",%d,%.2f\n", 
                atual->nome, 
                atual->quantidade, 
                atual->preco);
        atual = atual->proximo;
    }

    fclose(arquivo);
    printf("Estoque exportado para '%s' (pode abrir no Excel)\n", nomeArquivo);
}
