#!/bin/bash

###############################################################################
## ISCTE-IUL: Trabalho prático de Sistemas Operativos
##
## Aluno: Nº: 98804      Nome: Tomás Casaes Canavilhas Fernandes
## Nome do Módulo: agendamento.sh
## Descrição/Explicação do Módulo: Este script não aceita argumentos. Ele cria 
## automaticamente o ficheiro agenda.txt relacionando os dados do documento  
## enfermeiros.txt e cidadaos.txt. Este procura um enfermeiro disponivel e, caso
## encontre vai procurar cidadãos que residam na localidade do centro de saúde
## que esse enfermeiro está associado e que tenham menos de duas inoculações da
## vacina contra a Covid-19 e, marca automaticamente uma data para o supracitado
## enfermeiro vacinar esses cidadãos.
###############################################################################
touch agenda.txt
rm agenda.txt
    cat enfermeiros.txt | while read linha1; do
    DISPONIBILIDADE_ENFERMEIRO=`echo $linha1 | awk -F: '{print $5}'`
    CEDULA_ENFERMEIRO=`echo $linha1 | awk -F: '{print $1}'` 
    NOME_ENFERMEIRO=`echo $linha1 | awk -F: '{print $2}'`
    CENTRO_DE_SAUDE=`echo $linha1 | awk -F: '{print $3}'`
    CIDADE_ENFERMEIRO=`echo $linha1 | awk -F: '{print $3}' | cut -c3-`
        cat cidadaos.txt | while read linha2; do
        NOME_UTENTE=`echo $linha2 | awk -F: '{print $2}'` 
        CEDULA_UTENTE=`echo $linha2 | awk -F: '{print $1}'`
        CIDADE_UTENTE=`echo $linha2 | awk -F: '{print $4}'`
        VACINAS_UTENTE=`echo $linha2 | awk -F: '{print $6}'`
        DATA_VACINACAO=`echo $(date +%F)`
            if [[ $DISPONIBILIDADE_ENFERMEIRO -eq 1 ]]; then
                if [[ "$CIDADE_ENFERMEIRO" == "$CIDADE_UTENTE" ]]; then
                    if [ $VACINAS_UTENTE -lt 2 ]; then
                        paste -d: <(echo "$NOME_ENFERMEIRO") <(echo "$CEDULA_ENFERMEIRO") <(echo "$NOME_UTENTE") <(echo "$CEDULA_UTENTE") <(echo "$CENTRO_DE_SAUDE") <(echo "$DATA_VACINACAO") >> agenda.txt
                    fi
                fi
            fi
        done
    done
touch agenda.txt

cat agenda.txt


