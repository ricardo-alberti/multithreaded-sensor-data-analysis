# Multithreaded Sensor Data Analysis Project

### Description
This project is a multithreaded C application using **pthreads** to process environmental sensor data in JSON format from Caxias do Sul and Bento Gonçalves.

The program separates tasks into threads for data reading, statistical analysis, and logging. It computes minimum, maximum, and average values for temperature, humidity, and atmospheric pressure, along with their timestamps. It also estimates battery consumption, identifies Spreading Factors (SF), records execution logs, and measures total runtime.

### Build and Run

#### Requirements
- gcc
- Bash environment (Linux or WSL)

#### Build

    chmod +x compile.sh

    ./compile.sh

#### Run

    ./sensor-data-analysis

#### Notes
- The script creates the build/ directory automatically.
- yyjson.c is only compiled if build/yyjson.o does not exist.

---

### Descrição

Este projeto é uma aplicação em C com **pthreads** para processar dados de sensores ambientais em formato JSON de Caxias do Sul e Bento Gonçalves.

O programa separa as tarefas em threads para leitura de dados, análise estatística e geração de logs. Ele calcula valores mínimos, máximos e médias de temperatura, umidade e pressão atmosférica, com seus respectivos horários. Também estima o consumo de bateria, identifica Spreading Factors (SF), registra logs e mede o tempo total de execução.

### Compilação e Execução

#### Requisitos
- gcc
- Ambiente Bash (Linux ou WSL)

#### Compilar

    chmod +x compile.sh

    ./compile.sh

#### Executar

    ./sensor-data-analysis

#### Observações
- O script cria automaticamente a pasta build/.
- yyjson.c só é compilado se build/yyjson.o não existir.

## Sensor Data Source
- [CityLivingLab](https://www.citylivinglab.com)

## Build System
- [Meson (Unity Build)](https://mesonbuild.com/Unity-builds.html)

## Dependencies
- [yyjson](https://github.com/ibireme/yyjson)