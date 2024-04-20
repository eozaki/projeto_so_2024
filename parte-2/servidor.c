/******************************************************************************
 ** ISCTE-IUL: Trabalho prático 2 de Sistemas Operativos 2023/2024, Enunciado Versão 3+
 **
 ** Aluno: Nº: 122088       Nome: Erick Cordeiro Ozaki
 ** Nome do Módulo: servidor.c
 ** Descrição/Explicação do Módulo:
 **
 **
 ******************************************************************************/

// #define SO_HIDE_DEBUG                // Uncomment this line to hide all @DEBUG statements
#include "common.h"

/*** Variáveis Globais ***/
CheckIn clientRequest; // Variável que tem o pedido enviado do Cliente para o Servidor

/**
 * @brief Processamento do processo Servidor e dos processos Servidor Dedicado
 *        "os alunos não deverão alterar a função main(), apenas compreender o que faz.
 *         Deverão, sim, completar as funções seguintes à main(), nos locais onde está claramente assinalado
 *         '// Substituir este comentário pelo código da função a ser implementado pelo aluno' "
 */
int main () {
    // S1
    checkExistsDB_S1(FILE_DATABASE);
    // S2
    createFifo_S2(FILE_REQUESTS);
    // S3
    triggerSignals_S3(FILE_REQUESTS);

    int indexClient;       // Índice do cliente que fez o pedido ao servidor/servidor dedicado na BD

    // S4: CICLO1
    while (TRUE) {
        // S4
        clientRequest = readRequest_S4(FILE_REQUESTS); // S4: "Se houver erro (...) clientRequest.nif == -1"
        if (clientRequest.nif < 0)   // S4: "Se houver erro na abertura do FIFO ou na leitura do mesmo, (...)"
            continue;                // S4: "(...) e recomeça o Ciclo1 neste mesmo passo S4, lendo um novo pedido"

        // S5
        int pidServidorDedicado = createServidorDedicado_S5();
        if (pidServidorDedicado > 0) // S5: "o processo Servidor (pai) (...)"
            continue;                // S5: "(...) recomeça o Ciclo1 no passo S4 (ou seja, volta a aguardar novo pedido)"
        // S5: "o Servidor Dedicado (que tem o PID pidServidorDedicado) segue para o passo SD9"

        // SD9
        triggerSignals_SD9();
        // SD10
        CheckIn itemBD;
        indexClient = searchClientDB_SD10(clientRequest, FILE_DATABASE, &itemBD);
        // SD11
        checkinClientDB_SD11(&clientRequest, FILE_DATABASE, indexClient, itemBD);
        // SD12
        sendAckCheckIn_SD12(clientRequest.pidCliente);
        // SD13
        closeSessionDB_SD13(clientRequest, FILE_DATABASE, indexClient);
        so_exit_on_error(-1, "ERRO: O servidor dedicado nunca devia chegar a este ponto");
    }
}

/**
 *  "O módulo Servidor é responsável pelo processamento do check-in dos passageiros. 
 *   Está dividido em duas partes, um Servidor (pai) e zero ou mais Servidores Dedicados (filhos).
 *   Este módulo realiza as seguintes tarefas:"
 */

/**
 * @brief S1     Ler a descrição da tarefa S1 no enunciado
 * @param nameDB O nome da base de dados (i.e., FILE_DATABASE)
 */
void checkExistsDB_S1 (char *nameDB) {
    so_debug("< [@param nameDB:%s]", nameDB);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    if(access(nameDB, F_OK) == 0 && access(nameDB, W_OK) == 0 && access(nameDB, R_OK) == 0)
        so_success("S1", "");
    else {
        so_error("S1", "");
        exit(1);
    }

    so_debug(">");
}

/**
 * @brief S2       Ler a descrição da tarefa S2 no enunciado
 * @param nameFifo O nome do FIFO do servidor (i.e., FILE_REQUESTS)
 */
void createFifo_S2 (char *nameFifo) {
    so_debug("< [@param nameFifo:%s]", nameFifo);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    if(access(nameFifo, F_OK) == 0)
        remove(nameFifo);

    if(mkfifo(nameFifo, 0666) != 0) {
        so_error("S2", "");
        exit(1);
    }

    so_success("S2", "");

    so_debug(">");
}

/**
 * @brief S3   Ler a descrição da tarefa S3 no enunciado
 */
void triggerSignals_S3 () {
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    signal(SIGINT, trataSinalSIGINT_S6);
    signal(SIGCHLD, trataSinalSIGCHLD_S8);

    so_debug(">");
}

/**
 * @brief S4       O CICLO1 já está a ser feito na função main(). Ler a descrição da tarefa S4 no enunciado
 * @param nameFifo O nome do FIFO do servidor (i.e., FILE_REQUESTS)
 * @return CheckIn Elemento com os dados preenchidos. Se nif=-1, significa que o elemento é inválido
 */
CheckIn readRequest_S4 (char *nameFifo) {
    CheckIn request;
    request.nif = -1;   // Por omissão retorna erro
    so_debug("< [@param nameFifo:%s]", nameFifo);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    int fifo = open(nameFifo, O_RDONLY);
    read(fifo, &request, sizeof(request));

    close(fifo);

    if(request.nif <= 0 || request.pidCliente <= 0) {
      so_error("S4", "");
      deleteFifoAndExit_S7();
    }

    so_debug("> [@return nif:%d, senha:%s, pidCliente:%d]", request.nif, request.senha, request.pidCliente);

    so_success("S4", "%d %s %d", request.nif, request.senha, request.pidCliente);

    return request;
}

/**
 * @brief S5   Ler a descrição da tarefa S5 no enunciado
 * @return int PID do processo filho, se for o processo Servidor (pai),
 *             0 se for o processo Servidor Dedicado (filho), ou -1 em caso de erro.
 */
int createServidorDedicado_S5 () {
    int pid_filho = -1;    // Por omissão retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    pid_filho = fork();
    if(pid_filho < 0) {
      so_error("S5", "");
      deleteFifoAndExit_S7();
    }

    so_success("S5", "Servidor: Iniciei SD %d", pid_filho);
    so_debug("> [@return:%d]", pid_filho);
    return pid_filho;
}

/**
 * @brief S6            Ler a descrição das tarefas S6 e S7 no enunciado
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGINT_S6 (int sinalRecebido) {
    CheckIn request;
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno

    so_success("S6", "Servidor: Start Shutdown");
    int bd_passageiros = open("bd_passageiros.dat", O_RDONLY);
    if(bd_passageiros <= -1) {
      so_error("S6.1", "");
      deleteFifoAndExit_S7();
    }
    so_success("S6.1", "");

    while(1) {
      int db_read = read(bd_passageiros, &request, sizeof(request));
      if(db_read < 0) {
        so_error("S6.2", "");
        deleteFifoAndExit_S7();
      } else if(db_read == 0) {
        so_success("S6.2", "");
        deleteFifoAndExit_S7();
      } else if(request.pidServidorDedicado > 0) {
        kill(request.pidServidorDedicado, SIGUSR2);
        so_success("S6.3", "Servidor: Shutdown SD %d", request.pidServidorDedicado);
      }
    }

    so_debug(">");
}

/**
 * @brief S7 Ler a descrição da tarefa S7 no enunciado
 */
void deleteFifoAndExit_S7 () {
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno

    if(unlink("server.fifo") == 0) {
      so_success("S7", "Servidor: End Shutdown");
    } else {
      so_error("S7", "");
    }

    so_debug(">");
    exit(0);
}

/**
 * @brief S8            Ler a descrição da tarefa S8 no enunciado
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGCHLD_S8 (int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    pid_t child = wait(NULL);

    so_success("S8", "Servidor: Confirmo fim de SD %d", child);
    so_debug(">");
}

/**
 * @brief SD9  Ler a descrição da tarefa SD9 no enunciado
 */
void triggerSignals_SD9 () {
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    if(signal(SIGINT, SIG_IGN) == SIG_ERR || signal(SIGUSR2, trataSinalSIGUSR2_SD14) == SIG_ERR) {
      so_error("SD9", "");
      exit(1);
    } else
      so_success("SD9", "");

    so_debug(">");
}

/**
 * @brief SD10    Ler a descrição da tarefa SD10 no enunciado
 * @param request O pedido do cliente
 * @param nameDB  O nome da base de dados
 * @param itemDB  O endereço de estrutura CheckIn a ser preenchida nesta função com o elemento da BD
 * @return int    Em caso de sucesso, retorna o índice de itemDB no ficheiro nameDB.
 */
int searchClientDB_SD10 (CheckIn request, char *nameDB, CheckIn *itemDB) {
    int indexClient = 0;    // SD10:"inicia uma variável indexClient a 0"
    so_debug("< [@param request.nif:%d, request.senha:%s, nameDB:%s, itemDB:%p]", request.nif,
                                                                    request.senha, nameDB, itemDB);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    int bd_passageiros = open("bd_passageiros.dat", O_RDONLY);
    while(1) {
      int db_read = read(bd_passageiros, itemDB, sizeof(*itemDB));

      if(db_read < 0) {
        so_error("SD10.1", "");
        kill(request.pidCliente, SIGHUP);
        exit(1);
      } else if(db_read == 0) {
        so_error("SD10.1", "Cliente %d: não encontrado", request.nif);
        kill(request.pidCliente, SIGHUP);
        exit(1);
      }

      if(itemDB->nif == request.nif) {
        if(strcmp(itemDB->senha, request.senha) == 0) {
          so_success("SD10.3", "%d", indexClient);
          close("bd_passageiros.dat");
          break;
        } else {
          so_error("SD10.3", "Cliente %d: Senha errada", indexClient);
          kill(request.pidCliente, SIGHUP);
          exit(1);
        }
      }
      indexClient++;
    }

    close(bd_passageiros);

    so_debug("> [@return:%d, nome:%s, nrVoo:%s]", indexClient, itemDB->nome, itemDB->nrVoo);
    return indexClient;
}

/**
 * @brief SD11        Ler a descrição da tarefa SD11 no enunciado
 * @param request     O endereço do pedido do cliente (endereço é necessário pois será alterado)
 * @param nameDB      O nome da base de dados
 * @param indexClient O índica na base de dados do elemento correspondente ao cliente
 * @param itemDB      O elemento da BD correspondente ao cliente
 */
void checkinClientDB_SD11 (CheckIn *request, char *nameDB, int indexClient, CheckIn itemDB) {
    so_debug("< [@param request:%p, nameDB:%s, indexClient:%d, itemDB.pidServidorDedicado:%d]",
                                    request, nameDB, indexClient, itemDB.pidServidorDedicado);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    int bd_passageiros = open("bd_passageiros.dat", O_WRONLY);
    strcpy(request->nome, itemDB.nome);
    strcpy(request->nrVoo, itemDB.nrVoo);
    request->pidServidorDedicado = getpid();
    so_success("SD11.1", "%s %s %d", itemDB.nome, itemDB.nrVoo, request->pidServidorDedicado);
    so_debug("> [nome:%s, nrVoo:%s, pidServidorDedicado:%d]", request->nome,
                                                request->nrVoo, request->pidServidorDedicado);
    close(bd_passageiros);

    FILE* bd_passageiros1 = fopen("bd_passageiros.dat", "r+");
    if(bd_passageiros1 == NULL) {
      so_error("SD11.2", "");
      kill(request->pidCliente, SIGHUP);
      exit(1);
    }
    so_success("SD11.2", "");

    if(fseek(bd_passageiros1, indexClient, SEEK_SET) != 0) {
      so_error("SD11.3", "");
      kill(request->pidCliente, SIGHUP);
      exit(1);
    }
    so_success("SD11.3", "");

    int write_result = fputs(request, bd_passageiros1);
    fclose(bd_passageiros1);

    if(write_result <= 0) {
      so_error("SD11.4", "");
      kill(request->pidCliente, SIGHUP);
      exit(1);
    }
    so_success("SD11.4", "");

}

/**
 * @brief SD12       Ler a descrição da tarefa SD12 no enunciado
 * @param pidCliente PID (Process ID) do processo Cliente
 */
void sendAckCheckIn_SD12 (int pidCliente) {
    so_debug("< [@param pidCliente:%d]", pidCliente);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    srand((unsigned) time(NULL));
    int wait_time = (rand() % MAX_ESPERA) + 1;
    so_success("SD12", "%d", wait_time);
    sleep(wait_time);
    kill(pidCliente, SIGUSR1);

    so_debug(">");
}

/**
 * @brief SD13          Ler a descrição da tarefa SD13 no enunciado
 * @param clientRequest O endereço do pedido do cliente
 * @param nameDB        O nome da base de dados
 * @param indexClient   O índica na base de dados do elemento correspondente ao cliente
 */
void closeSessionDB_SD13 (CheckIn clientRequest, char *nameDB, int indexClient) {
    so_debug("< [@param clientRequest:%p, nameDB:%s, indexClient:%d]", &clientRequest, nameDB,
                                                                                    indexClient);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    clientRequest.pidCliente = -1;
    clientRequest.pidServidorDedicado = -1;
    FILE* db_conn = fopen("bd_passageiros.dat", "r+");
    if(db_conn == NULL) {
      so_error("SD13.1", "");
      exit(1);
    }
    so_success("SD13.1", "");

    if(fseek(db_conn, indexClient, SEEK_SET) != 0) {
      so_error("SD13.2", "");
      exit(1);
    }
    so_success("SD13.2", "");
    
    // fputs(clientRequest, db_conn);

    fclose(db_conn);
    so_debug("> [pidCliente:%d, pidServidorDedicado:%d]", clientRequest.pidCliente, 
                                                          clientRequest.pidServidorDedicado);
}

/**
 * @brief SD14          Ler a descrição da tarefa SD14 no enunciado
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGUSR2_SD14 (int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    so_success("SD14", "SD: Recebi pedido do Servidor para terminar");
    exit(0);
    so_debug(">");
}
