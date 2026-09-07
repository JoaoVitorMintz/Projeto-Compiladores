#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "lexAnaliser.h"

// Variável global
char *buffer;
char lexema[20];
char *ponteiro;
int linha;

// Variáveis temporárias para teste
FILE *arquivo;

TAtomo reconhece_numero(TInfoAtomo info) {
    char *ini_lexema = buffer;

    if(isdigit(*buffer)) {
        buffer++;
        goto q1;
    }
    return ERRO;
    
    q1:
        if (isdigit(*buffer)) {
            buffer++;
            goto q1;
        }
        if (*buffer == '.') {
            buffer++;
            goto q2;
        }
        return ERRO;
    
    q2:
        if (isdigit(*buffer)) {
            buffer++;
            goto q3;
        }
        return ERRO;
    
    q3:
        if (isdigit(*buffer)) {
            buffer++;
            goto q3;
        }
        if(isalpha(*buffer)) {
            return ERRO;
        }
    
    // Recorta lexema
    strncpy(lexema, ini_lexema, buffer-ini_lexema);
    lexema[buffer-ini_lexema] = '\0';

    return NUM;
}

TAtomo reconhece_id(TInfoAtomo info) {
    char *ini_lexema = buffer;

    if(islower(*buffer)) {
        buffer++;
        goto q1;
    }
    return ERRO;
    
    q1:
        if (islower(*buffer) || isdigit(*buffer)) {
            buffer++;
            goto q1;
        }
        if(isupper(*buffer)) return ERRO;
        
    
    // Recorta lexema
    strncpy(lexema, ini_lexema, buffer-ini_lexema);
    lexema[buffer-ini_lexema] = '\0';

    return IDENT;
}

// Função para obter o atomo e, dependendo do seu tipo (numero ou palavra), ele OU
// verifica a se o atomo da palavra reservado está certo ou insere novo atomo
// à tabela que será compartilhada entre analisador léxico e sintático
TInfoAtomo obter_atomo() {
    TInfoAtomo info;

    info.atomo = ERRO;
    info.linha = linha;

    while (*buffer == ' ' || *buffer == '\n' || *buffer == '\t' || *buffer == '\r') {
        if (*buffer == '\n' || *buffer == '\t' || *buffer == '\r' || *buffer == ' ') linha++; // Consome esse simbolo

        buffer++;
    }

    if (*buffer == 0) {
        info.atomo = EOS;
    } else if (isdigit(*buffer)) {
        info.atomo = reconhece_numero(info);
    } else if (islower(*buffer)) {
        info.atomo = reconhece_id(info);
    }

    return info;
}

// Função temporária para teste
void iniciar_lexico(FILE *arquivo) {
    linha = 1;

    if (fgets(buffer, sizeof(buffer), arquivo) != NULL) {
        ponteiro = buffer;
    } else {
        ponteiro = NULL;
    }
}