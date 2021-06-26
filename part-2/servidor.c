/******************************************************************************
 ** ISCTE-IUL: Trabalho prático de Sistemas Operativos
 **
 ** Aluno: Nº:       Nome: 
 ** Nome do Módulo: servidor.c
 ** Descrição/Explicação do Módulo: O processo servidor fica constantemente á espera
 ** de pedidos de vacinação. Quando recebe um pedido lê as variaveis do cidadao a ser
 ** vacinado e verifica se existe algum enfermeiro disponivel associado ao Centro de
 ** Saude da localidade do cidadao. Caso exista vai iniciar o processo de vacinação e
 ** atualizar o numero de vacinas dadas pelo enfermeiro no ficheiro enfermeiros.dat.
 ** Quando o processo de vacinação terminar, o servidor vai esperar por novos pedidos
 ** de vacinação.
 ******************************************************************************/
#include "common.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>

Vaga vagas[NUM_VAGAS];
Enfermeiro *ptr;
int NUM_ENF, n, pid_filho, pid_pai;
Cidadao cidadao;
int enf, vag;
int run = TRUE;
int exist_vaga = FALSE;

//Dá return do tamanho do ficheiro
int findSize(FILE *fp)
{
    int prev = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    fseek(fp, prev, SEEK_SET);
    return size;
}

//C6_&_S1 -> Escreve no ficheiro o PID do servidor
int input_PID()
{
    FILE *f;
    if (fopen("servidor.pid", "r"))
    {
        f = fopen("servidor.pid", "w");
        fprintf(f, "%d", getpid());
        fclose(f);
        sucesso("S1) Escrevi no ficheiro FILE_PID_SERVIDOR o PID: %d", getpid());
    }
    else
    {
        erro("S1) Não consegui registar o servidor!");
    }
}

//coloca a varial index_enfermeiro de todas as posiçoes do vetor vagas a -1
Vaga limpar_lista()
{
    for (int i = 0; i < NUM_VAGAS; i++)
    {
        vagas[i].index_enfermeiro = -1;
    }
    sucesso("S3) Iniciei a lista de %d vagas", NUM_VAGAS);
}

//Arma os sinais
void handler(int signum)
{
    if (signum == SIGUSR1)
    {
        signal(SIGINT, handler);
        FILE *f;
        if (fopen("servidor.pid", "r") == NULL)
        {
            erro("S5.1) Não foi possível abrir o ficheiro FILE_PEDIDO_VACINA");
        }
        if (f = fopen(FILE_PEDIDO_VACINA, "r"))
        {
            // Criar uma string que vai guardar todos os dados do cidadao
            // dividir a string em variaveis, ignorando o ':'
            char str[999];                // about citizens:
            fgets(str, 999, f);           // result array contains all data
            char *pch = strtok(str, ":"); // result[0] = num_utente
            char result[7][100];          // result[1] = nome
            for (int i = 0; pch != NULL; i++)
            {                            // result[2] = idade
                result[i][0] = '\0';     // result[3] = cidade
                strcpy(result[i], pch);  // result[4] = nr_telemovel
                pch = strtok(NULL, ":"); // result[5] = estado_vacinacao
            }                            // result[6] = PID_cidadao
            char CS_cidadao[] = "CS";
            strcat(CS_cidadao, result[3]);
            result[6][strlen(result[6]) - 1] = '\0';
            printf("Chegou o cidadão com o pedido nº %s, com nº utente %s, para ser vacinado no Centro de Saúde %s\n", result[6], result[0], CS_cidadao);
            sucesso("S5.1) Dados Cidadão: %s; %s; %s; %s; %s; 0", result[0], result[1], result[2], result[3], result[4]);
            //Associar cada variavel á estrutura Cidadao
            sscanf(result[0], "%d", &cidadao.num_utente);
            sscanf(result[1], "%s", cidadao.nome);
            sscanf(result[2], "%d", &cidadao.idade);
            sscanf(result[3], "%s", cidadao.localidade);
            sscanf(result[4], "%s", cidadao.nr_telemovel);
            sscanf(result[5], "%d", &cidadao.estado_vacinacao);
            sscanf(result[6], "%d", &cidadao.PID_cidadao);

            for (enf = 0; enf < NUM_ENF && run == TRUE; enf++)
            {
                if (strcmp(CS_cidadao, ptr[enf].CS_enfermeiro) == 0)
                {
                    if (ptr[enf].disponibilidade == 1)
                    {
                        for (vag = 0; vagas[vag].index_enfermeiro == -1 && vag < NUM_VAGAS && run == TRUE; vag++)
                        {
                            sucesso("S5.2.2) Há vaga para vacinação para o pedido %d", cidadao.PID_cidadao);
                            vagas[vag].cidadao = cidadao;
                            vagas[vag].index_enfermeiro = enf;
                            ptr[enf].disponibilidade = 0;
                            sucesso("S5.3) Vaga nº %d preenchida para o pedido %d", vag, cidadao.PID_cidadao);
                            exist_vaga = TRUE;
                            run = FALSE;
                            break;
                        }
                    }
                    else
                    {
                        erro("S5.2.1) Enfermeiro %d indisponível para o pedido %d para o Centro de Saúde %s", enf, cidadao.PID_cidadao, CS_cidadao);
                        kill(cidadao.PID_cidadao, SIGTERM);
                    }
                }
            }
            enf--;
            run = TRUE;
            if (exist_vaga == TRUE)
            {
                signal(SIGCHLD, handler);
                int new_pid = fork();

                if (new_pid < 0)
                {
                    erro("S5.4) Não foi possível criar o servidor dedicado");
                }
                else
                {
                    if (new_pid == 0)
                    {
                        //filho
                        pid_filho = getpid();
                        sucesso("S5.4) Servidor dedicado %d criado para o pedido %d", pid_filho, cidadao.PID_cidadao);
                        signal(SIGTERM, handler);
                        kill(cidadao.PID_cidadao, SIGUSR1);
                        sucesso("S5.6.2) Servidor dedicado inicia consulta de vacinação");
                        sleep(TEMPO_CONSULTA);
                        sucesso("S5.6.3) Vacinação terminada para o cidadão com o pedido nº %d", cidadao.PID_cidadao);
                        kill(cidadao.PID_cidadao, SIGUSR2);
                        sucesso("S5.6.4) Servidor dedicado termina consulta de vacinação");
                        kill(pid_filho, SIGKILL);
                    }
                    else
                    {
                        //pai
                        pid_pai = getpid();
                        vagas[vag].PID_filho = pid_filho;
                        sucesso("S5.5.1) Servidor dedicado %d na vaga %d", new_pid, vag);
                        sucesso("S5.5.2) Servidor aguarda fim do servidor dedicado %d\n", new_pid);
                    }
                }
            }
            else
            {
                erro("S5.2.2) Não há vaga para vacinação para o pedido %d", cidadao.PID_cidadao);
            }
            exist_vaga = false;
        }
        else
        {
            erro("S5.1) Não foi possível ler o ficheiro FILE_PEDIDO_VACINA\n");
        }
        exist_vaga = FALSE;
    }
    if (signum == SIGCHLD)
    {
        signal(SIGINT, handler);
        for (int i = 0; i < NUM_VAGAS; i++)
        {
            if (vagas[i].PID_filho == pid_filho)
            {
                vagas[i].index_enfermeiro = -1;
                sucesso("S5.5.3.1) Vaga %d que era do servidor dedicado %d libertada\n", vag, vagas[i].PID_filho);
                ptr[enf].disponibilidade = 1;
                sucesso("S5.5.3.2) Enfermeiro %d atualizado para disponível", enf);
                ptr[enf].num_vac_dadas++;
                sucesso("S5.5.3.3) Enfermeiro %d atualizado para %d vacinas dadas", enf, ptr[enf].num_vac_dadas);
                FILE *f;
                f = fopen(FILE_ENFERMEIROS, "wb");

                /*
                for (int i = 0; i < NUM_ENF; i++)
                {
                    printf("%d %s %s %d %d\n", ptr[i].ced_profissional, ptr[i].nome, ptr[i].CS_enfermeiro, ptr[i].num_vac_dadas, ptr[i].disponibilidade);
                }
                */

                fseek(f, 0, SEEK_SET);
                fwrite(ptr, sizeof(Enfermeiro), NUM_ENF, f);
                ptr[enf].num_vac_dadas;
                fclose(f);
                

                sucesso("S5.5.3.4) Ficheiro FILE_ENFERMEIROS %d atualizado para %d vacinas dadas", enf, ptr[enf].num_vac_dadas);
                sucesso("S5.5.3.5) Retorna");
                break;
            }
        }
    }
    if (signum == SIGTERM)
    {
        sucesso("S5.6.1) SIGTERM recebido, servidor dedicado termina Cidadão");
        kill(cidadao.PID_cidadao, SIGTERM);
        kill(pid_filho, SIGKILL);
    }
    if (signum == SIGINT)
    {
        remove(FILE_PID_SERVIDOR);
        sucesso("S6) Servidor terminado");
        free(ptr);
        kill(cidadao.PID_cidadao, SIGTERM);
        exit(0);
    }
}

int main()
{
    signal(SIGINT, handler);
    signal(SIGUSR1, handler);
    signal(SIGCHLD, handler);
    input_PID();
    Enfermeiro enfermeiro;
    Vaga vaga;

    FILE *fb;
    fb = fopen(FILE_ENFERMEIROS, "r");
    int sizeFile = findSize(fb);
    fclose(fb);

    int sizeStruct = sizeof(enfermeiro);
    NUM_ENF = (sizeFile / sizeStruct);

    ptr = (Enfermeiro *)malloc(NUM_ENF * sizeStruct);

    FILE *fo;
    if (fo = fopen(FILE_ENFERMEIROS, "rb"))
    {
        fseek(fo, 0, SEEK_SET);
        fread(ptr, sizeStruct, NUM_ENF, fo);
        sucesso("S2) Ficheiro FILE_ENFERMEIROS tem %d bytes, ou seja, %d enfermeiros\n", sizeFile, NUM_ENF);
    }
    else
    {
        erro("S2) Não consegui ler o ficheiro FILE_ENFERMEIROS!\n");
    }

    limpar_lista();

    while (1)
    {
        signal(SIGINT, handler);
        sucesso("S4) Servidor espera pedidos\n");
        sleep(30);
    }
}
