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

// Acima são apenas comentários do mini-sintático do professor

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Definição dos Átomos conforme exigido no Portugol
typedef enum {
    EOS, ERRO, COMENTARIO,
    ALGORITMO, VAR, INICIO, FIM, FUNCAO, PROCEDIMENTO,
    CARACTERE, INTEIRO, LOGICO,
    IDENT, CONSTINT, CONSTCHAR,
    PONTO_VIRGULA, PONTO, VIRGULA, DOIS_PONTOS,
    MAIS, MENOS, MULT, DIV_OP
} TAtomo;

// Estrutura de comunicação Léxico-Sintático
// Ainda não implementado
typedef struct {
    TAtomo atomo;
    int linha;
    union {
        int numero;
        char id[16];
        char ch;
    } atributo;
} TInfoAtomo;

// Alteração das variáveis globais do professor para o sintático
TInfoAtomo lookahead;
int linha = 1;

// Léxico Temporário (só para os testes enquanto n ão ta pronto)
TInfoAtomo obter_atomo() {
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
        case IDENT: return "identificador";
        case PONTO_VIRGULA: return ";";
        case PONTO: return ".";
        case VIRGULA: return ",";
        case DOIS_PONTOS: return ":";
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

// PROTÓTIPOS DAS FUNÇÕES DA GRAMÁTICA
void consome(TAtomo atomo_esperado);
void programa();
void bloco();
void declaracao_variaveis();
void lista_variaveis();
void declaracao_rotinas();
void tipo();
void comando_composto();
void expressao();
void termo();
void fator();

// Função consome adaptada para TAtomo e formato de erro
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

// Chamada pelo símbolo inicial
int main() {
    lookahead = obter_atomo();
    programa(); // Chama o símbolo inicial

    if (lookahead.atomo == EOS) {
        printf("%d linhas analisadas, programa sintaticamente correto\n", lookahead.linha);
    }
    return 0;
}

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
            consome(FUNCAO);
            // restante da regra de função será chamado aqui futuramente
        } else {
            consome(PROCEDIMENTO);
            // restante da regra de procedimento será chamado aqui futuramente
        }
    }
}

// Stubs temporários para permitir a compilação
// GPT pediu pra deixar essa parte por enquanto pra testar e ver se funciona
void tipo() {}
void comando_composto() {}
void expressao() {}
void termo() {}
void fator() {}
