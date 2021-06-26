/******************************************************************************
 ** ISCTE-IUL: Trabalho prático 3 de Sistemas Operativos
 **
 ** Aluno: Nº:       Nome: 
 ** Nome do Módulo: cidadao.c v3
 ** Descrição/Explicação do Módulo: 
 **
 **
 ******************************************************************************/
#include "common.h"
#include "utils.h"
#include <signal.h>
#include <unistd.h>
#include <sys/msg.h>

/* Variáveis globais */
int msg_id;             // ID da Fila de Mensagens usada
MsgCliente mensagem;    // Variável que tem a mensagem enviada do Cidadao para o Servidor
MsgServidor resposta;   // Variável que tem a mensagem de resposta enviadas do Servidor para o Cidadao
int n = 0;

void handler(int signum){
    if( signum == SIGALRM){
        n=1;
    }
}

/* Protótipos de funções */
void init_ipc();                    // Função a ser implementada pelos alunos
void cria_mensagem();               // Função a ser implementada pelos alunos
void envia_mensagem_servidor();     // Função a ser implementada pelos alunos
void espera_resposta_servidor();    // Função a ser implementada pelos alunos
void trata_resposta_servidor();     // Função a ser implementada pelos alunos
void pedido();                      // Função a ser implementada pelos alunos
void vacina();                      // Função a ser implementada pelos alunos
void cancela_pedido(int);           // Função a ser implementada pelos alunos
void print_info(Cidadao cidadao);   

int main() {    // Não é suposto que os alunos alterem nada na função main()
    // C1) Chama a função init_ipc(), que tenta abrir uma fila de mensagens IPC que tem a KEY IPC_KEY definida em common.h (alterar esta KEY para ter o valor do nº do aluno, como indicado nas aulas). Deve assumir que a fila de mensagens já foi criada. Se tal não aconteceu, dá erro e termina com exit status 1. Esta função, em caso de sucesso, preenche a variável global msg_id;
    init_ipc();
    // C2) Chama a função cria_mensagem()
    cria_mensagem();
    // C7) Arma e trata o sinal SIGINT para que, quando o utilizador interromper o processo Cidadão com <CTRL+C>, chame a função cancela_pedido()
    signal(SIGINT, cancela_pedido);
    // Faz o pedido ao servidor e aguarda a resposta do mesmo
    pedido();
    // C6) Inicia o processo de vacinação
    vacina();
}

/**
 * C1) tenta abrir uma fila de mensagens que tem a KEY IPC_KEY
 * definida em common.h (alterar esta KEY para ter o valor do nº do aluno, como indicado nas aulas).
 * Deve assumir que a fila de mensagens já foi criada.
 * Se tal não aconteceu, dá erro e termina com exit status 1.
 * Esta função, em caso de sucesso, preenche a variável global msg_id;
 */
void init_ipc() {
    debug("<");

    // Outputs esperados (os itens entre <> deverão ser substituídos pelos valores correspondentes):
    msg_id = msgget( IPC_KEY, 0600 | IPC_CREAT);
    exit_on_error(msg_id, "C1) Fila de Mensagens com a Key definida não existe ou não pode ser aberta");

    sucesso("C1) Fila de Mensagens com a Key %x aberta com o ID %d", IPC_KEY, msg_id);

    debug(">");
}

/**
 * C2.1) Pede ao Cidadão (utilizador) os seus dados, nomeadamente o número de utente e nome, obrigatoriamente nessa ordem, preenchendo os dados na variável global mensagem;
 * C2.2) Preenche os campos PID_cidadao da variável global mensagem com o PID deste processo Cidadão, tipo da mensagem com o tipo 1, e pedido = PEDIDO;
 */
void cria_mensagem() {
    debug("<");

    // C2.1) Pede ao Cidadão (utilizador) os seus dados, nomeadamente o número de utente e nome, obrigatoriamente nessa ordem, preenchendo os dados na variável global mensagem;
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):

    printf( "Introduza o numero do utente:\n" ); scanf( "%d" , &mensagem.dados.num_utente );
    printf( "Introduza o nome do utente:\n" ); scanf(" %[^\n]", mensagem.dados.nome);

    sucesso("C2.1) Dados Cidadão: %d, %s", mensagem.dados.num_utente, mensagem.dados.nome);

    // C2.2) Preenche os campos PID_cidadao da variável global mensagem com o PID deste processo Cidadão, tipo da mensagem com o tipo 1, e pedido = PEDIDO;
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    mensagem.dados.PID_cidadao = getpid();
    sucesso("C2.2) PID Cidadão: %d", getpid());

    debug(">");
}

/**
 * Envia a mensagem para a fila de mensagens; em caso de erro no envio, termina com erro e exit status 1.
 */
void envia_mensagem_servidor() {
    debug("<");

    // Envia a mensagem que está na variável global mensagem para a fila de mensagens; em caso de erro, termina com erro e exit status 1.
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    mensagem.tipo = 1;
    int status = msgsnd( msg_id, &mensagem, sizeof(mensagem.dados), 0);
    exit_on_error(status, "Não é possível enviar mensagem para o servidor");

    sucesso("Mensagem para o servidor enviada");

    debug(">");
}

/**
 * Espera a resposta do processo Servidor (na fila de mensagens com o tipo = PID_Cidadao) e preenche a mensagem enviada pelo processo Servidor na variável global resposta; 
 */
void espera_resposta_servidor() {
    debug("<");

    // Espera a resposta do processo Servidor (na fila de mensagens com o tipo = PID_Cidadao) e preenche a mensagem enviada pelo processo Servidor na variável global resposta; 
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    //for(;;){
        int status2 = msgrcv( msg_id, &resposta, sizeof(resposta.dados), getpid(), 0);
        exit_on_error(status2, "Não é possível ler a resposta do servidor");
        
        //resposta.tipo = mensagem.dados.PID_cidadao;
        sucesso("Servidor enviou resposta");
    //}

    debug(">");
}

/**
 * Envia o pedido ao servidor e aguarda a sua resposta
 */
void pedido() {
    debug("<");

    do {
        // C3) Envia um pedido de consulta de vacinação para o processo Servidor, chamando a função envia_mensagem_servidor(), que envia uma mensagem para a fila de mensagens com tipo 1, com pedido = PEDIDO e os dados do cidadão; em caso de erro, termina com erro e exit status 1.
        envia_mensagem_servidor();
        // C4) Chama a função espera_resposta_servidor(), que espera a resposta do processo Servidor (na fila de mensagens com o tipo = PID_Cidadao) e preenche a mensagem enviada pelo processo Servidor na variável global resposta; em caso de erro, termina com erro e exit status 1.
        espera_resposta_servidor();

        // C5) O comportamento do processo Cidadão agora irá depender da resposta enviada pelo processo Servidor no campo status:

        // C5.1) Se o status for DESCONHECIDO, imprime uma mensagem de erro, e termina com exit status 1;
        // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
        if( resposta.dados.status == DESCONHECIDO){
            erro("C5.1) Não existe registo do utente %d, %s", mensagem.dados.num_utente, mensagem.dados.nome);
            exit(1);
        }
        // C5.1) Se o status for NAOHAENFERMEIRO, imprime uma mensagem de erro, e termina com exit status 1;
        // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
        if( resposta.dados.status == NAOHAENFERMEIRO){
            erro("C5.1) Não existe enfermeiro na localidade do utente %d, %s", mensagem.dados.num_utente, mensagem.dados.nome);
            exit(1);
        }
        // C5.2) Se o status for VACINADO, imprime uma mensagem de sucesso, e termina com exit status 0;
        // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
        if( resposta.dados.status == VACINADO){
            sucesso("C5.2) O utente %d, %s já foi vacinado", mensagem.dados.num_utente, mensagem.dados.nome);
            exit(0);
        }
        // C5.2) Se o status for EMCURSO, imprime uma mensagem de sucesso, e termina com exit status 0;
        // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
        if( resposta.dados.status == EMCURSO){
            sucesso("C5.2) A vacinação do utente %d, %s já está em curso", mensagem.dados.num_utente, mensagem.dados.nome);
            exit(0);
        }
        // C5.3) Se o status for AGUARDAR, imprime uma mensagem de sucesso, aguarda (sem espera ativa!) um tempo correspondente a TEMPO_ESPERA segundos, e depois retorna ao ponto C3;
        // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
        if( resposta.dados.status == AGUARDAR){
            sucesso("C5.3) Utente %d, %s, por favor aguarde...", mensagem.dados.num_utente, mensagem.dados.nome);
            signal( SIGALRM, handler );
            alarm(TEMPO_ESPERA);
            while( n == 0 ) pause();
            envia_mensagem_servidor();
        }
        // C5.4) Se o status for OK, imprime uma mensagem de sucesso, e depois vai para o ponto C6.
        // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
        if( resposta.dados.status == OK){
            sucesso("C5.4) Utente %d, %s, vai agora ser vacinado", mensagem.dados.num_utente, mensagem.dados.nome);
        }
    } while (OK != resposta.dados.status);

    debug(">");
}

/**
 * Imprime informação sobre o utente
 * @param cidadao Cidadao a reportar
 **/
void print_info(Cidadao cidadao) {
    debug("<");
    printf("Número de utente: %d\n", cidadao.num_utente);
    printf("Nome            : %s\n", cidadao.nome);
    printf("Idade           : %d ano%s\n", cidadao.idade, cidadao.idade > 1 ? "s" : "");
    printf("Localidade      : %s\n", cidadao.localidade);
    printf("N.º Telemóvel   : %s\n", cidadao.nr_telemovel);
    printf("Vacina          : %dª dose\n", cidadao.estado_vacinacao + 1);
    debug(">");
}

/**
 * Espera a resposta do processo Servidor (na fila de mensagens com o tipo = PID_Cidadao) e preenche a mensagem enviada pelo processo Servidor na variável global resposta; 
 */
void vacina() {
    debug("<");

    // C6.1) Chama a função print_info(cidadao) com a informação recebida na resposta do processo Servidor, que irá imprimir a informação completa sobre o cidadão que vai ser vacinado;
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    sucesso("C6.1) Dados completos sobre o cidadão a ser vacinado");

    print_info(resposta.dados.cidadao);

    // C6.2) Chama novamente a função espera_resposta_servidor(), que espera uma nova resposta do processo Servidor (na fila de mensagens com o tipo = PID_Cidadao) e preenche a mensagem enviada pelo processo Servidor na variável global resposta;
    espera_resposta_servidor();
    // C6.3) O comportamento do processo Cidadão agora irá depender da resposta enviada pelo processo Servidor no campo status:

    // C6.3.1) Se o status for TERMINADA, imprime uma mensagem de sucesso, e termina com exit status 0;
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    if( resposta.dados.status == TERMINADA){
        sucesso("C6.3.1) Utente %d, %s vacinado com sucesso", mensagem.dados.num_utente, mensagem.dados.nome);
        exit(0);
    }
    // C6.3.2) Se o status for CANCELADA, imprime uma mensagem de erro, e termina com exit status 1;
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
        if( resposta.dados.status == CANCELADA ){
        erro("C6.3.2) O servidor cancelou a vacinação em curso");
        exit(1);
    }
    debug(">");
}

/**
 * C7) Quando o utilizador interromper o processo Cidadão com <CTRL+C>
 */
void cancela_pedido(int sinal) {
    debug("<");

    // C7.1) Escreve no ecrã uma mensagem;
    // Outputs esperados (os itens entre <> deverão ser substituídos pelos valores correspondentes):
    sucesso("C7.1) O cidadão cancelou a vacinação no processo %d", mensagem.dados.PID_cidadao);

    // C7.2) Altera a variável global mensagem, tornando pedido = CANCELAMENTO. Chama a função envia_mensagem_servidor(), que envia a mensagem para a fila de mensagens; em caso de erro no envio, afixa uma mensagem de erro e termina com exit status 1;
    mensagem.dados.pedido = CANCELAMENTO;
    envia_mensagem_servidor();

    // C7.3) Chama novamente a função espera_resposta_servidor(), que espera uma nova resposta do processo Servidor (na fila de mensagens com o tipo = PID_Cidadao) e preenche a mensagem enviada pelo processo Servidor na variável global resposta;
    espera_resposta_servidor();

    // C7.4) O comportamento do processo Cidadão agora irá depender da resposta enviada pelo processo Servidor no campo status:

    // C7.4.1) Se o status for CANCELADA, imprime uma mensagem de sucesso, e termina com exit status 0;
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    if( resposta.dados.status == CANCELADA ){
        erro(" 7.4.1) Servidor confirmou cancelamento");
        exit(0);
    }

    // C7.4.2) Se o status for TERMINADA, imprime mensagem sucesso, termina com exit status 0;
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    if ( resposta.dados.status == TERMINADA ){
        sucesso("7.4.2) A vacinação já tinha sido concluída");
        exit(0);
    }

    debug(">");
}