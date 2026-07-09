#include "MyWebServer.h"

// Constructor
MyWebServer::MyWebServer(uint16_t port, fs::FS &fileSystem) 
    : _server(port), _fs(&fileSystem), _ledEncendido(true) {}

// Inicialización
void MyWebServer::begin() {
    _server.begin();
    Serial.println("MyWebServer iniciado en el puerto 80.");
}

// Manejador de clientes (va en el loop)
void MyWebServer::handleClient() {
    EthernetClient client = _server.available();
    if (!client) return;

    Serial.println("¡Nuevo cliente conectado a la clase!");
    bool currentLineIsBlank = true;
    unsigned long timeout = millis();
    String HTTP_req = "";

    while (client.connected() && (millis() - timeout < 2000)) {
        if (client.available()) {
            char c = client.read();
            timeout = millis();

            if (HTTP_req.length() < 100) {
                HTTP_req += c;
            }

            if (c == '\n' && currentLineIsBlank) {
                Serial.print("Petición en clase: ");
                Serial.println(HTTP_req);
                
                // Procesamos la petición capturada
                procesarPeticion(client, HTTP_req);
                break;
            }

            if (c == '\n') { currentLineIsBlank = true; }
            else if (c != '\r') { currentLineIsBlank = false; }
        }
    }

    delay(15);
    client.stop();
    Serial.println("Cliente desconectado de la clase.");
}

// Procesador de rutas de la API y archivos estáticos
void MyWebServer::procesarPeticion(EthernetClient& client, String& HTTP_req) {
    
    // CASO 1: Controlar LED (ON)
    if (HTTP_req.indexOf("GET /api/led?status=1") >= 0) {
        _ledEncendido = true;
        Serial.println("Clase detectó: Hardware ENCENDIDO");

        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/plain");
        client.println("Connection: close");
        client.println();
        client.print("OK_ENCENDIDO");
    }
    // CASO 2: Controlar LED (OFF)
    else if (HTTP_req.indexOf("GET /api/led?status=0") >= 0) {
        _ledEncendido = false;
        Serial.println("Clase detectó: Hardware APAGADO");

        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/plain");
        client.println("Connection: close");
        client.println();
        client.print("OK_APAGADO");
    }
    // CASO 3: Solicitar Estado JSON
    else if (HTTP_req.indexOf("GET /api/estado") >= 0) {
        Serial.println("Clase enviando JSON de estado...");
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();
        
        if (_ledEncendido) {
            client.print("{\"status\":1}");
        } else {
            client.print("{\"status\":0}");
        }
    }
    // CASO 4: Archivo CSS
    else if (HTTP_req.indexOf("GET /styles.css") >= 0) {
        encriptarYEnviarArchivo(client, "/styles.css", "text/css");
    }
    // CASO 5: Archivo JS
    else if (HTTP_req.indexOf("GET /script.js") >= 0) {
        encriptarYEnviarArchivo(client, "/script.js", "application/javascript");
    }
    // CASO 6: Raíz o index.html
    else if (HTTP_req.indexOf("GET / HTTP") >= 0 || HTTP_req.indexOf("GET /index.html") >= 0) {
        encriptarYEnviarArchivo(client, "/index.html", "text/html", true);
    }
    // COMODÍN: 404 Not Found
    else {
        client.println("HTTP/1.1 404 Not Found");
        client.println("Content-Type: text/plain");
        client.println("Connection: close");
        client.println();
        client.println("Error 404: Ruta no encontrada en la clase.");
    }
}

// Función auxiliar para leer LittleFS y transmitir datos por bloques
void MyWebServer::encriptarYEnviarArchivo(EthernetClient& client, const char* path, const char* contentType, bool sendLength) {
    if (_fs->exists(path)) {
        File file = _fs->open(path, "r");
        
        client.println("HTTP/1.1 200 OK");
        client.print("Content-Type: ");
        client.println(contentType);
        if (sendLength) {
            client.printf("Content-Length: %d\n", file.size());
        }
        client.println("Connection: close");
        client.println();

        uint8_t buffer[64];
        while (file.available()) {
            int bytesLeidos = file.read(buffer, sizeof(buffer));
            client.write(buffer, bytesLeidos);
        }
        file.close();
    } else {
        client.println("HTTP/1.1 404 Not Found");
        client.println("Content-Type: text/plain");
        client.println("Connection: close");
        client.println();
        client.printf("Error 404: Archivo %s no existe.\n", path);
    }
}