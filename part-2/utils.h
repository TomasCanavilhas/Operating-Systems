/******************************************************************************
 ** ISCTE-IUL: Trabalho prático de Sistemas Operativos
 **
 ** Aluno: Nº:       Nome: 
 ** Nome do Módulo: utils.h v2
 ** Descrição/Explicação do Módulo: 
 **     Definição de funções utilitárias genéricas
 **
 ** OS ALUNOS NÃO DEVERÃO ACRESCENTAR NADA A ESTE FICHEIRO!!!
 **
 ******************************************************************************/
#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <string.h>

/******************************************************************************
 *  Macros para tratamento de mensagens de DEBUG
 ******************************************************************************/
#define TRUE 1
#define FALSE 0

/**
 *  Impressão de mensagens de DEBUG incluindo o módulo e a linha de código. Para ativar, alterar DEBUG_MODE para 1
 */
#define DEBUG_MODE TRUE   // Se estiver a TRUE, apresentará mensagens de DEBUG

/**
 *  Escreve uma mensagem de debug (parâmetros iguais ao printf) se DEBUG_MODE estiver a 1
 *  @param ... os argumentos são os mesmos que os do printf(), logo recebe uma string de formatação e depois um número variável de argumentos
 */
#define debug(fmt, ...) do { if (DEBUG_MODE) fprintf(stderr, "@@DEBUG@@:%s:%d:%s(): " fmt "\n", __FILE__, __LINE__, __func__, ## __VA_ARGS__); } while (0)


/******************************************************************************
 *  Macros para impressão de mensagens de sucesso e de erro
 ******************************************************************************/

/**
 *  Escreve uma mensagem de sucesso (parâmetros iguais ao printf), deve ser usado em todas as mensagens "positivas" que a aplicação mostra
 *  @param ... os argumentos são os mesmos que os do printf(), logo recebe uma string de formatação e depois um número variável de argumentos
 */
#define sucesso(fmt, ...) do { printf("@@Success@@: " fmt "\n", ## __VA_ARGS__); } while (0)

/**
 *  Escreve uma mensagem de erro (parâmetros iguais ao printf), deve ser usado em todas as mensagens "de erro" que a aplicação mostra.
 *  Estas mensagens de erro são as "de negócio", e não as de erros referentes a problemas técnicos como abertura de ficheiros, etc. (onde se deve usar perror)
 *  @param ... os argumentos são os mesmos que os do printf(), logo recebe uma string de formatação e depois um número variável de argumentos
 */
#define erro(fmt, ...) do { printf("@@Error@@: " fmt "\n", ## __VA_ARGS__); } while (0)


/******************************************************************************
 *  Macros para leitura de Strings de um ficheiro ou do STDIN
 ******************************************************************************/

/**
 *  Macro para leitura de Strings de um ficheiro
 *  Esta macro basicamente efetua a leitura de uma string de um ficheiro, definido um buffer_size. Semelhante a fgets(), mas removendo o '\n' e os caracteres extra.
 *  @param buffer        (char*) buffer onde vai ser depositada a informação
 *  @param buffer_size   (int) tamanho do buffer acima (em bytes)
 *  @param file          (FILE*) handler do ficheiro a ler
 */
#define my_fgets(buffer, buffer_size, file) ({ \
    char* _result = fgets(buffer, buffer_size, file); \
    if (NULL != _result) { \
        while ('\n' == buffer[0]) \
            _result = fgets(buffer, buffer_size, file); \
        if ('\n' == buffer[strlen(buffer) - 1]) \
            buffer[strlen(buffer) - 1] = '\0'; \
        else { \
            int c; \
            do \
                c = getc(file); \
            while ('\n' != c && EOF != c); \
        } \
    } \
    _result; \
})

/**
 *  Macro para leitura de Strings do STDIN
 *  Esta macro basicamente efetua a leitura de uma string do STDIN, definido um buffer_size. Semelhante a fgets(), mas removendo o '\n' e os caracteres extra.
 *  @param buffer        (char*) buffer onde vai ser depositada a informação
 *  @param buffer_size   (int) tamanho do buffer acima (em bytes)
 */
#define my_gets(buffer, buffer_size) my_fgets(buffer, buffer_size, stdin)


/* OS ALUNOS NÃO DEVERÃO ACRESCENTAR NADA A ESTE FICHEIRO!!! */

#endif