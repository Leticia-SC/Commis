#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estoque.h"
#include "pedido.h"
#include "cardapio.h"

void exibirMenu() {
    printf("\n=== SISTEMA DE RESTAURANTE ===\n");
    printf("1. Cadastrar item no estoque\n");
    printf("2. Listar estoque\n");
    printf("3. Registrar pedido\n");
    printf("4. Cancelar pedido\n");
    printf("5. Finalizar pedido (alterar status)\n");
    printf("6. Listar pedidos\n");
    printf("7. Buscar pedido por numero da mesa\n");
    printf("8. Ver estatisticas de vendas\n");
    printf("9. Gerenciar Cardapio\n");
    printf("10. Visualizar Cardapio Completo\n");
    printf("0. Sair\n");
    printf("Escolha uma opcao: ");
}

int main() {
    ItemEstoque* estoque = carregarEstoque("estoque.dat");
    Pedido* listaPedidos = carregarPedidos("pedidos.dat");
    ItemCardapio* cardapio = carregarCardapio("cardapio.dat");

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
                ordenarPedidos(&listaPedidos);

                printf("Numero da mesa: ");
                scanf("%d", &mesa);
                getchar();

                Pedido* novoPedido = (Pedido*)malloc(sizeof(Pedido));
                if (!novoPedido) {
                    printf("Erro ao alocar memoria para novo pedido.\n");
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
                        printf("Item '%s' nao existe no estoque. Nao pode adicionar ao pedido.\n", nome);
                    } else if (itemEstoque->quantidade < quantidade) {
                        printf("Quantidade insuficiente no estoque (disponivel: %d).\n", itemEstoque->quantidade);
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
                ordenarPedidos(&listaPedidos);

                if (listaPedidos == NULL) {
                    printf("Nenhum pedido para cancelar.\n");
                } else {
                    int mesaCancelar;
                    printf("Informe o numero da mesa do pedido que deseja cancelar: ");
                    scanf("%d", &mesaCancelar);
                    getchar();
            
                    int removido = removerPedidoPorMesa(&listaPedidos, mesaCancelar);
                    if (removido) {
                        printf("Pedido da mesa %d cancelado com sucesso.\n", mesaCancelar);
                    } else {
                        printf("Pedido da mesa %d nao encontrado.\n", mesaCancelar);
                    }
                }
                break;
            }
            
            case 5: { // Finalizar pedido (alterar status)
                ordenarPedidos(&listaPedidos);

                printf("Informe o numero da mesa do pedido para finalizar: ");
                scanf("%d", &mesa);
                getchar();

                Pedido* pedido = buscarPedidoPorMesa(listaPedidos, mesa);
                if (pedido == NULL) {
                    printf("Pedido da mesa %d nao encontrado.\n", mesa);
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
                ordenarPedidos(&listaPedidos);
                listarPedidosAbertos(listaPedidos);
                break;
            case 7: { // Buscar pedido por número da mesa
                printf("Numero da mesa para busca: ");
                scanf("%d", &mesa);
                getchar();

                Pedido* pedido = buscarPedidoPorMesa(listaPedidos, mesa);
                if (pedido == NULL) {
                    printf("Pedido nao encontrado para a mesa %d.\n", mesa);
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
            
            case 8:
                exibirEstatisticas(listaPedidos);
                break;

            case 9: {
                char nome[50], descricao[100], secao[30];
                float valor;
                
                printf("Nome do item: ");
                fgets(nome, sizeof(nome), stdin);
                nome[strcspn(nome, "\n")] = 0;
                
                printf("Descricao: ");
                fgets(descricao, sizeof(descricao), stdin);
                descricao[strcspn(descricao, "\n")] = 0;
                
                printf("Valor: ");
                while (1) {
                    char valorStr[50];
                    fgets(valorStr, sizeof(valorStr), stdin);
                    valorStr[strcspn(valorStr, "\n")] = 0;
                    
                    // Verifica se há vírgula e formata corretamente
                    char* virgula = strchr(valorStr, ',');
                    if (virgula != NULL) {
                        *virgula = '.';  // Substitui temporariamente para sscanf
                    }
                    
                    if (sscanf(valorStr, "%f", &valor) == 1 && valor > 0) {
                        break;
                    }
                    printf("Valor inválido. Use vírgula para decimais (ex: 7,50). Digite novamente: ");
                }
                
                printf("Secao (Bebidas, Sobremesas, Pratos Quentes, etc.): ");
                fgets(secao, sizeof(secao), stdin);
                secao[strcspn(secao, "\n")] = 0;
                
                adicionarItemCardapio(&cardapio, nome, descricao, valor, secao);
                salvarCardapio(cardapio, "cardapio.dat");
                break;
            }
            case 10:
                exibirCardapioPaginado(cardapio);
                break;

            case 0:
                printf("Saindo do sistema...\n");
                salvarCardapio(cardapio, "cardapio.dat");
                salvarEstoque(estoque, "estoque.dat");
                salvarPedidos(listaPedidos, "pedidos.dat");
                exportarEstoqueParaCSV(estoque, "estoque.csv");
                exportarPedidosParaCSV(listaPedidos, "pedidos.csv");
                break;

            default:
                printf("Opcao invalida.\n");
        }
    } while (opcao != 0);

    liberarEstoque(estoque);
    liberarPedidos(listaPedidos);

    return 0;
}
