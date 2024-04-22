/******************************************************************************
 ** ISCTE-IUL: Trabalho prático 2 de Sistemas Operativos 2023/2024, Enunciado Versão 3+
 **
 ** Aluno: Nº: 122088       Nome: Erick Cordeiro Ozaki
 ** Nome do Módulo: cliente.c
 ** Descrição/Explicação do Módulo:
 ** Verifica pela existencia do ficheiro FIFO do servidor, onde devem ser colocados os
 ** pedidos de check in nos voos;
 ** Identifica os utilizador através dos dados fornecidos e verifica sua autenticidade com senha;
 ** Escreve o pedido do check in no ficheiro FIFO e aguarda por resposta do servidor dentro do tempo limite;
 ******************************************************************************/

// #define SO_HIDE_DEBUG                // Uncomment this line to hide all @DEBUG statements
#include "common.h"

/**
 * @brief Processamento do processo Cliente
 *        "os alunos não deverão alterar a função main(), apenas compreender o que faz.
 *         Deverão, sim, completar as funções seguintes à main(), nos locais onde está claramente assinalado
 *         '// Substituir este comentário pelo código da função a ser implementado pelo aluno' "
 */
int main () {
    // C1
    checkExistsFifoServidor_C1(FILE_REQUESTS);
    // C2
    triggerSignals_C2();
    // C3 + C4
    CheckIn clientRequest = getDadosPedidoUtilizador_C3_C4();
    // C5
    writeRequest_C5(clientRequest, FILE_REQUESTS);
    // C6
    configureTimer_C6(MAX_ESPERA);
    // C7
    waitForEvents_C7();
    so_exit_on_error(-1, "ERRO: O cliente nunca devia chegar a este ponto");
}

/**
 *  "O módulo Cliente é responsável pela interação com o utilizador.
 *   Após o login do utilizador, este poderá realizar atividades durante o tempo da sessão.
 *   Assim, definem-se as seguintes tarefas a desenvolver:"
 */

/**
 * @brief C1       Ler a descrição da tarefa C1 no enunciado
 * @param nameFifo Nome do FIFO servidor (i.e., FILE_REQUESTS)
 */
void checkExistsFifoServidor_C1 (char *nameFifo) {
    so_debug("< [@param nameFifo:%s]", nameFifo);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    struct stat fs;
    int file_mode;

    file_mode = stat(nameFifo, &fs);

    if(file_mode == -1) {
        so_error("C1", "");
        exit(1);
    }

    if(access(nameFifo, F_OK) == 0 && S_ISFIFO(fs.st_mode))
        so_success("C1", "");
    else {
        so_error("C1", "");
        exit(1);
    }

    so_debug(">");
}

/**
 * @brief C2   Ler a descrição da tarefa C2 no enunciado
 */
void triggerSignals_C2 () {
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    signal(SIGUSR1, trataSinalSIGUSR1_C8);
    signal(SIGHUP, trataSinalSIGHUP_C9);
    signal(SIGINT, trataSinalSIGINT_C10);
    signal(SIGALRM, trataSinalSIGALRM_C11);

    so_debug(">");
}

/**
 * @brief C3+C4    Ler a descrição das tarefas C3 e C4 no enunciado
 * @return CheckIn Elemento com os dados preenchidos. Se nif=-1, significa que o elemento é inválido
 */
CheckIn getDadosPedidoUtilizador_C3_C4 () {
    CheckIn request;
    request.nif = -1;   // Por omissão retorna erro
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    printf("IscteFlight: Check-in Online\n");
    printf("----------------------------\n\n");
    printf("Introduza o NIF do passageiro: ");
    if(scanf("%d/", &request.nif) != 1 || request.nif >= 1000000000 || request.nif <= 0) {
      so_error("C3", "");
      exit(1);
    }

    printf("Introduza Senha do passageiro: ");
    if(scanf("%40s", request.senha) < 1) {
      so_error("C3", "");
      exit(1);
    }

    request.pidCliente = getpid();

    so_debug("> [@return nif:%d, senha:%s, pidCliente:%d]", request.nif, request.senha, request.pidCliente);
    so_success("C4", "%d %s %d", request.nif, request.senha, request.pidCliente);
    return request;
}

/**
 * @brief C5       Ler a descrição da tarefa C5 no enunciado
 * @param request  Elemento com os dados a enviar
A
 * @param nameFifo O nome do FIFO do servidor (i.e., FILE_REQUESTS)
 */
void writeRequest_C5 (CheckIn request, char *nameFifo) {
    so_debug("< [@param request.nif:%d, request.senha:%s, request.pidCliente:%d, nameFifo:%s]",
                                        request.nif, request.senha, request.pidCliente, nameFifo);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    FILE* fifo = fopen(nameFifo, "r+");
    if(!fifo) {
      so_error("C5", "");
      exit(1);
    }

    fprintf(fifo, "%d\n%s\n%d\n", request.nif, request.senha, request.pidCliente);
    if(fwrite(&request, sizeof(CheckIn), 1, fifo) != 1) {
      so_error("C5", "");
      exit(1);
    }

    fclose(fifo);
    so_success("C5", "");
    so_debug(">");
}

/**
 * @brief C6          Ler a descrição da tarefa C6 no enunciado
 * @param tempoEspera o tempo em segundos que queremos pedir para marcar o timer do SO (i.e., MAX_ESPERA)
 */
void configureTimer_C6 (int tempoEspera) {
    so_debug("< [@param tempoEspera:%d]", tempoEspera);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    alarm(tempoEspera);
    so_success("C6", "Espera resposta em %d segundos", tempoEspera);
    so_debug(">");
}

/**
 * @brief C7 Ler a descrição da tarefa C7 no enunciado
 */
void waitForEvents_C7 () {
    so_debug("<");

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    pause();

    so_debug(">");
}

/**
 * @brief C8            Ler a descrição da tarefa C8 no enunciado
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGUSR1_C8 (int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    so_success("C8", "Check-in concluído com sucesso");

    so_debug(">");
    exit(0);
}

/**
 * @brief C9            Ler a descrição da tarefa C9 no enunciado
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGHUP_C9 (int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    so_success("C9", "Check-in concluído com sucesso");

    so_debug(">");
    exit(1);
}

/**
 * @brief C10           Ler a descrição da tarefa C10 no enunciado
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGINT_C10 (int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    so_success("C10", "Cliente: Shutdown");
    so_debug(">");
    exit(0);
}

/**
 * @brief C11           Ler a descrição da tarefa C11 no enunciado
 * @param sinalRecebido nº do Sinal Recebido (preenchido pelo SO)
 */
void trataSinalSIGALRM_C11 (int sinalRecebido) {
    so_debug("< [@param sinalRecebido:%d]", sinalRecebido);

    // Substituir este comentário pelo código da função a ser implementado pelo aluno
    so_error("C11", "Cliente: Timeout");

    so_debug(">");
    exit(1);
}
