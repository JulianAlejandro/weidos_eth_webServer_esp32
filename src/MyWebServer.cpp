#include "MyWebServer.h"

MyWebServer::MyWebServer(uint16_t port, fs::FS &fileSystem) 
    : _server(port), _fs(&fileSystem), _ledEncendido(true) {}

void MyWebServer::begin() {
    _server.begin();
    Serial.println("MyWebServer iniciado por Wi-Fi en el puerto 80.");
}

void MyWebServer::handleClient() {
    WiFiClient client = _server.available(); // <-- WiFiClient
    if (!client) return;

    Serial.println("¡Nuevo cliente Wi-Fi conectado!");
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
                Serial.print("Petición: ");
                Serial.println(HTTP_req);
                procesarPeticion(client, HTTP_req);
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

void MyWebServer::procesarPeticion(WiFiClient& client, String& HTTP_req) {
    // El resto de esta función queda EXACTAMENTE IGUAL que en tu código original
    if (HTTP_req.indexOf("GET /api/led?status=1") >= 0) {
        _ledEncendido = true;
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/plain");
        client.println("Connection: close");
        client.println();
        client.print("OK_ENCENDIDO");
    }
    else if (HTTP_req.indexOf("GET /api/led?status=0") >= 0) {
        _ledEncendido = false;
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/plain");
        client.println("Connection: close");
        client.println();
        client.print("OK_APAGADO");
    }
    else if (HTTP_req.indexOf("GET /api/estado") >= 0) {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();
        client.print(_ledEncendido ? "{\"status\":1}" : "{\"status\":0}");
    }
    else if (HTTP_req.indexOf("GET /styles.css") >= 0) {
        encriptarYEnviarArchivo(client, "/styles.css", "text/css");
    }
    else if (HTTP_req.indexOf("GET /script.js") >= 0) {
        encriptarYEnviarArchivo(client, "/script.js", "application/javascript");
    }
    else if (HTTP_req.indexOf("GET / HTTP") >= 0 || HTTP_req.indexOf("GET /index.html") >= 0) {
        encriptarYEnviarArchivo(client, "/index.html", "text/html", true);
    }
    else {
        client.println("HTTP/1.1 404 Not Found");
        client.println("Content-Type: text/plain");
        client.println("Connection: close");
        client.println();
        client.println("Error 404: Ruta no encontrada.");
    }
}

void MyWebServer::encriptarYEnviarArchivo(WiFiClient& client, const char* path, const char* contentType, bool sendLength) {
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

/*
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

*/

// -----------------------------Código para el obtenendor de parametros--------------
/*
#include "MyWebServer.h"

// Constructor con valores por defecto iniciales (los mismos de tu interfaz visual)
MyWebServer::MyWebServer(uint16_t port, fs::FS &fileSystem) 
    : _server(port), _fs(&fileSystem),
      _macAddress("58:96:68:BD:9E:7D"), _staticIp("192.168.1.254"), 
      _submask("255.255.255.0"), _gateway("192.168.1.1"),
      _baudRate(9600), _dataBits(8), _parity("none"), _stopBits(1),
      _interFrameDelay(10), _responseTimeout(1000), _attempts(3) {}

// Inicialización
void MyWebServer::begin() {
    _server.begin();
    Serial.println("Weidmüller WebServer operativo en puerto 80.");
}

// Manejador del cliente Modbus WebUI
void MyWebServer::handleClient() {
    EthernetClient client = _server.available();
    if (!client) return;

    bool currentLineIsBlank = true;
    unsigned long timeout = millis();
    String HTTP_req = "";
    String postBody = "";
    bool leyendoCuerpoPost = false;
    int contentLength = 0;

    while (client.connected() && (millis() - timeout < 2000)) {
        if (client.available()) {
            char c = client.read();
            timeout = millis();

            // 1. Leer encabezados HTTP
            if (!leyendoCuerpoPost) {
                if (HTTP_req.length() < 150) {
                    HTTP_req += c;
                }

                // Capturar longitud del contenido si es un POST
                if (HTTP_req.endsWith("Content-Length: ")) {
                    contentLength = client.parseInt();
                }

                if (c == '\n' && currentLineIsBlank) {
                    // Si es un POST interceptamos el cuerpo, si es GET procesamos directo
                    if (HTTP_req.indexOf("POST") >= 0 && contentLength > 0) {
                        leyendoCuerpoPost = true;
                    } else {
                        procesarPeticion(client, HTTP_req);
                        break;
                    }
                }

                if (c == '\n') { currentLineIsBlank = true; }
                else if (c != '\r') { currentLineIsBlank = false; }
            } 
            // 2. Si es POST, leer el cuerpo (los datos enviados del formulario)
            else {
                postBody += c;
                if ((int)postBody.length() >= contentLength) {
                    Serial.println("Datos POST recibidos crudos: " + postBody);
                    parsearConfiguracionPost(postBody); // Procesar los datos
                    procesarPeticion(client, HTTP_req); // Responder al cliente
                    break;
                }
            }
        }
    }

    delay(15);
    client.stop();
}

// Procesador de rutas para index.html, configuration.html y el POST de guardado
void MyWebServer::procesarPeticion(EthernetClient& client, String& HTTP_req) {
    
    // Si viene de un procesamiento POST exitoso, redirigimos o recargamos la página con confirmación
    if (HTTP_req.indexOf("POST /configuration.html") >= 0 || HTTP_req.indexOf("POST / ") >= 0) {
        client.println("HTTP/1.1 303 See Other"); // Redirección limpia tras guardar formulario
        client.println("Location: /configuration.html");
        client.println("Connection: close");
        client.println();
        return;
    }

    // CASO 1: Pestaña System Information (Raíz o index.html)
    if (HTTP_req.indexOf("GET / ") >= 0 || HTTP_req.indexOf("GET /index.html") >= 0) {
        encriptarYEnviarArchivo(client, "/index.html", "text/html", true);
    }
    // CASO 2: Pestaña System Configuration
    else if (HTTP_req.indexOf("GET /configuration.html") >= 0) {
        encriptarYEnviarArchivo(client, "/configuration.html", "text/html", true);
    }
    // COMODÍN: 404
    else {
        client.println("HTTP/1.1 404 Not Found");
        client.println("Content-Type: text/plain");
        client.println("Connection: close");
        client.println();
        client.println("Error 404: Recurso de Gateway no encontrado.");
    }
}

// Función auxiliar para extraer variables del formato x-www-form-urlencoded del HTML
String extraerValorParametro(String data, String llave) {
    llave += "=";
    int posLlave = data.indexOf(llave);
    if (posLlave == -1) return "";
    
    int posInicio = posLlave + llave.length();
    int posFin = data.indexOf('&', posInicio);
    if (posFin == -1) posFin = data.length();
    
    String valor = data.substring(posInicio, posFin);
    // Reemplazo simple de caracteres codificados de URL comunes (%3A es ':', etc)
    valor.replace("%3A", ":");
    valor.replace("+", " ");
    return valor;
}

// Procesador interno de variables del formulario
void MyWebServer::parsearConfiguracionPost(String& postData) {
    Serial.println("--- Actualizando parámetros de pasarela Modbus ---");
    
    // IP Settings
    _macAddress = extraerValorParametro(postData, "mac_address");
    _staticIp   = extraerValorParametro(postData, "static_ip");
    _submask    = extraerValorParametro(postData, "submask");
    _gateway    = extraerValorParametro(postData, "gateway");

    // RTU Settings
    _baudRate         = extraerValorParametro(postData, "baud_rate").toInt();
    _dataBits         = extraerValorParametro(postData, "data_bits").toInt();
    _parity           = extraerValorParametro(postData, "parity");
    _stopBits         = extraerValorParametro(postData, "stop_bits").toInt();
    _interFrameDelay  = extraerValorParametro(postData, "inter_frame_delay").toInt();
    _responseTimeout  = extraerValorParametro(postData, "response_timeout").toInt();
    _attempts         = extraerValorParametro(postData, "attempts").toInt();

    // Verificación por consola serie de que los datos cambiaron en caliente
    Serial.println("Nueva IP Destino: " + _staticIp);
    Serial.print("Nuevo Baud Rate configurado: "); Serial.println(_baudRate);
}

// Transmisor de bloques desde LittleFS
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
        client.printf("Error LittleFS: %s no hallado.\n", path);
    }
}

*/
