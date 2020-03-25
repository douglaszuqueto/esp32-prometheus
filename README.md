# ESP32 Prometheus

Utilizando Prometheus com ESP32

![img](https://gblobscdn.gitbook.com/assets%2F-M15s4TnAJw6r_zYzKkX%2F-M3HLnucrqfFIB7ZmE3Q%2F-M3HMGF5p5p4hbrZabpI%2Fdashboard-v1.png?alt=media&token=8e621170-e22f-44cb-9ba9-64e4bf0e25b9)

## Introdução

Portanto, por meio desse artigo você aprenderá:

* Coletar métricas do ESP32
* Montar as métricas no formato do prometheus
* Expor as métricas coletadas através do webserver 
* Instalar o Grafana
* Instalar o Prometheus
* Integrar o Prometheus com ESP32
* Criar a dashboard de monitoramento

## Métricas monitoradas

* Uptime
* Temperatura interna
* Boot counter
* WiFi RSSI
* Memória total/utilizada
* Memória flash total/utilizada 

## Materiais necessários

* 01 x ESP32
* 01 x Raspberry(ou qualquer outra SBC compátivel / servidor)

## Tecnologias utilizadas

* Grafana
* Prometheus

## Serviços

Os serviços serão instalados levando-se em consideração o uso de uma Raspberry PI. Caso deseja fazer em outro ambiente que seja baseado em Linux, os procedimentos serão bem parecidos :)

### Grafana

Serviço para compor dashboards elegantes de monitoramento de forma simples, a qual possui suporte a múltiplos banco de dados(datasources). 

Ex: Prometheus, MySQL, PostgreSQL, dentre outros.

### Prometheus

Banco de dados focado em séries temporais - TSDB(Time Series Database), muito utilizado em ecossistemas IoT.

Exemplos de TSDB's:

* InfluxDB
* TimescaleDB
* Prometheus

## Desenvolvendo o projeto *Monitorando o ESP32 com Prometheus através do Grafana*

Diagrama de fluxo

<p align="center">
  <img src="https://github.com/douglaszuqueto/esp32-prometheus/raw/master/.github/diagrama-v1.png">
</p>

## Criando métricas

### Tipos de métricas aplicáveis

* Counter - apenas incremento
* Gauge - semelhante ao counter porém pode aumentar e diminuir

Exemplos:

* Counter: contador de boot
* Gauge: temperatura interna

### Layout do Prometheus

```
# esp32_uptime Uptime total do ESP32
# TYPE esp32_uptime gauge
esp32_uptime 23899
```

Primeira e segunda linhas são referente a comentários envolvendo o nome da métrica, seu tipo e descrição

Já a última linha, envolve o nome da métrica e seu devido valor.

Como podemos ver, o formato é muito simples.

### Função para abstrair a métrica a ser montada

A fim de não repetir muito código e concatenação de strings para formatar as métricas no formato prometheus, foi criado uma simples função para deixar o processo mais simples.

```c
void setMetric(String *p, String metric, String value) {
  *p += "# " + metric + "\n";
  *p += "# TYPE " + metric + " gauge\n";
  *p += "" + metric + " ";
  *p += value;
  *p += "\n";
}
```

Utilizando a função:

```c
String uptime = String(millis());

setMetric(&p, "esp32_uptime", uptime);
```

### Firmware

Baixe o repositório e faça o upload do firmware localizado na pasta **esp32-prometheus**

*Obs:* Antes de fazer o upload não esqueça de configurar as credenciais da rede wifi como também o IP estático do esp de acordo com sua rede.

Depois de efetuar o upload e o firmware estiver rodando, você pode acessar o webserver através da seguinte url: http://IP_ESP:80/metrics

Ao acessar, você deverá observar o seguinte resultado:

```
# esp32_uptime
# TYPE esp32_uptime gauge
esp32_uptime 7152784
# esp32_wifi_rssi
# TYPE esp32_wifi_rssi gauge
esp32_wifi_rssi -34
# esp32_heap_size
# TYPE esp32_heap_size gauge
esp32_heap_size 342316
# esp32_free_heap
# TYPE esp32_free_heap gauge
esp32_free_heap 262444
# esp32_sketch_size
# TYPE esp32_sketch_size gauge
esp32_sketch_size 728592
# esp32_flash_size
# TYPE esp32_flash_size gauge
esp32_flash_size 3145728
# esp32_available_size
# TYPE esp32_available_size gauge
esp32_available_size 2417136
# esp32_temperature
# TYPE esp32_temperature gauge
esp32_temperature 46.11
# esp32_boot_counter
# TYPE esp32_boot_counter gauge
esp32_boot_counter 7
```

## Instalação dos serviços

### Base

Antes de iniciarmos com a instalação propriamente dita, vamos criar uma pasta onde todos serviços ficarão armazenados.

```bash
# 1º
mkdir services

# 2º
cd services

# 3º => /home/pi/services
pwd
```

### Grafana

```bash
# 1º
wget https://dl.grafana.com/oss/release/grafana_6.6.2_armhf.deb

# 2º Efetuando a instalação
sudo dpkg -i grafana_6.6.2_armhf.deb

# 3º Habilitando o grafana para que inicie automaticamente ao iniciar a raspberry
sudo systemctl enable grafana-server 

# 4º Iniciando o serviço
sudo systemctl start grafana-server 

# 3º Visualizando o status
sudo systemctl status grafana-server 

```

### Prometheus

```bash
# 1º
wget https://github.com/prometheus/prometheus/releases/download/v2.16.0/prometheus-2.16.0.linux-armv7.tar.gz

# 2º
tar zxvf prometheus-2.16.0.linux-armv7.tar.gz

# 3º 
mv prometheus-2.16.0.linux-armv7 prometheus

# 4º 
cd prometheus

# 5º => /home/pi/services/prometheus
pwd

# 6º Criação do serviço do prometheus para se integrar ao systemd assim como o grafana já está
echo "
[Unit]
Description=Prometheus Server
Documentation=https://prometheus.io/docs/introduction/overview/
After=network-online.target

[Service]
User=pi
Restart=on-failure
ExecStart=/home/pi/services/prometheus/prometheus \
  --config.file=/home/pi/services/prometheus/prometheus.yml \
  --storage.tsdb.path=/home/pi/services/prometheus/data \
  --web.enable-lifecycle \
  --web.enable-admin-api

[Install]
WantedBy=multi-user.target
" | sudo tee > /etc/systemd/system/prometheus.service

# 4º
sudo systemctl daemon-reload

# 5º
sudo systemctl enable prometheus 

# 6º
sudo systemctl start prometheus 

# 7º
sudo systemctl status prometheus
```

## Teste dos serviços instalados

Se tudo deu certo na instalação dos serviços, você poderá fazer os acessos conforme instruções abaixo:

* Grafana

http://IP_RASP:3000

User: admin
Pass: admin

* Prometheus

http://IP_RASP:9090

## Configuração dos serviços

### Prometheus

```bash
# 1º
cd /home/pi/services/prometheus

# 2º
cat prometheus.yml
```


```yml
# my global config
global:
  scrape_interval:     15s # Set the scrape interval to every 15 seconds. Default is every 1 minute.
  evaluation_interval: 15s # Evaluate rules every 15 seconds. The default is every 1 minute.
  # scrape_timeout is set to the global default (10s).

# Alertmanager configuration
alerting:
  alertmanagers:
  - static_configs:
    - targets:
      # - alertmanager:9093

# Load rules once and periodically evaluate them according to the global 'evaluation_interval'.
rule_files:
  # - "first_rules.yml"
  # - "second_rules.yml"

# A scrape configuration containing exactly one endpoint to scrape:
# Here it's Prometheus itself.
scrape_configs:
  # The job name is added as a label `job=<job_name>` to any timeseries scraped from this config.
  - job_name: 'prometheus'

    # metrics_path defaults to '/metrics'
    # scheme defaults to 'http'.

    static_configs:
    - targets: ['localhost:9090']
```

Adicione as seguintes configurações no arquivo para que possamos ter o esp32 integrado ao prometheus.

```yml
# my global config
global:
  scrape_interval:     15s # Set the scrape interval to every 15 seconds. Default is every 1 minute.
  evaluation_interval: 15s # Evaluate rules every 15 seconds. The default is every 1 minute.
  # scrape_timeout is set to the global default (10s).

# Alertmanager configuration
alerting:
  alertmanagers:
  - static_configs:
    - targets:
      # - alertmanager:9093

# Load rules once and periodically evaluate them according to the global 'evaluation_interval'.
rule_files:
  # - "first_rules.yml"
  # - "second_rules.yml"

# A scrape configuration containing exactly one endpoint to scrape:
# Here it's Prometheus itself.
scrape_configs:
  # The job name is added as a label `job=<job_name>` to any timeseries scraped from this config.
  - job_name: 'prometheus'

    # metrics_path defaults to '/metrics'
    # scheme defaults to 'http'.

    static_configs:
    - targets: ['localhost:9090']

  # ESP32
  - job_name: 'esp32'
    scrape_interval: 5s
    static_configs:
    - targets: ['IP_ESP:80']
```

Finalizando a configuração:

```bash
# 1º - Comando HTTP para recarregar o arquivo de configuração que foi editado no passo acima
curl -X POST 127.0.0.1:9090/-/reload
```

Caso queira validar que o Prometheus continua funcionando normalmente e coletando as métricas do esp32, navegue até a url: http://IP_RASP:9090/targets

Se tiver sucesso, você terá uma tela como a da imagem abaixo mostrando um **"State Ok"**

<p align="center">
  <img src="https://github.com/douglaszuqueto/esp32-prometheus/raw/master/.github/prometheus-v1.png">
</p>

### Grafana

#### Adicionando datasource

Navegue até o menu **Configuration** > **Datasources** > *Add data source*

Na tela basicamente você precisa configurar apenas a variável **URL** localizada na seção *HTTP* com o IP da sua Raspberry

Ex: http://192.168.0.150:9090

<p align="center">
  <img src="https://github.com/douglaszuqueto/esp32-prometheus/raw/master/.github/grafana-datasource-v1.png">
</p>

#### Importando a dashboard

Copie o conteúdo do arquivo json localizado [aqui](https://github.com/douglaszuqueto/esp32-prometheus/blob/master/.github/dashboard-v1.json).

No grafana vá até a opção **Create(+)** > **Import**, cole o conteúdo copiado e clique em *load*. Na tele seguinte você pode alterar o nome da dashboard como também seu identificador - pode manter o mesmo em ambos!

<p align="center">
  <img src="https://github.com/douglaszuqueto/esp32-prometheus/raw/master/.github/grafana-import-v1.png">
</p>

## Conclusão

## Referências

* [Firmware para ESP32 com suporte a integração com WebService e configuração via WiFiManager](https://github.com/douglaszuqueto/esp32-http-firmware)
* [Comunicação remota utilizando RPC e ESP32](https://github.com/douglaszuqueto/esp32-rpc)
* [ESP8266 Prometheus Exporter for DS18B20 OneWire Temperature Sensors](https://github.com/theoretick/esp8266_prometheus_exporter_for_ds18b20_temperature_sensors)
* [Example - Hello Server](https://github.com/espressif/arduino-esp32/blob/master/libraries/WebServer/examples/HelloServer/HelloServer.ino)
* [Example - WiFiClientStaticIP](https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WiFiClientStaticIP/WiFiClientStaticIP.ino)
* [ESP32 internal temperature](https://github.com/espressif/arduino-esp32/issues/2422)
