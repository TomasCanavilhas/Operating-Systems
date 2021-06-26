#!/bin/bash

###############################################################################
## ISCTE-IUL: Trabalho prático de Sistemas Operativos
##
## Aluno: Nº: 98804       Nome: Tomás Casaes Canavilhas Fernandes
## Nome do Módulo: menu.sh
## Descrição/Explicação do Módulo: Este script cria uma interface entre o utilizador
## e os restantes scripts através da criação de um menu, facilitando a utilização dos 
## restantes scripts e torna-os também mais interativos.
###############################################################################
while :; do
    echo -e '\033[01;32m===========================\033[00;37m'
    echo -e '\033[01;32m         M E N U          \033[00;37m'
    echo -e '\033[01;32m===========================\033[00;37m'
    echo -e '\033[01;32m=\033[00;37m' 1. Listar cidadãos'\033[01;32m      =\033[00;37m'
    echo -e '\033[01;32m=\033[00;37m' 2. Adicionar enfermeiro'\033[01;32m =\033[00;37m'
    echo -e '\033[01;32m=\033[00;37m' 3. Stats'\033[01;32m                =\033[00;37m'
    echo -e '\033[01;32m=\033[00;37m' 4. Agendar vacinação'\033[01;32m    =\033[00;37m'
    echo -e '\033[01;32m=\033[00;37m' 0. Sair '\033[01;32m                =\033[00;37m'
    echo -e '\033[01;32m===========================\033[00;37m'

    echo "Introduza a opção que pretende: "
    read option

    #se print = 0 volta ao menu 
    if [[ $option == 0 ]]; then
        break

    elif [ $option == 1 ]; then
        echo -e '\033[05;31m==================================================\033[00;37m'
        echo -e '\033[05;31m         L I S T A   D E   C I D A D Ã O S        \033[00;37m'
        echo -e '\033[05;31m==================================================\033[00;37m'
        ./lista_cidadaos.sh
        echo -e '\033[05;31m==================================================\033[00;37m'

    

    elif [ $option == 2 ]; then
        echo -e '\033[01;32m==============================\033[00;37m'
        echo -e '\033[01;32mIntroduza o nome do enfermeiro\033[00;37m'
        echo -e '\033[01;32m==============================\033[00;37m'
        read nome
        echo -e '\033[01;32m==========================================\033[00;37m'
        echo -e '\033[01;32mIntroduza o número da cédula do enfermeiro\033[00;37m'
        echo -e '\033[01;32m==========================================\033[00;37m'
        read cedula
        echo -e '\033[01;32m===================================================\033[00;37m'
        echo -e '\033[01;32mIntroduza o Centro de Saúde vinculado ao enfermeiro\033[00;37m'
        echo -e '\033[01;32m===================================================\033[00;37m'
        read CS
        echo -e '\033[01;32m=============================\033[00;37m'
        echo -e '\033[01;32mO enfermeiro está disponível?\033[00;37m'
        echo -e '\033[01;32m=============================\033[00;37m'
        echo -e '\033[01;32m= 0 - Não                   =\033[00;37m'
        echo -e '\033[01;32m= 1 - Sim                   =\033[00;37m'
        echo -e '\033[01;32m=============================\033[00;37m'
        read disponibilidade
        
        echo -e '\033[05;31m============================================================\033[00;37m'
        echo -e '\033[05;31m            L I S T A   D E   E N F E R M E I R O S         \033[00;37m'
        echo -e '\033[05;31m============================================================\033[00;37m'
        ./adiciona_enfermeiros.sh "$nome" $cedula "$CS" $disponibilidade
        echo -e '\033[05;31m============================================================\033[00;37m'

    

    elif [ $option == 3 ]; then
        echo -e '\033[01;32m===========================\033[00;37m'
        echo -e '\033[01;32m        S T A T S\033[00;37m'
        echo -e '\033[01;32m===========================\033[00;37m'
        echo -e '\033[01;32m=\033[00;37m' 1 - Cidadaos'\033[01;32m            =\033[00;37m'
        echo -e '\033[01;32m=\033[00;37m' 2 - Registados'\033[01;32m          =\033[00;37m'
        echo -e '\033[01;32m=\033[00;37m' 3 - Enfermeiros'\033[01;32m         =\033[00;37m'
        echo -e '\033[01;32m=\033[00;37m' 0 - Voltar ao menu'\033[01;32m      =\033[00;37m'
        echo -e '\033[01;32m===========================\033[00;37m'
        echo "Introduza a opção que pretende: "
        read a
        if [ $a -eq 1 ]; then
            echo -e '\033[01;32m====================\033[00;37m'
            echo -e '\033[01;32mEscolha a localidade\033[00;37m'
            echo -e '\033[01;32m====================\033[00;37m'
            read b
            ./stats.sh cidadaos "$b"
        elif [ $a -eq 2 ]; then
            echo -e '\033[05;31m============================================================\033[00;37m'
            echo -e '\033[05;31m                   R E G I S T A D O S                      \033[00;37m'
            echo -e '\033[05;31m============================================================\033[00;37m'
            ./stats.sh registados
            echo -e '\033[05;31m============================================================\033[00;37m'

        elif [ $a -eq 3 ]; then
            echo -e '\033[05;31m============================================================\033[00;37m'
            echo -e '\033[05;31m                   E N F E R M E I R O S                    \033[00;37m'
            echo -e '\033[05;31m============================================================\033[00;37m'
            ./stats.sh enfermeiros
            echo -e '\033[05;31m============================================================\033[00;37m'
        elif [ $a -eq 0 ]; then 
            ./menu.sh
        else
        echo -e '\033[05;31mErro: Opção Inválida\033[00;37m'
        fi
    
    elif [ $option == 4 ]; then
        echo -e '\033[05;31m============================================================\033[00;37m'
        echo -e '\033[05;31m                        A G E N D A                         \033[00;37m'
        echo -e '\033[05;31m============================================================\033[00;37m'
        ./agendamento.sh
        echo -e '\033[05;31m============================================================\033[00;37m'
    
    else
    echo -e '\033[05;31mErro: Opção Inválida\033[00;37m'
    fi
done


