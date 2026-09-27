#ifndef LEXANALISER_H
#define LEXANALISER_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h> 

// Definição de TOKENS/ATOMOS:
typedef enum {
    ERRO,
    IDENT,
    CONSTCHAR, 
    CONSTINT, 
    EOS,

    MAIS,        // +
    MENOS,       // -
    MULT,        // *
    DIV_OP,      // /
    MAIOR,       // >
    MENOR,       // <
    IGUAL,       // =
    ATRIB,       // :=
    MAIOR_IGUAL, // >=
    MENOR_IGUAL, // <=
    DIFERENTE,   // <>

    ABRE_PAR,    // (
    FECHA_PAR,   // )
    ABRE_CHAVE,  // {
    FECHA_CHAVE, // }
    COMENTARIO,  
    PONTO_VIRGULA, // ;
    PONTO,        // .
    DOIS_PONTOS,  // :
    VIRGULA,      // ,

    ALGORITMO,
    CARACTERE,
    DIV,
    E,
    ENQUANTO,
    ENTAO,
    ESCREVA,
    FACA,
    FALSO,
    FIM,
    FUNCAO,
    INICIO,
    INTEIRO,
    LEIA,
    LOGICO,
    MOD,
    NAO,         
    OU,
    PROCEDIMENTO,
    SE,
    SENAO,
    VAR,
    VERDADEIRO
} TAtomo;

typedef struct {
    TAtomo atomo;
    int linha;
    union {
        int numero; 
        char id[16]; 
        char ch; 
    } atributo;
} TInfoAtomo;

TInfoAtomo obter_atomo(void); 
void reconhece_numero(TInfoAtomo *info_atomo);
void reconhece_id(TInfoAtomo *info_atomo);
void iniciar_lexico(char *buffer_linha, int i);

#endif
