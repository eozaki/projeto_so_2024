#!/bin/bash
# SO_HIDE_DEBUG=1                   ## Uncomment this line to hide all @DEBUG statements
# SO_HIDE_COLOURS=1                 ## Uncomment this line to disable all escape colouring
. ./so_utils.sh                     ## This is required to activate the macros so_success, so_error, and so_debug

###############################################################################
## ISCTE-IUL: Trabalho prático de Sistemas Operativos 2023/2024, Enunciado Versão 3+
##
## Aluno: Nº: 122088       Nome: Erick Cordeiro Ozaki
## Nome do Módulo: S2. Script: compra_bilhete.sh
## Descrição/Explicação do Módulo:
##
##
###############################################################################

## Este script não recebe nenhum argumento, e permite que o passageiro compre um bilhete para um voo da lista de voos disponíveis. Para realizar a compra, o passageiro deve fazer login para confirmar sua identidade e saldo disponível. Os voos disponíveis estão listados no ficheiro voos.txt. Se não quiser produzir o seu próprio ficheiro, pode utilizar o ficheiro exemplo fornecido, dando o comando: cp voos-exemplo.txt voos.txt. O mesmo procedimento pode ser realizado também com o ficheiro passageiros.txt.

FLIGHT_FILE=voos.txt
PASSENGERS_FILE=passageiros.txt
## S2.1. Validações e Pedido de informações interativo:
## S2.1.1. O script valida se os ficheiros voos.txt e passageiros.txt existem. Se algum não existir, dá so_error e termina. Caso contrário, dá so_success.
if [[ -f "$FLIGHT_FILE" && -f "$PASSENGERS_FILE" ]]; then
  so_success "S2.1.1"
else
  so_error "S2.1.1"
  exit 1
fi

## S2.1.2. Na plataforma é possível consultar os voos pela sua <Origem> ou <Destino>. Pedindo "Insira a cidade de origem ou destino do voo: _". O utilizador insere a cidade Origem ou Destino do voo (o interesse é que pesquise nos 2 campos). Caso o utilizador tenha introduzido uma cidade que não exista no ficheiro voos.txt, ou se não existirem voos com lugares disponíveis com origem ou destino nessa cidade, dá so_error e termina. Caso contrário, dá so_success <Cidade>.
read -p 'Insira a cidade de origem ou destino do voo: ' cityOfInterest

flights=""
count=0
while IFS=":" read -r codVoo origem destino data hora preco lotacao disponiveis
  do
  if [[ "$origem" == "$cityOfInterest" || "$destino" == "$cityOfInterest" ]] && [ "$disponiveis" != "0" ]; then
    ((count++))
    flights=$flights"$count.$origem para $destino, $data, Partida:$hora, Preço: $preco, Disponíveis:$disponiveis lugares\n"
  fi
done < "$FLIGHT_FILE"

if [[ -n $flights ]]; then
  so_success 'S2.1.2' $cityOfInterest
else
  so_error 'S2.1.2'
  exit 1
fi

## S2.1.3. O programa pede ao utilizador para inserir uma opção de voo, listando os voos que existem de acordo com a origem/destino inserida anteriormente, da seguinte forma: "Lista de voos, numerada, ou 0 para sair, Insira o voo que pretende reservar: _" O utilizador insere a opção do voo (neste exemplo, números de 1 a 3 ou 0). Se o utilizador escolheu um número de entre as opções de voos apresentadas (neste caso, entre 1 e 3), dá so_success <opção>. Caso contrário, dá so_error e termina.
# 1.Lisboa para Dubai, 2024-05-23, Partida:08h00, Preço: 320, Disponíveis:35 lugares
flights=$flights"0.Sair"
echo -e $flights
echo
read -p "Insira o voo que pretende reservar: " flightOfInterest;
numberOfFlights=$(echo -e $flights | wc -l)
numberOfFlights=$(($numberOfFlights - 1))
if [[ ! $flightOfInterest -gt $numberOfFlights ]] && [[ ! 1 -gt $flightOfInterest ]]; then
  so_success 'S2.1.3' $flightOfInterest
else
  so_error 'S2.1.3'
  exit 1
fi

## S2.1.4. O programa pede ao utilizador o seu <ID_passageiro>: "Insira o ID do seu utilizador: _" O utilizador insere o respetivo ID de passageiro (dica: UserId Linux). Se esse ID não estiver registado no ficheiro passageiros.txt, dá so_error e termina. Caso contrário, reporta so_success <ID_passageiro>.
read -p "Insira o ID do seu utilizador: " user
userMatch=$(cat "$PASSENGERS_FILE" | awk -F ":" '{print $1}' | grep -E "^$user$")

if [[ -n $userMatch ]]; then
  so_success 'S2.1.4' $user
else
  so_error 'S2.1.4'
  exit 1
fi

## S2.1.5. O programa pede ao utilizador a sua <Senha>: "Insira a senha do seu utilizador: _" O utilizador insere a respetiva senha. Caso o script veja que essa senha não é a registada para esse passageiro no ficheiro passageiros.txt, dá so_error e termina. Caso contrário, reporta so_success.
read -p "Insira a senha do seu utilizador: " senha
userPasswd=$(cat "$PASSENGERS_FILE" | awk -F ":" '{print $1":"$5}' | grep -E "^$user\:" | awk -F ":" '{print $2}')

if [[ $senha == $userPasswd ]]; then
  so_success 'S2.1.5'
else
  so_error 'S2.1.5' $userPasswd
  exit 1
fi
## S2.2. Processamento da resposta:
## S2.2.1. Valida se o passageiro possui <Saldo>, definido no ficheiro passageiros.txt, para comprar o bilhete selecionado no passo S2.1.3. Se a compra não é possível por falta de saldo, dá so_error <preço voo> <Saldo> e termina. Caso contrário, dá so_success <preço voo> <Saldo>.
saldo=$(cat "$PASSENGERS_FILE" | awk -F ":" '{print $1":"$6}' | grep -E "^$user\:" | awk -F ":" '{print $2}')

if [[ -f voos.tmp ]]; then
  rm -f voos.tmp
fi

echo -e $flights > voos.tmp
price=$(head -n $flightOfInterest voos.tmp | tail -n 1 | awk -F "," '{print $4}' | awk -F " " '{print $2}')

if [[ $saldo -lt $price ]]; then
  so_error 'S2.2.1' $price $saldo
  exit 1
else
  so_success 'S2.2.1' $saldo
fi

if [[ -f voos.tmp ]]; then
  rm -f voos.tmp
fi

## S2.2.2. Subtrai o valor do <preço voo> no <Saldo> do passageiro, e atualiza o ficheiro passageiros.txt. Em caso de erro (e.g., na escrita do ficheiro), dá so_error e termina. Senão, dá so_success <Saldo Atual>.
novoSaldo=$(($saldo - $price))
while IFS=":" read -r username nif nome email senha saldo
do
  if [[ $username == $user && $senha == $userPasswd ]]; then
    if [[ $(sed -i "s/$saldo$/$novoSaldo/" "$PASSENGERS_FILE") -eq 0 ]]; then
      so_success 'S2.2.2' $novoSaldo
    else
      so_error 'S2.2.2'
      exit 1
    fi
  fi
done < "$PASSENGERS_FILE"

## S2.2.3. Decrementa uma unidade aos lugares disponíveis do voo escolhidos no passo S2.1.3, e atualiza o ficheiro voos.txt. Em caso de erro (por exemplo, na escrita do ficheiro), dá so_error e termina. Senão, dá so_success.
flights=""
count=0
while IFS=":" read -r codVoo origem destino data hora preco lotacao disponiveis
  do
  if [[ "$origem" == "$cityOfInterest" || "$destino" == "$cityOfInterest" ]] && [ "$disponiveis" != "0" ]; then
    ((count++))
    flights=$flights"$codVoo:$disponiveis\n"
  fi
done < "$FLIGHT_FILE"

if [[ -f voos.tmp ]]; then
  rm -f voos.tmp
fi

echo -e $flights > voos.tmp
codigo=$(head -n $flightOfInterest voos.tmp | tail -n 1 | awk -F ":" '{print $1}')
available=$(head -n $flightOfInterest voos.tmp | tail -n 1 | awk -F ":" '{print $2}')

if [[ -f voos.tmp ]]; then
  rm -f voos.tmp
fi

destination=""
origin=""

while IFS=":" read -r codVoo origem destino data hora preco lotacao disponiveis
do
  if [[ $codVoo == $codigo ]]; then
    available=$(($disponiveis - 1))
    destination=$destino
    origin=$origem
    if [[ $(sed -i "s/:$disponiveis$/:$available/" "$FLIGHT_FILE") -eq 0 ]]; then
      so_success 'S2.2.3' $available
    else
      so_error 'S2.2.3'
      exit 1
    fi
  fi
done < "$FLIGHT_FILE"

## S2.2.4. Regista a compra no ficheiro relatorio_reservas.txt, inserido uma nova linha no final deste ficheiro. Em caso de erro (por exemplo, na escrita do ficheiro), dá so_error e termina. Caso contrário, dá so_success.
REPORT_FILE='relatorio_reservas.txt'
if ! [[ -f "$REPORT_FILE" ]]; then
  touch "$REPORT_FILE"
fi

maiorId=$(tail -n 1 "$REPORT_FILE" | cut -f1 -d ':')
if ! [[ -n $maiorId ]]; then
  maiorId=0
fi

idReserva=$((maiorId + 1))
data=$(date "+%Y-%m-%d:%Hh%M")
if [[ $(echo "$idReserva:$codigo:$origin:$destination:$price:$user:$data" >> "$REPORT_FILE") -eq 0 ]]; then
  so_success 'S2.2.4'
else
  so_error 'S2.2.4'
fi
