#include <stdio.h>
#include <stdlib.h>
#include "lexAnaliser.h"

// Declara a função principal do Sintático
// O nome específico é pedido pelo professor no arquivo
void parse_portugol_internal_v2(FILE *arq);

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
    parse_portugol_internal_v2(arquivo_fonte);

    fclose(arquivo_fonte);
    return 0;
}