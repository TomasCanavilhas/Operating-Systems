#!/bin/bash

###############################################################################
## ISCTE-IUL: Trabalho prático de Sistemas Operativos
##
## Aluno: Nº: 98804      Nome: Tomás Casaes Canavilhas Fernandes
## Nome do Módulo: lista_cidadaos.sh
## Descrição/Explicação do Módulo: Este script cria automaticamente uma nova base
## de dados chamada cidadaos.txt apartir do ficheiro listagem.txt. Cria um único numero
## para cada cidadão, copia o nome do cidadão, calcula a idade do mesmo, copia a sua 
## localidade assim como o seu numero de telemovel e apresenta o numero de inoculações 
## da vacina feitas nesse cidadão.
###############################################################################
#numero de cidadaos
c=$(cat listagem.txt | wc -l)
#1000 + numero de cidadaos
d=$((10000+$c))
#escreve todos os numeros entre 10000 e 1000 + numero de cidadaos, um por um
numero=`seq 10001 $d`
#filtra os nomes do documento e apaga a ultima letra
nome=`awk -F'|' '{print $1}' listagem.txt | cut -c 6- | sed 's/.$//'`
#filtra os anos de nascimento do documento e subtrai ao ano atual
idade=`awk -F'|' '{print $2}' listagem.txt | cut -c 27- | awk '{print 2021 - $1}'`
#filtra a localidade do documento e apaga a ultima letra
localidade=`awk -F'|' '{print $3}' listagem.txt | cut -c 13- | sed 's/.$//'`
#filtra os numeros de telemovel do documento
telemovel=`awk -F'|' '{print $4}' listagem.txt | cut -c 12-`
#cria um ciclo for que vai escrever 0 'n' vezes, sendo 'n' o numero de cidadaos
vacina=`for ((i=0; i<$c; i++));do echo 0; done`
#o comando paste vai concatenar todas as variaveis numa só linha e vai escrever no documento cidadaos.txt
paste -d: <(echo "$numero") <(echo "$nome") <(echo "$idade") <(echo "$localidade") <(echo "$telemovel") <(echo "$vacina") > cidadaos.txt
#abre o documento cidadaos.txt
cat cidadaos.txt