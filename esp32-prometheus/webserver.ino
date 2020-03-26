void startWebServer() {
  server.on("/", handleRoot);
  server.on("/metrics", handleMetrics);
  server.onNotFound(handleNotFound);
  server.begin();

  Serial.println("HTTP server started");
}

void handleRoot() {
  digitalWrite(LED_BUILTIN, 1);
  server.send(200, "text/plain", "hello from esp32!");
  digitalWrite(LED_BUILTIN, 0);
}

void handleMetrics() {
  digitalWrite(LED_BUILTIN, 1);
  server.send(200, "text/plain", getMetrics());
  digitalWrite(LED_BUILTIN, 0);
}

void handleNotFound() {
  digitalWrite(LED_BUILTIN, 1);
  server.send(404, "text/plain", "Not Found");
  digitalWrite(LED_BUILTIN, 0);
}
