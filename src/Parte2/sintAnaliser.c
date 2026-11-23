/*
Alunos:
João Vitor Garcia Aguiar Mintz 10440421
Giovanni Barreiro G. de Castro 10435745
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexAnaliser.h"
#include "tabela_simbolo.h"

// Variáveis de controle de arquivo e estado do Léxico
static FILE *arquivo_fonte = NULL;
static char buffer_linha[1024];
extern int linha;

TInfoAtomo lookahead;

int contador_L = 0; // Contador do MEPA para as chamadas de funções e desvios
// L1: NADA, DSVS L2 e DSVF L3 por exemplo
int escopo_atual = 0; // Escopo inicial é dado como global e, caso necessário, troca para local
int endereco_local = 0; // O endereço de cada variável
int endereco_global = 0;
TTipo tipo_atual = T_NAO_DEFINIDO; // Tipo global que sempre que se altera ao passar por tipo()
TCategoria categoria_atual = S_VARIAVEL; // Categoria global que se altera de acordo com necessidade
int total_parametros = 0;

// protótipos de Funções para o MEPA e Semantico
void mepa(const char *op);
void mepa_n(const char *op, int n);
void mepa_nn(const char *op, int n, int d);
int rotulos();
void desvia(const char *op, int rot);
void LN_NADA(int rot);
void erro_semantico_generico(char *motivo, char *ID);
void verifica_duplicado(char *nome);

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
        liberar_tabela();
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
        mepa_n("AMEM ", num_var);
    }

    int rotulo_Main = 0;
    if (lookahead.atomo == FUNCAO || lookahead.atomo == PROCEDIMENTO) {
        rotulo_Main = rotulos(); // Aqui é onde incrementa para o L(n)
        desvia("DSVS", rotulo_Main); // Chama a função de impressão inicial (main)
        // Esse imprime o primeiro DSVS L1 para ir para o Main
    }

    declaracao_rotinas();

    if (rotulo_Main != 0) {
        LN_NADA(rotulo_Main);
    }

    comando_composto();

    // Busca as mesmas variáveis para retirar da memória
    if (num_var > 0) {
        // Chama a função para liberação da memória
        mepa_n("DMEM", num_var);
    }
}

int declaracao_variaveis() {
    categoria_atual = S_VARIAVEL;
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
    // Apenas para, na hora de realizar o cálculo do endereço de parâmetro, ver a sequência de entrada
    // Se já houver parâmetros antes adicionados, ao invés dele reiniciar, mantém valor para cálculo
    int ordem_parametro = total_parametros + 1;

    // Para realizar posteriormente a correção do tipo caso necessário
    int n = 1;
    TNo *inseridos[64]; // nós criados neste vetor, para acertar o tipo depois

    verifica_duplicado(lookahead.atributo.id);
    // Feito dentro de lista_variaveis pois é aqui que pega cada uma das variáveis e salva seus IDs
    if (escopo_atual == 1) {
        // Parâmetro entra como escopo local mas não alteram o valor do endereço local
        if (categoria_atual == S_PARAMETRO) {
            inserir_tabela(lookahead.atributo.id, endereco_local, categoria_atual, tipo_atual, escopo_atual, ordem_parametro);
            ordem_parametro += 1;
        } else {
            inserir_tabela(lookahead.atributo.id, endereco_local, categoria_atual, tipo_atual, escopo_atual, 0);
            endereco_local += 1;
        }
    } else {
        // Verifica o escopo atual para incrementar o endereço global
        inserir_tabela(lookahead.atributo.id, endereco_global, categoria_atual, tipo_atual, escopo_atual, 0);
        endereco_global += 1;
    }
    inseridos[0] = buscar_simbolo(lookahead.atributo.id);
    consome(IDENT);
    while (lookahead.atomo == VIRGULA) {
        consome(VIRGULA);
        verifica_duplicado(lookahead.atributo.id); // verifica constante se é duplicado para verificação do semântico
        if (escopo_atual == 1) {
            // Parâmetro entra como escopo local mas não alteram o valor do endereço local
            if (categoria_atual == S_PARAMETRO) {
                inserir_tabela(lookahead.atributo.id, endereco_local, categoria_atual, tipo_atual, escopo_atual, ordem_parametro);
                ordem_parametro += 1;
            } else {
                inserir_tabela(lookahead.atributo.id, endereco_local, categoria_atual, tipo_atual, escopo_atual, 0);
                endereco_local += 1;
            }
        } else {
            inserir_tabela(lookahead.atributo.id, endereco_global, categoria_atual, tipo_atual, escopo_atual, 0);
            endereco_global += 1;
        } // Justamente para pegar CADA variável e salvar na tabela
        inseridos[n] = buscar_simbolo(lookahead.atributo.id);
        consome(IDENT);
        // n++ realiza a soma
        n++;
    }
    consome(DOIS_PONTOS);
    tipo();

    // Tipo desta lista é conhecido: corrige o tipo de cada variável inserida
    for (int i = 0; i < n; i++) {
        inseridos[i]->tipo = tipo_atual;
    }

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
    total_parametros = 0; // Reseta para caso haja mais de uma função/procedimento
    consome(FUNCAO);
    int rotulo_funcao = rotulos();

    LN_NADA(rotulo_funcao);
    mepa("ENPR");

    tipo(); // Tipo pois função retorna valores que devem ser analisados pelo semântico se o tipo bate com variável atribuída
    // Ideia é que para função, o endereço armazene o endereço do rótulo
    categoria_atual = S_FUNCAO;
    inserir_tabela(lookahead.atributo.id, rotulo_funcao, categoria_atual, tipo_atual, escopo_atual, 0); // 0 inicialmente, altera depois

    TNo *simbolo = buscar_simbolo(lookahead.atributo.id);
    if (simbolo == NULL) {
        erro_semantico_generico("Identificador não foi declarado", lookahead.atributo.id);
    }

    consome(IDENT);
    escopo_atual = 1; // Define escopo 1 já que deve talvez entrar de uma função ou procedimento (ideia de LOCK)

    parametros_formais();
    simbolo->num_parametros = total_parametros; // Passa para o simbolo o total de parâmetros

    // Inserir em variáveis locais da função/procedimento
    int variaveis_locais = declaracao_variaveis();
    if (variaveis_locais != 0) {
        mepa_n("AMEM", variaveis_locais);
    }
    comando_composto();

    escopo_atual = 0; // Define escopo 0 já que deve talvez sair de uma função ou procedimento (ideia de UNLOCK)
    // Verificação para não printar um DMEM 0
    if (variaveis_locais != 0) {
        mepa_n("DMEM", variaveis_locais);
    }
    if (total_parametros == 0) {
        mepa("RTPR");
    } else {
        mepa_n("RTPR", total_parametros);
    }
}

void declaracao_procedimento() {
    total_parametros = 0; // Reseta para caso haja mais de uma função/procedimento
    consome(PROCEDIMENTO);
    int rotulo_procedimento = rotulos();

    LN_NADA(rotulo_procedimento);
    mepa("ENPR");

    // Ideia é que para procedimento, o endereço armazene o endereço do rótulo
    categoria_atual = S_PROCEDIMENTO;
    inserir_tabela(lookahead.atributo.id, rotulo_procedimento, categoria_atual, tipo_atual, escopo_atual, 0); // Altera poucas linhas depois

    TNo *simbolo = buscar_simbolo(lookahead.atributo.id);
    if (simbolo == NULL) {
        erro_semantico_generico("Identificador não foi declarado", lookahead.atributo.id);
    }

    consome(IDENT);
    escopo_atual = 1; // Define escopo 1 já que deve talvez entrar de uma função ou procedimento (ideia de LOCK)

    parametros_formais();
    simbolo->num_parametros = total_parametros; // Passa para o simbolo o total de parâmetros

    // Inserir em variáveis locais da função/procedimento
    int variaveis_locais = declaracao_variaveis();
    // Verificação para não printar um AMEM 0
    if (variaveis_locais != 0) {
        mepa_n("AMEM", variaveis_locais);
    }
    comando_composto();

    escopo_atual = 0; // Define escopo 0 já que deve talvez sair de uma função ou procedimento (ideia de UNLOCK)
    // Verificação para não printar um DMEM 0
    if (variaveis_locais != 0) {
        mepa_n("DMEM", variaveis_locais);
    }
    if (total_parametros == 0) {
        mepa("RTPR");
    } else {
        mepa_n("RTPR", total_parametros);
    }
}

void tipo() {
    if (lookahead.atomo == CARACTERE) {
        consome(CARACTERE);
        tipo_atual = T_CARACTERE;
    } else if (lookahead.atomo == INTEIRO) {
        consome(INTEIRO);
        tipo_atual = T_INTEIRO; 
    }
    else if (lookahead.atomo == LOGICO) {
        consome(LOGICO);
        tipo_atual = T_LOGICO;
    } 
    else {
        printf("# %d:erro sintatico, esperado [tipo] encontrado [%s]\n", // Mensagem de Erro Sintático de Tipo
               lookahead.linha, nome_atomo(lookahead.atomo));
        liberar_tabela();
        exit(1);
    }
}

void parametros_formais() {
    consome(ABRE_PAR);
    if (lookahead.atomo == VAR || lookahead.atomo == IDENT) {
        parametro_formal();;
        while (lookahead.atomo == PONTO_VIRGULA) {
            consome(PONTO_VIRGULA);
            parametro_formal(); 
        }
    }
    consome(FECHA_PAR);
}

void parametro_formal() {
    categoria_atual = S_PARAMETRO;
    if (lookahead.atomo == VAR) {
        consome(VAR);
    }
    // Coleta o total de parâmetros
    total_parametros += lista_variaveis();
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
    } else if (lookahead.atomo == IDENT) { // Identificador, logo, variável ou função ou procedimento
        comando_atribuicao();
    } else {
        // Mensagem de Erro
        printf("# %d:erro sintatico, esperado [comando] encontrado [%s]\n", 
               lookahead.linha, nome_atomo(lookahead.atomo));

        liberar_tabela();
        exit(1);
    }
}

void comando_atribuicao() {
    TNo *simbolo = buscar_simbolo(lookahead.atributo.id); // Pega o nó com base no ID
    if (simbolo == NULL) {
        erro_semantico_generico("Identificador não foi declarado", lookahead.atributo.id);
    }

    consome(IDENT); // ID
    if (lookahead.atomo == ATRIB) {
        consome(ATRIB);
        expressao(); // Se for tipo a := (soma()) + b por exemplo:
        if (simbolo->categoria == S_FUNCAO) {
            // Caso seja uma função com retorno, ele printa o armazena com o endereço de retorno
            mepa_nn("ARMZ", 1, altera_endereco_retorno(simbolo->num_parametros));
        } else { // Se for só os simbolos mesmo a := b + c
            int endereco = simbolo->endereco;

            // Se for um parâmetro, corrige aqui o endereço
            if (simbolo->categoria == S_PARAMETRO) {
                endereco = altera_endereco_parametro(total_parametros, simbolo->num_parametros);
            }
            // se não, só imprime direto
            mepa_nn("ARMZ", simbolo->escopo, endereco);
        }
    } else if (lookahead.atomo == ABRE_PAR) {
        consome(ABRE_PAR);

        if (lookahead.atomo != FECHA_PAR) {
            lista_expressao();
        }
        consome(FECHA_PAR);
        desvia("CHPR", simbolo->endereco);
    }
}

// Função auxiliar para impressão do LEIT e do ARMZ para não ficar tão confuso
// Essa função também verifica sintática e semântica, caso a variável não tenha sido declarada
// Eu adicionei essa função para deixar o LEIT e o ARMZ juntos e facilitar a leitura
void leitura_variavel(){
    if (lookahead.atomo != IDENT) consome(IDENT);   // gera o erro sintático
    TNo* simbolo = buscar_simbolo(lookahead.atributo.id);

    if (simbolo == NULL) {
        erro_semantico_generico("Identificador não foi declarado", lookahead.atributo.id);
    }

    int endereco = simbolo->endereco;

    if (simbolo->categoria == S_PARAMETRO) {
        endereco = altera_endereco_parametro(total_parametros, simbolo->num_parametros);
    }
    mepa("LEIT"); // Chama o LEIT (Leitura) no MEPA
    mepa_nn("ARMZ", simbolo->escopo, endereco);  // Para a variável declarada que recebe o valor do LEIT
    consome(IDENT);
}

void comando_entrada() {
    consome(LEIA);
    consome(ABRE_PAR);
    leitura_variavel();
    while (lookahead.atomo == VIRGULA) {
        consome(VIRGULA);
        leitura_variavel();
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

    // Parte do MEPA:
    int rotulo_senao = rotulos(); // Adiciona um rótulo para o ELSE para o DSVF (desvia se falso)
    desvia("DSVF", rotulo_senao); // Chama a função de desvio (caso falso)

    comando();
    if (lookahead.atomo == SENAO) {
        // Parte do MEPA:
        int rotulo_fim = rotulos(); // Adiciona um rótulo para o fim do ELSE para o DSVS (desvia sempre)
        desvia("DSVS", rotulo_fim); // Chama a função de desvio
        LN_NADA(rotulo_senao); // Chama a função de rótulo para o ELSE
        
        consome(SENAO); 
        comando();

        LN_NADA(rotulo_fim); // Chama a função de rótulo para o FIM do ELSE
    } else {
        // Parte nova para o MEPA também:
        LN_NADA(rotulo_senao); // Chama a função de rótulo para o ELSE
        // Caso não tenha ELSE, ele apenas chama o rótulo do DSVF (desvia se falso)
    }
}

void comando_repeticao() {
    // Parte nova do MEPA: (for e while)
    int inicio_loop = rotulos(); // Adiciona um rótulo para o início de for/while
    int fim_loop = rotulos(); // Adiciona um rótulo para o fim de for/while
    LN_NADA(inicio_loop); // Chama a função de rótulo para o início do loop

    consome(ENQUANTO);
    expressao();
    consome(FACA);

    desvia("DSVF", fim_loop); // Chama a função de desvio (caso falso) para o fim do loop

    comando();

    desvia("DSVS", inicio_loop); // Chama a função de desvio (sempre) para o início do loop
    LN_NADA(fim_loop); // Chama a função de rótulo para o
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
        lookahead.atomo == MAIOR_IGUAL || lookahead.atomo == DIFERENTE) {
            // Aqui eu adicionei o DIFERENTE também que acho que a gente tinha esquecido
            TAtomo op = lookahead.atomo; // Pega o operador relacional para implementar no MEPA

            operador_relacional();
            expressao_simples();

            if (op == IGUAL) {
                mepa("CMIG");
            } else if (op == MENOR) {
                mepa("CMME");
            } else if (op == MENOR_IGUAL) {
                mepa("CMEG");
            } else if (op == MAIOR) {
                mepa("CMMA");
            } else if (op == MAIOR_IGUAL) {
                mepa("CMAG");
            } else if (op == DIFERENTE) {
                mepa("CMDG");
            }
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
        TAtomo op = lookahead.atomo; // Pega o operador de adição e subtração para implementar no MEPA
        operador_adicao();
        termo();
        if (op == MAIS) {
            mepa("SOMA");
        } else if (op == MENOS) {
            mepa("SUBT");
        } else if (op == MOD) {
            mepa("MOD");
        } else if (op == OU) {
            mepa("DISJ");
        }
        // Tem o MOD e o OU que eu não sei como fica no MEPA, então deixei de fora
        // 22/11 Implementei o MOD (mod) e o DISJ (OU)
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

        TAtomo op = lookahead.atomo; // Pega o operador de multiplicaçao e divisão para implementar no MEPA
        operador_multiplicacao();
        fator();

        if (op == MULT) {
            mepa("MULT");
        } else if (op == DIV) {
            mepa("DIVI");
        } else if (op == E) {
            mepa("CONJ");
        }
        // Aqui também, tem o E que eu não sei como fica no MEPA, deixei de fora
        // 22/11 Implementei o CONJ (E)
    }
}

void operador_multiplicacao() {
    if (lookahead.atomo == MULT) consome(MULT);
    else if (lookahead.atomo == DIV) consome(DIV);
    else if (lookahead.atomo == E) consome(E);
}

// Para quando a chamada aparece dentro de uma expressão
void fator() {
    if (lookahead.atomo == IDENT) {
        // Busca simbolo após comprovar que é um identificador
        TNo* simbolo = buscar_simbolo(lookahead.atributo.id);
        if (simbolo == NULL) {
            erro_semantico_generico("Identificador não declarado", lookahead.atributo.id);
        }


        consome(IDENT);
        // Caso seja parênteses, é ou chamada de FUNÇÃO ou PROCEDIMENTO dentro do fator
        if (lookahead.atomo == ABRE_PAR) {
            consome(ABRE_PAR);
            // Se for função, realiza o AMEM (procedimento não retorna valor) para retorno do valor
            if (simbolo->categoria == S_FUNCAO) {
                mepa_n("AMEM", 1);
            }
            lista_expressao();
            consome(FECHA_PAR);
            desvia("CHPR", simbolo->endereco);
        } else { // Se não for nenhum dos dois, é variável ou parâmetro
            if (simbolo->categoria == S_PARAMETRO) {
                int endereco = altera_endereco_parametro(total_parametros, simbolo->num_parametros);
                mepa_nn("CRVL", 1, endereco);
            } else { // É variável
                mepa_nn("CRVL", simbolo->escopo, simbolo->endereco);   // fator
            }
        }
    } else if (lookahead.atomo == CONSTINT) {
        mepa_n("CRCT", lookahead.atributo.numero); // Chama o CRCT no MEPA
        consome(CONSTINT);
    } else if (lookahead.atomo == CONSTCHAR) {
        // Não sei se tem constCHAR no mepa então também não coloquei
        consome(CONSTCHAR);
    } else if (lookahead.atomo == ABRE_PAR) {
        consome(ABRE_PAR);
        expressao();
        consome(FECHA_PAR);
    } else if (lookahead.atomo == MAIS || lookahead.atomo == MENOS || lookahead.atomo == NAO) {
        TAtomo op = lookahead.atomo; // Pega o operador unário para implementar no MEPA
        consome(op);
        fator();
        if (op == MENOS) mepa("INVR"); // Inverte o sinal
        else if (op == NAO) mepa("NEGA"); // Negação
        else if (lookahead.atomo == VERDADEIRO) mepa_n("CRCT", 1); // Chama o CRCT no MEPA para verdadeiro
        else if (lookahead.atomo == FALSO) mepa_n("CRCT", 0); // Chama o CRCT no MEPA para falso
    
        // Nos slides do professor tem um Teste := 'f' mas eu não vou implementar para CONSTCHAR

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

        liberar_tabela();
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
    inicializa_tabela();

    programa();

    if (lookahead.atomo != EOS) {
        printf("# %d:erro sintatico, conteudo extra apos o fim do programa [%s]\n", lookahead.linha, nome_atomo(lookahead.atomo));
    }
    imprimir_tabela();
    liberar_tabela();
}

// O LEIT e o DMEM já fazem parte de outras funções

// Função que chama os operadores
// *op são os operadores dados em char ("INPP", por exemplo)
// Essa será a base de implementação do MEPA
void mepa(const char *op) {
    // Inicializador do MEPA (INPP e PARA)
    // Os espaços na tela são para dar aqueles mesmos espaços que o professor tem nos exemplos dele
    // para deixar mais bonitinho e organizado.
    printf("    %s\n", op);
}

void mepa_n(const char *op, int n) {
    // AMEM (declaração de variáveis globais)
    printf("    %s %d\n", op, n);
    // O DMEM também funciona aqui, foi alterado na função bloco()
    // O CRCT também funciona aqui, foi alterado na função fator()
}

void mepa_nn(const char *op, int n, int d) {
    // Essa imprime o ARMZ (armazenamento de variáveis) e o CRVL (carrega variável)
    // O CRVL também funciona aqui
    // A diferença dessa pras outras é a qtd de parâmetros na impressão
    // Enquanto o mepa() imprime só o operador, o mepa_n() imprime o operador e um parâmetro, e o mepa_nn() imprime o operador e dois parâmetros
    printf("    %s %d,%d\n", op, n, d);
}

int rotulos() {
    // Incrementador dos Rótulos (L1, L2, L3...) (MEPA)
    return ++contador_L;
    // Ele não recebe parâmetros pois a variável contador_L é global
    // então ele só incrementa no valor global e retorna para a função de chamada.
}

// Rótulo Lx para a função no MEPA
// Aqui imprime o rótulo com base no parâmetro rot recebido
void desvia(const char *op, int rot) {
    printf("    %s L%d\n", op, rot);
}

// Rótulo Ln:NADA para as funções no MEPA
void LN_NADA(int rot) {
    printf("L%d:NADA\n", rot);
}

// TODO: Continuar a implementação do MEPA (falta bastante coisa ainda)
// Para o MEPA completo falta:
// CREN; CRVI e ARMI


/*

=====================================================================================

        IMPLEMENTAÇÃO DE FUNÇÕES AUXÍLIARES DO ANALISADOR SEMÂNTICO

=====================================================================================

*/
void erro_semantico_generico(char *motivo, char *id) {
    printf("# %d:erro semantico: %s, %s\n", lookahead.linha, motivo, id);
    liberar_tabela();
    exit(1);
}

// Erro se o nome já foi declarado no MESMO escopo
void verifica_duplicado(char *nome) {
    TNo *s = buscar_simbolo(nome);
    if (s != NULL && s->escopo == escopo_atual) {
        erro_semantico_generico("Identificador ja declarado", nome);
    }
}
