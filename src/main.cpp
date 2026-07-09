
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
        Serial.println("SII ESTA");
    }else{

      Serial.println("NO ESTA");
    }
}

void loop() {
  EthernetClient client = EthServer.available();
  if (client) {
    Serial.println("¡Nuevo cliente!");
    bool currentLineIsBlank = true;
    unsigned long timeout = millis();
    
    while (client.connected() && (millis() - timeout < 2000)) {
      if (client.available()) {
        char c = client.read();
        timeout = millis(); 
        
        if (c == '\n' && currentLineIsBlank) {
          
          // 3. Comprobar si el archivo HTML existe en la memoria
          if (LittleFS.exists("/index.html")) {
              File webFile = LittleFS.open("/index.html", "r");
              
              // Enviamos cabeceras HTTP limpias
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.printf("Content-Length: %d\n", webFile.size()); // Tamaño dinámico
              client.println("Connection: close");
              client.println();
              
              // Leemos el archivo en bloques y lo enviamos al cliente
              uint8_t buffer[64];
              while (webFile.available()) {
                  int bytesLeidos = webFile.read(buffer, sizeof(buffer));
                  client.write(buffer, bytesLeidos);
              }
              webFile.close();
          } else {
              // Si por algún motivo el archivo no está en la memoria
              client.println("HTTP/1.1 404 Not Found");
              client.println("Content-Type: text/plain");
              client.println();
              client.println("Error 404: Archivo index.html no encontrado en LittleFS.");
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

/*
#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):

class WeidosEthernetServer : public EthernetServer {
public:
    WeidosEthernetServer(uint16_t port) : EthernetServer(port) {}
    virtual void begin(uint16_t port) override {
        EthernetServer::begin(); 
    }
    virtual void begin() override {
        EthernetServer::begin(); 
    }
};

WeidosEthernetServer EthServer(80);

void setup() {
 
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Ethernet WebServer Example");

  // start the Ethernet connection and the server:
  Ethernet.init(ETHERNET_CS); 
  Ethernet.begin(mac, ip);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // start the server
  EthServer.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  EthernetClient client = EthServer.available();
  if (client) {
    Serial.println("¡Nuevo cliente!");
    bool currentLineIsBlank = true;
    
    // Almacenamos el tiempo actual para controlar el timeout
    unsigned long timeout = millis();
    
    // El bucle se mantendrá SOLO si el cliente está conectado Y no han pasado más de 2 segundos
    while (client.connected() && (millis() - timeout < 2000)) {
      if (client.available()) {
        char c = client.read();
        
        // Cada vez que el cliente nos envía un caracter, refrescamos el timeout
        timeout = millis(); 
        
        if (c == '\n' && currentLineIsBlank) {
          // [Aquí va todo tu bloque de enviar el HTTP y el HTML]
          
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();
          client.println("<!DOCTYPE HTML><html>Hola Mundo</html>");
          
          break; 
        }
        
        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    
    delay(15); 
    client.stop();
    Serial.println("Cliente desconectado de forma limpia.");
  }
}

*/