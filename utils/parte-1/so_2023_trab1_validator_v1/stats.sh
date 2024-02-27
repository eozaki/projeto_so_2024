#!/bin/bash

# Utilização: ./stats.sh passageiros | top <nr>"

erro=0
case "$1" in
    passageiros)
        if (( $# != 1 )); then
            echo "[*SO*] erro em stats.sh passageiros: número de parametros errado"
            erro=1
        else
            echo "[*SO*] ok stats.sh passageiros"
        fi

    ;;
    top)
        if (( $# != 2 )); then
            echo "[*SO*] erro em stats.sh top: número de parametros errado"
            erro=1
        else
            echo "[*SO*] ok stats.sh top '$2'"
        fi
    ;;
    *)
        echo "[*SO*] erro em stats.sh: parâmetros inválidos"
        erro=1
    ;;
esac

if (( $erro )); then
    i=0
    for v in $@; do
        ((i++))
        echo "[*SO*] $i. '$v'"
    done
fi
