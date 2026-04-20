# Sistema de Monitoramento Remoto via MQTT

Este projeto é um sistema de monitoramento de dispositivos distribuído, desenvolvido em **C++17** e **Qt 5**. O sistema utiliza o protocolo **MQTT** para comunicação assíncrona, permitindo que uma Central (Spy) solicite capturas de tela e do feed da webcam de Agentes (Targets) instalados em máquinas remotas.

A migração de Sockets TCP diretos para MQTT elimina a necessidade de abrir portas de firewall nas máquinas alvo (NAT traversal) e garante a resiliência da conexão através do padrão Publish/Subscribe.

---

## Arquitetura do Sistema

O sistema é composto por três nós principais:

1. **Broker MQTT (Mosquitto):** O servidor central de mensagens. Ele roteia os pacotes entre a Central e os Agentes. Roda nativamente na porta `1883`.
2. **DeviceAgent (O Alvo):** Um serviço executado em background na máquina monitorada.
   * Identifica seu próprio IP na rede local.
   * Assina o tópico `device/<IP_DO_ALVO>/command`.
   * Quando acionado, usa a API nativa do sistema (`QGuiApplication::primaryScreen` e `QCamera`) para capturar as imagens, empacota os dados e publica no tópico `device/<IP_DO_ALVO>/data`.
3. **CaptureController / Central (O Espião):** A interface de gerenciamento.
   * Utiliza um wildcard (curinga) para assinar o tópico `device/+/data`, permitindo escutar as imagens de todos os agentes da rede simultaneamente.
   * Controla a periodicidade das capturas via `QTimer`.
   * Desempacota os binários recebidos e salva as imagens localmente organizadas por IP e Timestamp.

---

## Dependências e Instalação (Debian/Ubuntu)

Para compilar e executar o sistema, você precisará dos pacotes de compilação C++, das bibliotecas do Qt 5 (incluindo Multimídia e Rede) e do ecossistema Mosquitto.

Execute o comando abaixo no terminal:

```bash
sudo apt update
sudo apt install build-essential cmake pkg-config
sudo apt install qtbase5-dev qt5-qmake qtmultimedia5-dev libqt5multimedia5-plugins
sudo apt install mosquitto mosquitto-clients libmosquitto-dev libsqlite3-dev
```

## Configuração do Broker Mosquitto

Por padrão, o Mosquitto (versão 2.0+) bloqueia conexões externas. Como a Central e os Agentes estão em máquinas diferentes, você precisa liberar o acesso na máquina onde o Broker está instalado (geralmente a mesma da Central).

1. Crie ou edite o arquivo de configuração local:

```bash
sudo nano /etc/mosquitto/conf.d/default.conf
```

2. Adicione as seguintes linhas para permitir conexões na rede local:

```bash
listener 1883
allow_anonymous true
```

3. Reinicie o serviço do Mosquitto para aplicar as regras:

```bash
sudo systemctl restart mosquitto
```

## Compilação

O projeto utiliza `CMake` como sistema de build em ambos os módulos.

### Compilando o Agente (Alvo)

Navegue até a pasta do Agente e execute:

```bash
./compiler.sh
```

*(Nota: Certifique-se de que a variável `brokerAddress` no arquivo `agent.hpp` esteja apontando para o IP real da sua máquina Central antes de compilar).*

Ao executar o compilador um serviço em segundo plano será criado e iniciado. Neste ponto a máquina alvo esta pronta, basta saber o *IP* da mesma para utilizar na central.

## Compilando a Central (Spy)

Navegue até a pasta da Central e execute os mesmos passos:

```bash
./compiler.sh
```

Ao terminar de compilar a tela de controle será iniciada e pode ser utilizada. Se esta tela for fechada não é necesário compilar no código novamente, basta executar:

```bash
./run.sh
```

## Estrutura do Payload de Dados

Para garantir a integridade da extração no lado do espião, o pacote MQTT enviado pelo Agente possui um cabeçalho customizado. O pacote contíguo de bytes é estruturado da seguinte forma:

1. Header (8 bytes):

   - quint32 (4 bytes): Tamanho do buffer da imagem da Tela.

   - quint32 (4 bytes): Tamanho do buffer da imagem da Webcam.

2. Payload: Array de bytes JPEG da Tela + Array de bytes JPEG da Webcam.

A Central lê os 8 primeiros bytes para saber exatamente onde "fatiar" o restante do buffer para salvar os dois arquivos JPG separadamente.

## Autores

- **Gabriella Arévalo Marques**  
	📧 [gabriellaarevalomarques@gmail.com](mailto:gabriellaarevalomarques@gmail.com)

- **Hebert Alan Kubis**  
	📧 [herbertkubis15@gmail.com](mailto:herbertkubis15@gmail.com)

