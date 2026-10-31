Como compilar:

gcc -Wall -Wno-unused-result -g -Og lexAnaliser.c sintAnaliser.c compilador.c -o compilador

Como rodar:

./compilador <nome_do_arquivo>.txt
ou
valgrind --leak-check=yes ./compilador <nome do arquivo de entrada>

O segundo é para validar erro de acesso à memória que em nossos testes, deu 0

No nosso projeto, foram realizados tanto o analisador léxico quanto o sintático seguindo com base as aulas
do miniLex.c e miniSint.c do professor Luba.