#include <stdlib.h>
#include <stdio.h>

#include "lexAnaliser.h"

// gcc -Wall -Wno-unused-result -g -Og compilador.c -o compilador 

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("ERRO: Deve ser definido apenas um arquivo\n");
        return 0;
    }

    FILE *arquivo = fopen(argv[1], "r");

    if (arquivo == NULL) {
        perror("ERRO: Nao foi possivel abrir o arquivo");
        return 0;
    }

    // TRECHO SERÁ REMOVIDO POSTERIORMENTE, APENAS TESTE DO LÉXICO:
    iniciar_lexico(arquivo);

    TInfoAtomo info;
    do {
        info = obter_atomo();

        printf("Linha %d: token = %d\n", info.linha, info.atomo);
    } while (info.atomo != EOS && info.atomo != ERRO);

    fclose(arquivo);

    return 0;
}