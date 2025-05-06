#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Estrutura do Pedido (como no documento)
typedef struct Pedido {
    int numeroMesa;
    char tipoAtendimento[20];
    char itens[200];
    char status[20];
    time_t dataHora; // Timestamp do pedido
    struct Pedido* proximo;
} Pedido;

// Cabeça da lista
Pedido* listaPedidos = NULL;

// Função para adicionar pedido
void adicionarPedido(int mesa, const char* tipo, const char* itens) {
    Pedido* novoPedido = (Pedido*)malloc(sizeof(Pedido));
    novoPedido->numeroMesa = mesa;
    strcpy(novoPedido->tipoAtendimento, tipo);
    strcpy(novoPedido->itens, itens);
    strcpy(novoPedido->status, "Pendente");
    novoPedido->dataHora = time(NULL);
    novoPedido->proximo = NULL;

    if (listaPedidos == NULL) {
        listaPedidos = novoPedido;
    } else {
        Pedido* temp = listaPedidos;
        while (temp->proximo != NULL) {
            temp = temp->proximo;
        }
        temp->proximo = novoPedido;
    }
    printf("Pedido da Mesa %d registrado!\n", mesa);
}

// Função para remover pedido pronto
void removerPedidoPronto() {
    if (listaPedidos == NULL) {
        printf("Nenhum pedido para remover.\n");
        return;
    }
    Pedido* temp = listaPedidos;
    listaPedidos = listaPedidos->proximo;
    free(temp);
    printf("Pedido finalizado e removido!\n");
}

// Função para listar pedidos
void listarPedidos() {
    Pedido* temp = listaPedidos;
    printf("\n--- Pedidos Pendentes ---\n");
    while (temp != NULL) {
        printf("Mesa: %d | Itens: %s | Status: %s\n", 
               temp->numeroMesa, temp->itens, temp->status);
        temp = temp->proximo;
    }
}

typedef struct Produto {
    int id;
    char nome[50];
    int quantidade;
} Produto;

Produto estoque[] = {
    {1, "Pizza Margherita", 10},
    {2, "Refrigerante", 20},
    {3, "Churrasco", 15},
    {4, "Vinho", 8}
};

void atualizarEstoque(const char* item, int quantidade) {
    for (int i = 0; i < sizeof(estoque)/sizeof(estoque[0]); i++) {
        if (strstr(estoque[i].nome, item) != NULL) {
            estoque[i].quantidade -= quantidade;
            printf("Estoque atualizado: %s | Novo total: %d\n", 
                   estoque[i].nome, estoque[i].quantidade);
            return;
        }
    }
    printf("Produto não encontrado no estoque.\n");
}

// Função para dividir e mesclar a lista (Merge Sort)
Pedido* merge(Pedido* a, Pedido* b) {
    if (a == NULL) return b;
    if (b == NULL) return a;

    Pedido* resultado = NULL;
    if (a->dataHora <= b->dataHora) {
        resultado = a;
        resultado->proximo = merge(a->proximo, b);
    } else {
        resultado = b;
        resultado->proximo = merge(a, b->proximo);
    }
    return resultado;
}

void mergeSort(Pedido** headRef) {
    Pedido* head = *headRef;
    if (head == NULL || head->proximo == NULL) return;

    Pedido* slow = head;
    Pedido* fast = head->proximo;
    while (fast != NULL && fast->proximo != NULL) {
        slow = slow->proximo;
        fast = fast->proximo->proximo;
    }
    Pedido* segundaParte = slow->proximo;
    slow->proximo = NULL;

    mergeSort(&head);
    mergeSort(&segundaParte);
    *headRef = merge(head, segundaParte);
}

int main() {
    // Registrar pedidos
    adicionarPedido(1, "À la carte", "Pizza Margherita, Refrigerante");
    adicionarPedido(3, "Rodízio", "Churrasco, Vinho");
    adicionarPedido(2, "À la carte", "Salada, Suco");

    // Atualizar estoque
    atualizarEstoque("Pizza Margherita", 1);
    atualizarEstoque("Churrasco", 1);

    // Ordenar pedidos por tempo
    mergeSort(&listaPedidos);
    printf("\n--- Pedidos Ordenados por Tempo ---\n");
    listarPedidos();

    // Finalizar primeiro pedido
    removerPedidoPronto();
    listarPedidos();

    return 0;
}
