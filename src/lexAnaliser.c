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

// Para comentários multi-linha, variável criada para salvar se o comentário foi
// aberto, porém, ainda não foi fechado
int comentario_aberto = 0;

// Variáveis temporárias para teste
FILE *arquivo;

// Funções criadas:
void reconhece_numero(TInfoAtomo *info);
void reconhece_id(TInfoAtomo *info);
void reconhece_reservada(TInfoAtomo *info);
void reconhece_constchar(TInfoAtomo *info);
void reconhece_comentari(TInfoAtomo *info);

// Versão constint -> digito+((E(+|ε)digito+)|ε):
void reconhece_numero(TInfoAtomo *info) {
    char *ini_lexema = buffer;
    info->atomo = ERRO;

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

        if (toupper(*buffer) == 'E') {
            buffer++;
            goto q2;
        }
        goto q4;
    
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
        if (isdigit(*buffer)) {
            buffer++;
            goto q3;
        }

    q4:
        // Recorta lexema
        strncpy(lexema, ini_lexema, buffer-ini_lexema);
        lexema[buffer-ini_lexema] = '\0';
        info->atomo = CONSTINT;

        // Se for por exemplo: 12E2 ou 12E+2, precisa calcular para inserir em atributo.numero
        char *pos_E = strchr(lexema, 'E'); // Localiza primeira ocorrencia de E
        if (pos_E != NULL) {
            *pos_E = '\0'; // Corta string em Base e Expoente

            int base = atoi(lexema); // Parte antes do E
            char *exp_str = pos_E + 1; // Parte depois do E
            if (*exp_str == '+') exp_str++;
            int expoente = atoi(exp_str);

            int potencia = 1;
            for (int i = 0; i < expoente; i++) {
                potencia *= 10;
            }
            info->atributo.numero = base * potencia;
        } else { // Senão, só atribui inteiro para numero
            info->atributo.numero = atoi(lexema); // Transforma String em número 
        }
        
        return;
}

// Versão indetifier -> letra(letra|_|digito)*
void reconhece_id(TInfoAtomo *info) {
    char *ini_lexema = buffer;
    info->atomo = ERRO;

    if (isalpha(*buffer)) {
        buffer++;
        goto q1;
    }
    return;

    q1:
        if(*buffer == '_') {
            buffer++;
            goto q1;
        } else if (isalpha(*buffer)) {
            buffer++;
            goto q1;
        } else if (isdigit(*buffer)) {
            buffer++;
            goto q1;
        }
    
        int tamanho = buffer - ini_lexema;

        // Verifica se é menor que 15
        if (tamanho > 15) {
            info->atomo = ERRO;
            return;
        }

        // Recorta lexema
        strncpy(info->atributo.id, ini_lexema, buffer-ini_lexema);
        info->atributo.id[buffer-ini_lexema] = '\0';
        info->atomo = IDENT;

        // Não é case sensitive, então, deixar tudo minusculo
        for (int i = 0; i < tamanho; i++) {
            info->atributo.id[i] = tolower(info->atributo.id[i]);
        }

        reconhece_reservada(info);

        return;
}

// Aproveitando o reconhece_id para verificar se o identificador coletado é
// palavra chave ou não
void reconhece_reservada(TInfoAtomo *info) {
    if (strcmp(info->atributo.id, "algoritmo") == 0) {
        info->atomo = ALGORITMO;
    } else if (strcmp(info->atributo.id, "caractere") == 0) {
        info->atomo = CARACTERE;
    } else if (strcmp(info->atributo.id, "div") == 0) {
        info->atomo = DIV;
    } else if (strcmp(info->atributo.id, "e") == 0) {
        info->atomo = E;
    } else if (strcmp(info->atributo.id, "enquanto") == 0) {
        info->atomo = ENQUANTO;
    } else if (strcmp(info->atributo.id, "entao") == 0) {
        info->atomo = ENTAO;
    } else if (strcmp(info->atributo.id, "escreva") == 0) {
        info->atomo = ESCREVA;
    } else if (strcmp(info->atributo.id, "faca") == 0) {
        info->atomo = FACA;
    } else if (strcmp(info->atributo.id, "falso") == 0) {
        info->atomo = FALSO;
    } else if (strcmp(info->atributo.id, "fim") == 0) {
        info->atomo = FIM;
    } else if (strcmp(info->atributo.id, "função") == 0) {
        info->atomo = FUNCAO;
    } else if (strcmp(info->atributo.id, "inicio") == 0) {
        info->atomo = INICIO;
    } else if (strcmp(info->atributo.id, "inteiro") == 0) {
        info->atomo = INTEIRO;
    } else if (strcmp(info->atributo.id, "leia") == 0) {
        info->atomo = LEIA;
    } else if (strcmp(info->atributo.id, "logico") == 0) {
        info->atomo = LOGICO;
    } else if (strcmp(info->atributo.id, "mod") == 0) {
        info->atomo = MOD;
    } else if (strcmp(info->atributo.id, "ou") == 0) {
        info->atomo = OU;
    } else if (strcmp(info->atributo.id, "procedimento") == 0) {
        info->atomo = PROCEDIMENTO;
    } else if (strcmp(info->atributo.id, "se") == 0) {
        info->atomo = SE;
    } else if (strcmp(info->atributo.id, "senao") == 0) {
        info->atomo = SENAO;
    } else if (strcmp(info->atributo.id, "var") == 0) {
        info->atomo = VAR;
    } else if (strcmp(info->atributo.id, "verdadeiro") == 0) {
        info->atomo = VERDADEIRO;
    } else {
        info->atomo = IDENT;
    }
}

// Reconhece comentário para ignorá-lo
void reconhece_comentario(TInfoAtomo *info) {
    while (*buffer != 0) {
        if (*buffer == '-' && *(buffer + 1) == '}') {
            buffer += 2;
            comentario_aberto = 0;
            info->atomo = COMENTARIO;
            return;
        }
        buffer++;
    }
    // Chegou ao final da linha, mas comentário continua aberto
    info->atomo = EOS; // Especifico para mudar de linha no obter_proximo_token()
}

// Reconhece constchar
void reconhece_constchar(TInfoAtomo *info) {
    info->atomo = ERRO;

    if (*buffer == '\'') {
        buffer++;
        goto q1;
    }
    return;

    q1:
        if (*buffer != '\0' && *buffer != '\'') { // Verifica se não é '' ou '
            char charactere_lido = *buffer;
            buffer++;
            if (*buffer == '\'') {
                buffer++;
                info->atomo = CONSTCHAR;
                info->atributo.ch = charactere_lido;
            }
        }
        return;
}

// Função para obter o atomo e, dependendo do seu tipo (numero ou palavra), ele OU
// verifica a se o atomo da palavra reservado está certo
TInfoAtomo obter_atomo(void) {
    TInfoAtomo info;

    info.atomo = ERRO;

    // Elimina espaços e faz a contagem de linhas
    while (*buffer == ' ' || *buffer == '\n' || *buffer == '\t' || *buffer == '\r') {
        if (*buffer == '\n') linha++; // Consome esse simbolo

        buffer++;
    }

    info.linha = linha;

    // Caso um comentário tenha sido aberto e nunca fechado, ele vai abrir direto
    // no reconhece_comentario() até encontrar o -}
    if (comentario_aberto) {
        reconhece_comentario(&info);

        if (info.atomo == COMENTARIO) {
            return info;
        }
    }

    // Obtenção de cada atomo:
    if (*buffer == 0) { // final de buffer
        info.atomo = EOS;
    } else if (isdigit(*buffer)) { // Reconhece numero
        reconhece_numero(&info);
    } else if (isalpha(*buffer)) { // Reconhece id ou palavra chave
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
    } else if (*buffer == '>' && *(buffer + 1) == '=') {
        info.atomo = MAIOR_IGUAL;
        buffer += 2;
    } else if (*buffer == '<' && *(buffer + 1) == '=') {
        info.atomo = MENOR_IGUAL;
        buffer += 2;
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
        buffer += 2;
    } else if (*buffer == '(') {
        info.atomo = ABRE_PAR;
        buffer++;
    } else if (*buffer == ')') {
        info.atomo = FECHA_PAR;
        buffer++;
    } else if (*buffer == '{' && *(buffer + 1) == '-') {
        buffer += 2;
        comentario_aberto = 1;
        reconhece_comentario(&info);
    } else if (*buffer == '{') {
        info.atomo = ABRE_CHAVE;
        buffer++;
    } else if (*buffer == '}') {
        info.atomo = FECHA_CHAVE;
        buffer++;
    } else if (*buffer == '\'') {
        reconhece_constchar(&info);
    } else if (*buffer == ';') {
        info.atomo = PONTO_VIRGULA;
        buffer++;
    } else if (*buffer == '.') {
        info.atomo = PONTO;
        buffer++;
    } else if (*buffer == ':') {
        info.atomo = DOIS_PONTOS;
        buffer++;
    } else if (*buffer == ',') {
        info.atomo = VIRGULA;
        buffer++;
    } else {
        return info;
    }

    return info;
}

// Função temporária para teste
void iniciar_lexico(char *buffer_linha, int i) {
    linha = i;
    buffer = buffer_linha;
}