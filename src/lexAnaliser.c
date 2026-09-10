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

// Versão constint -> digito+((E(+|ε)digito+)|ε):
void reconhece_numero(TInfoAtomo *info) {
    char *ini_lexema = buffer;
    info->atomo = buffer;

    if (isdigit(*buffer)) {
        buffer++;
        goto q1;
    }
    return;

    q1:
        if (isdigit(*buffer)) {
            buffer++;
            goto q1;
        }
        if (*buffer == 'E') {
            buffer++;
            goto q2;
        }
        return;
    
    q2:
        if (*buffer == '+') {
            buffer++;
        }

        if (isdigit(*buffer)) {
            buffer++;
            goto q3;
        }
        return;
    
    q3:
        if (idigit(*buffer)) {
            buffer++;
            goto q3;
        }

        // Recorta lexema
        strncpy(lexema, ini_lexema, buffer-ini_lexema);
        lexema[buffer-ini_lexema] = '\0';
        info->atomo = CONSTINT;
        info->atributo.numero = atoi(lexema); // Transforma texto em inteiro

        return;
}

// Versão numero -> digito+.digito+:
// void reconhece_numero(TInfoAtomo *info) {
//     char *ini_lexema = buffer;
//     info->atomo = ERRO;

//     if(isdigit(*buffer)) {
//         buffer++;
//         goto q1;
//     }
//     return;
    
//     q1:
//         if (isdigit(*buffer)) {
//             buffer++;
//             goto q1;
//         }
//         if (*buffer == '.') {
//             buffer++;
//             goto q2;
//         }
//         return;
    
//     q2:
//         if (isdigit(*buffer)) {
//             buffer++;
//             goto q3;
//         }
//         return;
    
//     q3:
//         if (isdigit(*buffer)) {
//             buffer++;
//             goto q3;
//         }
//         if(isalpha(*buffer)) {
//             return;
//         }
    
//     // Recorta lexema
//     strncpy(lexema, ini_lexema, buffer-ini_lexema);
//     lexema[buffer-ini_lexema] = '\0';
//     info->atomo = NUM;
//     info->atributo.numero = atof(lexema);

//     return;
// }

void reconhece_id(TInfoAtomo *info) {
    char *ini_lexema = buffer;
    info->atomo = ERRO;

    if(islower(*buffer)) {
        buffer++;
        goto q1;
    }
    return;
    
    q1:
        if (islower(*buffer) || isdigit(*buffer)) {
            buffer++;
            goto q1;
        }
        if(isupper(*buffer)) return;
        
    
    // Recorta lexema
    strncpy(lexema, ini_lexema, buffer-ini_lexema);
    info->atributo.id[buffer-ini_lexema] = '\0';
    info->atomo = IDENT;

    return;
}

// Função para obter o atomo e, dependendo do seu tipo (numero ou palavra), ele OU
// verifica a se o atomo da palavra reservado está certo ou insere novo atomo
// à tabela que será compartilhada entre analisador léxico e sintático
TInfoAtomo obter_atomo(void) {
    TInfoAtomo info;

    info.atomo = ERRO;

    // Elimina espaços e faz a contagem de linhas
    while (*buffer == ' ' || *buffer == '\n' || *buffer == '\t' || *buffer == '\r') {
        if (*buffer == '\n') linha++; // Consome esse simbolo

        buffer++;
    }

    info.linha = linha;

    // Obtenção de cada atomo:
    if (*buffer == 0) { // final de buffer
        info.atomo = EOS;
    } else if (isdigit(*buffer)) { // Reconhece numero
        reconhece_numero(&info);
    } else if (islower(*buffer)) { // Reconhece id
        reconhece_id(&info);
    } else if (*buffer == '*') {
        info.atomo = MULT;
        buffer++;
    } else if (*buffer == '+') {
        info.atomo = MAIS;
        buffer++;
    } else if (*buffer == '-') {
        info.atomo = MENOS;
        buffer++;
    } else if (*buffer == '/') {
        info.atomo = DIV;
        buffer++;
    } else if (*buffer == '>') {
        info.atomo = MAIOR;
        buffer++;
    } else if (*buffer == '<') {
        info.atomo = MENOR;
        buffer++;
    } else if (*buffer == '=') {
        info.atomo = IGUAL;
        buffer++;
    } else if (*buffer == ':' && *(buffer + 1) == '=') {
        info.atomo = ATRIB;
        buffer++;
    } else if (*buffer == '(') {
        info.atomo = ABRE_PAR;
        buffer++;
    } else if (*buffer == ')') {
        info.atomo = FECHA_PAR;
        buffer++;
    } else if (*buffer == '{') {
        info.atomo = ABRE_CHAVE;
        buffer++;
    } else if (*buffer == '}') {
        info.atomo = FECHA_CHAVE;
        buffer++;
    } else if (*buffer == '{' && *(buffer + 1) == '-') {
        info.atomo = ABRE_COM;
        buffer++;
    } else if (*buffer == '-' && *(buffer + 1) == '}') {
        info.atomo = FECHA_COM;
        buffer++;
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