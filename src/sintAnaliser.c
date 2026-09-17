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
    CARACTERE, INTEIRO, LOGICO, LEIA, ESCREVA, SE, ENTAO, SENAO, ENQUANTO, FACA,
    VERDADEIRO, FALSO, NAIO, OU, E, MOD, DIV,
    IDENT, CONSTINT, CONSTCHAR,
    PONTO_VIRGULA, PONTO, VIRGULA, DOIS_PONTOS,
    MAIS, MENOS, MULT, DIV_OP, ATRIB, ABRE_PAR, FECHA_PAR,
    IGUAL, DIFERENTE, MENOR, MENOR_IGUAL, MAIOR, MAIOR_IGUAL
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
        case ENTAO: return "entao";
        case SENAO: return "senao";
        case ENQUANTO: return "enquanto";
        case FACA: return "faca";
        case CARACTERE: return "caractere";
        case INTEIRO: return "inteiro";
        case LOGICO: return "logico";
        case VERDADEIRO: return "verdadeiro";
        case FALSO: return "falso";
        case NAIO: return "nao";
        case OU: return "ou";
        case E: return "e";
        case MOD: return "mod";
        case DIV: return "div";
        case IDENT: return "identificador";
        case CONSTINT: return "constint";
        case CONSTCHAR: return "constchar";
        case PONTO_VIRGULA: return ";";
        case PONTO: return ".";
        case VIRGULA: return ",";
        case DOIS_PONTOS: return ":";
        case ATRIB: return ":=";
        case ABRE_PAR: return "(";
        case FECHA_PAR: return ")";
        case IGUAL: return "=";
        case DIFERENTE: return "<>";
        case MENOR: return "<";
        case MENOR_IGUAL: return "<=";
        case MAIOR: return ">";
        case MAIOR_IGUAL: return ">=";
        case MAIS: return "+";
        case MENOS: return "-";
        case MULT: return "*";
        case DIV_OP: return "/";
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
void programa(void);
void bloco(void);
void declaracao_variaveis(void);
void lista_variaveis(void);
void declaracao_rotinas(void);
void declaracao_funcao(void);
void declaracao_procedimento(void);
void tipo(void);
void parametros_formais(void);
void parametro_formal(void);
void comando_composto(void);
void comando(void);
void comando_atribuicao(void);
void comando_entrada(void);
void comando_saida(void);
void comando_condicional(void);
void comando_repeticao(void);
void lista_expressao(void);
void expressao(void);
void operador_relacional(void);
void expressao_simples(void);
void operador_adicao(void);
void termo(void);
void operador_multiplicacao(void);
void fator(void);

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

int main(void) {
    lookahead = obter_atomo();
    programa();

    if (lookahead.atomo == EOS) {
        printf("%d linhas analisadas, programa sintaticamente correto\n", lookahead.linha);
    }
    return 0;
}

// IMPLEMENTAÇÃO DAS REGRAS

// 1. <programa> ::= algoritmo identificador ';' <bloco> '.'
void programa(void) {
    consome(ALGORITMO);
    consome(IDENT);
    consome(PONTO_VIRGULA);
    bloco();
    consome(PONTO);
}

// 2. <bloco> ::= <declaração_variáveis> <declaração_de_rotinas> <comando_composto>
void bloco(void) {
    declaracao_variaveis();
    declaracao_rotinas();
    comando_composto();
}

// 3. <declaração_variáveis> ::= [ var <lista_variaveis> ';' { <lista_variaveis> ';' } ]
void declaracao_variaveis(void) {
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
void lista_variaveis(void) {
    consome(IDENT);
    while (lookahead.atomo == VIRGULA) {
        consome(VIRGULA);
        consome(IDENT);
    }
    consome(DOIS_PONTOS);
    tipo();
}

// 5. <declaracao_de_rotinas> ::= { <declaração_de_função> | <declaração_de_procedimento> }
void declaracao_rotinas(void) {
    while (lookahead.atomo == FUNCAO || lookahead.atomo == PROCEDIMENTO) {
        if (lookahead.atomo == FUNCAO) {
            declaracao_funcao();
        } else {
            declaracao_procedimento();
        }
    }
}

// 6. <declaração_de_função>
void declaracao_funcao(void) {
    consome(FUNCAO);
    tipo();
    consome(IDENT);
    parametros_formais();
    declaracao_variaveis();
    comando_composto();
}

// 7. <declaracao_de_procedimento>
void declaracao_procedimento(void) {
    consome(PROCEDIMENTO);
    consome(IDENT);
    parametros_formais();
    declaracao_variaveis();
    comando_composto();
}

// 8. <tipo> ::= caractere | inteiro | logico
void tipo(void) {
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
void parametros_formais(void) {
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
void parametro_formal(void) {
    if (lookahead.atomo == VAR) {
        consome(VAR);
    }
    lista_variaveis();
}

// 11. <comando_composto> ::= inicio <comando> { ';' <comando> } fim
void comando_composto(void) {
    consome(INICIO);
    comando();
    while (lookahead.atomo == PONTO_VIRGULA) {
        consome(PONTO_VIRGULA);
        comando();
    }
    consome(FIM);
}

// 12. <comando>
void comando(void) {
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
void comando_atribuicao(void) {
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

// 14. <comando_entrada> ::= leia '(' identificador { ',' identificador } ')'
void comando_entrada(void) {
    consome(LEIA);
    consome(ABRE_PAR);
    consome(IDENT);
    while (lookahead.atomo == VIRGULA) {
        consome(VIRGULA);
        consome(IDENT);
    }
    consome(FECHA_PAR);
}

// 15. <comando_saida> ::= escreva '(' <lista_expressao> ')'
void comando_saida(void) {
    consome(ESCREVA);
    consome(ABRE_PAR);
    lista_expressao();
    consome(FECHA_PAR);
}

// 16. <comando_condicional> ::= se <expressao> entao <comando> [ senao <comando> ]
void comando_condicional(void) {
    consome(SE);
    expressao();
    consome(ENTAO);
    comando();
    if (lookahead.atomo == SENAO) {
        consome(SENAO);
        comando();
    }
}

// 17. <comando_repeticao> ::= enquanto <expressao> faca <comando>
void comando_repeticao(void) {
    consome(ENQUANTO);
    expressao();
    consome(FACA);
    comando();
}

// 18. <lista_expressao> ::= <expressao> { ',' <expressao> }
void lista_expressao(void) {
    expressao();
    while (lookahead.atomo == VIRGULA) {
        consome(VIRGULA);
        expressao();
    }
}

// 19. <expressao> ::= <expressao_simples> [ <operador_relacional> <expressao_simples> ]
void expressao(void) {
    expressao_simples();
    if (lookahead.atomo == IGUAL || lookahead.atomo == DIFERENTE ||
        lookahead.atomo == MENOR || lookahead.atomo == MENOR_IGUAL ||
        lookahead.atomo == MAIOR || lookahead.atomo == MAIOR_IGUAL) {
        operador_relacional();
        expressao_simples();
    }
}

// 20. <operador_relacional> ::= '=' | '<>' | '<' | '<=' | '>' | '>='
void operador_relacional(void) {
    if (lookahead.atomo == DIFERENTE) consome(DIFERENTE);
    else if (lookahead.atomo == MENOR) consome(MENOR);
    else if (lookahead.atomo == MENOR_IGUAL) consome(MENOR_IGUAL);
    else if (lookahead.atomo == MAIOR_IGUAL) consome(MAIOR_IGUAL);
    else if (lookahead.atomo == MAIOR) consome(MAIOR);
    else if (lookahead.atomo == IGUAL) consome(IGUAL);
}

// 21. <expressao_simples> ::= <termo> { <operador_adicao> <termo> }
void expressao_simples(void) {
    termo();
    while (lookahead.atomo == MAIS || lookahead.atomo == MENOS ||
           lookahead.atomo == MOD || lookahead.atomo == OU) {
        operador_adicao();
        termo();
    }
}

// 22. <operador_adicao> ::= '+' | '-' | mod | ou
void operador_adicao(void) {
    if (lookahead.atomo == MAIS) consome(MAIS);
    else if (lookahead.atomo == MENOS) consome(MENOS);
    else if (lookahead.atomo == MOD) consome(MOD);
    else if (lookahead.atomo == OU) consome(OU);
}

// 23. <termo> ::= <fator> { <operador_multiplicacao> <fator> }
void termo(void) {
    fator();
    while (lookahead.atomo == MULT || lookahead.atomo == DIV_OP ||
           lookahead.atomo == DIV || lookahead.atomo == E) {
        operador_multiplicacao();
        fator();
    }
}

// 24. <operador_multiplicacao> ::= '*' | '/' | div | e
void operador_multiplicacao(void) {
    if (lookahead.atomo == MULT) consome(MULT);
    else if (lookahead.atomo == DIV_OP) consome(DIV_OP);
    else if (lookahead.atomo == DIV) consome(DIV);
    else if (lookahead.atomo == E) consome(E);
}

// 25. <fator> ::= identificador [ '(' <lista_expressao> ')' ] | constint | constchar |
//                 '(' <expressao> ')' | ( '+' | '-' | nao ) <fator> | verdadeiro | falso
void fator(void) {
    if (lookahead.atomo == IDENT) {
        consome(IDENT);
        if (lookahead.atomo == ABRE_PAR) {
            consome(ABRE_PAR);
            lista_expressao();
            consome(FECHA_PAR);
        }
    } else if (lookahead.atomo == CONSTINT) {
        consome(CONSTINT);
    } else if (lookahead.atomo == CONSTCHAR) {
        consome(CONSTCHAR);
    } else if (lookahead.atomo == ABRE_PAR) {
        consome(ABRE_PAR);
        expressao();
        consome(FECHA_PAR);
    } else if (lookahead.atomo == MAIS || lookahead.atomo == MENOS || lookahead.atomo == NAIO) {
        if (lookahead.atomo == MAIS) consome(MAIS);
        else if (lookahead.atomo == MENOS) consome(MENOS);
        else consome(NAIO);
        fator();
    } else if (lookahead.atomo == VERDADEIRO) {
        consome(VERDADEIRO);
    } else if (lookahead.atomo == FALSO) {
        consome(FALSO);
    } else {
        printf("#%d: erro sintatico, esperado [fator] encontrado [%s]\n", 
               lookahead.linha, nome_atomo(lookahead.atomo));
        exit(1);
    }
}
