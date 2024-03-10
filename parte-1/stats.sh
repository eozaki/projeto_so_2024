#!/bin/bash
# SO_HIDE_DEBUG=1                   ## Uncomment this line to hide all @DEBUG statements
# SO_HIDE_COLOURS=1                 ## Uncomment this line to disable all escape colouring
. ./so_utils.sh                     ## This is required to activate the macros so_success, so_error, and so_debug

###############################################################################
## ISCTE-IUL: Trabalho prático de Sistemas Operativos 2023/2024, Enunciado Versão 3+
##
## Aluno: Nº: a122088      Nome: Erick Cordeiro Ozaki
## Nome do Módulo: S4. Script: stats.sh
## Descrição/Explicação do Módulo:
##
##
###############################################################################

## Este script obtém informações sobre o sistema, afixando resultados diferentes no STDOUT consoante os argumentos passados na sua invocação. A sintaxe resumida é: ./stats.sh <passageiros>|<top <nr>>

REPORT_FILE='relatorio_reservas.txt'

## S4.1. Validações:
## S4.1.1. Valida os argumentos recebidos e, conforme os mesmos, o número e tipo de argumentos recebidos. Se não respeitarem a especificação, dá so_error e termina. Caso contrário, dá so_success.
if [[ $# -eq 0 ]]; then
  so_error 'S4.1.1'
  exit 1
fi

if [[ ! $1 == 'passageiros' ]] && [[ ! $1 == 'top' ]]; then
  so_error 'S4.1.1'
fi

if [[ $1 == 'top' ]] && [[ $# -lt 2 ]]; then
  so_error 'S4.1.1'
fi

so_success 'S4.1.1'

## S4.2. Invocação do script:
## S4.2.1. Se receber o argumento passageiros, (i.e., ./stats.sh passageiros) cria um ficheiro stats.txt onde lista o nome de todos os utilizadores que fizeram reservas, por ordem decrescente de número de reservas efetuadas, e mostrando o seu valor total de compras. Em caso de erro (por exemplo, se não conseguir ler algum ficheiro necessário), dá so_error e termina. Caso contrário, dá so_success e cria o ficheiro. Em caso de empate no número de reservas, lista o primeiro do ficheiro. Preste atenção ao tratamento do singular e plural quando se escreve “reserva” no ficheiro).

if [[ "$1" == "passageiros" ]]; then
  TEMP_STATS='stats.tmp'
  if [[ -f "$TEMP_STATS" ]]; then
    rm -f "$TEMP_STATS"
  fi

  users=$(cat "$REPORT_FILE" | awk -F ':' '{print $6}' | uniq)

  tmp=""
  while IFS=":" read -r idR codigo origem destino preco user timestamp
  soma=0
  do
    if [[ $user == $usuario ]]; then
    fi
  done < $users

  if [[ ! $(echo -e "$tmp" > "$TEMP_STATS") -eq 0 ]]; then
    so_error 'S4.2.1'
  fi
fi

## S4.2.2. Se receber o argumento top <nr:number>, (e.g., ./stats.sh top 4), cria um ficheiro stats.txt onde lista os <nr> (no exemplo, os 4) voos mais rentáveis (que tiveram melhores receitas de vendas), por ordem decrescente. Em caso de erro (por exemplo, se não conseguir ler algum ficheiro necessário), dá so_error e termina. Caso contrário, dá so_success e cria o ficheiro. Em caso de empate, lista o primeiro do ficheiro.
