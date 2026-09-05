#ifndef LEXANALISER_H
#define LEXANALISER_H

typedef enum {
    ERRO,
    NUM,
    IDENT,
    EOS
} TAtomo;

typedef struct {
    TAtomo atomo;
    int linha;
    union {
        float numero;
        char ID[16];
    } atributo;
} TInfoAtomo;

TInfoAtomo obter_atomo();

#endif