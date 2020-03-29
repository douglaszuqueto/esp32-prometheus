# ESP32 Prometheus

Monitorando o ESP32 com Prometheus através do Grafana

![img](https://gblobscdn.gitbook.com/assets%2F-M15s4TnAJw6r_zYzKkX%2F-M3HLnucrqfFIB7ZmE3Q%2F-M3HMGF5p5p4hbrZabpI%2Fdashboard-v1.png?alt=media&token=8e621170-e22f-44cb-9ba9-64e4bf0e25b9)

## Introdução

Em diversos projetos envolvendo sensoriamento, se faz necessário o monitoramento dos dispositivos conectados na rede. Dessa forma, necessitamos utilizar ou desenvolver sistemas capazes de suprir essa demanda para poder analisar e monitorar qualquer anomalia que possa ocorrer na comunicação dos devices. 

Com um sistema implementado e com a coleta de algumas métricas ,podemos por exemplo detectar reboots inesperados, temperatura interna elevada, perca de pacotes devido a oscilação do sinal wifi e etc. Tais métricas serão abordadas em um tópico futuro.

Nesse artigo, você aprenderá como desenvolver um sistema para monitoramento do ESP32 utilizando serviços de código aberto e gratuitos denominados Prometheus e Grafana.

Portanto, por meio desse artigo você aprenderá:

* Coletar métricas do ESP32
* Montar as métricas no formato do prometheus
* Expor as métricas coletadas através do webserver 
* Instalar o Grafana
* Instalar o Prometheus
* Integrar o Prometheus com ESP32
* Criar a dashboard de monitoramento

Agora, iniciaremos a apresentação completa do desenvolvimento do projeto Monitorando o ESP32 com Prometheus através do Grafana.

## Desenvolvendo o projeto *Monitorando o ESP32 com Prometheus através do Grafana*

Antes de botarmos a mão na massa, a fim de se ter uma clareza melhor sobre o problema que queremos resolver, observe abaixo o diagrama de fluxo que iremos implementar nesse projeto.

<p align="center">
  <img src="https://github.com/douglaszuqueto/esp32-prometheus/raw/master/.github/diagrama-v2.png">
</p>

Basicamente esse diagrama pode ser dividido em 3 partes, sendo elas:

* 1 - Prometheus => ESP32
* 2 - Grafana => Prometheus
* 3 - Usuário => Grafana

Agora que demos uma pincelada sobre o fluxo que iremos implementar, seguimos para os materiais que serão necessários para o desenvolvimento do projeto.

## Materiais necessários

Como requisitos básicos para o desenrolar do projeto de monitoramento, precisaremos dos itens citados a seguir:

* 01 x ESP32
* 01 x Raspberry(ou qualquer outra SBC compatível / servidor)

A Raspberry será onde ficará hospedado os serviços necessários para rodar o projeto. Já o ESP32 será o "core" da aplicação, é com ele que conseguiremos coletar as métricas que serão abordadas no tópico abaixo.

## Métricas monitoradas

Monitorar algumas métricas de nossos dispositivos IoT são de suma importáncia, portanto veja na lista abaixo o que iremos será monitorado.

* Uptime
* Temperatura interna
* Boot counter
* WiFi RSSI
* Memória total/utilizada
* Memória flash total/utilizada

Tendo uma noção das métricas que serão monitoradas, podemos ir para o próximo tópico onde iremos tratar sobre os dois serviços que serão utilizados no projeto.

## Serviços

Os serviços serão instalados levando-se em consideração o uso de uma Raspberry PI. Caso deseja fazer em outro ambiente que seja baseado em Linux, os procedimentos serão bem parecidos :)

Na sequência abordaremos o Prometheus, o primeiro serviço que será utilizado neste projeto.

### Prometheus

Banco de dados focado em séries temporais - TSDB(Time Series Database), muito utilizado em ecossistemas IoT.

Exemplos de TSDB's:

* InfluxDB
* TimescaleDB
* Prometheus

Já ouviu o famoso ditado: "Dividir para conquistar", pois então.. temos um banco de dados, mas como conseguiremos observar as métricas de forma gráfica? É ai que o Grafana entra na jogada!

### Grafana

Serviço para compor dashboards elegantes de monitoramento de forma simples, a qual possui suporte a múltiplos banco de dados(datasources). 

Ex: Prometheus, MySQL, PostgreSQL, dentre outros.

Agora que sabemos um pouco mais sobre os serviços que utilizaremos neste projeto, podemos partir para a criação das métricas no prometheus.

## Métricas no Prometheus

Para estar criando métricas e integrando ao prometheus, você precisa ter uma noção base dos tipos compátiveis e como cada métrica é montada respeitando o layout do serviço. 

### Tipos

Como não temos nenhum cliente prometheus para o ESP32, se faz necessário montar manualmente as métricas, e dentre os tipos disponíveis do Prometheus, os mais acessíveis para se implementar são os citados logo abaixo.

* Counter - apenas incremento
* Gauge - semelhante ao counter porém pode aumentar e diminuir

Exemplos:

* Counter: contador de boot
* Gauge: temperatura interna

Para implementação dessas métricas, temos que seguir um padrão determinado pelo prometheus, veja na sequência como deve ser esse modelo.

### Layout

O prometheus segue a risca um formato para que as métricas expostas se tornem válida. Então ele impõe uma estrutura e consequentemente precisamos nos adaptar a ela.

Abaixo segue um exemplo referente a uma métrica que será coletada:

```
# esp32_uptime Uptime total do ESP32
# TYPE esp32_uptime gauge
esp32_uptime 23899
```

Primeira e segunda linhas são referentes a comentários envolvendo o nome da métrica, seu tipo e descrição.

Já a última linha, envolve o nome da métrica e seu devido valor.

Como podemos ver, o formato é muito simples e fácil de ser implementado manualmente conforme necessidade(exemplo do projeto).

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
String p = "";

String uptime = String(millis());

setMetric(&p, "esp32_uptime", uptime);
```

Agora que já temos uma noção dos tipos de métricas aplicáveis, quais métricas serão monitoradas e principalmente o formato que devemos seguir, colocaremos a mão na massa começando com o firmware da nosso hardware.

### Firmware

Baixe o [repositório](https://github.com/douglaszuqueto/esp32-prometheus/archive/master.zip) e faça o upload do firmware localizado na pasta **esp32-prometheus**.

*Obs:* Antes de fazer o upload não esqueça de configurar as credenciais da rede wifi como também o IP estático do esp de acordo com sua rede.

Depois de efetuar o upload e o firmware estiver rodando, você pode acessar o webserver através da seguinte url: http://IP_ESP:80/metrics.

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

O resultado acima nada mais é que o layout do prometheus já implementado, observe que cada métrica que será monitorada corresponde à um bloco de 3 linhas.

Agora que já temos o firmware rodando no esp32, podemos seguir em frente com a preparação da raspberry para hospedar o prometheus juntamente com o grafana.

## Instalação dos serviços

Antes de iniciarmos com a instalação propriamente dita, vamos criar uma pasta onde os serviços ficarão armazenados na raspberry.

*Obs:* Certifique-se que você está logado com o usuário padrão da raspberry - usuário **pi**.

```bash
# 1º
mkdir services

# 2º
cd services

# 3º => /home/pi/services
pwd
```

Pronto! Podemos proceder com a instalação do Grafana.

### Grafana

```bash
# 1º
wget https://dl.grafana.com/oss/release/grafana_6.6.2_armhf.deb

# 2º Efetuando a instalação
sudo dpkg -i grafana_6.6.2_armhf.deb

# 3º Habilitando o grafana para iniciar automaticamente ao iniciar a raspberry
sudo systemctl enable grafana-server 

# 4º Iniciando o serviço
sudo systemctl start grafana-server 

# 3º Visualizando o status
sudo systemctl status grafana-server 

```

Tudo ok?! Hora de instalar o prometheus, nosso último serviço. 

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

Todas instalações feitas. Caso queira validar tudo que foi feito, atente-se ao próximo tópico.

## Teste dos serviços instalados

Se tudo deu certo na instalação dos serviços, você poderá fazer os acessos conforme instruções abaixo:

* Grafana

http://IP_RASP:3000

User: admin

Pass: admin

* Prometheus

http://IP_RASP:9090

Rodou tudo certinho? Olha que beleza!

Agora já temos todos serviços essenciais rodando em nosso embarcado. Nos tópicos a seguir serão de suma importância, serão neles que toda a integração final ocorrerá.

## Configuração dos serviços

Com o Grafana e Prometheus instalados, seguiremos com o procedimento de configuração dos serviços.

### Prometheus

```bash
# 1º
cd /home/pi/services/prometheus

# 2º
cat prometheus.yml
```

Com o último comando acima, você deverá ver em seu terminal o arquivo de configuração abaixo:

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

É nesse arquivo que todas configurações são declaradas. Então temos que dizer para esse arquivo qual *URL* o prometheus deverá ir para requisitar as métricas que estão 'hospedadas' no esp32.

Portanto abre o arquivo *prometheus.yml* e adicione as seguintes configurações no final do arquivo para assim termos o esp32 integrado.

```yml
  # ESP32
  - job_name: 'esp32'
    scrape_interval: 5s
    static_configs:
    - targets: ['IP_ESP:80']
```

*Obs:* Tente respeitar a indentação do arquivo. A declaração acima deve estar alinhada com **- job_name:** que encontra-se logo acima.

Finalizando a configuração:

```bash
# 1º - Comando HTTP para recarregar o arquivo de configuração que foi editado no passo acima
curl -X POST 127.0.0.1:9090/-/reload
```

Caso queira validar que o Prometheus continua funcionando normalmente e coletando as métricas do esp32, navegue até a url: http://IP_RASP:9090/targets.

Se tiver sucesso, você terá uma tela como a da imagem abaixo mostrando um **"State UP"**.

<p align="center">
  <img src="https://github.com/douglaszuqueto/esp32-prometheus/raw/master/.github/prometheus-v1.png">
</p>

Agora temos parte do diagrama de fluxo implementado. O prometheus já está apto a recolher as métricas do esp32 dentre o intervalo de 5 segundos.

Com o prometheus já salvando os dados, resta adicionar a dashboard para começar o monitoramento do device.

### Grafana

Por default, o grafana vem 'zerado', e como queremos pegar os dados salvos do prometheus, é necessário a criação/configuração de um datasource.

#### Adicionando datasource

Navegue até o menu **Configuration** > **Datasources** > *Add data source*.

Na tela, basicamente você precisa configurar apenas a variável **URL** localizada na seção *HTTP* com o IP da sua Raspberry.

Ex: http://192.168.0.150:9090

<p align="center">
  <img src="https://github.com/douglaszuqueto/esp32-prometheus/raw/master/.github/grafana-datasource-v1.png">
</p>

Com o datasource criado, já podemos importar a dashboard que foi especialmente desenvolvida para este projeto.

#### Importando a dashboard

Copie o conteúdo do arquivo json localizado [aqui](https://github.com/douglaszuqueto/esp32-prometheus/blob/master/.github/dashboard-v1.json).

No grafana vá até a opção **Create(+)** > **Import**, cole o conteúdo copiado e clique em *load*. Na tela seguinte você pode alterar o nome da dashboard como também seu identificador - pode manter o mesmo em ambos!

<p align="center">
  <img src="https://github.com/douglaszuqueto/esp32-prometheus/raw/master/.github/grafana-import-v1.png">
</p>

Como foi visto, eu entreguei uma dashboard pronta para ser utilizada, ela é totalmente customizável. Como forma de estudo, recomendo que veja como cada painel foi configurado.

Para não se extender muito neste projeto, deixarei um bônus para que você possa ter um gostinho de como criar alguns painéis.

## Bônus

Como bônus, será demonstrado como você pode adicionar outros painéis com as devidas métricas oriundas do prometheus.

Passos básicos:

Com a dashboard aberta, clique em **Add Panel**, logo na sequência, **Add Query**.

Observe a tela na figura abaixo:

<p align="center">
  <img src="https://github.com/douglaszuqueto/esp32-prometheus/raw/master/.github/grafana-add-panel-v2.png">
</p>

Por padrão, na metade superior será apresentado um gráfico vazio e na parte inferior é onde você fará as configurações.

Na retângulo 1 basicamente temos 4 principais menus, sendo eles:

* Queries
* Visualization
* General
* Alert

Queries é o menu que fará o link com o datasource - no nosso caso se trata do prometheus.

Nesse menu, temos um campo(retângulo 3) onde iremos inserir a respectiva query de consulta ao prometheus. Ela retornará os dados para que os mesmos sejam utilizados no gráfico.

Exemplo:

```
esp32_temperature{instance="$instance"}
```

Basicamente estamos requisitando a temperatura do esp filtrando pela "instância", que é o ip do esp. Essa instância é genérica, ou seja, você pode alterar(select localizado no topo da dashboard) entre os devices que estão integrados ao prometheus.

Logo que a query é colocada no campo, o gráfico já é preenchido.

Veja abaixo como criar um *gauge* e uma *table*.

### Criando um *Gauge*

Baseando-se no exemplo acima, vamos navegar até o menu *Visualization* e trocar o gráfico por *Gauge*. E é isso, automaticamente o componente será mudado e preenchido com os valores, tudo isso devido ao datasource já estar configurado e com a query preenchida.

### Criando uma *Table*

Ainda com o exemplo anterior, apenas mude para o componente *Table*, percebe que o mesmo processo ocorre. Agora temos uma tabela de histórico das temperaturas :)

## Conclusão

Portanto, a partir do desenvolvimento desse projeto, foi possível montar um sistema simples e eficaz para monitoramento do ESP32 utilizando projetos de código aberto.

Dentre as inúmeras formas de realizar tal monitoramento, essa talvez, seja uma das formas mais simples. Pois se faz necessário apenas um pouco de conhecimento para encaixar as peças.

Foi obtido um resultado bem satisfatório, e com a dashboard pronta, você pode aproveitar para estudar como cada componente foi configurado, e assim, aprender a criar suas próprias dashboards de acordo com seu gosto e necessidades.

Fizemos um uso simples das tecnologias, porém, pode se avançar bastante, incluindo sistemas de alerta via email, telegram dentre outros tipos que já são embarcados nas tecnologias utilizadas.


## Referências

* [Firmware para ESP32 com suporte a integração com WebService e configuração via WiFiManager](https://github.com/douglaszuqueto/esp32-http-firmware)
* [Comunicação remota utilizando RPC e ESP32](https://github.com/douglaszuqueto/esp32-rpc)
* [ESP8266 Prometheus Exporter for DS18B20 OneWire Temperature Sensors](https://github.com/theoretick/esp8266_prometheus_exporter_for_ds18b20_temperature_sensors)
* [Example - Hello Server](https://github.com/espressif/arduino-esp32/blob/master/libraries/WebServer/examples/HelloServer/HelloServer.ino)
* [Example - WiFiClientStaticIP](https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WiFiClientStaticIP/WiFiClientStaticIP.ino)
* [ESP32 internal temperature](https://github.com/espressif/arduino-esp32/issues/2422)
