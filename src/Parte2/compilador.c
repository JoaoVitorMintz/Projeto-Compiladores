/*
Alunos:
João Vitor Garcia Aguiar Mintz 10440421
Giovanni Barreiro G. de Castro 10435745
*/


#include <stdio.h>
#include <stdlib.h>
#include "lexAnaliser.h"

// Declara a função principal do Sintático
void iniciar_sintatico(FILE *arq);
void iniciar_MEPA(FILE *arq);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("ERRO: Informe o arquivo de entrada. Ex: ./compilador entrada.txt\n");
        return 1;
    }

    FILE *arquivo_fonte = fopen(argv[1], "r");
    if (arquivo_fonte == NULL) {
        perror("ERRO: Nao foi possivel abrir o arquivo");
        return 1;
    }

    // Chama a função no Sintatico
    iniciar_sintatico(arquivo_fonte);

    // Chama a função do MEPA
    iniciar_MEPA(arquivo_fonte);

    fclose(arquivo_fonte);
    return 0;
}
