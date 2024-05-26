/******************************************************************************
 ** ISCTE-IUL: Trabalho prático 3 de Sistemas Operativos 2023/2024, Enunciado Versão 3+
 **
 ** Aluno: Nº:       Nome:
 ** Nome do Módulo: cliente.c
 ** Descrição/Explicação do Módulo:
 **
 **
 ******************************************************************************/

// #define SO_HIDE_DEBUG                // Uncomment this line to hide all @DEBUG statements
#include "defines.h"

/*** Variáveis Globais ***/
int msgId;                              // Variável que tem o ID da Message Queue
MsgContent clientRequest;               // Variável que serve para as mensagens trocadas entre Cliente e Servidor
int nrTentativasEscolhaLugar = 0;       // Variável que indica quantas tentativas houve até conseguir encontrar um lugar

/**
 * @brief Processamento do processo Cliente
 *        "os alunos não deverão alterar a função main(), apenas compreender o que faz.
 *         Deverão, sim, completar as funções seguintes à main(), nos locais onde está claramente assinalado
 *         '// Substituir este comentário pelo código da função a ser implementado pelo aluno' "
 */
int main () {
    // C1
    msgId = initMsg_C1();
    so_exit_on_error(msgId, "initMsg_C1");
    // C2
    so_exit_on_error(triggerSignals_C2(), "triggerSignals_C2");
    // C3
    so_exit_on_error(getDadosPedidoUtilizador_C3(), "getDadosPedidoUtilizador_C3");
    // C4
    so_exit_on_error(sendRequest_C4(), "sendRequest_C4");
    // C5: CICLO6
    while (TRUE) {
        // C5
        configureTimer_C5(MAX_ESPERA);
        // C6
        so_exit_on_error(readResponseSD_C6(), "readResponseSD_C6");
        // C7
        int lugarEscolhido = trataResponseSD_C7();
        if (RETURN_ERROR == lugarEscolhido)
            terminateCliente_C9();
        // C8
        if (RETURN_ERROR == sendSeatChoice_C8(lugarEscolhido))
            terminateCliente_C9();
    }
}

/**
 *  "O módulo Cliente é responsável pela interação com o utilizador.
 *   Após o login do utilizador, este poderá realizar atividades durante o tempo da sessão.
 *   Assim, definem-se as seguintes tarefas a desenvolver:"
 */

/**
 * @brief C1: Ler a descrição da tarefa no enunciado
 * @return o valor de msgId em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int initMsg_C1 () {
    msgId = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    msgId = msgget(IPC_KEY, 0);
    if(msgId > 0) {
      so_success("C1", "%d", msgId);
    } else {
      so_error("C1", "");
      return msgId;
    }

    so_debug("> [@return:%d]", msgId);
    return msgId;
}

/**
 * @brief C2: Ler a descrição da tarefa no enunciado
 * @return RETURN_SUCCESS (0) em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int triggerSignals_C2 () {
    int result = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    if(signal(SIGHUP, trataSinalSIGHUP_C10) == SIG_ERR || signal(SIGINT, trataSinalSIGINT_C11) == SIG_ERR || signal(SIGALRM, trataSinalSIGALRM_C12) == SIG_ERR) {
      so_error("C2", "");
    } else {
      so_success("C2", "");
      result = 0;
    }

    so_debug("> [@return:%d]", result);
    return result;
}

/**
 * @brief C3: Ler a descrição da tarefa no enunciado
 * @return RETURN_SUCCESS (0) em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int getDadosPedidoUtilizador_C3 () {
    int result = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    printf("IscteFlight: Check-in Online\n");
    printf("----------------------------\n\n");
    printf("Introduza o NIF do passageiro: ");
    if(scanf("%d/", &clientRequest.msgData.infoCheckIn.nif) != 1 || clientRequest.msgData.infoCheckIn.nif >= 1000000000 || clientRequest.msgData.infoCheckIn.nif <= 0) {
      so_error("C3", "");
      return result;
    }

    printf("Introduza Senha do passageiro: ");
    if(scanf("%40s", clientRequest.msgData.infoCheckIn.senha) < 1) {
      so_error("C3", "");
      return result;
    }
    result = 0;
    clientRequest.msgData.infoCheckIn.pidCliente = getpid();

    so_success("C3", "%d %s %d", clientRequest.msgData.infoCheckIn.nif, clientRequest.msgData.infoCheckIn.senha, clientRequest.msgData.infoCheckIn.pidCliente);
 
    so_debug("> [@return:%d]", result);
    return result;
}

/**
 * @brief C4: Ler a descrição da tarefa no enunciado
 * @return RETURN_SUCCESS (0) em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int sendRequest_C4 () {
    int result = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    clientRequest.msgType = MSGTYPE_LOGIN;
    clientRequest.msgData.infoCheckIn.pidCliente = getpid();
    clientRequest.msgData.infoCheckIn.pidServidorDedicado = PID_INVALID;

    result = msgsnd(msgId, &clientRequest, sizeof(clientRequest.msgData), 0);
    if(result != 0) so_error("C4", "");
    else so_success("C4", "");

    so_debug("> [@return:%d]", result);
    return result;
}

/**
 * @brief C5: Ler a descrição da tarefa no enunciado
 * @param tempoEspera o tempo em segundos que queremos pedir para marcar o timer do SO (i.e., MAX_ESPERA)
 */
void configureTimer_C5 (int tempoEspera) {
    so_debug("< [@param tempoEspera:%d]", tempoEspera);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    alarm(tempoEspera);
    so_success("C5", "Espera resposta em %d segundos", tempoEspera);

    so_debug(">");
}

/**
 * @brief C6: Ler a descrição da tarefa no enunciado
 * @return RETURN_SUCCESS (0) em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int readResponseSD_C6 () {
    int result = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    int read = msgrcv(msgId, &clientRequest, sizeof(clientRequest.msgData), getpid(), 0);

    if(read <= 0) so_error("C6", "");
    else
      so_success(
          "C6",
          "%d %d %d",
          clientRequest.msgData.infoCheckIn.nif,
          clientRequest.msgData.infoCheckIn.lugarEscolhido,
          clientRequest.msgData.infoCheckIn.pidCliente
          );

    so_debug("> [@return:%d]", result);
    return read <= 0 ? -1 : 0;
}

/**
 * @brief C7: Ler a descrição da tarefa no enunciado
 * @return Nº do lugar escolhido (0..MAX_SEATS-1) em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int trataResponseSD_C7 () {
    int result = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    alarm(0);
    if(clientRequest.msgData.infoCheckIn.pidServidorDedicado == PID_INVALID) {
      so_error("C7.2", "");
      exit(1);
    }

    char emptySeatsString[MAX_SEATS * 3];
    strcpy(emptySeatsString, "");

    if(clientRequest.msgData.infoCheckIn.lugarEscolhido != EMPTY_SEAT) {
      so_success("C7.3", "Reserva concluída: %s %s %d", clientRequest.msgData.infoVoo.origem, clientRequest.msgData.infoVoo.destino, clientRequest.msgData.infoCheckIn.lugarEscolhido);
      exit(0);
    } else {
      if(nrTentativasEscolhaLugar == 0) so_success("C7.4.1", "");
      else so_error("C7.4.1", "Erro na reserva de lugar");
    }

    printf("IscteFlight: Voo %s\n", clientRequest.msgData.infoVoo.nrVoo);
    printf("----------------------------\n");
    for(int i = 0; i < MAX_SEATS; i++) {
      if(clientRequest.msgData.infoVoo.lugares[i] == EMPTY_SEAT) {
        char emptySeat[1];
        if(strcmp(emptySeatsString, "") != 0) strcat(emptySeatsString, ",");
        sprintf(emptySeat, " %d", i);
        strcat(emptySeatsString, emptySeat);
      }
    }
    printf("Lugares disponíveis:%s", emptySeatsString);
    printf("\nIntroduza o lugar que deseja reservar: ");
    int chosenSeat;
    if(scanf("%d", &chosenSeat) < 1) {
      so_error("C7.4.3", "");
      return -1;
    }

    if(chosenSeat >= MAX_SEATS || clientRequest.msgData.infoVoo.lugares[chosenSeat] != EMPTY_SEAT) {
      so_error("C7.4.3", "");
      return -1;
    }

    so_success("C7.4.3", "%d", chosenSeat);
    result = chosenSeat;
    nrTentativasEscolhaLugar++;
    so_debug("> [@return:%d]", result);
    return result;
}

/**
 * @brief C8: Ler a descrição da tarefa no enunciado
 * @param lugarEscolhido índice do array lugares que o utilizador escolheu, entre 0 e MAX_SEATS-1
 * @return RETURN_SUCCESS (0) em caso de sucesso, ou RETURN_ERROR (-1) em caso de erro
 */
int sendSeatChoice_C8 (int lugarEscolhido) {
    int result = RETURN_ERROR; // Por omissão, retorna erro
    so_debug("< [@param lugarEscolhido:%d]", lugarEscolhido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    MsgContent msgToSend;
    msgToSend.msgType = clientRequest.msgData.infoCheckIn.pidServidorDedicado;
    msgToSend.msgData.infoCheckIn.nif = clientRequest.msgData.infoCheckIn.nif;
    msgToSend.msgData.infoCheckIn.lugarEscolhido = lugarEscolhido;
    msgToSend.msgData.infoCheckIn.pidCliente = getpid();

    result = msgsnd(msgId, &msgToSend, sizeof(msgToSend.msgData), 0);
    if(result == 0)
      so_success("C8", "%d %d %d", msgToSend.msgData.infoCheckIn.nif, lugarEscolhido, getpid());
    else
      so_error("C8", "");

    so_debug("> [@return:%d]", result);
    return result;
}

/**
 * @brief C9: Ler a descrição da tarefa no enunciado
 */
void terminateCliente_C9 () {
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    if(clientRequest.msgData.infoCheckIn.pidServidorDedicado == PID_INVALID) {
      so_error("C9", "");
      exit(0);
    }

    int s = kill(clientRequest.msgData.infoCheckIn.pidServidorDedicado, SIGUSR1);
    so_success("C9", "");
    if(s != 0) so_error("C9", "");

    so_debug(">");
    exit(0);
}

/**
 * @brief C10: Ler a descrição da tarefa no enunciado
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGHUP_C10 (int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    so_success("C10", "Check-in concluído sem sucesso");
    exit(0);

    so_debug(">");
}

/**
 * @brief C11: Ler a descrição da tarefa no enunciado
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGINT_C11 (int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    so_success("C11", "Cliente: Shutdown");
    terminateCliente_C9();

    so_debug(">");
}

/**
 * @brief C12: Ler a descrição da tarefa no enunciado
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGALRM_C12 (int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    so_error("C12", "Cliente: Timeout");
    terminateCliente_C9();

    so_debug(">");
}
