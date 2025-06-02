#ifndef PEDIDO_H
#define PEDIDO_H

#define MAX_NOME 50
#define MAX_STATUS 20

// Item do pedido (lista encadeada)
typedef struct ItemPedido {
    char nome[MAX_NOME];
    int quantidade;
    struct ItemPedido* proximo;
} ItemPedido;

// Pedido (lista encadeada)
typedef struct Pedido {
    int numeroMesa;
    int totalItens;
    float totalValor;
    char status[MAX_STATUS];
    ItemPedido* itens;
    struct Pedido* proximo;
} Pedido;

// Funções para gerenciar itens do pedido
void inserirItemPedido(ItemPedido** listaItens, const char* nome, int quantidade);
void liberarItensPedido(ItemPedido* listaItens);

// Funções para gerenciar pedidos
void inserirPedido(Pedido** listaPedidos, Pedido* novoPedido);
int removerPedidoPorMesa(Pedido** listaPedidos, int numeroMesa);
Pedido* buscarPedidoPorMesa(Pedido* listaPedidos, int numeroMesa);
void alterarStatusPedido(Pedido* pedido, const char* novoStatus);
void listarPedidosAbertos(Pedido* lista);
void listarPedidosFinalizados(Pedido* lista);
void liberarPedidos(Pedido* listaPedidos);
void exibirEstatisticas(Pedido* listaPedidos);


#endif
