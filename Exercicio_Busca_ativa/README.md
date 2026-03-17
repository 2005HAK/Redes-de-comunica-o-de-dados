# Sistema de Monitoramento Remoto (Client/Server)

Este projeto implementa uma arquitetura Cliente-Servidor em **C++17** e **Qt 5** para monitoramento remoto de dispositivos. O sistema permite o cadastro de máquinas alvo e a captura sob demanda da tela primária e do feed da webcam via TCP.

## Arquitetura do Sistema

O projeto é dividido em dois componentes principais:

### 1. Servidor (Device Agent)
Roda de forma invisível na máquina alvo como um serviço de usuário em segundo plano (*daemon*).
* **Captura de Tela:** Utiliza a API nativa do sistema (`QGuiApplication::primaryScreen()`) para capturar o display ativo.
* **Captura de Webcam:** Implementação assíncrona orientada a eventos usando `Qt Multimedia` (`QCamera` e `QCameraImageCapture`), garantindo estabilidade do sensor de hardware antes da extração do *frame*.
* **Comunicação:** Escuta requisições na porta **TCP 55555**. Quando recebe o comando de captura, empacota as imagens e as envia de volta ao cliente.

### 2. Cliente (Device Controller)
A interface de gerenciamento que o administrador utiliza para monitorar as máquinas.
* **Gerenciamento de Dispositivos:** Interface gráfica para adicionar, remover e listar máquinas.
* **Segurança e Validação:** Bloqueia entradas inválidas utilizando Expressões Regulares (Regex) para nomes de dispositivos e endereços IPv4.
* **Persistência de Dados (SQLite3):** Armazena os dispositivos cadastrados em um banco local.
* **Recepção de Dados:** Conecta-se aos agentes, envia o gatilho de requisição e decodifica o pacote binário recebido para exibição.

---

## Execução e Dependências (Linux/Debian)

Para rodar o codigo basta executar o *compiler.sh* da pasta server no computador alvo (garanta que o cmake esta instalado na máquina). Na máquina cliente execute o *compiler.sh* da pasta client (o programa abrirá ao terminar de compilar ou, se fechado, pode ser aberto rodando o *run.sh*)

```bash
sudo apt update
sudo apt install build-essential cmake pkg-config
./compiler.sh