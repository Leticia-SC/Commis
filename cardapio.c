#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cardapio.h"

void adicionarItemCardapio(ItemCardapio** cardapio, const char* nome, const char* descricao, float valor, const char* secao) {
    ItemCardapio* novoItem = (ItemCardapio*)malloc(sizeof(ItemCardapio));
    if (!novoItem) {
        printf("Erro ao alocar memória para item do cardápio.\n");
        return;
    }

    strncpy(novoItem->nome, nome, MAX_NOME - 1);
    novoItem->nome[MAX_NOME - 1] = '\0';
    
    strncpy(novoItem->descricao, descricao, MAX_DESCRICAO - 1);
    novoItem->descricao[MAX_DESCRICAO - 1] = '\0';
    
    strncpy(novoItem->secao, secao, MAX_SECAO - 1);
    novoItem->secao[MAX_SECAO - 1] = '\0';
    
    novoItem->valor = valor;
    novoItem->proximo = NULL;

    if (*cardapio == NULL) {
        *cardapio = novoItem;
    } else {
        ItemCardapio* temp = *cardapio;
        while (temp->proximo != NULL) {
            temp = temp->proximo;
        }
        temp->proximo = novoItem;
    }
}

void liberarCardapio(ItemCardapio* cardapio) {
    while (cardapio != NULL) {
        ItemCardapio* temp = cardapio;
        cardapio = cardapio->proximo;
        free(temp);
    }
}

void listarSecoesDisponiveis(ItemCardapio* cardapio) {
    printf("\n=== SEÇÕES DISPONÍVEIS ===\n");
    
    char secoes[20][MAX_SECAO] = {0};
    int count = 0;

    ItemCardapio* atual = cardapio;
    while (atual != NULL) {
        int encontrado = 0;
        for (int i = 0; i < count; i++) {
            if (strcmp(secoes[i], atual->secao) == 0) {
                encontrado = 1;
                break;
            }
        }
        if (!encontrado) {
            strcpy(secoes[count++], atual->secao);
        }
        atual = atual->proximo;
    }

    for (int i = 0; i < count; i++) {
        printf("%d. %s\n", i+1, secoes[i]);
    }
}

// Função auxiliar para exibir um item com formatação BR (vírgula decimal)
static void exibirItemFormatado(const ItemCardapio* item, int numero) {
    char valorFormatado[20];
    
    // Formata o valor com 2 casas decimais
    snprintf(valorFormatado, sizeof(valorFormatado), "%.2f", item->valor);
    
    // Substitui ponto por vírgula
    char* ponto = strchr(valorFormatado, '.');
    if (ponto != NULL) {
        *ponto = ',';
    }
    
    printf("\n%d. %s\n", numero, item->nome);
    printf("   Descricao: %s\n", item->descricao);
    printf("   Valor: R$ %s\n", valorFormatado);
}

void exibirCardapioPaginado(ItemCardapio* cardapio) {
    char secoes[20][MAX_SECAO] = {0};
    int numSecoes = 0;

    // Lista todas as seções únicas
    ItemCardapio* temp = cardapio;
    while (temp != NULL) {
        int encontrado = 0;
        for (int i = 0; i < numSecoes; i++) {
            if (strcmp(secoes[i], temp->secao) == 0) {
                encontrado = 1;
                break;
            }
        }
        if (!encontrado) {
            strcpy(secoes[numSecoes++], temp->secao);
        }
        temp = temp->proximo;
    }

    // Exibe cada seção com paginação
    for (int s = 0; s < numSecoes; s++) {
        printf("\n=== %s ===\n", secoes[s]);
        
        ItemCardapio* atual = cardapio;
        int contador = 0;
        int pagina = 1;
        int itensNaPagina = 0;

        while (atual != NULL) {
            if (strcmp(atual->secao, secoes[s]) == 0) {
                if (itensNaPagina == 0) {
                    printf("\n--- Pagina %d ---\n", pagina);
                }

                exibirItemFormatado(atual, ++contador);
                
                itensNaPagina++;
                
                if (itensNaPagina >= ITENS_POR_PAGINA) {
                    printf("\nPressione Enter para continuar...");
                    getchar();
                    itensNaPagina = 0;
                    pagina++;
                }
            }
            atual = atual->proximo;
        }
    }
}

void salvarCardapio(ItemCardapio* cardapio, const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo para salvar cardapio.\n");
        return;
    }

    ItemCardapio* atual = cardapio;
    while (atual != NULL) {
        fprintf(arquivo, "%s|%s|%.2f|%s\n", 
                atual->nome, 
                atual->descricao, 
                atual->valor, 
                atual->secao);
        atual = atual->proximo;
    }

    fclose(arquivo);
}

ItemCardapio* carregarCardapio(const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Arquivo de cardapio nao encontrado. Criando novo cardapio.\n");
        return NULL;
    }

    ItemCardapio* cardapio = NULL;
    char linha[200];
    
    while (fgets(linha, sizeof(linha), arquivo)) {
        char nome[MAX_NOME], descricao[MAX_DESCRICAO], secao[MAX_SECAO];
        float valor;
        
        // Remove quebra de linha
        linha[strcspn(linha, "\n")] = 0;
        
        // Parse dos campos
        char* token = strtok(linha, "|");
        if (token) strncpy(nome, token, MAX_NOME - 1);
        nome[MAX_NOME - 1] = '\0';
        
        token = strtok(NULL, "|");
        if (token) strncpy(descricao, token, MAX_DESCRICAO - 1);
        descricao[MAX_DESCRICAO - 1] = '\0';
        
        // Processamento especial do valor (com vírgula)
        token = strtok(NULL, "|");
        if (token) {
            // Faz uma cópia para manipulação
            char valorStr[50];
            strncpy(valorStr, token, sizeof(valorStr) - 1);
            valorStr[sizeof(valorStr) - 1] = '\0';
            
            // Substitui vírgula por ponto para conversão
            char* virgula = strchr(valorStr, ',');
            if (virgula != NULL) {
                *virgula = '.';
            }
            
            valor = atof(valorStr);
        }
        
        token = strtok(NULL, "|");
        if (token) strncpy(secao, token, MAX_SECAO - 1);
        secao[MAX_SECAO - 1] = '\0';

        // Adiciona o item ao cardápio
        adicionarItemCardapio(&cardapio, nome, descricao, valor, secao);
    }

    fclose(arquivo);
    return cardapio;
}