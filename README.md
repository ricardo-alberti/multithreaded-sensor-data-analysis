# Multithreaded Sensor Data Analysis Project

### Description
This project is a multithreaded C application using **pthreads** to process environmental sensor data in JSON format from Caxias do Sul and Bento Gonçalves.

The program separates tasks into threads for data reading, statistical analysis, and logging. It computes minimum, maximum, and average values for temperature, humidity, and atmospheric pressure, along with their timestamps. It also estimates battery consumption, identifies Spreading Factors (SF), records execution logs, and measures total runtime.

#### Threads Used

A total of 4 threads were used, all created in the `analyser.c` file, each responsible for a specific stage of the processing pipeline:

- **2 JSON reader threads (`reader_threads`)**: Responsible for reading the input JSON files in parallel.
- **1 logger thread (`logger_thread`)**: Responsible for writing application logs, recording events and relevant information during execution.
- **1 processing thread (`record_resolver_thread`)**: Responsible for processing the records read by the reader threads.

#### Dates Used

To define the analyzed period in the files, the following fields were used:

- `created_at` in the file `data/mqtt_senzemo_cx_bg.json`
- `payload_date` in the file `data/senzemo_cx_bg.json`

For the individual analyses of temperature, humidity, and atmospheric pressure, the `time` fields present in each variable within the following objects were used:

- `payload` in the file `data/mqtt_senzemo_cx_bg.json`
- `brute_data` in the file `data/senzemo_cx_bg.json`

### Build and Run

#### Requirements
- gcc
- Bash environment (Linux or WSL)

#### Build

```bash
chmod +x compile.sh
```

```bash
./compile.sh
```

#### Run

```bash
./sensor-data-analysis
```

#### Notes
- The script creates the build/ directory automatically.
- yyjson.c is only compiled if build/yyjson.o does not exist.

---

### Descrição

Este projeto é uma aplicação em C com **pthreads** para processar dados de sensores ambientais em formato JSON de Caxias do Sul e Bento Gonçalves.

O programa separa as tarefas em threads para leitura de dados, análise estatística e geração de logs. Ele calcula valores mínimos, máximos e médias de temperatura, umidade e pressão atmosférica, com seus respectivos horários. Também estima o consumo de bateria, identifica Spreading Factors (SF), registra logs e mede o tempo total de execução.

#### Threads Utilizadas

Foram utilizadas 4 threads, criadas no arquivo `analyser.c`, cada uma responsável por uma etapa específica do processamento:

- **2 threads de leitura dos arquivos JSON (`reader_threads`)**: Responsáveis por realizar a leitura dos arquivos de entrada de forma paralela.
- **1 thread de logger (`logger_thread`)**: Responsável pela escrita de logs da aplicação, registrando eventos e informações relevantes durante a execução.
- **1 thread de processamento (`record_resolver_thread`)**: Responsável pelo processamento dos registros lidos pelas threads de leitura.

#### Datas Utilizadas

Para definição do período analisado nos arquivos, foram utilizados os seguintes campos:

- `created_at` no arquivo `data/mqtt_senzemo_cx_bg.json`
- `payload_date` no arquivo `data/senzemo_cx_bg.json`

Já para as análises individuais de temperatura, umidade e pressão atmosférica, foram utilizados os campos `time` presentes em cada variável dentro dos objetos:

- `payload` no arquivo `data/mqtt_senzemo_cx_bg.json`
- `brute_data` no arquivo `data/senzemo_cx_bg.json`

### Compilação e Execução

#### Requisitos
- gcc
- Ambiente Bash (Linux ou WSL)

#### Compilar

```bash
chmod +x compile.sh
```

```bash
./compile.sh
```

#### Executar

```bash
./sensor-data-analysis
```

#### Observações
- O script cria automaticamente a pasta build/.
- yyjson.c só é compilado se build/yyjson.o não existir.

## Sensor Data Source
- [CityLivingLab](https://www.citylivinglab.com)

## Build System
- [Meson (Unity Build)](https://mesonbuild.com/Unity-builds.html)

## Dependencies
- [yyjson](https://github.com/ibireme/yyjson)