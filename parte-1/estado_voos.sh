#!/bin/bash
# SO_HIDE_DEBUG=1                   ## Uncomment this line to hide all @DEBUG statements
# SO_HIDE_COLOURS=1                 ## Uncomment this line to disable all escape colouring
. ./so_utils.sh                     ## This is required to activate the macros so_success, so_error, and so_debug

###############################################################################
## ISCTE-IUL: Trabalho prático de Sistemas Operativos 2023/2024, Enunciado Versão 3+
##
## Aluno: Nº: 122088       Nome: Erick Cordeiro Ozaki
## Nome do Módulo: S3. Script: estado_voos.sh
## Descrição/Explicação do Módulo:
##
##
###############################################################################

## Este script não recebe nenhum argumento, e é responsável pelo relatório do estado dos voos que pertencem à plataforma IscteFlight.

FLIGHT_FILE="voos.txt"
FLIGHT_PAGE="voos_disponiveis.html"

## S3.1. Validações:
## S3.1.1. O script valida se o ficheiro voos.txt existe. Se não existir, dá so_error e termina. Senão, dá so_success.
if [[ -f "$FLIGHT_FILE" ]]; then
  so_success "S3.1.1"
else
  so_error "S3.1.1"
  exit 1
fi

## S3.1.2. O script valida se os formatos de todos os campos de cada linha do ficheiro voos.txt correspondem à especificação indicada em S2, nomeadamente se respeitam os formatos de data e de hora. Se alguma linha não respeitar, dá so_error <conteúdo da linha> e termina. Caso contrário, dá so_success.
# FR2101:Lisboa:Dubai:2024-05-23:08h00:320:210:35
FLIGHT_REGEX='^.{6}:([A-Z]|[a-z]|\ )+:([A-Z]|[a-z]|\ )+:[0-9]{4}-[0-9]{2}-[0-9]{2}:[0-9]{2}h[0-9]{2}:[0-9]+:[0-9]+:[0-9]+'
while IFS="" read -r linha
do
  if ! [[ "$linha" =~ $FLIGHT_REGEX ]]; then
    so_error 'S3.1.2' "$linha"
    exit 1
  fi
done < "$FLIGHT_FILE"

so_success 'S3.1.2'

## S3.2. Processamento do script:
## S3.2.1. O script cria uma página em formato HTML, chamada voos_disponiveis.html, onde lista os voos com lugares disponíveis, indicando nº, origem, destino, data, hora, lotação, nº de lugares disponíveis, e nº de lugares ocupados (para isso deve calcular a diferença dos anteriores). Em caso de erro (por exemplo, se não conseguir escrever no ficheiro), dá so_error e termina. Caso contrário, dá so_success.
# <h2>Voo: PT6202, De: Tóquio Para: Singapura, Partida em 2024-03-10 16h45</h2>
# <ul>
# <li><b>Lotação:</b> 210 Lugares</li>
# <li><b>Lugares Disponíveis:</b> 35 Lugares</li>
# <li><b>Lugares Ocupados:</b> 175 Lugares</li>
# </ul>

if [[ -f "$FLIGHT_PAGE" ]]; then
  rm "$FLIGHT_PAGE"
fi

htmlContent="<html><head><meta charset=\"UTF-8\"><title>IscteFlight: Lista de Voos Disponíveis</title></head>\n"
htmlContent=$htmlContent"<body><h1>Lista atualizada em $(date +'%Y-%m-%d %H:%M:%S')</h1>\n"
while IFS=":" read -r codVoo origem destino data hora preco lotacao disponiveis
do
  if [[ "$disponiveis" -gt 0 ]]; then
    htmlContent=$htmlContent"<h2>Voo: $codVoo, De: $origem Para: $destino, Partida em $data $hora</h2>\n"
    htmlContent=$htmlContent"<ul>\n"
    htmlContent=$htmlContent"<li><b>Lotação:</b> $lotacao Lugares</li>\n"
    htmlContent=$htmlContent"<li><b>Lugares Disponíveis:</b> $disponiveis Lugares</li>\n"
    htmlContent=$htmlContent"<li><b>Lugares Ocupados:</b> $(($lotacao - $disponiveis)) Lugares</li>\n"
    htmlContent=$htmlContent"</ul>\n"
  fi
done < "$FLIGHT_FILE"
htmlContent=$htmlContent"</body></html>"

if [[ $(echo -e "$htmlContent" > "$FLIGHT_PAGE") -eq 0 ]]; then
  so_success 'S3.2.1'
else
  so_error 'S3.2.1'
fi

## S3.3. Invocação do script estado_voos.sh:
## S3.3.1. Altere o ficheiro cron.def fornecido, por forma a configurar o seu sistema para que o script seja executado de hora em hora, diariamente. Nos comentários no início do ficheiro cron.def, explique a configuração realizada, e indique qual o comando que deveria utilizar para despoletar essa configuração. O ficheiro cron.def alterado deverá ser submetido para avaliação juntamente com os outros Shell scripts.
