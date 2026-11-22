#ifndef TABELA_SIMBOLO_H
#define TABELA_SIMBOLO_H

// Trecho que o professor solicita que seja implementado no nosso programa fonte:
#define PRIME_NUMBER 211  // numero primo para reduzir colisoes  

// Semelhante ao que foi feito para o Analisador Léxico
typedef enum { // S de simbolo
   S_VARIAVEL,
   S_PARAMETRO,
   S_FUNCAO,
   S_PROCEDIMENTO
} TCategoria;

typedef enum { // T de tipo
   T_INTEIRO,
   T_CARACTERE,
   T_LOGICO,
   T_NAO_DEFINIDO
} TTipo;

typedef struct _TNo{ 
   char ID[16]; 
   int endereco;
   TCategoria categoria;
   TTipo tipo;
   int escopo; // 0 é global enquanto 1 é local
   int num_parametros;
   struct _TNo *prox; 
} TNo;

typedef struct { 
    TNo *entradas[PRIME_NUMBER]; 
} TTabelaSimbolos;

void inserir_tabela(char *s, int endereco, TCategoria categoria, TTipo tipo, int escopo);
void inicializa_tabela();
void liberar_tabela();
TNo* buscar_simbolo(char *s);
void imprimir_tabela();
void atualizar_no_tipo(char *s, TTipo tipo);

char *imprimir_categoria(TCategoria categoria);
char *imprimir_tipo(TTipo tipo);
char *imprimir_escopo(int escopo);

#endif
