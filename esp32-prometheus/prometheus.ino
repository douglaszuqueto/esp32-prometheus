String getMetrics() {
  String p = "";

  int sketch_size = ESP.getSketchSize();
  int flash_size =  ESP.getFreeSketchSpace();
  int available_size = flash_size - sketch_size;
  float temperature = ((temprature_sens_read() - 32) / 1.8);

  setMetric(&p, "esp32_uptime", String(millis()));
  setMetric(&p, "esp32_wifi_rssi", String(WiFi.RSSI()));
  setMetric(&p, "esp32_heap_size", String(ESP.getHeapSize()));
  setMetric(&p, "esp32_free_heap", String(ESP.getFreeHeap()));
  setMetric(&p, "esp32_sketch_size", String(sketch_size));
  setMetric(&p, "esp32_flash_size", String(flash_size));
  setMetric(&p, "esp32_available_size", String(available_size));
  setMetric(&p, "esp32_temperature", String(temperature));
  setMetric(&p, "esp32_boot_counter", String(getBootCounter()));

  return p;
}

/**
   Layout

   # esp32_uptime
   # TYPE esp32_uptime gauge
   esp32_uptime 23899

*/
void setMetric(String *p, String metric, String value) {
  *p += "# " + metric + "\n";
  *p += "# TYPE " + metric + " gauge\n";
  *p += "" + metric + " ";
  *p += value;
  *p += "\n";
}

int getBootCounter() {
  return preferences.getInt("boot");
}

void incrementBootCounter() {
  int boot = getBootCounter();
  preferences.putInt("boot", (boot + 1));
}

void setupStorage() {
  preferences.begin("storage", false);
}

void closeStorage() {
  preferences.end();
}

