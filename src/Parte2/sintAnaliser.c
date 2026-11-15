/*
Alunos:
João Vitor Garcia Aguiar Mintz 10440421
Giovanni Barreiro G. de Castro 10435745
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexAnaliser.h"

// Variáveis de controle de arquivo e estado do Léxico
static FILE *arquivo_fonte = NULL;
static char buffer_linha[1024];
extern int linha;

TInfoAtomo lookahead;

// Variável global de validador do MEPA
int gerador_MEPA = 0; // Apenas validador do mepa, quando é chamado ele valida como 1

// rotótipos de Funções para o MEPA
void mepa(const char *op);
void AMEM(const char *op, int n);

// Protótipos das regras da gramática
void consome(TAtomo atomo_esperado);
const char* nome_atomo(TAtomo atomo);
void programa();
void bloco();
int declaracao_variaveis(); // Mudança de void para int
int lista_variaveis();      // para retornar os valores para o AMEM
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
TInfoAtomo obter_proximo_token(void) {
    TInfoAtomo info = obter_atomo();

    while (1) {
        if (info.atomo == COMENTARIO) {
            // Comentando essa parte para não perder a coesão com a parte 1
            // Retirando os prints da impressão de análise sintática
            // printf("# %d:comentario\n", info.linha);
            info = obter_atomo();
            continue;
        }

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
        case NAO: return "nao";
        case IDENT: return "identificador";
        case CONSTINT: return "constint";
        case CONSTCHAR: return "constchar";
        case PONTO_VIRGULA: return "ponto_virgula";
        case PONTO: return "ponto";
        case VIRGULA: return "virgula";
        case DOIS_PONTOS: return "dois_pontos";
        case ATRIB: return "atribuicao";
        case ABRE_PAR: return "abre_par";
        case FECHA_PAR: return "fecha_par";
        case ABRE_CHAVE: return "abre_chave";
        case FECHA_CHAVE: return "fecha_chave";
        case IGUAL: return "igual";
        case MENOR: return "menor";
        case MENOR_IGUAL: return "menor_igual";
        case MAIOR: return "maior";
        case MAIOR_IGUAL: return "maior_igual";
        case DIFERENTE: return "diferente";
        case MAIS: return "mais";
        case MENOS: return "menos";
        case MULT: return "multiplicacao";
        case EOS: return "fim de arquivo";
        default: return "simbolo";
    }
}

// Função para exibir o símbolo literal quando ocorrer erro sintático
const char* simbolo_atomo(TAtomo atomo) {
    switch((int)atomo) {
        case PONTO_VIRGULA: return ";";
        case PONTO: return ".";
        case VIRGULA: return ",";
        case DOIS_PONTOS: return ":";
        case ATRIB: return ":=";
        case ABRE_PAR: return "(";
        case FECHA_PAR: return ")";
        case ABRE_CHAVE: return "{";
        case FECHA_CHAVE: return "}";
        case IGUAL: return "=";
        case MENOR: return "<";
        case MENOR_IGUAL: return "<=";
        case MAIOR: return ">";
        case MAIOR_IGUAL: return ">=";
        case DIFERENTE: return "<>";
        case MAIS: return "+";
        case MENOS: return "-";
        case MULT: return "*";
        default: return nome_atomo(atomo);
    }
}

void consome(TAtomo atomo_esperado) {
    if (lookahead.atomo == atomo_esperado) {

        // Remoção dos PRINTs da análise sintática
        /*
        if (lookahead.atomo == IDENT) {
            printf("# %d:identificador: %s\n", lookahead.linha, lookahead.atributo.id);
        } else if (lookahead.atomo == CONSTINT) {
            printf("# %d:constint: %d\n", lookahead.linha, lookahead.atributo.numero);
        } else if (lookahead.atomo == CONSTCHAR) {
            printf("# %d:constchar: %c\n", lookahead.linha, lookahead.atributo.ch);
        } else {
            printf("# %d:%s\n", lookahead.linha, nome_atomo(lookahead.atomo));
        }
        */

        lookahead = obter_proximo_token();
    } else {
        printf("# %d:erro sintatico, esperado [%s] encontrado [%s]\n",
               lookahead.linha, 
               simbolo_atomo(atomo_esperado), 
               simbolo_atomo(lookahead.atomo));
        exit(1);
    }
}

// Regras gramaticais
void programa() {
    consome(ALGORITMO);
    consome(IDENT);
    consome(PONTO_VIRGULA);
        
    // Início para o MEPA
    mepa("INPP");
    bloco();
    consome(PONTO);
    mepa("PARA");
}

void bloco() {
    // Busca o número de variáveis para o MEPA também
    int num_var = declaracao_variaveis();
    if (num_var) {
        // Chama a função da declaração de variáveis globais
        AMEM("AMEM", num_var);
    }

    declaracao_rotinas();
    comando_composto();

    // Busca as mesmas variáveis para retirar da memória
    if (num_var > 0) {
        // Chama a função para liberação da memória
        AMEM("DMEM", num_var);
    }
}

int declaracao_variaveis() {
    int total_var = 0;
    if (lookahead.atomo == VAR) {
        consome(VAR);
        total_var += lista_variaveis();
        consome(PONTO_VIRGULA);
        while (lookahead.atomo == IDENT) {
            total_var += lista_variaveis();
            consome(PONTO_VIRGULA);
        }
    }
    return total_var;
}

int lista_variaveis() {
    // Soma das variáveis para o MEPA
    int n = 1;

    consome(IDENT);
    while (lookahead.atomo == VIRGULA) {
        consome(VIRGULA);
        consome(IDENT);
        // n++ realiza a soma
        n++;
    }
    consome(DOIS_PONTOS);
    tipo();
    return n; // A função agora é int para retornar os valores (isso se aplica a declaracao_variaveis e lista_variaveis)
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
        printf("# %d:erro sintatico, esperado [tipo] encontrado [%s]\n", // Mensagem de Erro Sintático de Tipo
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
    } else {
        // Mensagem de Erro
        printf("# %d:erro sintatico, esperado [comando] encontrado [%s]\n", 
               lookahead.linha, nome_atomo(lookahead.atomo));
        exit(1);
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
    mepa("LEIT"); // Chama o LEIT (Leitura) no MEPA
    consome(IDENT);
    while (lookahead.atomo == VIRGULA) {
        consome(VIRGULA);
        mepa("LEIT");
        consome(IDENT);
    }
    consome(FECHA_PAR);
}

void comando_saida() {
    consome(ESCREVA);
    consome(ABRE_PAR);
    lista_expressao();
    mepa("IMPR"); // Chama o IMPR (impressão) em MEPA
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
    if (lookahead.atomo == IGUAL || lookahead.atomo == MENOR || 
        lookahead.atomo == MENOR_IGUAL || lookahead.atomo == MAIOR || 
        lookahead.atomo == MAIOR_IGUAL) {
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
    while (lookahead.atomo == MULT || lookahead.atomo == DIV || 
        lookahead.atomo == E) {
        operador_multiplicacao();
        fator();
    }
}

void operador_multiplicacao() {
    if (lookahead.atomo == MULT) consome(MULT);
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
    } else if (lookahead.atomo == MAIS || lookahead.atomo == MENOS || lookahead.atomo == NAO) {
        if (lookahead.atomo == MAIS) consome(MAIS);
        else if (lookahead.atomo == MENOS) consome(MENOS);
        else consome(NAO);
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
void iniciar_sintatico(FILE *arq) {
    arquivo_fonte = arq;
    linha = 1; // Força a contagem a iniciar na linha 1

    if (fgets(buffer_linha, sizeof(buffer_linha), arquivo_fonte) != NULL) {
        iniciar_lexico(buffer_linha, linha);
    }

    lookahead = obter_proximo_token();

    programa();

    if (lookahead.atomo == EOS) {
        printf("%d linhas analisadas, programa sintaticamente correto\n", linha);
    } else {
        printf("# %d:erro sintatico, conteudo extra apos o fim do programa [%s]\n", 
               lookahead.linha, nome_atomo(lookahead.atomo));
    }
}

/*

=====================================================================================

                    GERAÇÃO DE CÓDIGO MEPA

=====================================================================================


*/

// Para a impressão da MEPA, vou manter os prints porém vou comentá-los, para manter a coesão com o código anterior
// Aqui eu só copiei o iniciar_sintático, ou seja, ele relê o arquivo inteiro para montar o MEPA, por isso do rewind
void iniciar_MEPA(FILE *arq){
    rewind(arq);
    arquivo_fonte = arq;
    linha = 1; // Força a contagem a iniciar na linha 1

    if (fgets(buffer_linha, sizeof(buffer_linha), arquivo_fonte) != NULL) {
        iniciar_lexico(buffer_linha, linha);
    }

    lookahead = obter_proximo_token();

    gerador_MEPA = 1;
    programa();
}

// O LEIT e o DMEM já fazem parte de outras funções

// Função que chama os operadores
// *op são os operadores dados em char ("INPP", por exemplo)
void mepa(const char *op) {
    // Inicializador do MEPA (INPP e PARA)
    // Os espaços na tela são para dar aqueles mesmos espaços que o professor tem nos exemplos dele
    // para deixar mais bonitinho e organizado.
    if (gerador_MEPA) printf("    %s\n", op);
}

void AMEM(const char *op, int n) {
    // AMEM (declaração de variáveis globais)
    if (gerador_MEPA) printf("    %s %d\n", op, n);
    // O DMEM também funciona aqui, foi alterado na função bloco()
}


// TODO: Continuar a implementação do MEPA (falta bastante coisa ainda)
// Para o exemplo que eu estou usando (teste 6) falta: 
// CRVL; CRCT; DSVS; MULT; ARMZ;
// Também falta os L1; L2, etc


/*

=====================================================================================

                    ANALISADOR SEMÂNTICO

=====================================================================================


*/


// TODO: Começar a implementação do Analisador Semântico
