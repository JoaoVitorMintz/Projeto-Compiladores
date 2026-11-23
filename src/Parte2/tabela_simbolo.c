/*
Alunos:
João Vitor Garcia Aguiar Mintz 10440421
Giovanni Barreiro G. de Castro 10435745
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tabela_simbolo.h"

TTabelaSimbolos tabela; // Tabela global

int hashMack(char *s);

// Inicio da nossa implementação:
void inserir_tabela(char *s, int endereco, TCategoria categoria, TTipo tipo, int escopo, int num_parametros) {
    int valor_hash = 0;
    
    // Iniciando como um ponteiro para poder, ao criar uma lista, apontar para outro nó
    TNo *no;

    // Faz o cálculo do endereço pelo código do professor, o hash_mack.o:
    valor_hash = hashMack(s);

    // Pega o tamanho do array ID:
    int tamanho_ID = sizeof(no->ID) - 1; // -1 para '\0'

    // Fazemos nó apontar para o primeiro elemento da lista na tabela onde caiu:
    no = tabela.entradas[valor_hash];

    // Se já tiver endereço inserido na tabela, ele entra na lista do endereço até inseri-lo ao final
    if (tabela.entradas[valor_hash] != NULL) {

        // Enquanto o valor no endereço 'valor_hash' estiver preenchido, passar de nó em nó até achar onde inserir novo valor
        while (no->prox != NULL) {
            no = no->prox;
        }

        // Cria um novo nó para inserir no final da lista
        // O erro de Segmentation fault tava dando aqui pq ele tentava alocar em um nó que não existia
        TNo *novoNo = (TNo*)malloc(sizeof(TNo));
        no->prox = NULL;
        no = novoNo;


        // Como ID de TNo é um array, não pode receber direto s porque s é um ponteiro para array de char, portanto,
        // é necessário que copiemos esse ponteiro para ser inserido no ID semelhantemente realizado no léxico (entretanto,
        // lá era usado para reescrever o lexema analisado enquanto aqui é para inserir em ID)
        strncpy(no->ID, s, tamanho_ID);
        no->ID[tamanho_ID] = '\0';
        no->categoria = categoria;
        no->endereco = endereco;
        no->tipo = tipo;
        no->num_parametros = num_parametros;
        no->escopo = escopo;
    } else {
        // aloca o primeiro nó e faz ele ser a cabeça da lista
        no = (TNo*)malloc(sizeof(TNo));
        tabela.entradas[valor_hash] = no;

        // Se não, já insere direto nesse endereço
        strncpy(no->ID, s, tamanho_ID);
        no->ID[tamanho_ID] = '\0';
        no->categoria = categoria;
        no->endereco = endereco;
        no->tipo = tipo;
        no->num_parametros = num_parametros;
        no->escopo = escopo;
    }
    
    // Após preencher o nó de determinada posição da tabela, ele coloca o próximo como NULL
    no->prox = NULL;

    tabela.entradas[valor_hash] = no;
}

TNo* buscar_simbolo(char *s) { // * pois vai retornar o ponteiro do nó com os valores atribuídos ao valor que busca
    int valor_hash = hashMack(s);
    TNo *no = tabela.entradas[valor_hash]; // Acessa o nó da posição na tabela

    while (no != NULL) {
        if (strcmp(no->ID, s) == 0) { // Faz String Compare para ver se o ID que temos é igual o solicitado
            return no; // caso seja, retorna este nó
        }
        no = no->prox; // Passa de nó em nó
    }
    // Caso não encontre o nó (não foi inserido anteriormente na tabela), retorna NULL.
    return NULL;
}

// VALIDAR SE PODE INICIAR A TABELA OU SE SÓ PODE O HASHMACK
void inicializa_tabela() {
    /* 
    Insere na tabela que é um vetor vários nós vazios. Dentro de inserir_tabela irá inserir os valores daquela
    posição e criar um novo nó daquela posição na lista. Caso não seja um nó vazio, ele apenas insere no nó vazio
    da lista, mas isso no hashMack
    */
    for (int i = 0; i < PRIME_NUMBER; i++) {
        tabela.entradas[i] = NULL;
    }

}

// Função para liberar a tabela da memória
void liberar_tabela() {
    TNo *no;
    TNo *proximo;

    // Passa por cada posição da tabela
    for (int i = 0; i < PRIME_NUMBER; i++) {
        no = tabela.entradas[i]; // Pega o nó da posição da tabela

        while (no != NULL) { // Enquanto não tiver uma lista encadeada naquela posição da tabela
            proximo = no->prox; // Salva o ponteiro do próximo nó

            free(no); // Libera o nó

            no = proximo; // Prossegue para próximo nó
        }
        tabela.entradas[i] = NULL; // Após limpar a lista na posição da tabela, atribui null
    }
}

void imprimir_tabela() {
    TNo *no;

    for (int i = 0; i < PRIME_NUMBER; i++) {
        no = tabela.entradas[i];
        if (no == NULL) {
            continue;
        } else {
            printf("Entrada na tabela hash: %d\n\n", i);
            while (no != NULL) {
                printf("ID: %s\n", no->ID);
                printf("Endereço: %d\n", no->endereco);
                printf("Categoria: %s\n", imprimir_categoria(no->categoria));
                printf("Tipo: %s\n", imprimir_tipo(no->tipo));
                printf("Escopo: %s\n", imprimir_escopo(no->escopo));
                no = no->prox;
            }
        }
    }
}

// FUNÇÕES APENAS PARA ESCREVER NA HORA DE IMPRIMIR A TABELA
char *imprimir_categoria(TCategoria categoria) {
    switch (categoria) {
        case S_VARIAVEL: return "Variavel";
        case S_FUNCAO: return "Funcao";
        case S_PARAMETRO: return "Parametro";
        case S_PROCEDIMENTO: return "Procedimento";
        default: return "Desconhecida";
    }
}

char *imprimir_tipo(TTipo tipo) {
    switch (tipo) {
        case T_INTEIRO: return "Inteiro";
        case T_LOGICO: return "Lógico";
        case T_CARACTERE: return "Caractere";
        default: return "Desconhecido";
    }
}

char *imprimir_escopo(int escopo) {
    if (escopo == 1) {
        return "Local";
    } else {
        return "Global";
    }
}

// Fórmula do cálculo de endereço de parâmetro: -(n+3-i) sendo n = qtd_parametros e i = qual_parametro
int altera_endereco_parametro(int qtd_parametros, int parametro_atual) {
    return -((qtd_parametros + 3) -parametro_atual);
}

// Fórmula do cálculo de endereço para retorno no D[1]: -(n+3) sendo n o número de variáveis de retorno (igual parâmetro)
int altera_endereco_retorno(int qtd_retorno) {
    return -(qtd_retorno + 3);
}