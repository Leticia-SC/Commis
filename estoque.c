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

void adicionarItem(ItemEstoque** estoque, ItemEstoque* novoItem) {
    if (*estoque == NULL) {
        *estoque = novoItem;  // Lista vazia
    } else {
        ItemEstoque* temp = *estoque;
        while (temp->proximo != NULL) {
            temp = temp->proximo;
        }
        temp->proximo = novoItem;
    }
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
        printf("Item: %s | Quantidade: %d | Preço: R$ %.2f\n",
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
        adicionarItem(estoque, novo);
    }
}

void liberarEstoque(ItemEstoque* estoque) {
    while (estoque != NULL) {
        ItemEstoque* temp = estoque;
        estoque = estoque->proximo;
        free(temp);
    }
}
