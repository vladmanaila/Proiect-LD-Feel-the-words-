#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char *ssid = "TP-Link_24E8";
const char *password = "22923929";

WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", R"rawliteral(
    <html>
      <head>
        <title>File Upload</title>
        <style>
          body {
            background-color: #f0f0f0;
            font-family: Arial, Helvetica, sans-serif;
            color: #333;
            padding: 40px;
          }
        </style>
      </head>
      <body>
        <h1>Select a file to upload</h1>
        <form method="POST" action="/upload" enctype="multipart/form-data">
          <input type="file" name="file"><br><br>
          <input type="submit" value="Upload">
        </form>
      </body>
    </html>
  )rawliteral");
}

// Asta e doar un placeholder, ESP32 clasic nu poate salva fișiere fără SPIFFS/LittleFS
void handleUpload() {
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    Serial.printf("Upload started: %s\n", upload.filename.c_str());
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    Serial.printf("Receiving %u bytes\n", upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    Serial.printf("Upload finished. Total: %u bytes\n", upload.totalSize);
    server.send(200, "text/plain", "File uploaded!");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/upload", HTTP_POST, []() {
    server.send(200);
  }, handleUpload);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
