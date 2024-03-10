#!/bin/bash
# SO_HIDE_DEBUG=1                   ## Uncomment this line to hide all @DEBUG statements
# SO_HIDE_COLOURS=1                 ## Uncomment this line to disable all escape colouring
. ./so_utils.sh                     ## This is required to activate the macros so_success, so_error, and so_debug

###############################################################################
## ISCTE-IUL: Trabalho prático de Sistemas Operativos 2023/2024, Enunciado Versão 3+
##
## Aluno: Nº: 122088       Nome: Erick Ozaki
## Nome do Módulo: S5. Script: menu.sh
## Descrição/Explicação do Módulo:
##
##
###############################################################################

## Este script invoca os scripts restantes, não recebendo argumentos. Atenção: Não é suposto que volte a fazer nenhuma das funcionalidades dos scripts anteriores. O propósito aqui é simplesmente termos uma forma centralizada de invocar os restantes scripts.

## S5.1. Apresentação:
## S5.1.1. O script apresenta (pode usar echo, cat ou outro, sem “limpar” o ecrã) um menu com as opções indicadas no enunciado (1 a 5, ou 0 para sair).
selection=1
while [[ ! "$selection" == 0 ]];
do
  menu="MENU:\n"
  menu=$menu"1: Regista/Atualiza saldo do passageiro\n"
  menu=$menu"2: Reserva/Compra de bilhetes\n"
  menu=$menu"3: Atualiza Estado dos voos\n"
  menu=$menu"4: Estatísticas - Passageiros\n"
  menu=$menu"5: Estatísticas - Top Voos + Rentáveis\n"
  menu=$menu"0: Sair\n"

  echo -e "$menu"
  read -p "Opção: " selection
  ## S5.2. Validações:
  ## S5.2.1. Aceita como input do utilizador um número. Valida que a opção introduzida corresponde a uma opção válida. Se não for, dá so_error <opção> (com a opção errada escolhida), e volta ao passo S5.1 (ou seja, mostra novamente o menu). Caso contrário, dá so_success <opção>.
  if [[ $selection -lt 0 ]] || [[ $selection -gt 5 ]]; then
    so_error 'S5.2.1' $selection
  else
    so_success 'S5.2.1' $selection
  fi

  ## S5.2.2. Analisa a opção escolhida, e mediante cada uma delas, deverá invocar o sub-script correspondente descrito nos pontos S1 a S4 acima. No caso das opções 1 e 4, este script deverá pedir interactivamente ao utilizador as informações necessárias para execução do sub-script correspondente, injetando as mesmas como argumentos desse sub-script:
  ## S5.2.2.1. Assim sendo, no caso da opção 1, o script deverá pedir ao utilizador sucessivamente e interactivamente os dados a inserir (Nome, Senha, NIF e Saldo a adicionar). Repare que, no exemplo dado no enunciado, não foi inserido qualquer NIF, portanto, indicando que não pretende registar o passageiro, mas sim apenas adicionar saldo ao mesmo, correspondendo esta introdução assim ao primeiro exemplo de invocação via argumentos indicada em S1, que se destina apenas a adicionar 500 euros ao saldo do passageiro já existente "Fábio Cardoso", validando essa adição de créditos pela inserção da senha do passageiro. Este script não deverá fazer qualquer validação dos dados inseridos, já que essa validação é feita no script S1. Após receber os dados, este script invoca o Sub-Script: regista_passageiro.sh com os argumentos recolhidos do utilizador. Após a execução do sub-script, dá so_success e volta ao passo S5.1.
  if [[ "$selection" -eq 1 ]]; then
    echo -e "Regista passageiro / Atualiza saldo passageiro:\n"
    read -p "Indique o nome: " nome
    read -p "Indique a senha: " senha
    read -p "Para registar o passageiro, insira o NIF: " nif
    read -p "Indique o saldo a adicionar ao passageiro: " saldo

    ./regista_passageiro.sh "$nome" $senha $saldo $nif

    so_success 'S5.2.2.1'
  fi 

  ## S5.2.2.2. No caso da opção 2, o script invoca o Sub-Script: compra_bilhete.sh. Após a execução do sub-script, dá so_success e volta para o passo S5.1.
  if [[ "$selection" -eq 2 ]]; then
    ./compra_bilhete.sh

    so_success 'S5.2.2.2'
  fi

  ## S5.2.2.3. No caso da opção 3, o script invoca o Sub-Script: estado_voos.sh. Após a execução do sub-script, dá so_success e volta para o passo S5.1.
  if [[ "$selection" -eq 3 ]]; then
    ./estado_voos.sh

    so_success 'S5.2.2.3'
  fi

  ## S5.2.2.4. No caso da opção 4, o script invoca o Sub-Script: stats.sh com o argumento necessário. Após a execução do sub-script, dá so_success e volta para o passo S5.1.
  if [[ "$selection" -eq 4 ]]; then
    ./stats.sh passageiros

    so_success 'S5.2.2.4'
  fi

  ## S5.2.2.5. No caso da opção 5, o script deverá pedir ao utilizador o nº de voos a listar, antes de invocar o Sub-Script: stats.sh. Após a execução do sub-script, dá so_success e volta para o passo S5.1. Apenas a opção 0 (zero) permite sair deste Script: menu.sh. Até escolher esta opção, o menu deverá ficar em ciclo, permitindo realizar múltiplas operações iterativamente (e não recursivamente, ou seja, não deverá chamar o Script: menu.sh novamente). 
  if [[ "$selection" -eq 5 ]]; then
    echo -e "Estatísticas - Top Voos + Rentáveis:" 
    read -p "Indique o número de voos a listar: " numero
    ./stats.sh top "$numero"

    so_success 'S5.2.2.5'
  fi

done

exit 0
