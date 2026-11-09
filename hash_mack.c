// Tabela de simbolos desenvolvida pelos alunos
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

// Trecho que o professor solicita que seja implementado no nosso programa fonte:
#define PRIME_NUMBER 211  // numero primo para reduzir colisoes 

typedef struct _TNo{ 
   char ID[16]; 
   int endereco;
   // aqui você deve declarar os outros campos que julgar necessário para 
   // armazenar os identificadores 
 
   struct _TNo *prox; 
} TNo; 

typedef struct { 
    TNo *entradas[PRIME_NUMBER]; 
} TTabelaSimbolos;

int calcular_endereco(char *s);
int hashMack(char *s);
void inicializa_tabela();

// Tabela global:
TTabelaSimbolos tabela;

// Inicio da nossa implementação:
int hashMack(char *s) {
    int valor_hash = 0;
    // Iniciando como um ponteiro para poder, ao criar uma lista, apontar para outro nó
    TNo *no;
    TNo *no_a_inserir = (TNo*)malloc(sizeof(TNo)); // Nó que será construído para vir na posição vazia do no->prox

    if (no_a_inserir == NULL) {
        printf("ERRO ao alocar memória");
        exit(1);
    }

    srand(time(NULL)); // Inicializa o tempo para servir de seed para valores diferentes a cada execução

    // Faz o cálculo do endereço:
    valor_hash = calcular_endereco(s);

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

        // Como ID de TNo é um array, não pode receber direto s porque s é um ponteiro para array de char, portanto,
        // é necessário que copiemos esse ponteiro para ser inserido no ID semelhantemente realizado no léxico (entretanto,
        // lá era usado para reescrever o lexema analisado enquanto aqui é para inserir em ID)
        strncpy(no_a_inserir->ID, s, tamanho_ID);
        no_a_inserir->ID[tamanho_ID] = '\0';
        no_a_inserir->endereco = valor_hash;
        no->prox = no_a_inserir;
    } else {
        // Se não, já insere direto nesse endereço
        strncpy(no_a_inserir->ID, s, tamanho_ID);
        no_a_inserir->ID[tamanho_ID] = '\0';
        no_a_inserir->endereco = valor_hash;
        tabela.entradas[valor_hash] = no_a_inserir;
    }
    // Após preencher o nó de determinada posição da tabela, ele pega e cria um novo nó para começar inserir caso outro
    // valor caia nessa posição:
    no_a_inserir->prox = NULL;
    
    return valor_hash;
}

// VALIDAR SE PODE INICIAR A TABELA OU SE SÓ PODE O HASHMACK
void inicializa_tabela() {
    /* 
    Insere na tabela que é um vetor vários NULLs. Dentro de hashMack irá inserir na tabela os valores daquela
    posição e criar um novo nó daquela posição na lista. Caso não seja um nó vazio, ele apenas insere no nó vazio
    da lista, mas isso no hashMack
    */
    for (int i = 0; i < PRIME_NUMBER; i++) {
        tabela.entradas[i] = NULL;
    }
}

// Para simplificar, usaremos o método da divisão: H(k) = k (mod m)
int calcular_endereco(char *s) {
    int k = strlen(s) * rand(); // Pega tamanho da letra e multiplica pelo valor de RAND_MAX

    return (k % PRIME_NUMBER);
}