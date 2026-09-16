/*
Gramatica na notacao BNF
<expressao>::=<expressao>'+'<termo>|<expressão>'-'<termo>|<termo>
<termo>::=<termo> '*'<fator>|<termo>'/'<fator>|<fator>
<fator>::='a'|'b'|'c'|...|'1'|'2'|'3'|...|'('<expressão>')'

Converter para notacao EBNF
fatorar a esquerda
<expressao>::=<expressao> ('+'<termo>|'-'<termo>) <termo>

fatorar a direita
<expressao>::= <expressao> ('+'|'-') <termo> !  <termo>

eliminando a recursividade a esquerda
<expressao>::= <termo> {('+'|'-') <termo>}

<termo>::=<termo> ('*'|'/') <fator>|<fator>

Gramatica notacao EBNF
<expressao>::= <termo> {('+'|'-') <termo>}
<termo>::=<fator> {('*'|'/') <fator>}
<fator>::='a'|'b'|'c'|...|'1'|'2'|'3'|...|'('<expressão>')'

gcc ASDR3.c -o ASDR3
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Definição Completa dos Átomos conforme exigido no Portugol
typedef enum {
    EOS, ERRO, COMENTARIO,
    ALGORITMO, VAR, INICIO, FIM, FUNCAO, PROCEDIMENTO,
    CARACTERE, INTEIRO, LOGICO, LEIA, ESCREVA, SE, ENQUANTO,
    IDENT, CONSTINT, CONSTCHAR,
    PONTO_VIRGULA, PONTO, VIRGULA, DOIS_PONTOS,
    MAIS, MENOS, MULT, DIV_OP, ATRIB, ABRE_PAR, FECHA_PAR
} TAtomo;

// Estrutura de comunicação Léxico-Sintático
typedef struct {
    TAtomo atomo;
    int linha;
    union {
        int numero;
        char id[16];
        char ch;
    } atributo;
} TInfoAtomo;

// Variáveis globais do sintático
TInfoAtomo lookahead;
int linha = 1;

// Léxico Temporário
TInfoAtomo obter_atomo(void) {
    TInfoAtomo info;
    info.atomo = EOS;
    info.linha = linha;
    return info;
}

// Auxiliar para mensagens de erro sintático
const char* nome_atomo(TAtomo atomo) {
    switch(atomo) {
        case ALGORITMO: return "algoritmo";
        case VAR: return "var";
        case INICIO: return "inicio";
        case FIM: return "fim";
        case FUNCAO: return "funcao";
        case PROCEDIMENTO: return "procedimento";
        case LEIA: return "leia";
        case ESCREVA: return "escreva";
        case SE: return "se";
        case ENQUANTO: return "enquanto";
        case IDENT: return "identificador";
        case PONTO_VIRGULA: return ";";
        case PONTO: return ".";
        case VIRGULA: return ",";
        case DOIS_PONTOS: return ":";
        case ATRIB: return ":=";
        case ABRE_PAR: return "(";
        case FECHA_PAR: return ")";
        case EOS: return "fim de arquivo";
        default: return "simbolo";
    }
}

void imprime_atomo(TInfoAtomo info) {
    printf("#%d:", info.linha);
    if (info.atomo == IDENT) {
        printf(" identificador: %s\n", info.atributo.id);
    } else {
        printf(" %s\n", nome_atomo(info.atomo));
    }
}

// PROTÓTIPOS DE TODAS AS FUNÇÕES DA GRAMÁTICA
void consome(TAtomo atomo_esperado);
void programa();
void bloco();
void declaracao_variaveis();
void lista_variaveis();
void declaracao_rotinas();
void declaracao_funcao();
void declaracao_procedimento();
void tipo();
void parametros_formais();
void parametro_formal();
void comando_composto();
void comando();
void comando_atribuicao();
void comando_entrada();
void comando_saida();
void comando_condicional();
void comando_repeticao();
void lista_expressao();
void expressao();
void termo();
void fator();

// Função consome adaptada
void consome(TAtomo atomo_esperado) {
    if (lookahead.atomo == atomo_esperado) {
        imprime_atomo(lookahead);
        do {
            lookahead = obter_atomo();
        } while (lookahead.atomo == COMENTARIO);
    } else {
        printf("#%d: erro sintatico, esperado [%s] encontrado [%s]\n",
               lookahead.linha, nome_atomo(atomo_esperado), nome_atomo(lookahead.atomo));
        exit(1);
    }
}

int main() {
    lookahead = obter_atomo();
    programa();

    if (lookahead.atomo == EOS) {
        printf("%d linhas analisadas, programa sintaticamente correto\n", lookahead.linha);
    }
    return 0;
}

// IMPLEMENTAÇÃO DAS REGRAS IMPLEMENTADAS ATÉ AGORA

// 1. <programa> ::= algoritmo identificador ';' <bloco> '.'
void programa() {
    consome(ALGORITMO);
    consome(IDENT);
    consome(PONTO_VIRGULA);
    bloco();
    consome(PONTO);
}

// 2. <bloco> ::= <declaração_variáveis> <declaração_de_rotinas> <comando_composto>
void bloco() {
    declaracao_variaveis();
    declaracao_rotinas();
    comando_composto();
}

// 3. <declaração_variáveis> ::= [ var <lista_variaveis> ';' { <lista_variaveis> ';' } ]
void declaracao_variaveis() {
    if (lookahead.atomo == VAR) {
        consome(VAR);
        lista_variaveis();
        consome(PONTO_VIRGULA);
        while (lookahead.atomo == IDENT) {
            lista_variaveis();
            consome(PONTO_VIRGULA);
        }
    }
}

// 4. <lista_variaveis> ::= identificador { ',' identificador } ':' <tipo>
void lista_variaveis() {
    consome(IDENT);
    while (lookahead.atomo == VIRGULA) {
        consome(VIRGULA);
        consome(IDENT);
    }
    consome(DOIS_PONTOS);
    tipo();
}

// 5. <declaracao_de_rotinas> ::= { <declaração_de_função> | <declaração_de_procedimento> }
void declaracao_rotinas() {
    while (lookahead.atomo == FUNCAO || lookahead.atomo == PROCEDIMENTO) {
        if (lookahead.atomo == FUNCAO) {
            declaracao_funcao();
        } else {
            declaracao_procedimento();
        }
    }
}

// 6. <declaração_de_função>
void declaracao_funcao() {
    consome(FUNCAO);
    tipo();
    consome(IDENT);
    parametros_formais();
    declaracao_variaveis();
    comando_composto();
}

// 7. <declaracao_de_procedimento>
void declaracao_procedimento() {
    consome(PROCEDIMENTO);
    consome(IDENT);
    parametros_formais();
    declaracao_variaveis();
    comando_composto();
}

// 8. <tipo> ::= caractere | inteiro | logico
void tipo() {
    if (lookahead.atomo == CARACTERE) {
        consome(CARACTERE);
    } else if (lookahead.atomo == INTEIRO) {
        consome(INTEIRO);
    } else if (lookahead.atomo == LOGICO) {
        consome(LOGICO);
    } else {
        printf("#%d: erro sintatico, esperado [tipo] encontrado [%s]\n", 
               lookahead.linha, nome_atomo(lookahead.atomo));
        exit(1);
    }
}

// 9. <parâmetros_formais>
void parametros_formais() {
    consome(ABRE_PAR);
    if (lookahead.atomo == VAR || lookahead.atomo == IDENT) {
        parametro_formal();
        while (lookahead.atomo == PONTO_VIRGULA) {
            consome(PONTO_VIRGULA);
            parametro_formal();
        }
    }
    consome(FECHA_PAR);
}

// 10. <parâmetro_formal> ::= [var] <lista_variaveis>
void parametro_formal() {
    if (lookahead.atomo == VAR) {
        consome(VAR);
    }
    lista_variaveis();
}

// 11. <comando_composto> ::= inicio <comando> { ';' <comando> } fim
void comando_composto() {
    consome(INICIO);
    comando();
    while (lookahead.atomo == PONTO_VIRGULA) {
        consome(PONTO_VIRGULA);
        comando();
    }
    consome(FIM);
}

// 12. <comando>
void comando() {
    if (lookahead.atomo == LEIA) {
        comando_entrada();
    } else if (lookahead.atomo == ESCREVA) {
        comando_saida();
    } else if (lookahead.atomo == SE) {
        comando_condicional();
    } else if (lookahead.atomo == ENQUANTO) {
        comando_repeticao();
    } else if (lookahead.atomo == INICIO) {
        comando_composto();
    } else if (lookahead.atomo == IDENT) {
        comando_atribuicao();
    }
}

// 13. <comando_atribuição>
void comando_atribuicao() {
    consome(IDENT);
    if (lookahead.atomo == ATRIB) {
        consome(ATRIB);
        expressao();
    } else if (lookahead.atomo == ABRE_PAR) {
        consome(ABRE_PAR);
        lista_expressao();
        consome(FECHA_PAR);
    }
}

// Stubs temporários para as funções pendentes
void comando_entrada() {}
void comando_saida() {}
void comando_condicional(void) {}
void comando_repeticao(void) {}
void lista_expressao(void) {}
void expressao(void) {}
void termo(void) {}
void fator(void) {}
