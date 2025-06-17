#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estoque.h"
#include "pedido.h"
#include "cardapio.h"

typedef enum {
    CLIENTE = 1,
    GARCOM,
    GERENTE
} PerfilUsuario;

PerfilUsuario login() {
    int opcao;
    printf("\n=== LOGIN ===\n");
    printf("1. Cliente\n");
    printf("2. Garcom\n");
    printf("3. Gerente\n");
    printf("Selecione o tipo de usuario: ");
    scanf("%d", &opcao);
    getchar();

    if (opcao == GERENTE) {
        char senha[20];
        printf("Digite a senha do gerente: ");
        fgets(senha, sizeof(senha), stdin);
        senha[strcspn(senha, "\n")] = 0;
        if (strcmp(senha, "admin123") != 0) {
            printf("Senha incorreta. Acesso como Cliente.\n");
            return CLIENTE;
        }
    }

    if (opcao < 1 || opcao > 3) {
        printf("Opção invalida. Acesso como Cliente.\n");
        return CLIENTE;
    }

    return (PerfilUsuario)opcao;
}

void exibirMenu(PerfilUsuario perfil) {
    printf("\n=== SISTEMA DE RESTAURANTE ===\n");
    switch (perfil) {
        case CLIENTE:
            printf("1. Visualizar Cardapio\n");
            printf("0. Sair\n");
            break;
        case GARCOM:
            printf("1. Registrar pedido\n");
            printf("2. Cancelar pedido\n");
            printf("3. Finalizar pedido\n");
            printf("4. Listar pedidos\n");
            printf("5. Buscar pedido por mesa\n");
            printf("0. Sair\n");
            break;
        case GERENTE:
            printf("1. Cadastrar item no estoque\n");
            printf("2. Listar estoque\n");
            printf("3. Gerenciar Cardapio\n");
            printf("4. Visualizar Cardapio\n");
            printf("5. Ver estatisticas\n");
            printf("0. Sair\n");
            break;
    }
    printf("Escolha uma opcao: ");
}

int main() {
    PerfilUsuario perfil = login();

    ItemEstoque* estoque = carregarEstoque("estoque.dat");
    Pedido* listaPedidos = carregarPedidos("pedidos.dat");
    ItemCardapio* cardapio = carregarCardapio("cardapio.dat");

    int opcao;
    do {
        exibirMenu(perfil);
        scanf("%d", &opcao);
        getchar(); // limpa \n

        switch (perfil) {
            case CLIENTE:
                switch (opcao) {
                    case 1:
                        exibirCardapioPaginado(cardapio);
                        break;
                    case 0:
                        printf("Saindo...\n");
                        break;
                    default:
                        printf("Opcao invalida para cliente.\n");
                }
                break;

            case GARCOM:
                switch (opcao) {
                    case 1: { // Registrar pedido
                        int mesa, quantidade;
                        char nome[50];
                        ordenarPedidos(&listaPedidos);

                        printf("Numero da mesa: ");
                        scanf("%d", &mesa);
                        getchar();

                        Pedido* novoPedido = (Pedido*)malloc(sizeof(Pedido));
                        if (!novoPedido) {
                            printf("Erro ao alocar memoria.\n");
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
                            printf("Nome do item: ");
                            fgets(nome, sizeof(nome), stdin);
                            nome[strcspn(nome, "\n")] = 0;

                            printf("Quantidade: ");
                            scanf("%d", &quantidade);
                            getchar();

                            ItemEstoque* itemEstoque = buscarItem(estoque, nome);
                            if (itemEstoque == NULL) {
                                printf("Item nao encontrado.\n");
                            } else if (itemEstoque->quantidade < quantidade) {
                                printf("Estoque insuficiente.\n");
                            } else {
                                inserirItemPedido(&novoPedido->itens, nome, quantidade);
                                novoPedido->totalItens += quantidade;
                                novoPedido->totalValor += quantidade * itemEstoque->preco;
                                printf("Item adicionado ao pedido.\n");
                            }

                            printf("Adicionar mais itens? (s/n): ");
                            scanf(" %c", &continuar);
                            getchar();
                        }

                        inserirPedido(&listaPedidos, novoPedido);
                        printf("Pedido registrado com sucesso.\n");
                        break;
                    }
                    case 2: { // Cancelar pedido
                        int mesa;
                        printf("Mesa a cancelar: ");
                        scanf("%d", &mesa);
                        getchar();

                        if (removerPedidoPorMesa(&listaPedidos, mesa)) {
                            printf("Pedido cancelado.\n");
                        } else {
                            printf("Pedido nao encontrado.\n");
                        }
                        break;
                    }
                    case 3: { // Finalizar pedido
                        int mesa;
                        printf("Mesa a finalizar: ");
                        scanf("%d", &mesa);
                        getchar();

                        Pedido* pedido = buscarPedidoPorMesa(listaPedidos, mesa);
                        if (pedido == NULL) {
                            printf("Pedido nao encontrado.\n");
                        } else {
                            alterarStatusPedido(pedido, "Finalizado");
                            ItemPedido* item = pedido->itens;
                            while (item != NULL) {
                                ItemEstoque* est = buscarItem(estoque, item->nome);
                                if (est && est->quantidade >= item->quantidade) {
                                    est->quantidade -= item->quantidade;
                                }
                                item = item->proximo;
                            }
                            printf("Pedido finalizado.\n");
                        }
                        break;
                    }
                    case 4:
                        ordenarPedidos(&listaPedidos);
                        listarPedidosAbertos(listaPedidos);
                        break;
                    case 5: {
                        int mesa;
                        printf("Buscar mesa: ");
                        scanf("%d", &mesa);
                        getchar();

                        Pedido* pedido = buscarPedidoPorMesa(listaPedidos, mesa);
                        if (pedido) {
                            printf("Mesa %d | Status: %s | Itens: %d | Valor: R$ %.2f\n",
                                   pedido->numeroMesa, pedido->status, pedido->totalItens, pedido->totalValor);
                            ItemPedido* item = pedido->itens;
                            while (item) {
                                printf(" - %s: %d\n", item->nome, item->quantidade);
                                item = item->proximo;
                            }
                        } else {
                            printf("Pedido nao encontrado.\n");
                        }
                        break;
                    }
                    case 0:
                        printf("Saindo...\n");
                        break;
                    default:
                        printf("Opcao invalida para garcom.\n");
                }
                break;

            case GERENTE:
                switch (opcao) {
                    case 1: { // Cadastrar estoque
                        char nome[50];
                        int qtd;
                        float preco;

                        printf("Nome do item: ");
                        fgets(nome, sizeof(nome), stdin);
                        nome[strcspn(nome, "\n")] = 0;

                        printf("Quantidade: ");
                        scanf("%d", &qtd);
                        printf("Preço: ");
                        scanf("%f", &preco);
                        getchar();

                        adicionarItem(&estoque, criarItem(nome, qtd, preco), 1);
                        printf("Item cadastrado.\n");
                        break;
                    }
                    case 2:
                        listarEstoque(estoque);
                        break;
                    case 3: { // Cadastrar item no cardápio
                        char nome[50], descricao[100], secao[30];
                        float valor;
                        
                        printf("Nome: ");
                        fgets(nome, sizeof(nome), stdin);
                        nome[strcspn(nome, "\n")] = 0;
                        
                        printf("Descricao: ");
                        fgets(descricao, sizeof(descricao), stdin);
                        descricao[strcspn(descricao, "\n")] = 0;
                        
                        printf("Valor (use virgula para decimal): ");
                        while (1) {
                            char valorStr[50];
                            fgets(valorStr, sizeof(valorStr), stdin);
                            valorStr[strcspn(valorStr, "\n")] = 0;
                            char* virgula = strchr(valorStr, ',');
                            if (virgula) *virgula = '.';
                            if (sscanf(valorStr, "%f", &valor) == 1 && valor > 0) break;
                            printf("Valor invalido. Tente novamente: ");
                        }

                        printf("Secao: ");
                        fgets(secao, sizeof(secao), stdin);
                        secao[strcspn(secao, "\n")] = 0;

                        adicionarItemCardapio(&cardapio, nome, descricao, valor, secao);
                        salvarCardapio(cardapio, "cardapio.dat");
                        printf("Item adicionado ao cardapio.\n");
                        break;
                    }
                    case 4:
                        exibirCardapioPaginado(cardapio);
                        break;
                    case 5:
                        exibirEstatisticas(listaPedidos);
                        break;
                    case 0:
                        printf("Saindo...\n");
                        break;
                    default:
                        printf("Opcao invalida para gerente.\n");
                }
                break;
        }

    } while (opcao != 0);

    // Finalização
    salvarCardapio(cardapio, "cardapio.dat");
    salvarEstoque(estoque, "estoque.dat");
    salvarPedidos(listaPedidos, "pedidos.dat");
    exportarEstoqueParaCSV(estoque, "estoque.csv");
    exportarPedidosParaCSV(listaPedidos, "pedidos.csv");

    liberarEstoque(estoque);
    liberarPedidos(listaPedidos);

    return 0;
}
