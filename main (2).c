#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estoque.h"
#include "pedido.h"

void exibirMenu() {
    printf("\n=== SISTEMA DE RESTAURANTE ===\n");
    printf("1. Cadastrar item no estoque\n");
    printf("2. Listar estoque\n");
    printf("3. Registrar pedido\n");
    printf("4. Cancelar pedido\n");
    printf("5. Finalizar pedido (alterar status)\n");
    printf("6. Listar pedidos abertos\n");
    printf("7. Listar pedidos finalizados\n");
    printf("8. Buscar pedido por número da mesa\n");
    printf("9. Ver estatísticas de vendas\n");
    printf("0. Sair\n");
    printf("Escolha uma opção: ");
}

int main() {
    ItemEstoque* estoque = NULL;
    Pedido* listaPedidos = NULL;

    int opcao;
    char nome[50];
    int quantidade;
    float preco;
    int mesa;
    char status[20];
    
    do {
        exibirMenu();
        scanf("%d", &opcao);
        getchar();  // limpa o \n do buffer

        switch(opcao) {
            case 1: // Cadastrar item no estoque
                printf("Nome do item: ");
                fgets(nome, sizeof(nome), stdin);
                nome[strcspn(nome, "\n")] = 0;

                printf("Quantidade: ");
                scanf("%d", &quantidade);

                printf("Preço: ");
                scanf("%f", &preco);
                getchar();

                adicionarItem(&estoque, criarItem(nome, quantidade, preco));
                printf("Item adicionado ao estoque.\n");
                break;

            case 2: // Listar estoque
                listarEstoque(estoque);
                break;

            case 3: { // Registrar pedido
                printf("Número da mesa: ");
                scanf("%d", &mesa);
                getchar();

                Pedido* novoPedido = (Pedido*)malloc(sizeof(Pedido));
                if (!novoPedido) {
                    printf("Erro ao alocar memória para novo pedido.\n");
                    break;
                }
                novoPedido->numeroMesa = mesa;
                novoPedido->totalItens = 0;
                novoPedido->totalValor = 0.0f;
                strcpy(novoPedido->status, "Em andamento");
                novoPedido->itens = NULL;
                novoPedido->proximo = NULL;

                char continuar = 's';
                while (continuar == 's' || continuar == 'S') {
                    printf("Nome do item para pedido: ");
                    fgets(nome, sizeof(nome), stdin);
                    nome[strcspn(nome, "\n")] = 0;

                    printf("Quantidade: ");
                    scanf("%d", &quantidade);
                    getchar();

                    // Verifica item no estoque e quantidade disponível
                    ItemEstoque* itemEstoque = buscarItem(estoque, nome);
                    if (itemEstoque == NULL) {
                        printf("Item '%s' não existe no estoque. Não pode adicionar ao pedido.\n", nome);
                    } else if (itemEstoque->quantidade < quantidade) {
                        printf("Quantidade insuficiente no estoque (disponível: %d).\n", itemEstoque->quantidade);
                    } else {
                        inserirItemPedido(&novoPedido->itens, nome, quantidade);
                        novoPedido->totalItens += quantidade;
                        novoPedido->totalValor += quantidade * itemEstoque->preco;
                        printf("Item adicionado ao pedido.\n");
                    }

                    printf("Adicionar mais itens ao pedido? (s/n): ");
                    scanf(" %c", &continuar);
                    getchar();
                }

                inserirPedido(&listaPedidos, novoPedido);
                printf("Pedido registrado com sucesso.\n");
                break;
            }

            case 4: { // Cancelar pedido por número da mesa
                if (listaPedidos == NULL) {
                    printf("Nenhum pedido para cancelar.\n");
                } else {
                    int mesaCancelar;
                    printf("Informe o número da mesa do pedido que deseja cancelar: ");
                    scanf("%d", &mesaCancelar);
                    getchar();
            
                    int removido = removerPedidoPorMesa(&listaPedidos, mesaCancelar);
                    if (removido) {
                        printf("Pedido da mesa %d cancelado com sucesso.\n", mesaCancelar);
                    } else {
                        printf("Pedido da mesa %d não encontrado.\n", mesaCancelar);
                    }
                }
                break;
            }
            
            case 5: { // Finalizar pedido (alterar status)
                printf("Informe o número da mesa do pedido para finalizar: ");
                scanf("%d", &mesa);
                getchar();

                Pedido* pedido = buscarPedidoPorMesa(listaPedidos, mesa);
                if (pedido == NULL) {
                    printf("Pedido da mesa %d não encontrado.\n", mesa);
                } else {
                    alterarStatusPedido(pedido, "Finalizado");
                    // Atualizar o estoque após finalizar pedido
                    ItemPedido* itemPedido = pedido->itens;
                    while (itemPedido != NULL) {
                        ItemEstoque* itemEstoque = buscarItem(estoque, itemPedido->nome);
                        if (itemEstoque != NULL) {
                            if (itemEstoque->quantidade >= itemPedido->quantidade) {
                                itemEstoque->quantidade -= itemPedido->quantidade;
                            } else {
                                printf("Estoque insuficiente para o item %s.\n", itemPedido->nome);
                            }
                        }
                        itemPedido = itemPedido->proximo;
                    }
                    printf("Pedido finalizado e estoque atualizado.\n");
                }
                break;
            }

            case 6:
                listarPedidosAbertos(listaPedidos);
                break;
            case 7:
                listarPedidosFinalizados(listaPedidos);
                break;

            case 8: { // Buscar pedido por número da mesa
                printf("Número da mesa para busca: ");
                scanf("%d", &mesa);
                getchar();

                Pedido* pedido = buscarPedidoPorMesa(listaPedidos, mesa);
                if (pedido == NULL) {
                    printf("Pedido não encontrado para a mesa %d.\n", mesa);
                } else {
                    printf("Pedido da mesa %d | Status: %s | Total itens: %d | Valor total: R$ %.2f\n",
                        pedido->numeroMesa, pedido->status, pedido->totalItens, pedido->totalValor);
                    ItemPedido* item = pedido->itens;
                    while (item != NULL) {
                        printf("  - %s: %d\n", item->nome, item->quantidade);
                        item = item->proximo;
                    }
                }
                break;
            }
            
            case 9:
                exibirEstatisticas(listaPedidos);
                break;

            case 0:
                printf("Saindo do sistema...\n");
                break;

            default:
                printf("Opção inválida.\n");
        }
    } while (opcao != 0);

    liberarEstoque(estoque);
    liberarPedidos(listaPedidos);

    return 0;
}
