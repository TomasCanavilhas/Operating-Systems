/******************************************************************************
 ** ISCTE-IUL: Trabalho prático 3 de Sistemas Operativos
 **
 ** Aluno: Nº:       Nome: 
 ** Nome do Módulo: servidor.c v3
 ** Descrição/Explicação do Módulo: 
 **
 **
 ******************************************************************************/
#include "common.h"
#include "utils.h"
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/shm.h>

/* Variáveis globais */
int msg_id;             // ID da Fila de Mensagens IPC usada
int sem_id;             // ID do array de Semáforos IPC usado
int shm_id;             // ID da Memória Partilhada IPC usada
Database* db;           // Database utilizada, que estará em Memória Partilhada
MsgCliente mensagem;    // Variável que tem a mensagem enviada do Cidadao para o Servidor
MsgServidor resposta;   // Variável que tem a mensagem de resposta enviadas do Servidor para o Cidadao
int vaga_ativa;         // Índice da BD de Vagas que foi reservado pela função reserva_vaga()
int pausa = 0, pid_filho;

/* Protótipos de funções */
void init_ipc();                    // Função a ser implementada pelos alunos
void init_database();               // Função a ser implementada pelos alunos
void espera_mensagem_cidadao();     // Função a ser implementada pelos alunos
void trata_mensagem_cidadao();      // Função a ser implementada pelos alunos
void envia_resposta_cidadao();      // Função a ser implementada pelos alunos
void processa_pedido();             // Função a ser implementada pelos alunos
void vacina();                      // Função a ser implementada pelos alunos
void cancela_pedido();              // Função a ser implementada pelos alunos
void servidor_dedicado();           // Função a ser implementada pelos alunos
int reserva_vaga(int, int);         // Função a ser implementada pelos alunos
void liberta_vaga(int);             // Função a ser implementada pelos alunos
void termina_servidor(int);         // Função a ser implementada pelos alunos
void termina_servidor_dedicado(int);// Função a ser implementada pelos alunos
void sem_mutex_up();
void sem_mutex_down();

void handler(int signum){
    if( signum == SIGALRM ){
        pausa = 1;
    }
}

int main() {    // Não é suposto que os alunos alterem nada na função main()
    signal(SIGINT, termina_servidor);   // Arma o sinal SIGINT para que se receber <CTRL+C>, chama a função que termina o Servidor
    signal(SIGCHLD, SIG_IGN);
    // S1) Chama a função init_ipc(), que tenta criar uma fila de mensagens que tem a KEY IPC_KEY definida em common.h (alterar esta KEY para ter o valor do nº do aluno, como indicado nas aulas). Deve assumir que a fila de mensagens já foi criada. Se tal não aconteceu, dá erro e termina com exit status 1. Esta função, em caso de sucesso, preenche a variável global msg_id;
    init_ipc();
    // S2) Chama a função init_database(), que inicia a base de dados
    init_database();
    while (TRUE) {
        // S3) Chama a função espera_mensagem_cidadao(), que espera uma mensagem (na fila de mensagens com o tipo = 1) e preenche a mensagem enviada pelo processo Cidadão na variável global mensagem; em caso de erro, termina com erro e exit status 1;
        espera_mensagem_cidadao();
        // S4) O comportamento do processo Servidor agora irá depender da mensagem enviada pelo processo Cidadão no campo pedido:
        trata_mensagem_cidadao();
    }
}

/**
 * S1) Chama a função init_ipc(), que tenta criar:
 *     • uma fila de mensagens IPC;
 *     • um array de semáforos IPC de dimensão 1;
 *     • uma memória partilhada IPC de dimensão suficiente para conter um elemento Database.
 *     Todos estes elementos têm em comum serem criados com a KEY IPC_KEY definida em common.h (alterar esta KEY para ter o valor do nº do aluno, como indicado nas aulas), e com permissões 0600. Se qualquer um destes elementos IPC já existia anteriormente, dá erro e termina com exit status 1. Esta função, em caso de sucesso, preenche as variáveis globais respetivas msg_id, sem_id, e shm_id;
 *     O semáforo em questão será usado com o padrão “Mutex”, pelo que será iniciado com o valor 1;
 */
void init_ipc() {
    debug("<");

    // S1) Tenta criar:
    // Todos estes elementos têm em comum serem criados com a KEY IPC_KEY definida em common.h (alterar esta KEY para ter o valor do nº do aluno, como indicado nas aulas), e com permissões 0600. Se qualquer um destes elementos IPC já existia anteriormente, dá erro e termina com exit status 1. Esta função, em caso de sucesso, preenche as variáveis globais respetivas msg_id, sem_id, e shm_id;
    // • uma fila de mensagens IPC;
    msg_id =  msgget ( IPC_KEY , IPC_CREAT | 0666 );
    exit_on_error(msg_id, "S1) Fila de Mensagens com a Key definida já existe ou não pode ser criada");
    


    // • um array de semáforos IPC de dimensão 1;
    sem_id =  semget ( IPC_KEY , 1, IPC_CREAT | 0666 );
    exit_on_error(sem_id, "S1) Semáforo com a Key definida já existe ou não pode ser criada");    

    // O semáforo em questão será usado com o padrão “Mutex”, pelo que será iniciado com o valor 1;
    int mutex =  semctl ( sem_id, IPC_RMID, SETVAL, 1);
    exit_on_error(mutex, "S1) Semáforo com a Key definida não pode ser iniciado com o valor 1");

    // • uma memória partilhada IPC de dimensão suficiente para conter um elemento Database.
    shm_id = shmget ( IPC_KEY, sizeof(Database), IPC_CREAT | 0666 );
    exit_on_error(shm_id, "S1) Memória Partilhada com a Key definida já existe ou não pode ser criada");

    sucesso("S1) Criados elementos IPC com a Key 0x%x: MSGid %d, SEMid %d, SHMid %d", IPC_KEY, msg_id, sem_id, shm_id);

    debug(">");
}

/**
 * Lê um ficheiro binário
 * @param   filename    Nome do ficheiro a ler
 * @param   buffer      Ponteiro para o buffer onde armazenar os dados
 * @param   maxsize     Tamanho máximo do ficheiro a ler
 * @return              Número de bytes lidos, ou 0 em caso de erro
 */
int read_binary(char* filename, void* buffer, const size_t maxsize) {
    struct stat st;
    // A função stat() preenche uma estrutura com dados do ficheiro, incluindo o tamanho do ficheiro.
    // stat() retorna -1 se erro
    exit_on_error(stat(filename, &st), "Erro no cálculo do tamanho do ficheiro");
    // O tamanho do ficheiro é maior do que o tamanho do buffer alocado?
    if (st.st_size > maxsize)
        exit_on_error(-1, "O buffer não tem espaço para o ficheiro");

    FILE* f = fopen(filename, "r");
    // fopen retorna NULL se erro
    exit_on_null(f, "Erro na abertura do ficheiro");

    // fread está a ler st.st_size elementos, logo retorna um valor < st.st_size se erro
    if (fread(buffer, 1, st.st_size, f) < st.st_size)
        exit_on_error(-1, "Erro na leitura do ficheiro");

    fclose(f);
    return st.st_size; // retorna o tamanho do ficheiro
}

/**
 * Grava um ficheiro binário
 * @param   filename    Nome do ficheiro a escrever
 * @param   buffer      Ponteiro para o buffer que contém os dados
 * @param   size        Número de bytes a escrever
 * @return              Número de bytes escrever, ou 0 em caso de erro
 */
int save_binary(char* filename, void* buffer, const size_t size) {
    FILE* f = fopen(filename, "w");
    // fopen retorna NULL se erro
    exit_on_null(f, "Erro na abertura do ficheiro");
   
    // fwrite está a escrever size elementos, logo retorna um valor < size se erro
    if (fwrite(buffer, 1, size, f) < size)
        exit_on_error(-1, "Erro na escrita do ficheiro");

    fclose(f);
    return size;
}

/**
 * S2) Inicia a base de dados:
 *     • Associa a variável global db com o espaço de Memória Partilhada alocado para shm_id; se não o conseguir, dá erro e termina com exit status 1;
 *     • Lê o ficheiro FILE_CIDADAOS e armazena o seu conteúdo na base de dados usando a função read_binary(), assim preenchendo os campos db->cidadaos e db->num_cidadaos. Se não o conseguir, dá erro e termina com exit status 1;
 *     • Lê o ficheiro FILE_ENFERMEIROS e armazena o seu conteúdo na base de dados usando a função read_binary(), assim preenchendo os campos db->enfermeiros e db->num_enfermeiros. Se não o conseguir, dá erro e termina com exit status 1;
 *     • Inicia o array db->vagas, colocando todos os campos de todos os elementos com o valor -1.
 */
void init_database() {
    debug("<");

    // S2) Inicia a base de dados:
    // • Associa a variável global db com o espaço de Memória Partilhada alocado para shm_id; se não o conseguir, dá erro e termina com exit status 1;
    db = (Database*) shmat(shm_id, NULL, 0);
    exit_on_null(db, "S2) Erro a ligar a Memória Dinâmica ao projeto");

    // • Lê o ficheiro FILE_CIDADAOS e armazena o seu conteúdo na base de dados usando a função read_binary(), assim preenchendo os campos db->cidadaos e db->num_cidadaos. Se não o conseguir, dá erro e termina com exit status 1;
    int read_cidadaos = read_binary( FILE_CIDADAOS , db->cidadaos, MAX_CIDADAOS * sizeof(Cidadao) );
    exit_on_error(read_cidadaos, "Erro na abertura do ficheiro");
    db->num_cidadaos = read_cidadaos / sizeof(Cidadao);
    if( db->num_cidadaos < 0 ){
        erro("Erro na leitura do ficheiro");
        exit(1);
    }

    // • Lê o ficheiro FILE_ENFERMEIROS e armazena o seu conteúdo na base de dados usando a função read_binary(), assim preenchendo os campos db->enfermeiros e db->num_enfermeiros. Se não o conseguir, dá erro e termina com exit status 1;
    int read_enfermeiros = read_binary(FILE_ENFERMEIROS, db->enfermeiros, MAX_ENFERMEIROS * sizeof(Enfermeiro) );
    exit_on_error(read_enfermeiros, "Erro na abertura do ficheiro");
    db->num_enfermeiros = read_enfermeiros / sizeof(Enfermeiro);
    if( db->num_enfermeiros < 0 ){
        erro("Erro na leitura do ficheiro");
        exit(1);
    }    

    // • Inicia a Base de Dados de Vagas, db->vagas, colocando o campo index_cidadao de todos os elementos com o valor -1.
    for( int i = 0; i < MAX_VAGAS; i++ ){
        db->vagas[i].index_cidadao = -1;
    }

    sucesso("S2) Base de dados carregada com %d cidadãos e %d enfermeiros", db->num_cidadaos, db->num_enfermeiros);

    debug(">");
}

/**
 * Espera uma mensagem (na fila de mensagens com o tipo = 1) e preenche a variável global mensagem, assim como preenche o tipo da resposta com o PID_cidadao recebido.
 * Em caso de erro, termina com erro e exit status 1;
 */
void espera_mensagem_cidadao() {
    debug("<");
    // Espera uma mensagem (na fila de mensagens com o tipo = 1) e preenche a variável global mensagem,
    int status = msgrcv( msg_id, &mensagem, sizeof(mensagem.dados), 1, 0);
    exit_on_error(status, "Não é possível ler a mensagem do Cidadao");
    // assim como preenche o tipo da resposta com o PID_cidadao recebido.
    resposta.tipo = mensagem.dados.PID_cidadao;

    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    sucesso("Cidadão enviou mensagem");
    debug(">");
}

/**
 * Tratamento das mensagens do Cidadao
 */
void trata_mensagem_cidadao() {
    debug("<");

    // S4) O comportamento do processo Servidor agora irá depender da variável global mensagem enviada pelo processo Cidadão no campo pedido

    if (mensagem.dados.pedido == PEDIDO) {
        // S4.1) Se o pedido for PEDIDO, imprime uma mensagem e avança para o passo S5;
        // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
        sucesso("S4.1) Novo pedido de vacinação de %d: %d, %s", mensagem.dados.PID_cidadao, mensagem.dados.num_utente, mensagem.dados.nome);
        processa_pedido();
    } else if (mensagem.dados.pedido == CANCELAMENTO) {
        // S4.2) Se o estado for CANCELAMENTO, imprime uma mensagem, e avança para o passo S10;
        // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
        sucesso("S4.2) Cancelamento de vacinação de %d: %d, %s", mensagem.dados.PID_cidadao, mensagem.dados.num_utente, mensagem.dados.nome);
        cancela_pedido();
    }

    debug(">");
}

/**
 * Estando a mensagem de resposta do processo Servidor na variável global resposta, envia essa mensagem para a fila de mensagens com o tipo = PID_Cidadao 
 */
void envia_resposta_cidadao() {
    debug("<");
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    resposta.tipo = mensagem.dados.PID_cidadao;
    int status_snd = msgsnd( msg_id, &resposta, sizeof(resposta.dados), 0);
    exit_on_error(status_snd, "Não é possível enviar resposta para o cidadão");

    sucesso("Resposta para o cidadão enviada");

    debug(">");
}

/**
 * S5) Processa um pedido de vacinação e envia uma resposta ao processo Cidadão. Para tal, essa função faz vários checks, atualizando o campo status da resposta:
 */
void processa_pedido() {
    debug("<");
    sem_mutex_down();
    int a = 0;
    resposta.dados.status = OK;


    // S5.1) Procura o num_utente e nome na base de dados (BD) de Cidadãos:
    for(int i = 0; i != db->num_cidadaos; i++){
        if(mensagem.dados.num_utente != db->cidadaos[i].num_utente){

        } else {
            // • Se o utilizador (Cidadão) for encontrado na BD Cidadãos, os dados do cidadão deverão ser copiados da BD Cidadãos para o campo cidadao da resposta;
            resposta.dados.cidadao = db -> cidadaos[i];
            a = 1;
            sucesso("S5.1) Cidadão %d, %s encontrado, estado_vacinacao=%d, status=%d", mensagem.dados.num_utente, mensagem.dados.nome, resposta.dados.cidadao.estado_vacinacao, resposta.dados.status);
        
            if(db->cidadaos[i].estado_vacinacao == 2){
                // • Se o Cidadão na BD Cidadãos tiver estado_vacinacao = 2 => status = VACINADO;
                resposta.dados.status = VACINADO;
                break;
            }
            
            if(db->cidadaos[i].PID_cidadao > 0){
                // • Se o Cidadão na BD Cidadãos tiver PID_cidadao > 0 => status = EMCURSO; caso contrário, afeta o PID_cidadao da BD Cidadãos com o valor do PID_cidadao da mensagem;
                resposta.dados.status = EMCURSO;
                break;
            } else {
                db->cidadaos[i].PID_cidadao = mensagem.dados.PID_cidadao;
                break;
            }
        }
    }
    if(a == 0){
        // • Se o utilizador (Cidadão) não for encontrado na BD Cidadãos => status = DESCONHECIDO;
        resposta.dados.status = DESCONHECIDO;
        erro("S5.1) Cidadão %d, %s  não foi encontrado na BD Cidadãos", mensagem.dados.num_utente, mensagem.dados.nome);
    }

    // S5.2) Caso o Cidadão esteja em condições de ser vacinado (i.e., se status não for DESCONHECIDO, VACINADO nem EMCURSO), procura o enfermeiro correspondente na BD Enfermeiros:
    int size = 100;
    a = 0; int b = 0;
    char CS[100] = {"CS"};
    strncat(CS, resposta.dados.cidadao.localidade, size);
    if(resposta.dados.status != DESCONHECIDO && resposta.dados.status != VACINADO && resposta.dados.status != EMCURSO){
        for(int i = 0; i<db->num_enfermeiros; i++){
            if(strcmp(db->enfermeiros[i].CS_enfermeiro, CS) != 0){
                // • Se não houver centro de saúde, ou não houver nenhum enfermeiro no centro de saúde correspondente => status = NAOHAENFERMEIRO;            
                resposta.dados.status = NAOHAENFERMEIRO;
            } else if(db->enfermeiros[i].disponibilidade != 1){
                // • Se há enfermeiro, mas este não tiver disponibilidade => status = AGUARDAR.
                resposta.dados.status = AGUARDAR;
            } else {
                a = 1;
                sucesso("S5.2) Enfermeiro do CS %s encontrado, disponibilidade=%d, status=%d", resposta.dados.cidadao.localidade, db->enfermeiros[i].disponibilidade, resposta.dados.status);
                // S5.3) Caso o enfermeiro esteja disponível, procura uma vaga para vacinação na BD Vagas. Para tal, chama a função reserva_vaga(Index_Cidadao, Index_Enfermeiro) usando os índices do Cidadão e do Enfermeiro nas respetivas BDs:
                // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
                for(int j = 0; j < MAX_VAGAS; j++){
                    if( reserva_vaga(db->vagas[j].index_cidadao, db->vagas[j].index_enfermeiro) == -1){
                        // • Se essa função não conseguiu encontrar uma vaga livre => status = AGUARDAR.
                        resposta.dados.status = AGUARDAR;
                    } else {
                        // • Se essa função tiver encontrado e reservado uma vaga => status = OK;
                        resposta.dados.status = OK;
                        sucesso("S5.3) Foi reservada a vaga %d para vacinação, status=%d", vaga_ativa, resposta.dados.status);
                        b = 1;
                        break;
                    }
                }
                break;
            }
        }
    }
    if( a == 0 ){
        erro("S5.2) Enfermeiro do CS %s não foi encontrado na BD Cidadãos", resposta.dados.cidadao.localidade);
    }
    if( b == 0 ){
        erro("S5.3) Não foi encontrada nenhuma vaga livre para vacinação");
    }
    sem_mutex_up();
    // S5.4) Se no final de todos os checks, status for OK, chama a função vacina(),
    if (OK == resposta.dados.status) vacina();
    // S5.4) caso contrário, chama a função envia_resposta_cidadao(), que envia a resposta ao Cidadão;
    else envia_resposta_cidadao();
    debug(">");
}

/**
 * S6) Processa a vacinação
 */
void vacina() {
    debug("<");
    sem_mutex_down();
    // S6.1) Cria um processo filho através da função fork();
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    int pid_fork = fork();
    exit_on_error(pid_fork, "S6.1) Não foi possível criar um novo processo");

    if (pid_fork == 0) {   // Processo FILHO
        // S6.2) O processo filho chama a função servidor_dedicado();
        pid_filho = getpid();
        sucesso("S6.1) Criado um processo filho com PID_filho=%d", pid_filho);
        servidor_dedicado();
    } else {          // Processo PAI
        // S6.3) O processo pai regista o process ID do processo filho no campo PID_filho na BD de Vagas com o índice da variável global vaga_ativa;
        db->vagas[vaga_ativa].PID_filho = pid_filho;
    }
    sem_mutex_up();
    debug(">");
}

/**
 * S7) Servidor Dedicado
 */
void servidor_dedicado() {
    debug("<");
    // S7.1) Arma o sinal SIGTERM;
    signal(SIGTERM, termina_servidor_dedicado);

    // S7.2) Envia a resposta para o Cidadao, chamando a função envia_resposta_cidadao(). Implemente também esta função, que envia a mensagem resposta para o cidadao, contendo os dados do Cidadao preenchidos em S5.1 e o campo status = OK;
    envia_resposta_cidadao();

    // S7.3) Coloca a disponibilidade do enfermeiro afeto à vaga_ativa com o valor 0 (Indisponível);
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    db->enfermeiros[db->vagas[vaga_ativa].index_enfermeiro].disponibilidade = 0;
    sucesso("S7.3) Enfermeiro associado à vaga %d indisponível", vaga_ativa);

    // S7.4) Imprime uma mensagem;
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    sucesso("S7.4) Vacina em curso para o cidadão %d, %s, e com o enfermeiro %d, %s na vaga %d", mensagem.dados.num_utente, mensagem.dados.nome, db->enfermeiros[db->vagas[vaga_ativa].index_enfermeiro].ced_profissional, db->enfermeiros[db->vagas[vaga_ativa].index_enfermeiro].nome, vaga_ativa);
    // S7.4) Aguarda (em espera passiva!) TEMPO_CONSULTA segundos;
    sleep( TEMPO_CONSULTA );

    // S7.5) Envia nova resposta para o Cidadao, chamando a função envia_resposta_cidadao() contendo os dados do Cidadao preenchidos em S5.1 e o campo status = TERMINADA, para indicar que a consulta terminou com sucesso;
    resposta.dados.status = TERMINADA;
    envia_resposta_cidadao();

    // S7.6) Atualiza os dados do cidadão (incrementa estado_vacinacao) na BD de Cidadãos
    db->cidadaos[db->vagas[vaga_ativa].index_cidadao].estado_vacinacao++;
    // S7.6) Atualiza os dados do cidadão (PID_cidadao = -1) na BD de Cidadãos
    db->cidadaos[db->vagas[vaga_ativa].index_cidadao].PID_cidadao = -1;
    // S7.6) Atualiza os dados do enfermeiro (incrementa nr_vacinas_dadas) na BD de Enfermeiros;
    db->enfermeiros[db->vagas[vaga_ativa].index_enfermeiro].nr_vacinas_dadas++;
    // S7.6) Atualiza os dados do enfermeiro (coloca disponibilidade=1) na BD de Enfermeiros;
    db->enfermeiros[db->vagas[vaga_ativa].index_enfermeiro].disponibilidade = 1;


    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    sucesso("S7.6) Cidadão atualizado na BD para estado_vacinacao=%d, Enfermeiro atualizado na BD para nr_vacinas_dadas=%d e disponibilidade=%d", db->cidadaos[db->vagas[vaga_ativa].index_cidadao].estado_vacinacao, db->enfermeiros[db->vagas[vaga_ativa].index_enfermeiro].nr_vacinas_dadas, db->enfermeiros[db->vagas[vaga_ativa].index_enfermeiro].disponibilidade);

    // S7.7) Liberta a vaga vaga_ativa da BD de Vagas, invocando a função liberta_vaga(vaga_ativa);
    liberta_vaga(vaga_ativa);



    // S7.8) Termina o processo Servidor Dedicado (filho) com exit status 0.
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    sucesso("S7.8) Servidor dedicado Terminado"); 
    exit(0);

    debug(">");
}

/**
 * S8) Tenta reservar uma vaga livre na BD de Vagas
 */
int reserva_vaga(int index_cidadao, int index_enfermeiro) {
    debug("<");
    vaga_ativa = -1;
    // S8.1) Procura uma vaga livre (index_cidadao < 0) na BD de Vagas. Se encontrar uma entrada livre:
    for( int i = 0; i < MAX_VAGAS; i++){
        if( db->vagas[i].index_cidadao < 0 ){
            // S8.1.1) Atualiza o valor da variável global vaga_ativa com o índice da vaga encontrada;
            // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
            vaga_ativa = i;
            sucesso("S8.1.1) Encontrou uma vaga livre com o index %d", vaga_ativa);
            // S8.1.2) Atualiza a entrada de Vagas vaga_ativa com o índice do cidadão e do enfermeiro
            db->vagas[vaga_ativa].index_cidadao = index_cidadao;
            db->vagas[vaga_ativa].index_enfermeiro = index_enfermeiro;
            break;
        }
    }
    // S8.1.3) Retorna o valor do índice de vagas vaga_ativa ou -1 se não encontrou nenhuma vaga
    return vaga_ativa;
    debug(">");
}

/**
 * S9) Liberta a vaga da BD de Vagas, colocando o campo index_cidadao dessa entrada da BD de Vagas com o valor -1
 */
void liberta_vaga(int index_vaga) {
    debug("<");
    sem_mutex_down();
    db->vagas[index_vaga].index_cidadao = -1;
    sucesso("S9) A vaga com o index %d foi libertada", index_vaga);
    sem_mutex_up();
    debug(">");
}

/**
 * Ações quando o servidor processa um pedido de cancelamento do Cidadao
 */
void cancela_pedido() {
    debug("<");

    // S10) Processa o cancelamento de um pedido de vacinação e envia uma resposta ao processo Cidadão. Para este efeito, a função:
    // S10.1) Procura na BD de Vagas a vaga correspondente ao Cidadao em questão (procura por index_cidadao). Se encontrar a entrada correspondente, obtém o PID_filho do Servidor Dedicado correspondente;
    int pid_filho_vaga, a = 0;
    for( int i = 0; i < MAX_CIDADAOS; i++){
        if( db->cidadaos[i].num_utente == mensagem.dados.num_utente ){
            for( int j = 0; j < MAX_VAGAS; j++){
                if ( db->vagas[j].index_cidadao == i ){
                    a = 1;
                    pid_filho_vaga = db->vagas[j].PID_filho;
                    sucesso("S10.1) Foi encontrada a sessão do cidadão %d, %s na sala com o index %d", mensagem.dados.num_utente, mensagem.dados.nome, j);
                }
            }
        }
    }
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    if( a == 0 ) erro("S10.1) Não foi encontrada nenhuma sessão do cidadão %d, %s", mensagem.dados.num_utente, mensagem.dados.nome);
    a = 0;

    // S10.2) Envia um sinal SIGTERM ao processo Servidor Dedicado (filho) que está a tratar da vacinação;
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    kill( pid_filho_vaga, SIGTERM);
    sucesso("S10.2) Enviado sinal SIGTERM ao Servidor Dedicado com PID=%d", pid_filho_vaga);
    debug(">");
}

/**
 * Ações quando o servidor recebeu um <CTRL+C>
 */
void termina_servidor(int sinal) {
    debug("<");

    // S11) Implemente a função termina_servidor(), que irá tratar do fecho do servidor, e que:

    // S11.1) Envia um sinal SIGTERM a todos os processos Servidor Dedicado (filhos) ativos;
    for( int i = 0; i < MAX_VAGAS; i++){
        if( db->vagas[i].index_cidadao != -1){
            kill( db->vagas[i].PID_filho, SIGTERM);
        }
    }

    // S11.2) Grava o ficheiro FILE_ENFERMEIROS, usando a função save_binary();
    // printf("sizeof: enfermeiros: %d cidadaos: %d", sizeof(db->enfermeiros), sizeof(db->cidadaos));
    // printf("sizeof: enfermeiros: %d cidadaos: %d", db->num_enfermeiros*sizeof(Enfermeiro), db->num_cidadaos*sizeof(Cidadao));
    save_binary(FILE_ENFERMEIROS, db->enfermeiros, db->num_enfermeiros*sizeof(Enfermeiro));
    // S11.3) Grava o ficheiro FILE_CIDADAOS, usando a função save_binary();
    save_binary(FILE_CIDADAOS, db->cidadaos, db->num_cidadaos*sizeof(Cidadao));
    // S11.4) Remove do sistema (IPC Remove) os semáforos, a Memória Partilhada e a Fila de Mensagens.
    shmctl( shm_id, IPC_RMID, 0 );
    msgctl( msg_id, IPC_RMID, 0 );
    semctl( sem_id, 0, IPC_RMID, 0 );
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    sucesso("S11.4) Servidor Terminado");
    // S11.5) Termina o processo servidor com exit status 0.
    exit(0);

    debug(">");
}

void termina_servidor_dedicado(int sinal) {
    debug("<");

    // S12) Implemente a função termina_servidor_dedicado(), que irá tratar do fecho do servidor dedicado, e que:

    // S12.1) Envia a resposta para o Cidadao, chamando a função envia_resposta_cidadao() com o campo status=CANCELADA, para indicar que a consulta foi cancelada;
    resposta.dados.status = CANCELADA;
    envia_resposta_cidadao();
    // S12.2) Liberta a vaga vaga_ativa da BD de Vagas, invocando a função liberta_vaga(vaga_ativa);
    liberta_vaga(vaga_ativa);

    // S12.3) Termina o processo do servidor dedicado com exit status 0;
    // Outputs esperados (itens entre <> substituídos pelos valores correspondentes):
    sucesso("S12.3) Servidor Dedicado Terminado");
    exit(0);

    debug(">");
}

/**
 * Altera o semáforo definido para Mutex na aplicação: Lock
 */
void sem_mutex_down() {
    struct sembuf lower = { .sem_num = 0, .sem_op = -1 };
    exit_on_error(semop(sem_id, &lower, 1), "Mutex-Lock: Não foi possível atualizar o semáforo");
    debug("MUTEX: Locked");
}

/**
 * Altera o semáforo definido para Mutex na aplicação: Unlock
 */
void sem_mutex_up() {
    struct sembuf raise = { .sem_num = 0, .sem_op = +1 };
    exit_on_error(semop(sem_id, &raise, 1), "Mutex-Unlock: Não foi possível atualizar o semáforo");
    debug("MUTEX: Unlocked");
}