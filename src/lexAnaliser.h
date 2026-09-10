#ifndef LEXANALISER_H
#define LEXANALISER_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h> // atof

// Definição de TOKENS/ATOMOS:
typedef enum {
    ERRO,
    NUM,
    IDENT,
    CONSTCHAR, // Específico do portugol
    CONSTINT, // Específico do portugol
    EOS,

    MAIS, // +
    MENOS, // -
    MULT, // *
    DIV, // /
    MAIOR, // >
    MENOR, // <
    IGUAL, // =
    ATRIB, // :=

    ABRE_PAR, // (
    FECHA_PAR, // )
    ABRE_CHAVE, // {
    FECHA_CHAVE, // }
    ABRE_COM, // {-
    FECHA_COM, // -}

    INICIO,
    SE,
    SENAO,
    ENQUANTO,
    PARA,
    FUNCAO
} TAtomo;

typedef struct {
    TAtomo atomo;
    int linha;
    union {
        int numero; // atributo do átomo constint (constante inteira)
        char id[16]; // atributo identificador
        char ch; // atributo do átomo constchar (constante caractere)
    } atributo;
} TInfoAtomo;

TInfoAtomo obter_atomo(void); // Implementado no analisador léxico
void reconhece_numero(TInfoAtomo *info_atomo);
void reconhece_id(TInfoAtomo *info_atomo);
void iniciar_lexico(FILE *arquivo);

#endif