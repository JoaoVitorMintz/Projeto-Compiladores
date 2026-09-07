#ifndef LEXANALISER_H
#define LEXANALISER_H

// Definição de TOKENS/ATOMOS:
typedef enum {
    ERRO,
    NUM,
    IDENT,
    CONSTCHAR, // Específico do portugol
    EOS,
    MAIS,
    MENOS,
    MULT,
    DIV,
    ABRE_PAR,
    FECHA_PAR,
    ABRE_CHAVE,
    FECHA_CHAVE,
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

TInfoAtomo obter_atomo(); // Implementado no analisador léxico
void iniciar_lexico(FILE *arquivo);

#endif