#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "lexAnaliser.h"

// Variável global
char *buffer;
char lexema[20];
int linha;


// Função para obter o atomo e, dependendo do seu tipo (numero ou palavra), ele OU
// verifica a se o atomo da palavra reservado está certo ou insere novo atomo
// à tabela que será compartilhada entre analisador léxico e sintático
TInfoAtomo obter_atomo(char* atomo) {
    TInfoAtomo info;

    info.atomo = ERRO;

    while (*buffer == ' ' || *buffer == '\n') {
        if (*buffer == '\n') linha++; // Consome esse simbolo

        buffer++;
    }

    if (*buffer == 0) {
        info.atomo = EOS;
    } else if (isdigit(*buffer)) {
        info.atomo = reconhece_numero();
    } else if (islower(*buffer)) {
        info.atomo = reconhece_id();
    }

    return info;
}