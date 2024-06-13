# PSA - Projeto de rastreabilidade e desboqueio por visão de uma _e-bike_

**P**rojeto em **S**istemas de **A**utomação - Grupo 2 - Turma P4 - Universidade de Aveiro - 2023/24

## Lista de Conteúdos

- [Introdução](#introduction)
- [Componentes utilizados](#components)
- [Protocolos utilizados](#protocols)
- [Linguagens de programação utilizadas](#languages)
- [_Softwares_ utilizados](#softwares)
- [Autores](#authors)


---
## Introduction

<p align="justify">
No contexto atual de urbanização acelerada e crescente preocupação com a sustentabilidade ambiental, as bicicletas elétricas, ou <i>e-bikes</i>, surgem como uma alternativa eficiente e ecológica para o transporte urbano. No entanto, a gestão e a segurança dessas <i>e-bikes</i> apresentam desafios significativos, como o rastreamento em tempo real e o controlo do acesso apenas a utilizadores autorizados. Para abordar estas questões, este projeto propõe o desenvolvimento de um sistema integrado de rastreamento e desbloqueio das <i>e-bikes</i>, utilizando tecnologias relacionadas com a <i>Internet of Things</i> (IoT). Desta forma, os objetivos para o projeto consistiram no desenvolvimento de: </p>

- Um sistema de visão artificial que leia um código QR e o descodifique;
- Um sistema que colete e guarde variáveis relacionadas com a localização GPS (latitude, longitude e velocidade);
- Um sistema de envio da informação relevante para um servidor remoto;
- Um sistema de processamento e armazenamento da informação da e-bike;
- Uma interface gráfica (_dashboard_) para monitorização da informação.


---
## Componentes utilizados

<p align="justify">
Para a criação da solução para este projeto, foram utilizados os seguintes componentes:</p>

- _ESP32_ como principal central de processamento na _e-bike_;
- _ESP32-CAM_ para o módulo de visão do projeto;
- Módulo de GPS <i>NEO-6M</i> para obter as coordenadas da _e-bike_, bem como outras informações relevantes, como a velocidade;
- Modem GPRS SIM900 para comunicação remota;
- Servidor remoto para integrar a interface IoT (_Internet of Things_), _broker_ MQTT e a base de dados;
- Opto-relé para destrancar o cadeado da _e-bike_.


![Prototipo_Compressed](https://github.com/RBastos36/PSA-eBike-Project/assets/145439743/723f937c-b5d7-47ad-88e4-f71a5076ce88)

<p align="center">
Protótipo do <i>hardware</i> utilizado.
</p>

[![Demonstração do protótipo em funcionamento](https://img.youtube.com/vi/8IhwF9_wa2A/0.jpg)](https://www.youtube.com/watch?v=8IhwF9_wa2A)
<p align="center">
Demonstração do funcionamento do protótipo.
</p>


---
## Protocolos utilizados


Para o desenvolvimento deste projeto, foram utilizados os seguintes protocolos:
- **MQTT**
  - Descrição: protocolo de comunicação leve e de formato publicação/assinatura, projetado para conexões remotas onde a largura de banda é limitada. Amplamente utilizado em aplicações de _Internet of Things_ (IoT) devido à eficiência na transmissão de mensagens entre dispositivos através de redes instáveis ou de baixa largura de banda.


- **TCP/IP**
  - Descrição: conjunto de protocolos de comunicação utilizado para a interconexão de dispositivos em redes de computadores; o TCP garante a entrega confiável e ordenada de um fluxo de bytes entre aplicações em rede, enquanto o IP trata do endereçamento e roteamento dos pacotes de dados através de redes interligadas, formando a base da _Internet_.


- **UART**
  - Descrição: protocolo de comunicação série assíncrona utilizado para a transmissão de dados entre dispositivos eletrónicos. Converte os dados paralelos de um dispositivo em um formato série e vice-versa, permitindo a comunicação ponto a ponto sem a necessidade de um _clock_ sincronizado.

---
## Linguagens de programação utilizadas

Para este projeto foram utilizados os seguintes linguagens de programação:

- **C++** aplicado no _Arduino IDE_, para programar os microcontroladores _ESP_;
- **Node-Red** para criar a interface gráfica e processar a informação a entrar no servidor;
- **Python** para explorar outras alternativas para a visão artificial utilizando microcontroladores.


---
## _Softwares_ utilizados

Para a execução deste projeto, foram usados os seguintes _softwares_, de forma a agilizar o processo:

- **XAMPP** para correr a base de dados, o _broker_ e a _dashboard_ IoT no servidor remoto;
- **Mosquitto** como _broker_ MQTT noi servidor remoto;
- **Arduino IDE** para programar os microcontroladores.

---
## Autores

- **[Adriano Ferreira](https://github.com/AdrianoFF10)**
  - Informação:
    - Email: adrianofigueiredo7@ua.pt

- **[Ricardo Bastos](https://github.com/RBastos36)**
  - Informação:
    - Email: r.bastos@ua.pt
   
- **Tiago Oliveira**
  - Informação:
    - Email: tiago.oliv@ua.pt
