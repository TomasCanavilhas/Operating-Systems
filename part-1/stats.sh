#!/bin/bash

###############################################################################
## ISCTE-IUL: Trabalho prático de Sistemas Operativos
##
## Aluno: Nº: 98804      Nome: Tomás Casaes Canavilhas Fernandes
## Nome do Módulo: stats.sh
## Descrição/Explicação do Módulo: Este script tem três funções.
## A função cidadaos tem como objetivo contar o numero de cidadãos presentes 
## numa dada localidade.
## A função registados apresenta o nome e numero de identificação dos cidadaos
## com mais de 60 anos, por ordem decrescente.
## A função enfermeiros apresenta o nome dos enfermeiros que se encontram 
## disponiveis nesse exato momento.
###############################################################################
## Aceita apenas caracteres entre A-Z para a localidade
if [[ "${2}" =~ [^a-zA-Z' '\t] ]]; then
    echo -e '\033[05;31mErro: Cidade inválida. Apenas pode conter caracteres entre A-Z\033[00;37m'
else
a=0
b=`echo $2`

cidadaos () {
## Inicia um ciclo while para ler a coluna da localidade no documento cidadaos.txt
while read -r linha; do
## Este comando ignora letras maiusculas e minusculas
shopt -s nocasematch
# Caso localidade = localidade // itera a variavel i em uma unidade // caso contrario nada acontece // esac (case ao contrário, fim da condição)
case "$linha" in $b ) let a=$((a+1)) ;; *) ;; esac
done <<< "$(cat cidadaos.txt | awk -F: '{print $4}')" 
echo O número de cidadãos registados em $b é $a
}

registados () {
## Filtra a coluna das idades por ordem decrescente e escolhe apenas idades superiores a 60
## Após essa filtragem, mostra o nome e numero desses cidadãos
cat cidadaos.txt | sort -t':' -nrk3 | awk -F: '{if($3>60)print$2" | "$1}'
}

enfermeiros () { 
    # Mostra o nome dos enfermeiros disponiveis
    touch enfermeiros.txt
    cat enfermeiros.txt | while read linha; do
        VAR2=`echo $linha | awk -F: '{print $5}'`
        if [[ "$VAR2" -eq 1 ]]; then 
            echo $linha | awk -F: '{print $2}'
        
        fi
    done 
}

$1 $2

fi
