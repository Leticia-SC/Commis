#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "pedido.h"

// Insere item no final da lista de itens do pedido
void inserirItemPedido(ItemPedido** listaItens, const char* nome, int quantidade) {
    ItemPedido* novoItem = (ItemPedido*)malloc(sizeof(ItemPedido));
    if (!novoItem) {
        printf("Erro ao alocar memória para item do pedido.\n");
        return;
    }
    strncpy(novoItem->nome, nome, MAX_NOME - 1);
    novoItem->nome[MAX_NOME - 1] = '\0';
    novoItem->quantidade = quantidade;
    novoItem->proximo = NULL;

    if (*listaItens == NULL) {
        *listaItens = novoItem;
    } else {
        ItemPedido* temp = *listaItens;
        while (temp->proximo != NULL) {
            temp = temp->proximo;
        }
        temp->proximo = novoItem;
    }
}

// Libera memória dos itens de um pedido
void liberarItensPedido(ItemPedido* listaItens) {
    while (listaItens != NULL) {
        ItemPedido* temp = listaItens;
        listaItens = listaItens->proximo;
        free(temp);
    }
}

// Insere pedido no final da lista de pedidos
void inserirPedido(Pedido** listaPedidos, Pedido* novoPedido) {
    if (*listaPedidos == NULL) {
        *listaPedidos = novoPedido;
    } else {
        Pedido* temp = *listaPedidos;
        while (temp->proximo != NULL) {
            temp = temp->proximo;
        }
        temp->proximo = novoPedido;
    }
    novoPedido->proximo = NULL;
}

// Remove pedido da lista pelo número da mesa, retorna 1 se removido, 0 se não encontrado
int removerPedidoPorMesa(Pedido** listaPedidos, int numeroMesa) {
    if (*listaPedidos == NULL) return 0;

    Pedido* atual = *listaPedidos;
    Pedido* anterior = NULL;

    while (atual != NULL) {
        if (atual->numeroMesa == numeroMesa) {
            // Remove da lista
            if (anterior == NULL) {
                // É o primeiro da lista
                *listaPedidos = atual->proximo;
            } else {
                anterior->proximo = atual->proximo;
            }
            liberarPedidos(atual->itens);
            free(atual);
            return 1;
        }
        anterior = atual;
        atual = atual->proximo;
    }
    return 0;
}


// Busca pedido pelo número da mesa
Pedido* buscarPedidoPorMesa(Pedido* listaPedidos, int numeroMesa) {
    Pedido* temp = listaPedidos;
    while (temp != NULL) {
        if (temp->numeroMesa == numeroMesa) {
            return temp;
        }
        temp = temp->proximo;
    }
    return NULL;
}

// Altera o status do pedido
void alterarStatusPedido(Pedido* pedido, const char* novoStatus) {
    if (pedido == NULL) return;
    strncpy(pedido->status, novoStatus, MAX_STATUS - 1);
    pedido->status[MAX_STATUS - 1] = '\0';
}

void listarPedidosAbertos(Pedido* lista) {
    printf("\n=== PEDIDOS ABERTOS ===\n");
    Pedido* atual = lista;
    int encontrados = 0;
    while (atual != NULL) {
        if (strcmp(atual->status, "Em andamento") == 0) {
            printf("\nMesa: %d | Total Itens: %d | Valor: R$ %.2f | Status: %s\n",
                   atual->numeroMesa, atual->totalItens, atual->totalValor, atual->status);

            ItemPedido* item = atual->itens;
            while (item != NULL) {
                printf("  - %s: %d\n", item->nome, item->quantidade);
                item = item->proximo;
            }
            encontrados++;
        }
        atual = atual->proximo;
    }

    if (encontrados == 0) {
        printf("Nenhum pedido aberto.\n");
    }
}

void listarPedidosFinalizados(Pedido* lista) {
    printf("\n=== PEDIDOS FINALIZADOS ===\n");
    Pedido* atual = lista;
    int encontrados = 0;
    while (atual != NULL) {
        if (strcmp(atual->status, "Finalizado") == 0) {
            printf("\nMesa: %d | Total Itens: %d | Valor: R$ %.2f | Status: %s\n",
                   atual->numeroMesa, atual->totalItens, atual->totalValor, atual->status);

            ItemPedido* item = atual->itens;
            while (item != NULL) {
                printf("  - %s: %d\n", item->nome, item->quantidade);
                item = item->proximo;
            }
            encontrados++;
        }
        atual = atual->proximo;
    }

    if (encontrados == 0) {
        printf("Nenhum pedido finalizado.\n");
    }
}


// Libera toda a lista de pedidos
void liberarPedidos(Pedido* listaPedidos) {
    while (listaPedidos != NULL) {
        Pedido* temp = listaPedidos;
        listaPedidos = listaPedidos->proximo;

        liberarItensPedido(temp->itens);
        free(temp);
    }
}

void exibirEstatisticas(Pedido* listaPedidos) {
    int totalPedidos = 0;
    float valorTotal = 0.0;
    int totalItens = 0;

    // Contador por nome de item (simples - máx. 100 itens únicos)
    char nomes[100][50];
    int quantidades[100];
    int totalUnicos = 0;

    Pedido* p = listaPedidos;
    while (p != NULL) {
        if (strcmp(p->status, "Finalizado") == 0) {
            totalPedidos++;
            valorTotal += p->totalValor;
            totalItens += p->totalItens;

            ItemPedido* item = p->itens;
            while (item != NULL) {
                // Verifica se já está na lista
                int encontrado = 0;
                for (int i = 0; i < totalUnicos; i++) {
                    if (strcmp(nomes[i], item->nome) == 0) {
                        quantidades[i] += item->quantidade;
                        encontrado = 1;
                        break;
                    }
                }
                if (!encontrado && totalUnicos < 100) {
                    strcpy(nomes[totalUnicos], item->nome);
                    quantidades[totalUnicos] = item->quantidade;
                    totalUnicos++;
                }
                item = item->proximo;
            }
        }
        p = p->proximo;
    }

    // Item mais vendido
    char itemMaisVendido[50] = "";
    int maxVendidos = -1;
    for (int i = 0; i < totalUnicos; i++) {
        if (quantidades[i] > maxVendidos) {
            maxVendidos = quantidades[i];
            strcpy(itemMaisVendido, nomes[i]);
        }
    }

    // Exibir estatísticas
    printf("\n=== Estatísticas de Vendas ===\n");
    printf("Total de pedidos finalizados: %d\n", totalPedidos);
    printf("Total arrecadado: R$ %.2f\n", valorTotal);
    printf("Quantidade total de itens vendidos: %d\n", totalItens);
    if (maxVendidos > 0)
        printf("Item mais vendido: %s (%d unidades)\n", itemMaisVendido, maxVendidos);
    else
        printf("Nenhum item vendido ainda.\n");
}
