#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexAnaliser.h"

// Diretiva exigida pela especificação
int flag_valida_fci_2026 = 1;

// Compatibilidade de enumerações com o lexAnaliser.h
#ifndef PONTO_VIRGULA
#define PONTO_VIRGULA 50
#define PONTO         51
#define VIRGULA       52
#define DOIS_PONTOS   53
#define DIV_OP        54
#define DIFERENTE     55
#define NAIO          56
#endif

// Variáveis de controle de arquivo e estado do Léxico
static FILE *arquivo_fonte = NULL;
static char buffer_linha[1024];
extern int linha;

TInfoAtomo lookahead;

// Protótipos das regras da gramática
void consome(TAtomo atomo_esperado);
const char* nome_atomo(TAtomo atomo);
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
void operador_relacional();
void expressao_simples();
void operador_adicao();
void termo();
void operador_multiplicacao();
void fator();

// Obtém o próximo token tratando mudanças de linha e ignorando comentários
TInfoAtomo obter_proximo_token() {
    TInfoAtomo info = obter_atomo();

    while (1) {
        // Se for um comentário, ignora e busca o próximo
        if (info.atomo == COMENTARIO) {
            info = obter_atomo();
            continue;
        }

        // Se a linha terminou (EOS), carrega a próxima linha do arquivo
        if (info.atomo == EOS) {
            if (arquivo_fonte != NULL && fgets(buffer_linha, sizeof(buffer_linha), arquivo_fonte) != NULL) {
                linha++;
                iniciar_lexico(buffer_linha, linha);
                info = obter_atomo();
                continue;
            }
        }

        break;
    }

    return info;
}

const char* nome_atomo(TAtomo atomo) {
    switch((int)atomo) {
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
        case MENOR: return "<";
        case MENOR_IGUAL: return "<=";
        case MAIOR: return ">";
        case MAIOR_IGUAL: return ">=";
        case MAIS: return "+";
        case MENOS: return "-";
        case MULT: return "*";
        case EOS: return "fim de arquivo";
        default: return "simbolo";
    }
}

void consome(TAtomo atomo_esperado) {
    if (lookahead.atomo == atomo_esperado) {
        // Imprime a saída padrão exigida no trabalho
        if (lookahead.atomo == IDENT) {
            printf("# %d:identificador: %s\n", lookahead.linha, lookahead.atributo.id);
        } else if (lookahead.atomo == CONSTINT) {
            printf("# %d:constint: %d\n", lookahead.linha, lookahead.atributo.numero);
        } else if (lookahead.atomo == CONSTCHAR) {
            printf("# %d:constchar: %c\n", lookahead.linha, lookahead.atributo.ch);
        } else {
            printf("# %d:%s\n", lookahead.linha, nome_atomo(lookahead.atomo));
        }

        lookahead = obter_proximo_token();
    } else {
        printf("# %d:erro sintatico, esperado [%s] encontrado [%s]\n",
               lookahead.linha, nome_atomo(atomo_esperado), nome_atomo(lookahead.atomo));
        exit(1);
    }
}

// Regras gramaticais
void programa() {
    consome(ALGORITMO);
    consome(IDENT);
    consome(PONTO_VIRGULA);
    bloco();
    consome(PONTO);
}

void bloco() {
    declaracao_variaveis();
    declaracao_rotinas();
    comando_composto();
}

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

void lista_variaveis() {
    consome(IDENT);
    while (lookahead.atomo == VIRGULA) {
        consome(VIRGULA);
        consome(IDENT);
    }
    consome(DOIS_PONTOS);
    tipo();
}

void declaracao_rotinas() {
    while (lookahead.atomo == FUNCAO || lookahead.atomo == PROCEDIMENTO) {
        if (lookahead.atomo == FUNCAO) {
            declaracao_funcao();
        } else {
            declaracao_procedimento();
        }
    }
}

void declaracao_funcao() {
    consome(FUNCAO);
    tipo();
    consome(IDENT);
    parametros_formais();
    declaracao_variaveis();
    comando_composto();
}

void declaracao_procedimento() {
    consome(PROCEDIMENTO);
    consome(IDENT);
    parametros_formais();
    declaracao_variaveis();
    comando_composto();
}

void tipo() {
    if (lookahead.atomo == CARACTERE) consome(CARACTERE);
    else if (lookahead.atomo == INTEIRO) consome(INTEIRO);
    else if (lookahead.atomo == LOGICO) consome(LOGICO);
    else {
        printf("# %d:erro sintatico, esperado [tipo] encontrado [%s]\n", 
               lookahead.linha, nome_atomo(lookahead.atomo));
        exit(1);
    }
}

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

void parametro_formal() {
    if (lookahead.atomo == VAR) {
        consome(VAR);
    }
    lista_variaveis();
}

void comando_composto() {
    consome(INICIO);
    comando();
    while (lookahead.atomo == PONTO_VIRGULA) {
        consome(PONTO_VIRGULA);
        comando();
    }
    consome(FIM);
}

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

void comando_atribuicao() {
    consome(IDENT);
    if (lookahead.atomo == ATRIB) {
        consome(ATRIB);
        expressao();
    } else if (lookahead.atomo == ABRE_PAR) {
        consome(ABRE_PAR);
        if (lookahead.atomo != FECHA_PAR) {
            lista_expressao();
        }
        consome(FECHA_PAR);
    }
}

void comando_entrada() {
    consome(LEIA);
    consome(ABRE_PAR);
    consome(IDENT);
    while (lookahead.atomo == VIRGULA) {
        consome(VIRGULA);
        consome(IDENT);
    }
    consome(FECHA_PAR);
}

void comando_saida() {
    consome(ESCREVA);
    consome(ABRE_PAR);
    lista_expressao();
    consome(FECHA_PAR);
}

void comando_condicional() {
    consome(SE);
    expressao();
    consome(ENTAO);
    comando();
    if (lookahead.atomo == SENAO) {
        consome(SENAO);
        comando();
    }
}

void comando_repeticao() {
    consome(ENQUANTO);
    expressao();
    consome(FACA);
    comando();
}

void lista_expressao() {
    expressao();
    while (lookahead.atomo == VIRGULA) {
        consome(VIRGULA);
        expressao();
    }
}

void expressao() {
    expressao_simples();
    if (lookahead.atomo == IGUAL || lookahead.atomo == DIFERENTE ||
        lookahead.atomo == MENOR || lookahead.atomo == MENOR_IGUAL ||
        lookahead.atomo == MAIOR || lookahead.atomo == MAIOR_IGUAL) {
        operador_relacional();
        expressao_simples();
    }
}

void operador_relacional() {
    if (lookahead.atomo == DIFERENTE) consome(DIFERENTE);
    else if (lookahead.atomo == MENOR) consome(MENOR);
    else if (lookahead.atomo == MENOR_IGUAL) consome(MENOR_IGUAL);
    else if (lookahead.atomo == MAIOR_IGUAL) consome(MAIOR_IGUAL);
    else if (lookahead.atomo == MAIOR) consome(MAIOR);
    else if (lookahead.atomo == IGUAL) consome(IGUAL);
}

void expressao_simples() {
    termo();
    while (lookahead.atomo == MAIS || lookahead.atomo == MENOS ||
           lookahead.atomo == MOD || lookahead.atomo == OU) {
        operador_adicao();
        termo();
    }
}

void operador_adicao() {
    if (lookahead.atomo == MAIS) consome(MAIS);
    else if (lookahead.atomo == MENOS) consome(MENOS);
    else if (lookahead.atomo == MOD) consome(MOD);
    else if (lookahead.atomo == OU) consome(OU);
}

void termo() {
    fator();
    while (lookahead.atomo == MULT || lookahead.atomo == DIV_OP ||
           lookahead.atomo == DIV || lookahead.atomo == E) {
        operador_multiplicacao();
        fator();
    }
}

void operador_multiplicacao() {
    if (lookahead.atomo == MULT) consome(MULT);
    else if (lookahead.atomo == DIV_OP) consome(DIV_OP);
    else if (lookahead.atomo == DIV) consome(DIV);
    else if (lookahead.atomo == E) consome(E);
}

void fator() {
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
        printf("# %d:erro sintatico, esperado [fator] encontrado [%s]\n", 
               lookahead.linha, nome_atomo(lookahead.atomo));
        exit(1);
    }
}

// Função de inicialização com a assinatura exata pedida na especificação
void parse_portugol_internal_v2(FILE *arq) {
    arquivo_fonte = arq;
    linha = 1;
    flag_valida_fci_2026 = 1;

    // Lê a primeira linha do arquivo
    if (fgets(buffer_linha, sizeof(buffer_linha), arquivo_fonte) != NULL) {
        iniciar_lexico(buffer_linha, linha);
    }

    // Pega o primeiro token válido do programa
    lookahead = obter_proximo_token();

    // Inicia a validação a partir da regra principal
    programa();

    if (lookahead.atomo == EOS) {
        printf("%d linhas analisadas, programa sintaticamente correto\n", lookahead.linha);
    } else {
        printf("# %d:erro sintatico, conteudo extra apos o fim do programa [%s]\n", 
               lookahead.linha, nome_atomo(lookahead.atomo));
    }
}