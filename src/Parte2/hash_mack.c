// Tabela de simbolos desenvolvida pelos alunos
#include <stdlib.h>
#include <string.h>

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
void liberar_tabela();
int hashMack(char *s);

// Inicio da nossa implementação:
int hashMack(char *s) {
    int valor_hash = 0;
    TTabelaSimbolos tabela;
    // Iniciando como um ponteiro para poder, ao criar uma lista, apontar para outro nó
    TNo *no;

    // Faz o cálculo do endereço:
    valor_hash = calcular_endereco(s);

    // Pega o tamanho do array ID:
    int tamanho_ID = sizeof(no->ID) - 1;

    // Fazemos nó apontar para o primeiro elemento da lista na tabela onde caiu:
    no = tabela.entradas[valor_hash];

    // Se já tiver endereço inserido na tabela, ele entra na lista do endereço até inseri-lo ao final
    if (tabela.entradas[valor_hash] != 0) {

        // Enquanto o valor no endereço 'valor_hash' estiver preenchido, passar de nó em nó até achar onde inserir novo valor
        while (no->prox != 0) {
            no = no->prox;
        }

        // Como ID de TNo é um array, não pode receber direto s porque s é um ponteiro para array de char, portanto,
        // é necessário que copiemos esse ponteiro para ser inserido no ID semelhantemente realizado no léxico (entretanto,
        // lá era usado para reescrever o lexema analisado enquanto aqui é para inserir em ID)
        strncpy(no->ID, s, tamanho_ID);
        no->ID[tamanho_ID] = '\0';
        no->endereco = valor_hash;
    } else {
        // Se não, já insere direto nesse endereço
        strncpy(no->ID, s, tamanho_ID);
        no->ID[tamanho_ID] = '\0';
        no->endereco = valor_hash;
    }
    
    // Após preencher o nó de determinada posição da tabela, ele pega e cria um novo nó para começar inserir caso outro
    // valor caia nessa posição:
    TNo *novoNo = (TNo*)malloc(sizeof(TNo));
    no->prox = novoNo;

    tabela.entradas[valor_hash] = no;
    
    return valor_hash;
}

// Para simplificar, usaremos o método da divisão: H(k) = k (mod m)
int calcular_endereco(char *s) {
    int k = strlen(s);

    return (k % PRIME_NUMBER);
}