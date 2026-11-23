Como compilar

gcc -Wall -Wno-unused-result -g compilador.c hash_mack.o lexAnaliser.c sintAnaliser.c tabela_simbolo.c -o compilador

Como rodar:

./compilador <nome_do_arquivo>.txt
ou
ou
valgrind --leak-check=yes ./compilador <nome_do_arquivo>.txt

O segundo é para validar erro de acesso à memória que em nossos testes, deu 0

Para esse projeto, não conseguimos desenvolver o CREN, CRVI e ARMI pois não compreendemos direito como implementar. 
Com relação à erros ou bugs, não foi encontrado nenhum ao realizar nossos testes que pegamos de base dos exemplos de aula.

Uma decisão de arquitetura foi fazer semelhantemente ao analisador léxico com o TAtomo e compartilhar as categorias e tipos 
da tabela de simbolos com o analisado sintatico para acessar e alterar no diretamente qualquer simbolo. 
Outra decisão foi que quando é analisado a lista de parâmetros,  armazenamos sua sequência, o i da fórmula, 
em num_parametros para quando o parâmetro for acessado, por isso já saberemos a quantidade n de parametros, calcularmos o 
deslocamento pela fórmula -(n+3-i) para gerar o CRVL ou ARMZ corretamente

Para a implementação do Semântico, tivemos muita dificuldade em implementar alguns tópicos: 
  1. Tipo incorreto de parâmetro (iniciamos a implementação porém não conseguimos implementar no final)
      IMPORTANTE: Para essa implementação, deixamos os protótipos e variáveis globais, mas não foram implementadas
      Variáveis globais (TTipo tipos_argumentos[100]; int indice_argumento = 0;

Acreditamos que esse seja o único problema do semântico
P.S. No link de referência colocado ao final do código, ele diz que o uso de palavras reservadas é um erro semântico, porém
mantivemos a implementação anterior e a declaração de erro como sintática
link de referência para o semântico: https://www.geeksforgeeks.org/compiler-design/semantic-analysis-in-compiler-design/
