#!/bin/bash

###############################################################################
## ISCTE-IUL: Trabalho prático de Sistemas Operativos
##
## Aluno: Nº: 98804       Nome: Tomás Casaes Canavilhas Fernandes
## Nome do Módulo: adiciona_enfermeiros.sh
## Descrição/Explicação do Módulo: Este script permite adicionar enfermeiros a uma base de 
## dados chamada enfermeiros.txt fornecendo o nome, nº da cedula, Centro de Saude associado
## e a sua disponibilidade (0 - Indisponivel // 1 - Disponivel).
###############################################################################
#inicia o documento enfermeiros.txt
touch enfermeiros.txt
#conta os bits da cedula
letras=`echo "$2" | wc -m`
#Apenas aceita caracteres de A-Z para o nome
if [[ "${1}" =~ [^a-zA-Z' '\t] ]]; then
  echo -e '\033[05;31mErro: Nome inválido. Apenas pode conter caracteres entre A-Z\033[00;37m'
#Apenas aceita caracteres entre 0-9 para a cédula
elif [[ "${2}" =~ [^0-9] ]]; then
  echo -e '\033[05;31mErro: Cédula inválida. Apenas pode conter números entre 0-9\033[00;37m'
#Apenas aceita numeros com exatamente 5 digitos
elif [[ $letras -ne 6 ]]; then
  echo -e '\033[05;31mErro: Cédula inválida. Apenas pode conter 5 dígitos numéricos\033[00;37m'
#Apenas aceita caracteres de A-Z para o Centro de Saude
elif [[ "${3}" =~ [^a-zA-Z' '\t] ]]; then
  echo -e '\033[05;31mErro: Centro de Saúde inválido. Apenas pode conter caracteres entre A-Z\033[00;37m'
#Apenas aceita 0 ou 1 para a disponibilidade
elif  [[ "${4}" =~ [^0-1] ]]; then
  echo -e '\033[05;31mErro: Disponibilidade inválida. Apenas pode conter caracteres entre 0-1\033[00;37m'
#Exige 4 argumentos para iniciar o script
elif [[ "$#" -eq 4 ]]; then
  cond1=`grep "$3" enfermeiros.txt | awk -F: '{print $3}'`
  cond2=`grep "$2" enfermeiros.txt | awk -F: '{print $1}'`

    #Verificar se já existe um enfermeiro registado neste CS
    if [ "$cond1" = "$3" ]; then
		echo Erro: O Centro de Saúde introduzido já tem um enfermeiro registado
    #Caso não esteja:
    else 
      #Verificar se o enfermeiro está registado noutro CS
	    if [ "$cond2" = "$2" ]; then 
			  CS=`grep $2 enfermeiros.txt | awk -F: '{print $3}'`
			  echo Erro: Este enfermeiro já se encontra registado em $CS
      #Caso não esteja, regista-o no documento enfermeiros.txt 	
      else
        paste -d: <(echo "$2") <(echo "$1") <(echo "$3") <(echo 0) <(echo "$4") >> enfermeiros.txt        
	    fi	
    fi
#Caso não dê os argumentos exigidos
else
  echo 'Erro: Síntaxe: $0 <número cédula profissional>:<nome>:<centro saúde associado>:<nº de vacinações efetuadas>:<disponibilidade>'
fi
#Abre o documento enfermeiros.txt
cat enfermeiros.txt