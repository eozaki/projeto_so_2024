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

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
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

    // Substituir este comentário pelo código da função a ser implementado pelo aluno

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

    // Substituir este comentário pelo código da função a ser implementado pelo aluno

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

    // Substituir este comentário pelo código da função a ser implementado pelo aluno

    so_debug("> [@return:%d]", result);
    return result;
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

    so_debug("> [@return:%d]", pid_filho);
    return pid_filho;
}

/**
 * @brief S7: Ler a descrição da tarefa no enunciado
 */
void terminateServidor_S7 () {
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno

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

    so_debug(">");
}

/**
 * @brief S9: Ler a descrição da tarefa no enunciado
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGCHLD_S9 (int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno

    so_debug(">");
}

/**
 * @brief SD10: Ler a descrição da tarefa no enunciado
 * @return RETURN_SUCCESS (0) em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int triggerSignals_SD10 () {
    int result = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno

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

    so_debug("> [@return:%d]", result);
    return result;
}

/**
 * @brief SD18: Ler a descrição da tarefa no enunciado
 */
void terminateServidorDedicado_SD18 () {
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno

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
