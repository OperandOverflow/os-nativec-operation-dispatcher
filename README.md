## Overview

- Nome do projeto: AdmPor
- Tecnologias: C, linux APIs, POSIX
- Última atualização em: 20 de Maio de 2023

## Autores

- Guilherme Marcelo <fc58173> - [@fc58173](https://git.alunos.di.fc.ul.pt/fc58173)
- Eduardo Santos    <fc58185> - [@fc58185](https://git.alunos.di.fc.ul.pt/fc58185)
- Xi Wang           <fc58183> - [@fc58183](https://git.alunos.di.fc.ul.pt/fc58183)

## Sumário

- No âmbito da unidade curricular Sistemas Operativos no ano lectivo 2022-23, na componente teórico-prática, este projeto foi desenvolvido com o objetivo geral de desenvolver uma aplicação em C para simular o fluxo central de um serviço de administração portuária com várias operações, tais como efetuar cargas, descargas, armazenagem de mercadorias, onde o modelo de interação entre as entidades envolvidas (clientes, intermediários e empresas) é o modelo produtor/consumidor. O objetivo desta fase do projeto é usar mecanismos de sincronização entre processos para resolver algumas limitações de implementação que foram relatadas na fase anterior do desenvolvimento do projeto.


## Limitações da implementação
Nesta seção, vão estar indicadas as escolhas mais relevantes no desenvolvimento do trabalho, isto é, aquela que consideramos as mais decisivas no desenho geral da nossa solução e que também limitam o seu funcionamento.


* **[circular_buffer]** read_client_interm_buffer - O processo de leitura nesse tipo de buffer não funciona quando o tamanho do buffer é 1. Como os pointers de leitura e escrita ficam no mesmo indice, o nosso procedimento de leitura retorna por assumir que este se encontra vazio.
* **[IDs]** - De modo a facilitar o acesso dos recursos específicos de cada cliente/intermediário/empresa e também validar se os parâmetros introduzidos pelo utilizador eram válidos, assumimos que o ID de cada entidade estavam compreendidos entre 0 e (número de entidades - 1). Isso significa que, o utilizador, a priori, que os IDs estão a seguir essa regra.
* **[operation_number]** - A contagem de operações criadas globalmente na aplicação foi guardada em uma variável global, operation_number. Essa variável também é utilizada para que a id de uma operação tenha a sua "ordem" de criação (o pedido com id 0 foi criado antes do pedido com id 1 et cetera). Essa indicação de ordem também foi utilizada no procedimento enterprise_process_operation, de modo a verificarmos se a empresa deveria agendar ou executar cada pedido. 