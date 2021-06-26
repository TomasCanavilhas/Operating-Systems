/******************************************************************************
 ** ISCTE-IUL: Trabalho prático 3 de Sistemas Operativos
 **
 ** Aluno: Nº:       Nome: Este Módulo não precisa ser entregue
 ** Nome do Módulo: common.h
 ** Descrição/Explicação do Módulo: 
 **     Definição das estruturas de dados comuns aos módulos servidor e cidadao
 **     Os alunos não deverão alterar os nomes dos ficheiros definidos em FILE_*
 **
 ** OS ALUNOS NÃO DEVERÃO ACRESCENTAR NADA A ESTE FICHEIRO!
 **
 ******************************************************************************/
#ifndef __COMMON_H__
#define __COMMON_H__

#define IPC_KEY 0x0a98804      // KEY para todos os elementos de IPC utilizados. Esta KEY deverá ser substituída pelo nº de aluno

#define FILE_ENFERMEIROS   "enfermeiros.dat"    // Ficheiro que tem a lista de enfermeiros associados aos Centros de Saúde para vacinação
#define FILE_CIDADAOS      "cidadaos.dat"       // Ficheiro que tem a lista de enfermeiros associados aos Centros de Saúde para vacinação

#define MAX_VAGAS 5             // Número máximo de cidadãos que poderão estar a ser vacinados simultâneamente
#define MAX_CIDADAOS 1000       // Número máximo de cidadãos suportado pela base de dados (10 milhões?)
#define MAX_ENFERMEIROS 15      // Número máximo de enfermeiros suportado pela base de dados

#define TEMPO_CONSULTA 5        // Tempo em segundos que leva a realizar uma consulta de vacinação pelo processo Servidor-Filho
#define TEMPO_ESPERA 1          // Tempo em segundos que o Cidadao deverá esperar antes de fazer nova tentativa de consulta

typedef struct {                // Estrutura associada a um Cidadão:
    int num_utente;             // Nº de Utente do cidadão
    char nome[100];             // Nome do Cidadão
    int idade;                  // Idade do Cidadão
    char localidade[100];       // Localidade de residência do Cidadão. É usado para determinar o Centro de Saúde do Cidadão: "CS"+localidade
    char nr_telemovel[10];      // Telefone de contacto do Cidadão
    int estado_vacinacao;       // Valores Possíveis: 0 (não vacinado), 1 (vacinado 1ª inoculação), 2 (vacinado 2ª inoculação), -1 (vacinação em curso)
    int PID_cidadao;            // PID do processo cliente invocado pelo Cidadão para ser vacinado
} Cidadao;                      // Cada pedido de um processo Cidadão estará associado a uma estrutura destas.

typedef struct {                // Estrutura associada a um Enfermeiro:
    int ced_profissional;       // Cédula Profissional do Enfermeiro
    char nome[100];             // Nome do Enfermeiro
    char CS_enfermeiro[100];    // Centro de Saúde associado ao Enfermeiro, no formato "CS"+localidade
    int nr_vacinas_dadas;       // Número de vacinas administradas pelo Enfermeiro. Iniciado a 0, este número será atualizado ao longo do tempo
    int disponibilidade;        // Disponibilidade do Enfermeiro: Durante o tempo que o Enfermeiro estiver a dar uma vacina, o Enfermeiro estará indisponível (0), caso contrário, estará disponível (1)
} Enfermeiro;                   // Haverá tantos elementos quantos os existentes no ficheiro FILE_ENFERMEIROS.

typedef struct {                // Estrutura associada a uma vaga de um Cidadão a ser vacinado:
    int index_enfermeiro;       // Enfermeiro associado à sessão de vacinação (índice do array de Enfermeiros)
    int index_cidadao;          // Cidadao associado à sessão de vacinação (índice do array de Cidadaos)
    int PID_filho;              // PID do processo Servidor-Filho dedicado a uma sessão de vacinação
} Vaga;                         // Poderão existir no máximo NUM_VAGAS elementos deste tipo.

typedef enum {                  // Enumerado com valores possíveis para os estados das mensagens:
    PEDIDO,                     // Pede o início da vacina
    CANCELAMENTO                // Cancela uma sessão de vacinação em curso
} TipoPedido;

typedef struct {                // Estrutura utilizada para envio de mensagens pela fila de mensagens do Cidadao para o Servidor
    long tipo;                  // Tipo da mensagem (Address): 1 para a comunicação do Cidadao para o Servidor Geral
    struct {                    // Estrutura de dados da mensagem
        TipoPedido pedido;      // Tipo de pedido realizado pelo Cidadao (pode ser PEDIDO ou CANCELAMENTO)
        int num_utente;         // Nº de Utente do cidadão
        char nome[100];         // Nome do Cidadão
        int PID_cidadao;        // PID do processo cliente invocado pelo Cidadão para ser vacinado
    } dados;                    // Estrutura de dados específico desta mensagem
} MsgCliente;                   // Estas mensagens são as enviadas do Cidadao para o Servidor Geral

typedef enum {                  // Enumerado com valores possíveis para os estados do servidor:
    OK,                         // Indica que o pedido de vacinação foi aceite 
    AGUARDAR,                   // Indica que o pedido tem de ser repetido mais tarde (ou enfermeiro indisponível ou não há vaga)
    DESCONHECIDO,               // Indica que o pedido foi realizado com um num_utente não encontrado na BD de Cidadaos
    VACINADO,                   // Indica que o pedido foi rejeitado porque o cidadão já levou as 2 doses de vacina
    EMCURSO,                    // Indica que o pedido foi rejeitado porque já há outra consulta de vacinação em curso para esse cidadão
    NAOHAENFERMEIRO,            // Indica que o centro de saúde desse cidadão não tem nenhum enfermeiro registado ou o próprio centro de saúde não existe
    CANCELADA,                  // Indica que a sessão de vacinação foi cancelada
    TERMINADA                   // Indica que a sessão de vacinação terminou
} StatusServidor;

typedef struct {                // Estrutura utilizada para envio de mensagens pela fila de mensagens do Servidor Dedicado para o Cidadao
    long tipo;                  // Tipo da mensagem (Address): PID_cidadao para a comunicação do Servidor Dedicado para o Cidadao
    struct {                    // Estrutura de dados da mensagem
        StatusServidor status;  // Estado do Servidor como resposta ao pedido do Cidadao
        Cidadao cidadao;        // Dados do Cidadão em questão
    } dados;                    // Estrutura de dados específico desta mensagem
} MsgServidor;                  // Estas mensagens são as enviadas do Servidor Dedicado para o Cidadao

typedef struct {                                // Estrutura associada à Base de dados do Servidor
    Cidadao cidadaos[MAX_CIDADAOS];             // Array de Cidadãos que já foram vacinados
    int num_cidadaos;                           // Nº de Cidadãos registados no array acima
    Enfermeiro enfermeiros[MAX_ENFERMEIROS];    // Array de Enfermeiros
    int num_enfermeiros;                        // Nº de Enfermeiros registados no array acima
    Vaga vagas[MAX_VAGAS];                      // Vagas para vacinação
} Database;

#if IPC_KEY == 0x0a123456
    #error Não alterou o valor de IPC_KEY, por favor modifique esta constante usando o seu numero de aluno
#endif

/* OS ALUNOS NÃO DEVERÃO ACRESCENTAR NADA A ESTE FICHEIRO! */

#endif
