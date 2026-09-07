#ifndef TABLE_H
#define TABLE_H

typedef struct {
    int id;
    char nome[16];
} Simbolo;

void iniciarlizar_tabela();
int inserir_simbolo(char *nome);
Simbolo buscar_simbolo(char *nome);

#endif