#!/bin/bash

# Utilização: ./regista_passageiro.sh <nome> <senha> <saldo> [<nif>]

if (( $# < 3 || $# > 4 )); then
    echo "[*SO*] erro em regista_passageiro.sh: número de parametros errado: $#"
    i=0
    for v in "$@"; do
        ((i++))
        echo "$i. '$v'"
    done
else
    if (( $# == 3 )); then
        echo "[*SO*] regista_passageiro.sh '$1' '$2' '$3'"
    else
        echo "[*SO*] regista_passageiro.sh '$1' '$2' '$3' '$4'"
    fi
fi
