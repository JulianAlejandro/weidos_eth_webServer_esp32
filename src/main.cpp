
#include <WiFi.h>
#include <LittleFS.h>
#include "MyWebServer.h"

const char* WIFI_SSID = "Juls";
const char* WIFI_PASS = "tempura777";

MyWebServer serverManager(80, LittleFS);

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }
  Serial.println("Servidor Web Wi-Fi HTTP (Sin cifrar)");

  if (!LittleFS.begin(true)) {
    Serial.println("¡Error crítico en LittleFS!");
  } else {
    Serial.println("LittleFS montado correctamente.");
  }

  // Conexión a la red Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Conectando a Wi-Fi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n¡Wi-Fi Conectado!");

  // Iniciamos el servidor
  serverManager.begin();
  Serial.print("Servidor HTTP listo en: http://");
  Serial.println(WiFi.localIP());
}

void loop() {
  serverManager.handleClient();
}


/*

#include <SPI.h>
#include <Ethernet.h>
#include <LittleFS.h>
#include "MyWebServer.h" // <-- Incluimos tu nueva clase

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);

// Instanciamos tu clase pasando el puerto 80 y pasándole LittleFS directamente
MyWebServer serverManager(80, LittleFS);

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }
  Serial.println("Servidor Modular con Clase Prototipo");

  if (!LittleFS.begin(true)) {
    Serial.println("¡Error crítico en LittleFS!");
  } else {
    Serial.println("LittleFS montado correctamente.");
  }

  Ethernet.init(ETHERNET_CS); 
  Ethernet.begin(mac, ip);

  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield no encontrada.");
    while (true) { delay(1); }
  }

  // Iniciamos el servidor a través de la clase
  serverManager.begin();
  Serial.print("Servidor listo en: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  // Dejamos que la clase se encargue de todo el trabajo sucio de la red
  serverManager.handleClient();
  
  // Si en el futuro necesitas interactuar con el estado del LED en tu Arduino, puedes hacer:
  // if (serverManager.getLedStatus() == true) { 
  //     digitalWrite(PIN_FISCO, HIGH); 
  // }
}



*/



//---------------------------------CODIGO PARA EL OBTENEDOR DE PARAMETROS----------------------------
/*
#include <SPI.h>
#include <Ethernet.h>
#include <LittleFS.h>
#include "MyWebServer.h" // <-- Incluimos tu nueva clase

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);

// Instanciamos tu clase pasando el puerto 80 y pasándole LittleFS directamente
MyWebServer serverManager(80, LittleFS);

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }
  Serial.println("Servidor Modular con Clase Prototipo");

  if (!LittleFS.begin(true)) {
    Serial.println("¡Error crítico en LittleFS!");
  } else {
    Serial.println("LittleFS montado correctamente.");
  }

  Ethernet.init(ETHERNET_CS); 
  Ethernet.begin(mac, ip);

  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield no encontrada.");
    while (true) { delay(1); }
  }

  // Iniciamos el servidor a través de la clase
  serverManager.begin();
  Serial.print("Servidor listo en: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  // Dejamos que la clase se encargue de todo el trabajo sucio de la red
  serverManager.handleClient();

  int tiempoEsperaModbus = serverManager.getResponseTimeout(); 
  long velocidadBaudios  = serverManager.getBaudRate();

  
  
  // Si en el futuro necesitas interactuar con el estado del LED en tu Arduino, puedes hacer:
  // if (serverManager.getLedStatus() == true) { 
  //     digitalWrite(PIN_FISCO, HIGH); 
  // }
}

*/

/*
#include <SPI.h>
#include <Ethernet.h>
#include <LittleFS.h> // <-- 1. IMPORTANTE: Incluir la librería

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);

class WeidosEthernetServer : public EthernetServer {
public:
    WeidosEthernetServer(uint16_t port) : EthernetServer(port) {}
    virtual void begin(uint16_t port) override { EthernetServer::begin(); }
    virtual void begin() override { EthernetServer::begin(); }
};

WeidosEthernetServer EthServer(80);

bool ledEncendido = true;

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }
  Serial.println("Ethernet WebServer + LittleFS Example");

  // 2. Inicializar el sistema de archivos LittleFS
 if (!LittleFS.begin(true)) {
    Serial.println("¡Error crítico en LittleFS!");
  } else {
    Serial.println("LittleFS iniciado/formateado con éxito.");
  }
  Serial.println("LittleFS montado correctamente.");

  Ethernet.init(ETHERNET_CS); 
  Ethernet.begin(mac, ip);

  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield no encontrada.");
    while (true) { delay(1); }
  }

  EthServer.begin();
  Serial.print("Servidor en: ");
  Serial.println(Ethernet.localIP());
    if (LittleFS.exists("/index.html")) {
        Serial.println("se encontro el fichero");
    }else{
        Serial.println("No se encontro el fichero");
    }
}

void loop() {
  EthernetClient client = EthServer.available();
  if (client) {
    Serial.println("¡Nuevo cliente!");
    bool currentLineIsBlank = true;
    unsigned long timeout = millis();
    
    String HTTP_req = ""; // <-- NUEVO: Aquí guardaremos la primera línea de la petición

    while (client.connected() && (millis() - timeout < 2000)) {
      if (client.available()) {
        char c = client.read();
        timeout = millis(); 
        
        // Guardamos los caracteres de la primera línea (hasta encontrar el salto de línea)
        if (HTTP_req.length() < 100) {
            HTTP_req += c;
        }
        
        if (c == '\n' && currentLineIsBlank) {
          
          // Imprimimos en el Monitor Serie lo que el navegador nos pidió exactamente
          Serial.print("Petición recibida: ");
          Serial.println(HTTP_req);

          // ------------------------------------------------------------------
          // CASO 1: El JavaScript movió el interruptor a ON (status=1)
          // ------------------------------------------------------------------
          if (HTTP_req.indexOf("GET /api/led?status=1") >= 0) {
            ledEncendido = true;
            // 1. Aquí enciendes el hardware real: digitalWrite(PIN_BOMBILLA, HIGH);
            Serial.println("Bombilla real ENCENDIDA");

            // 2. Le respondes al JavaScript confirmando el éxito
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/plain");
            client.println("Connection: close");
            client.println();
            client.print("OK_ENCENDIDO"); // <-- El texto que espera el JavaScript
          }
          else if (HTTP_req.indexOf("GET /api/led?status=0") >= 0) {
            ledEncendido = false;
            Serial.println("Bombilla real APAGADA");

            // 2. Le respondes al JavaScript
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/plain");
            client.println("Connection: close");
            client.println();
            client.print("OK_APAGADO"); // <-- El texto que espera el JavaScript
          }
          else if(HTTP_req.indexOf("GET /api/estado") >= 0){

            Serial.println("Enviando JSON de estado actual...");

            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: application/json"); // <-- Cambiamos el tipo a JSON
            client.println("Connection: close");
            client.println();
    
            // Construimos el JSON dinámicamente según nuestra variable de la RAM
            if (ledEncendido) {
              client.print("{\"status\":1}");
            } else {
              client.print("{\"status\":0}");
            }
          }else if (HTTP_req.indexOf("GET /styles.css") >= 0) {
            if (LittleFS.exists("/styles.css")) {
                File cssFile = LittleFS.open("/styles.css", "r");
                client.println("HTTP/1.1 200 OK");
                client.println("Content-Type: text/css"); // <-- CLAVE: Le decimos al navegador que esto es CSS
                client.println("Connection: close");
                client.println();
        
                uint8_t buffer[64];
                while (cssFile.available()) {
                    int bytes = cssFile.read(buffer, sizeof(buffer));
                    client.write(buffer, bytes);
                }
                cssFile.close();
              }
          }else if (HTTP_req.indexOf("GET /script.js") >= 0) {
            if (LittleFS.exists("/script.js")) {
                File jsFile = LittleFS.open("/script.js", "r");
                client.println("HTTP/1.1 200 OK");
                client.println("Content-Type: application/javascript"); // <-- CLAVE: Esto es JS
                client.println("Connection: close");
                client.println();
        
                uint8_t buffer[64];
                while (jsFile.available()) {
                    int bytes = jsFile.read(buffer, sizeof(buffer));
                    client.write(buffer, bytes);
                }
                jsFile.close();
            }
          }
          // ------------------------------------------------------------------
          // CASO 3: Es una petición normal de la página web (Entrar a la IP)
          // ------------------------------------------------------------------
          else if (HTTP_req.indexOf("GET / ") >= 0 || HTTP_req.indexOf("GET /index.html") >= 0) {
              if (LittleFS.exists("/index.html")) {
                  File webFile = LittleFS.open("/index.html", "r");
                  
                  client.println("HTTP/1.1 200 OK");
                  client.println("Content-Type: text/html");
                  client.printf("Content-Length: %d\n", webFile.size());
                  client.println("Connection: close");
                  client.println();
                  
                  uint8_t buffer[64];
                  while (webFile.available()) {
                      int bytesLeidos = webFile.read(buffer, sizeof(buffer));
                      client.write(buffer, bytesLeidos);
                  }
                  webFile.close();
              } else {
                  client.println("HTTP/1.1 404 Not Found");
                  client.println("Content-Type: text/plain");
                  client.println();
                  client.println("Error 404: Archivo index.html no encontrado.");
              }
          }else {
              // COMODÍN: Cualquier otra cosa que no conozcamos (como el favicon.ico) se lleva un 404 directo sin leer LittleFS
              client.println("HTTP/1.1 404 Not Found");
              client.println("Content-Type: text/plain");
              client.println("Connection: close");
              client.println();
              client.println("Error 404: Ruta no encontrada.");
          }
          break; 
        }
        
        if (c == '\n') { currentLineIsBlank = true; } 
        else if (c != '\r') { currentLineIsBlank = false; }
      }
    }
    
    delay(15); 
    client.stop();
    Serial.println("Cliente desconectado.");
  }
}

*/
