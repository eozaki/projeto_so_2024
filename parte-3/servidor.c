/******************************************************************************
 ** ISCTE-IUL: Trabalho prático 3 de Sistemas Operativos 2023/2024, Enunciado Versão 1+
 **
 ** Aluno: Nº:       Nome:
 ** Nome do Módulo: servidor.c
 ** Descrição/Explicação do Módulo:
 **
 **
 ******************************************************************************/

// #define SO_HIDE_DEBUG                // Uncomment this line to hide all @DEBUG statements
#include "defines.h"

/*** Variáveis Globais ***/
int shmId;                              // Variável que tem o ID da Shared Memory
int msgId;                              // Variável que tem o ID da Message Queue
int semId;                              // Variável que tem o ID do Grupo de Semáforos
MsgContent clientRequest;               // Variável que serve para as mensagens trocadas entre Cliente e Servidor
DadosServidor *database = NULL;         // Variável que vai ficar com UM POINTER PARA a memória partilhada
int indexClient = -1;                   // Índice do passageiro que fez o pedido ao servidor/servidor dedicado na BD
int indexFlight = -1;                   // Índice do voo do passageiro que fez o pedido ao servidor/servidor dedicado na BD
int nrServidoresDedicados = 0;          // Número de servidores dedicados (só faz sentido no processo Servidor)

/**
 * @brief Processamento do processo Servidor e dos processos Servidor Dedicado
 *        "os alunos não deverão alterar a função main(), apenas compreender o que faz.
 *         Deverão, sim, completar as funções seguintes à main(), nos locais onde está claramente assinalado
 *         '// Substituir este comentário pelo código da função a ser implementado pelo aluno' "
 */
int main () {
    // S1
    shmId = initShm_S1();
    if (RETURN_ERROR == shmId) terminateServidor_S7();
    // S2
    msgId = initMsg_S2();
    if (RETURN_ERROR == msgId) terminateServidor_S7();
    // S3
    semId = initSem_S3();
    if (RETURN_ERROR == semId) terminateServidor_S7();
    // S4
    if (RETURN_ERROR == triggerSignals_S4()) terminateServidor_S7();

    // S5: CICLO1
    while (TRUE) {
        // S5
        int result = readRequest_S5();
        if (CICLO1_CONTINUE == result) // S5: "Se receber um sinal (...) retorna o valor CICLO1_CONTINUE"
            continue;                  // S5: "para que main() recomece automaticamente o CICLO1 no passo S5"
        if (RETURN_ERROR == result) terminateServidor_S7();
        // S6
        int pidServidorDedicado = createServidorDedicado_S6();
        if (pidServidorDedicado > 0)   // S6: "o processo Servidor (pai) (...) retorna um valor > 0"
            continue;                  // S6: "(...) recomeça o Ciclo1 no passo S4 (ou seja, volta a aguardar novo pedido)"
        if (RETURN_ERROR == pidServidorDedicado) terminateServidor_S7();
        // S6: "o Servidor Dedicado (...) retorna 0 para que main() siga automaticamente para o passo SD10

        // SD10
        if (RETURN_ERROR == triggerSignals_SD10()) terminateServidorDedicado_SD18();
        // SD11
        indexClient = searchClientDB_SD11();
        int erroValidacoes = TRUE;
        if (RETURN_ERROR != indexClient) {
            // SD12: "Se o passo SD11 concluiu com sucesso: (...)"
            indexFlight = searchFlightDB_SD12();
            if (RETURN_ERROR != indexFlight) {
                // SD13: "Se os passos SD11 e SD12 tiveram sucesso, (...)"
                if (!updateClientDB_SD13())
                    erroValidacoes = FALSE; // erroValidacoes = "houve qualquer erro nas validações dos passos SD11, SD12, ou SD13"
            }
        }
        // SD14: CICLO5
        int escolheuLugarDisponivel = FALSE;
        while (!escolheuLugarDisponivel) {
            // SD14.1: erroValidacoes = "houve qualquer erro nas validações dos passos SD11, SD12, ou SD13"
            if (RETURN_ERROR == sendResponseClient_SD14(erroValidacoes)) terminateServidorDedicado_SD18();
            if (erroValidacoes)
                terminateServidorDedicado_SD18();

            // SD15: "Se os pontos anteriores tiveram sucesso, (...)"
            if (RETURN_ERROR == readResponseClient_SD15()) terminateServidorDedicado_SD18();
            // SD16
            if (RETURN_ERROR == updateFlightDB_SD16())  // SD16: "Se lugarEscolhido no pedido NÃO estiver disponível (...) retorna erro (-1)"
                continue;                               // SD16: "para que main() recomece o CICLO5 em SD14"
            else
                escolheuLugarDisponivel = TRUE;
        }
        sendConfirmationClient_SD17();
        terminateServidorDedicado_SD18();
    }
}

/**
 *  "O módulo Servidor é responsável pelo processamento do check-in dos passageiros.
 *   Está dividido em duas partes, um Servidor (pai) e zero ou mais Servidores Dedicados (filhos).
 *   Este módulo realiza as seguintes tarefas:"
 */

/**
 * @brief S1: Ler a descrição da tarefa no enunciado
 * @return o valor de shmId em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int initShm_S1 () {
    shmId = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("<");

    int passageirosExists = access(FILE_DATABASE_PASSAGEIROS, F_OK) == 0 && access(FILE_DATABASE_PASSAGEIROS, W_OK) == 0 && access(FILE_DATABASE_PASSAGEIROS, R_OK) == 0;
    int voosExists = access(FILE_DATABASE_VOOS, F_OK) == 0 && access(FILE_DATABASE_VOOS, W_OK) == 0 && access(FILE_DATABASE_VOOS, R_OK) == 0;

    if(!(passageirosExists && voosExists))
        so_error("S1.1", "");
    else {
        so_success("S1.1", "");

        shmId = shmget(IPC_KEY, sizeof(DadosServidor), 0);
        if(shmId < 0)
            so_error("S1.2", "");
        else {
            so_success("S1.2", "");
            if((database = shmat(shmId, NULL, 0)) != (DadosServidor*) -1)
              so_success("S1.2.1", "%d", shmId);
            else {
              shmId = RETURN_ERROR;
              so_error("S1.2.1", "");
            }
            return shmId;
        }

        if(errno != ENOENT) {
          so_error("S1.3", "");
          return RETURN_ERROR;
        } else
          so_success("S1.3", "");

        shmId = shmget(IPC_KEY, sizeof(DadosServidor), IPC_CREAT | IPC_EXCL);
        if(shmId < 0) {
          so_error("S1.4", "");
          return RETURN_ERROR;
        } else
          so_success("S1.4", "%d", shmId);

        database = (DadosServidor*) shmat(shmId, 0, 0);
        for(int i = 0; i < MAX_PASSENGERS; i++)
           database->listClients[i].nif = PASSENGER_NOT_FOUND;
        for(int i = 0; i < MAX_FLIGHTS; i++)
           strcpy(database->listFlights[i].nrVoo, FLIGHT_NOT_FOUND);
        so_success("S1.5", "");

        FILE* passengers = fopen(FILE_DATABASE_PASSAGEIROS, "r");
        if(passengers == NULL) {
          so_error("S1.6", "");
          return -1;
        }
        fread(&database->listClients, sizeof(CheckIn), MAX_PASSENGERS, passengers);
        if(ferror(passengers) != 0) {
          so_error("S1.6", "");
          return -1;
        }
        fclose(passengers);
        so_success("S1.6", "");

        FILE* flights = fopen(FILE_DATABASE_VOOS, "r");
        if(flights == NULL) {
          so_error("S1.7", "");
          return -1;
        }
        fread(&database->listFlights, sizeof(Voo), MAX_FLIGHTS, flights);
        if(ferror(flights) != 0) {
          so_error("S1.7", "");
          return -1;
        }
        fclose(flights);
        so_success("S1.7", "");
    }

    so_debug("> [@return:%d]", shmId);
    return shmId;
}

/**
 * @brief S2: Ler a descrição da tarefa no enunciado
 * @return o valor de msgId em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int initMsg_S2 () {
    msgId = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("<");

    if(msgget(IPC_KEY, 0) > 0) {
      if(msgctl(IPC_KEY, IPC_RMID, 0) == RETURN_ERROR) {
        so_error("S2.1", "");
        return msgId;
      }
      so_success("S2.1", "");
    }

    msgId = msgget(IPC_KEY, IPC_CREAT | IPC_EXCL);
    if(msgId > 0) {
      so_success("S2.2", "%d", msgId);
    } else {
      so_error("S2.2", "");
      return msgId;
    }


    so_debug("> [@return:%d]", msgId);
    return msgId;
}

/**
 * @brief S3: Ler a descrição da tarefa no enunciado
 * @return o valor de semId em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int initSem_S3 () {
    semId = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("<");

    if(semget(IPC_KEY, 3, 0) > 0) {
      if(semctl(IPC_KEY, IPC_RMID, 0) == RETURN_ERROR) {
        so_error("S3.1", "");
        return semId;
      }
      so_success("S3.1", "");
    }

    semId = semget(IPC_KEY, 3, IPC_CREAT | IPC_EXCL);
    if(msgId > 0) {
      so_success("S3.2", "%d", semId);
    } else {
      so_error("S3.2", "");
      return semId;
    }

    int sem1 = semctl(semId, SEM_PASSAGEIROS, SETVAL, 1);
    int sem2 = semctl(semId, SEM_VOOS, SETVAL, 1);
    int sem3 = semctl(semId, SEM_NR_SRV_DEDICADOS, SETVAL, 0);

    if(sem1 < 0 || sem2 < 0 || sem3 < 0) {
      so_error("S3.3", "");
      return -1;
    }

    so_success("S3.3", "");

    so_debug("> [@return:%d]", semId);
    return semId;
}

/**
 * @brief S4: Ler a descrição da tarefa no enunciado
 * @return RETURN_SUCCESS (0) em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int triggerSignals_S4 () {
    int result = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    if(signal(SIGINT, trataSinalSIGINT_S8) == SIG_ERR || signal(SIGCHLD, trataSinalSIGCHLD_S9) == SIG_ERR)
      so_error("S4", "");
    else {
      result = 0;
      so_success("S4", "");
    }


    so_debug("> [@return:%d]", result);
    return result;
}

/**
 * @brief S5: O CICLO1 já está a ser feito na função main(). Ler a descrição da tarefa no enunciado
 * @return RETURN_SUCCESS (0) em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int readRequest_S5 () {
    int result = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("<");

    result = msgrcv(msgId, &clientRequest, sizeof(clientRequest.msgData), MSGTYPE_LOGIN, 0);
    if(result <= 0) {
      if(errno == EINTR) return CICLO1_CONTINUE;

      so_error("S5", "");
      return RETURN_ERROR;
    } else
      so_success(
          "S5",
          "%d %s %d",
            clientRequest.msgData.infoCheckIn.nif,
            clientRequest.msgData.infoCheckIn.senha,
            clientRequest.msgData.infoCheckIn.pidCliente
      );

    so_debug("> [@return:%d]", result);
    return 0;
}

/**
 * @brief S6: Ler a descrição da tarefa no enunciado
 * @return PID do processo filho, se for o processo Servidor (pai),
 *         0 se for o processo Servidor Dedicado (filho),
 *         ou PID_INVALID (-1) em caso de erro
 */
int createServidorDedicado_S6 () {
    int pid_filho = PID_INVALID;    // Por omissão retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    pid_filho = fork();
    if(pid_filho == 0) {
      so_success("S6", "Servidor Dedicado: Nasci");
      return 0;
    } else if(pid_filho < 0) {
      so_error("S6", "");
      return PID_INVALID;
    }

    so_success("S6", "Servidor: Iniciei SD %d", pid_filho);
    nrServidoresDedicados++;

    return pid_filho;

    so_debug("> [@return:%d]", pid_filho);
    return pid_filho;
}

/**
 * @brief S7: Ler a descrição da tarefa no enunciado
 */
void terminateServidor_S7 () {
    so_debug("<");

    struct sembuf DOWN = { SEM_NR_SRV_DEDICADOS, -nrServidoresDedicados, 0 };
    so_success("S7", "Servidor: Start Shutdown");

    if(database == NULL) {
      so_error("S7.1", "");
    } else {
      // S7.1
      so_success("S7.1", "");

      // S7.2
      for(int i = 0; i < MAX_PASSENGERS; i++)
        if(database->listClients[i].pidServidorDedicado > 0) {
          kill(database->listClients[i].pidServidorDedicado, SIGUSR2);
          so_success("S7.2", "Servidor: Shutdown SD %d", database->listClients[i].pidServidorDedicado);
        }
      semop(semId, &DOWN, 1);
      so_success("S7.3", "");

      FILE* pass_db_conn = fopen(FILE_DATABASE_PASSAGEIROS, "wb");
      FILE* flights_db_conn = fopen(FILE_DATABASE_VOOS, "wb");

      if(!pass_db_conn || !flights_db_conn) {
        so_error("S7.4", "");
      } else {
        int w1 = fwrite(&database->listClients, sizeof(CheckIn), MAX_PASSENGERS, pass_db_conn);
        int w2 = fwrite(&database->listFlights, sizeof(Voo), MAX_FLIGHTS, flights_db_conn);

        if(w1 + w2 < 0) so_error("S7.4", "");
        else so_success("S7.4", "");

        fclose(pass_db_conn);
        fclose(flights_db_conn);
      }
    }

    shmctl(shmId, IPC_RMID, 0);
    semctl(semId, IPC_RMID, 0);
    msgctl(msgId, IPC_RMID, 0);

    so_success("S7.5", "Servidor: End Shutdown");

    so_debug(">");
    exit(0);
}

/**
 * @brief S8: Ler a descrição da tarefa no enunciado
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGINT_S8 (int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    so_success("S8", "");
    terminateServidor_S7();

    so_debug(">");
}

/**
 * @brief S9: Ler a descrição da tarefa no enunciado
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGCHLD_S9 (int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    pid_t child = wait(NULL);

    struct sembuf UP_BY_ONE = { SEM_NR_SRV_DEDICADOS, 1, 0 };
    semop(semId, &UP_BY_ONE, 1);

    so_success("S9", "Servidor: Confirmo fim de SD %d", child);

    so_debug(">");
}

/**
 * @brief SD10: Ler a descrição da tarefa no enunciado
 * @return RETURN_SUCCESS (0) em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int triggerSignals_SD10 () {
    int result = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("<");

    if(signal(SIGINT, SIG_IGN) == SIG_ERR || signal(SIGUSR1, trataSinalSIGUSR1_SD19) == SIG_ERR || signal(SIGUSR2, trataSinalSIGUSR2_SD20) == SIG_ERR) {
      so_error("SD10", "");
    } else {
      so_success("SD10", "");
      result = 0;
    }

    so_debug("> [@return:%d]", result);
    return result;
}

/**
 * @brief SD11: Ler a descrição da tarefa no enunciado
 * @return indexClient em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int searchClientDB_SD11 () {
    indexClient = -1;    // SD11: Inicia a variável indexClient a -1 (índice da lista de passageiros de database)
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    int i;
    for(i = 0; i < MAX_PASSENGERS; i++)
      if(clientRequest.msgData.infoCheckIn.nif == database->listClients[i].nif) break;

    if(i >= MAX_PASSENGERS)
      so_error("SD11.1", "Cliente %d: não encontrado", clientRequest.msgData.infoCheckIn.nif);
    else if(strcmp(database->listClients[i].senha, clientRequest.msgData.infoCheckIn.senha) != 0)
      so_error("SD11.3", "Cliente %d: Senha errada", clientRequest.msgData.infoCheckIn.nif);
    else {
      so_success("SD11.3", "%d", i);
      indexClient = i;
    }

    so_debug("> [@return:%d]", indexClient);
    return indexClient;
}

/**
 * @brief SD12: Ler a descrição da tarefa no enunciado
 * @return indexFlight em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int searchFlightDB_SD12 () {
    indexFlight = -1;    // SD12: Inicia a variável indexFlight a -1 (índice da lista de voos de database)
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    int i;
    for(i = 0; i < MAX_FLIGHTS; i++)
      if(strcmp(database->listClients[indexClient].nrVoo, database->listFlights[i].nrVoo) == 0) break;

    if(i >= MAX_FLIGHTS)
      so_error("SD12.1", "Voo %s: não encontrado", database->listClients[indexClient].nrVoo);
    else {
      so_success("SD12.2", "%d", i);
      indexFlight = i;
    }

    so_debug("> [@return:%d]", indexFlight);
    return indexFlight;
}

/**
 * @brief SD13: Ler a descrição da tarefa no enunciado
 * @return RETURN_SUCCESS (0) em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int updateClientDB_SD13 () {
    int result = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    so_success("SD13.1", "Start Check-in: %d %d", clientRequest.msgData.infoCheckIn.nif, clientRequest.msgData.infoCheckIn.pidCliente);

    CheckIn c = database->listClients[indexClient];
    struct sembuf DOWN = { SEM_PASSAGEIROS, -1, 0 };
    struct sembuf UP = { SEM_PASSAGEIROS, 1, 0 };
    semop(semId, &DOWN, 1);
    if(c.pidCliente != PID_INVALID || c.lugarEscolhido != EMPTY_SEAT) {
      so_error("SD13.2", "Cliente %d: Já fez check-in", c.nif);
      semop(semId, &UP, 1);
      return result;
    }
    sleep(4);
    semop(semId, &UP, 1);
    
    database->listClients[indexClient].pidServidorDedicado = getpid();
    database->listClients[indexClient].pidCliente = clientRequest.msgData.infoCheckIn.pidCliente;
    result = 0;

    so_success("SD13.5", "End Check-in: %d %d", clientRequest.msgData.infoCheckIn.nif, clientRequest.msgData.infoCheckIn.pidCliente);

    so_debug("> [@return:%d]", result);
    return result;
}

/**
 * @brief SD14: Ler a descrição da tarefa no enunciado
 * @param erroValidacoes booleano que diz se houve algum erro nas validações de SD11, SD12 e SD13
 * @return RETURN_SUCCESS (0) em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int sendResponseClient_SD14 (int erroValidacoes) {
    int result = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("< [@param erroValidacoes:%d]", erroValidacoes);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    MsgContent msgToSend;
    msgToSend.msgType = clientRequest.msgData.infoCheckIn.pidCliente;
    if(erroValidacoes) {
      so_error("SD14.1", "");
      msgToSend.msgData.infoCheckIn.pidServidorDedicado = PID_INVALID;
    } else {
      so_success("SD14.1", "");
      msgToSend.msgData.infoCheckIn.pidServidorDedicado = getpid();
      msgToSend.msgData.infoCheckIn.lugarEscolhido = EMPTY_SEAT;
      msgToSend.msgData.infoVoo = database->listFlights[indexFlight];
    }

    result = msgsnd(msgId, &msgToSend, sizeof(msgToSend.msgData), 0);
    if(!result)
      so_success("SD14.2", "");
    else
      so_error("SD14.2", "");

    so_debug("> [@return:%d]", result);
    return result;
}

/**
 * @brief SD15: Ler a descrição da tarefa no enunciado
 * @return RETURN_SUCCESS (0) em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int readResponseClient_SD15 () {
    int result = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    int read = msgrcv(msgId, &clientRequest, sizeof(clientRequest.msgData), getpid(), 0);
    if (read <= 0)
      so_error("SD15", "");
    else {
      so_success("SD15", "%d %d %d", clientRequest.msgData.infoCheckIn.nif, clientRequest.msgData.infoCheckIn.lugarEscolhido, clientRequest.msgData.infoCheckIn.pidCliente);
      result = 0;
    }

    so_debug("> [@return:%d]", result);
    return result;
}

/**
 * @brief SD16: Ler a descrição da tarefa no enunciado
 * @return RETURN_SUCCESS (0) em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int updateFlightDB_SD16 () {
    int result = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    so_success("SD16.1", "Start Reserva lugar: %s %d %d", clientRequest.msgData.infoCheckIn.nrVoo, clientRequest.msgData.infoCheckIn.nif, clientRequest.msgData.infoCheckIn.lugarEscolhido);
    
    struct sembuf DOWN = { SEM_VOOS, -1, 0 };
    struct sembuf UP = { SEM_VOOS, 1, 0 };
    semop(semId, &DOWN, 1);
    if(database->listFlights[indexFlight].lugares[clientRequest.msgData.infoCheckIn.lugarEscolhido] != EMPTY_SEAT) {
      so_error("SD16.2", "Cliente %d: Lugar já estava ocupado", clientRequest.msgData.infoCheckIn.nif);
      semop(semId, &UP, 1);
      return result;
    }
    sleep(4);
    database->listFlights[indexFlight].lugares[clientRequest.msgData.infoCheckIn.lugarEscolhido] =
      clientRequest.msgData.infoCheckIn.nif;

    semop(semId, &UP, 1);
    database->listClients[indexClient].lugarEscolhido = clientRequest.msgData.infoCheckIn.lugarEscolhido;

    result = 0;

    so_success(
        "SD16.6",
        "End Reserva lugar: %s %d %d",
        database->listFlights[indexFlight].nrVoo, 
        database->listClients[indexClient].nif,
        clientRequest.msgData.infoCheckIn.lugarEscolhido
        );

    so_debug("> [@return:%d]", result);
    return result;
}

/**
 * @brief SD17: Ler a descrição da tarefa no enunciado
 * @return RETURN_SUCCESS (0) em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int sendConfirmationClient_SD17 () {
    int result = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    MsgContent msgToSend;
    msgToSend.msgType = clientRequest.msgData.infoCheckIn.pidCliente;
    msgToSend.msgData.infoCheckIn.pidServidorDedicado = getpid();
    msgToSend.msgData.infoCheckIn.lugarEscolhido = clientRequest.msgData.infoCheckIn.lugarEscolhido;
    strcpy(msgToSend.msgData.infoVoo.origem, clientRequest.msgData.infoVoo.origem);
    strcpy(msgToSend.msgData.infoVoo.destino, clientRequest.msgData.infoVoo.destino);
    result = msgsnd(msgId, &msgToSend, sizeof(msgToSend.msgData), 0);

    if(result) so_error("SD17", "");
    else so_success("SD17", "");

    so_debug("> [@return:%d]", result);
    return result;
}

/**
 * @brief SD18: Ler a descrição da tarefa no enunciado
 */
void terminateServidorDedicado_SD18 () {
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    if(indexClient >= 0) {
      database->listClients[indexClient].pidCliente = PID_INVALID;
      database->listClients[indexClient].pidServidorDedicado = PID_INVALID;
    }

    so_success("SD18", "");

    so_debug(">");
    exit(0);
}

/**
 * @brief SD19: Ler a descrição da tarefa no enunciado
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGUSR1_SD19 (int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    so_success("SD19", "SD: Recebi pedido do Cliente para terminar");
    terminateServidorDedicado_SD18();

    so_debug(">");
}

/**
 * @brief SD20: Ler a descrição da tarefa no enunciado
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGUSR2_SD20 (int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno

    so_debug(">");
}
