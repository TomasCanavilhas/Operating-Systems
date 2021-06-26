/******************************************************************************
 ** ISCTE-IUL: Trabalho prático de Sistemas Operativos
 **
 ** Aluno: Nº:       Nome: 
 ** Nome do Módulo: common.h
 ** Descrição/Explicação do Módulo: 
 **     Definição das estruturas de dados comuns aos módulos servidor e cidadao
 **     Os alunos não deverão alterar os nomes dos ficheiros definidos em FILE_*
 **
 ** OS ALUNOS NÃO DEVERÃO ACRESCENTAR NADA A ESTE FICHEIRO!!!
 **
 ******************************************************************************/
#ifndef __COMMON_H__
#define __COMMON_H__

#include "utils.h"

#define FILE_PEDIDO_VACINA "pedidovacina.txt"   // Nome do Ficheiro associado a um pedido de vacinas
#define FILE_PID_SERVIDOR  "servidor.pid"       // Nome do Ficheiro que armazena o PID do servidor
#define FILE_ENFERMEIROS   "enfermeiros.dat"    // Nome do Ficheiro que tem a lista de enfermeiros associados aos Centros de Saúde para vacinação

#define NUM_VAGAS 5          // Número de vagas de cidadãos que poderão estar a ser vacinados simultâneamente
#define TEMPO_CONSULTA 600   // Tempo em segundos que leva a realizar uma consulta de vacinação pelo processo Servidor-Filho

typedef struct {             // Estrutura associada a um Cidadão:
    int num_utente;          // Nº de Utente do cidadão
    char nome[100];          // Nome do Cidadão
    int idade;               // Idade do Cidadão
    char localidade[100];    // Localidade de residência do Cidadão. É usado para determinar o Centro de Saúde do Cidadão: "CS"+localidade
    char nr_telemovel[10];   // Telefone de contacto do Cidadão
    int estado_vacinacao;    // Valores Possíveis: 0 (não vacinado), 1 (vacinado 1ª inoculação) ou 2 (vacinado 2ª inoculação)
    int PID_cidadao;         // PID do processo cliente invocado pelo Cidadão para ser vacinado
} Cidadao;                   // Cada pedido de um processo Cidadão estará associado a uma estrutura destas.

typedef struct {             // Estrutura associada a um Enfermeiro:
    int ced_profissional;    // Cédula Profissional do Enfermeiro
    char nome [100];         // Nome do Enfermeiro
    char CS_enfermeiro[100]; // Centro de Saúde associado ao Enfermeiro, no formato "CS"+localidade
    int num_vac_dadas;       // Número de vacinas administradas pelo Enfermeiro. Iniciado a 0, este número será atualizado ao longo do tempo
    int disponibilidade;     // Disponibilidade do Enfermeiro: Durante o tempo que o Enfermeiro estiver a dar uma vacina, o Enfermeiro estará indisponível (0), caso contrário, estará disponível (1)
} Enfermeiro;                // Haverá tantos elementos quantos os existentes no ficheiro FILE_ENFERMEIROS.

typedef struct {             // Estrutura associada a uma vaga de um Cidadão a ser vacinado:
    int PID_filho;           // PID do processo Servidor-Filho dedicado a uma sessão de vacinação
    Cidadao cidadao;         // Cidadão associado à sessão de vacinação
    int index_enfermeiro;    // Enfermeiro associado à sessão de vacinação (índice do array de Enfermeiros)
} Vaga;                      // Poderão existir no máximo NUM_VAGAS elementos deste tipo.


/* OS ALUNOS NÃO DEVERÃO ACRESCENTAR NADA A ESTE FICHEIRO!!! */

#endif