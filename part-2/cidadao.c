/******************************************************************************
 ** ISCTE-IUL: Trabalho prático de Sistemas Operativos
 **
 ** Aluno: Nº:98804       Nome:Tomás Casaes Canavilhas Fernandes 
 ** Nome do Módulo: cidadao.c
 ** Descrição/Explicação do Módulo: Este modulo permite criar um ficheiro chamado
 ** pedidovacina.txt que vai armazenar as informações do cidadão que foram providenciadas.
 ** Após a criação deste ficheiro vai enviar um sinal (SIGUSR1) ao processo servidor para 
 ** que este inicie a processo de vacinação. Mais tarde, irá receber uma resposta do pedido
 ** feito.
 ******************************************************************************/
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h> 

    //LER SERVIDOR.PID C6 -> Lê o PID do ficheiro FILE_PID_SERVIDOR
    int read_pid () {
        FILE *f;
        int pid;

        //Caso não exista o ficheiro
        if ( !(f = fopen(FILE_PID_SERVIDOR, "r"))) return 0;

        //Lê o PID do ficheiro servidor.pid
        fscanf(f,"%d", &pid);
        fclose(f);
        return pid;
    }

    int n = 0;

    //Handler -> Arma os sinais
    void handler(int signum) {
        if(signum == SIGINT) {
            sucesso("C5) O cidadão cancelou a vacinação, o pedido nº %d foi cancelado\n", getpid());
            remove("pedidovacina.txt");
            exit(0);
        }
        if(signum == SIGUSR1) {
            sucesso("C7) Vacinação do cidadão com o pedido nº %d em curso", getpid());
            remove("pedidovacina.txt");
        } 
        if(signum == SIGUSR2) {
            sucesso("C7) Vacinação do cidadão com o pedido nº %d concluída", getpid());
            exit(0);
        }
        if(signum == SIGTERM) {
            sucesso("C9) Não é possível vacinar o cidadão no pedido nº %d", getpid());
            remove("pedidovacina.txt");
            exit(0);
        }
        if(signum == SIGALRM) {
            erro("C3) Não é possível iniciar o processo de vacinação neste momento\n");
            n = 1;
        }
    }

    int exists(const char *fname) {
        FILE *file;
        if ((file = fopen(fname, "r"))) {
            fclose(file);
            return 1;
        }
        return 0;
    }

int main() {
        
    Cidadao cidadao;
    Enfermeiro enfermeiro;

    //ESTADO_VACINAÇÃO C1
    cidadao.estado_vacinacao = 0;

    //Input dos dados do cidadao

    //CEDULA C1
    printf( "Introduza o numero do utente: "); scanf( "%d" , &cidadao.num_utente );
    
    //NOME C1
    printf("Introduza o nome do utente: "); scanf(" %[^\n]", cidadao.nome); 

    //IDADE C1
    printf( "Introduza a idade do utente: "); scanf( "%d" , &cidadao.idade );
    
    //CIDADE C1
    printf("Introduza a cidade do utente: "); scanf(" %[^\n]", cidadao.localidade);

    //TELEMOVEL C1
    printf("Introduza o numero de telemovel do utente: "); scanf(" %[^\n]", cidadao.nr_telemovel);
    

    //PID C2 
    cidadao.PID_cidadao = getpid();

    //Output C1 -> Output dos dados do cidadao
    sucesso("C1) %d;%s;%d;%s;%s;%d;%d;\n",cidadao.num_utente, cidadao.nome, cidadao.idade, cidadao.localidade, cidadao.nr_telemovel, cidadao.estado_vacinacao, cidadao.PID_cidadao);

    //Output C2 -> Output do PID do cidadao
    sucesso("C2) PID Cidadão: %d\n", cidadao.PID_cidadao);
    signal(SIGINT, handler);

    //C3 e C10 -> Cria o ficheiro FILE_PEDIDO_VACINA onde vai guardar os dados do cidadao
    while(exists(FILE_PEDIDO_VACINA)){
        signal(SIGALRM, handler);
        alarm(5);
        while(n == 0) pause();
        n = 0; 
    }
    sucesso("C3) Ficheiro FILE_PEDIDO_VACINA pode ser criado\n");
    FILE *f;
    f = fopen(FILE_PEDIDO_VACINA, "w");
    fprintf(f,"%d:%s:%d:%s:%s:%d:%d\n",cidadao.num_utente, cidadao.nome, cidadao.idade, cidadao.localidade, cidadao.nr_telemovel, cidadao.estado_vacinacao, cidadao.PID_cidadao);
    fclose(f);
    if(exists(FILE_PEDIDO_VACINA)){
        sucesso("C4) Ficheiro FILE_PEDIDO_VACINA criado e preenchido\n");
    } else {
        erro("C4) Não é possível criar o ficheiro FILE_PEDIDO_VACINA\n");
    }

    //SIGINT C4
    signal(SIGINT, handler);

    //SIGUSR1 C6 -> lê o pid do servidor se existir e envia um sinal ao servidor para ver é possivel iniciar a vacinaçao
        if(exists(FILE_PID_SERVIDOR)){
            int PID_SERVIDOR = read_pid();
            kill(PID_SERVIDOR, SIGUSR1);
            sucesso("C6) Sinal enviado ao Servidor: %d", PID_SERVIDOR);
        } else {
            erro("C6) Não existe ficheiro FILE_PID_SERVIDOR!");
        }

    //C8 e C9 -> Arma os sinais
    signal(SIGUSR1, handler);
    signal(SIGUSR2, handler);
    signal(SIGTERM, handler);

    //espera pela resposta do servidor
    while(1);

}