#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <Preferences.h>

#define LED_BUILTIN 2

const char* ssid = ""; // Coloque o nome de sua rede WiFi
const char* password = ""; // Coloque a senha da sua rede WiFi

IPAddress ip(192, 168, 0, 170); // Fixe um IP de acordo com sua rede
IPAddress gw(192, 168, 0, 1); // Não esqueça de mudar o gateway caso a faixa de IP seja diferente
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);
Preferences preferences;

void setup(void) {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);
  Serial.begin(115200);

  setupStorage();
  incrementBootCounter();

  WiFi.mode(WIFI_STA);
  if (!WiFi.config(ip, gw, subnet)) {
    Serial.println("STA Failed to configure");
  }

  WiFi.begin(ssid, password);

  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  startWebServer();
}

void loop(void) {
  server.handleClient();
}

#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();
