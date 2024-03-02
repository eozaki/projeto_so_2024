#!/bin/bash
# SO_HIDE_DEBUG=1                   ## Uncomment this line to hide all @DEBUG statements
# SO_HIDE_COLOURS=1                 ## Uncomment this line to disable all escape colouring
# . so_utils.sh                     ## This is required to activate the macros so_success, so_error, and so_debug

###############################################################################
## ISCTE-IUL: Trabalho prático de Sistemas Operativos 2023/2024, Enunciado Versão 3+
##
## Aluno: Nº: 122088       Nome: Erick Cordeiro Ozaki
## Nome do Módulo: S1. Script: regista_passageiro.sh
## Descrição/Explicação do Módulo:
##
##
###############################################################################

## Este script é invocado quando um novo passageiro se regista na plataforma IscteFlight. Este script recebe todos os dados por argumento, na chamada da linha de comandos. Os passageiros são registados no ficheiro passageiros.txt. Deve receber as informações do passageiro como argumentos pela seguinte ordem: <Nome:string> <Senha:string>  <Saldo a adicionar:number> [<NIF:number>]
so_error() {
  echo "AAAAAAAAAAAAAAAAAAAAAA$1"
}

so_success() {
  echo "daora!$1"
}

#####################
# CONSTANTS
#####################
NUMBER_REGEX='^\d+$'
VAT_LENGTH_REGEX='^\d{9}$'
PASSENGERS_FILE=passageiros.txt
TIGRE_USERS_FILE=/etc/passwd

## S1.1. Valida os argumentos passados e os seus formatos:
## S1.1.1. Valida os argumentos passados, avaliando se são em número suficiente (mínimo 3, máximo 4). Em caso de erro, dá so_error S1.1.1 e termina. Caso contrário, dá so_success S1.1.1.
if [[ $# < 3 || $# > 4 ]]; then
  so_error "S1.1.1"
  exit 1
else
  so_success "S1.1.1"
fi

## S1.1.2. Valida se o argumento <Nome> corresponde ao nome de um utilizador do servidor Tigre. Se não corresponder ao nome de nenhum utilizador do Tigre, dá so_error S1.1.2 e termina. Senão, dá so_success S1.1.2.
userOnPasswdFile=$(grep "$1" $TIGRE_USERS_FILE -c)
if ! [[ $userOnPasswdFile -lt 1 ]]; then
  so_error "S1.1.2"
  exit 1
fi

if [[ $(cat /etc/passwd | grep -c $1) -eq 1 ]]; then
  so_success "S1.1.2"
else
  so_error "S1.1.2"
  exit 1
fi

## S1.1.3. Valida se o argumento <Saldo a adicionar> tem formato “number” (inteiro positivo ou 0). Se não tiver, dá so_error S1.1.3 e termina. Caso contrário, dá so_success S1.1.3.
validateAmountIsNumberValue() {
  if ! [[ $1 =~ $NUMBER_REGEX ]]; then
    so_error "S1.1.3"
    exit 1
  fi

  so_success "S1.1.3"
}

## S1.1.4. Valida se o argumento opcional <NIF> (só no caso de ser passado, i.e., se tiver valor) tem formato “number” com 9 (nove) dígitos. Se não for, dá so_error S1.1.4 e termina. Caso contrário, dá so_success S1.1.4.
validateVATNumber() {
  if ! [[ $1 =~ $NUMBER_REGEX && $1 =~ VAT_LENGTH_REGEX ]]; then
    so_error "S1.1.4"
    exit 1
  fi

  so_success "S1.1.4"
}

## S1.2. Associa os dados passados com a base de dados dos passageiros registados:
## S1.2.1. Verifica se o ficheiro passageiros.txt existe. Se o ficheiro existir, dá so_success S1.2.1 e continua no passo S1.2.3. Se não existir, dá so_error S1.2.1, e continua.
checkForPassengersDataFile() {
  if ! [[ -f $PASSENGERS_FILE ]]; then
    so_error "S1.2.1"
  fi

  so_success "S1.2.1"
}

## S1.2.2. Cria o ficheiro passageiros.txt. Se der erro, dá so_error S1.2.2 e termina. Senão, dá so_success S1.2.2.
createPassengersDataFile() {
  if ! [[ $(touch $PASSENGERS_FILE) -eq 0 ]]; then
    so_error "S1.2.2"
  fi

  so_success "S1.2.2"
}

## S1.2.3. Caso o passageiro <Nome> passado já exista no ficheiro passageiros.txt, dá so_success S1.2.3, e continua no passo S1.3.  Senão, dá so_error S1.2.3, e continua.
findPassengerOnList() {
  countOfNameOccurences=$(grep $1 $PASSENGERS_FILE -c)
  if [[ $countOfNameOccurences -gt 0 ]]; then
    so_success "S1.2.3"
  else
    so_error "S1.2.3"
  fi
}

## S1.2.4. Como o passageiro <Nome> não existe no ficheiro, terá de o registar. Para isso, valida se <NIF> (campo opcional) foi mesmo passado. Se não foi, dá so_error S1.2.4 e termina. Senão, dá so_success S1.2.4.
checkVAT() {
  if ! [[ -n "$1" ]]; then
    so_error "S1.2.4"
    exit 1
  fi

  so_success "S1.2.4"
}


## S1.2.5. Define o campo <ID_passageiro>, como sendo o UserId Linux associado ao utilizador de nome <Nome> no servidor Tigre. Em caso de haver algum erro na operação, dá so_error S1.2.5 e termina. Caso contrário, dá so_success S1.2.5 <ID_passageiro> (substituindo pelo campo definido).
getTigreIdForUser() {
  if ! [[ -f $TIGRE_USERS_FILE ]]; then
    so_error "S1.2.5"
  fi

  return $(grep $1 $TIGRE_USERS_FILE | cut -f3 -d ':')
}

## S1.2.6. Define o campo <Email>, gerado a partir do <Nome> introduzido pelo utilizador, usando apenas o primeiro e o último nome (dica: https://moodle23.iscte-iul.pt/mod/forum/discuss.php?d=5344), convertendo-os para minúsculas apenas, colocando um ponto entre os dois nomes, e domínio iscteflight.pt. Assim sendo, um exemplo seria “david.gabriel@iscteflight.pt”. Se houver algum erro na operação (e.g., o utilizador “root” tem menos de 2 nomes), dá so_error S1.2.6 e termina. Caso contrário, dá so_success S1.2.6 <Email> (substituindo pelo campo gerado). Ao registar um novo passageiro no sistema, o número inicial de <Saldo> tem o valor 0 (zero).
createEmailAddressForUser() {
  fullName=($1)

  if [[ ${#fullName[@]} -lt 2 ]]; then
    so_error "S1.2.6"
    exit 1
  fi

  email=${fullName[0]}.${fullName[${#fullName[@]} - 1]}@iscteflight.pt
  email=$(echo "$email" | awk '{print tolower($0)}')

  return 0
}

## S1.2.7. Regista o utilizador numa nova linha no final do ficheiro passageiros.txt, seguindo a sintaxe:   <ID_passageiro>:<NIF>:<Nome>:<Email>:<Senha>:<Saldo>. Em caso de haver algum erro na operação (e.g., erro na escrita do ficheiro), dá so_error S1.2.7 e termina. Caso contrário, dá so_success S1.2.7 <linha> (substituindo pela linha completa escrita no ficheiro).

## S1.3. Adiciona créditos na conta de um passageiro que existe no ficheiro passageiros.txt:
## S1.3.1. Tendo já encontrado um “match” passageiro com o Nome <Nome> no ficheiro, valida se o campo <Senha> passado corresponde à senha registada no ficheiro. Se não corresponder, dá so_error S1.3.1 e termina. Caso contrário, dá so_success S1.3.1.

## S1.3.2. Mesmo que tenha sido passado um campo <NIF> (opcional), ignora-o. Adiciona o valor passado do campo <Saldo a adicionar> ao valor do <Saldo> registado no ficheiro passageiros.txt para o passageiro em questão, atualizando esse valor no ficheiro passageiros.txt. Se houver algum erro na operação (e.g., erro na escrita do ficheiro), dá so_error S1.3.2 e termina. Caso tudo tenha corrido bem, dá o resultado so_success S1.3.2 <Saldo> (substituindo pelo valor saldo atualizado no ficheiro passageiros.txt).

## S1.4. Lista todos os passageiros registados, mas ordenados por saldo:
## S1.4.1. O script deve criar um ficheiro chamado passageiros-saldos-ordenados.txt igual ao que está no ficheiro passageiros.txt, com a mesma formatação, mas com os registos ordenados por ordem decrescente do campo <Saldo> dos passageiros. Se houver algum erro (e.g., erro na leitura ou escrita do ficheiro), dá so_error S1.4.1, e termina. Caso contrário, dá so_success S1.4.1.
