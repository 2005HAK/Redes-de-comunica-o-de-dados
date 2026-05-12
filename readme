# Projeto de Redes: Question and Answer Protocol (QAP)

Este repositório contém a implementação do **QAP (Question and Answer Protocol)**, um protocolo de camada de aplicação customizado, desenvolvido sobre sockets TCP para uma arquitetura Cliente-Servidor de perguntas e respostas.

Este projeto foi desenvolvido como requisito acadêmico para a disciplina de Redes de Computadores, com foco no estudo de desenho de protocolos, implementação via *sockets* puros e análise de desempenho (overhead de cabeçalho).

## 🎯 Objetivos do Projeto

* Desenvolver um protocolo de aplicação do zero.
* Escrever a especificação do protocolo no formato RFC.
* Implementar o Cliente e o Servidor utilizando Sockets TCP.
* Avaliar e comparar o *overhead* do protocolo criado com padrões da indústria (HTTP e MQTT).

## 🛠️ Tecnologias Utilizadas

* **Linguagem:** Python 3.x
* **Bibliotecas:** `socket` (comunicação de rede) e `struct` (manipulação de dados binários).
* **Protocolo de Transporte:** TCP (Transmission Control Protocol)

## 📦 Estrutura do Repositório

* `server.py`: Código-fonte do servidor QAP, que hospeda a base de conhecimento e processa as requisições.
* `client.py`: Código-fonte do cliente QAP, responsável por enviar as perguntas e exibir as respostas.
* `Relatorio_Avaliacao.pdf`: (Adicione este arquivo) Relatório contendo a RFC do protocolo e a análise comparativa de desempenho.

## ⚙️ Especificação do Protocolo (Resumo)

O QAP é um protocolo binário projetado para ter o menor *overhead* possível. Toda mensagem possui um cabeçalho fixo de exatamente **4 bytes**:

* **1 Byte:** Versão do Protocolo (`0x01`).
* **1 Byte:** Tipo de Mensagem (`0x01` para Pergunta, `0x02` para Resposta).
* **2 Bytes:** Tamanho do *Payload* (Carga Útil).

O *overhead* fixo de 4 bytes torna o QAP extremamente mais eficiente que o HTTP (que gera 100+ bytes de overhead por mensagem) e o coloca no mesmo nível de eficiência de protocolos otimizados para IoT como o MQTT.

## 🚀 Como Executar

**Pré-requisitos:** Certifique-se de ter o Python 3.x instalado em sua máquina.

1. Clone este repositório ou baixe os arquivos fonte.
2. Abra um terminal e inicie o servidor:
   ```bash
   python server.py
O servidor ficará escutando na porta 8081.

3. Abra um segundo terminal (mantendo o servidor rodando) e inicie o cliente:

   ```Bash
   python client.py
## Autores 

- **Gabriella Arévalo Marques**  
	📧 [gabriellaarevalomarques@gmail.com](mailto:gabriellaarevalomarques@gmail.com)

- **Hebert Alan Kubis**  
	📧 [herbertkubis15@gmail.com](mailto:herbertkubis15@gmail.com)
