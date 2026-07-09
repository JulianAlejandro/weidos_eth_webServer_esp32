#ifndef MY_WEBSERVER_H
#define MY_WEBSERVER_H

#include <Arduino.h>
#include <Ethernet.h>
#include <FS.h>       // Necesario para interactuar con la clase base de LittleFS
#include <LittleFS.h>

class WeidosEthernetServer : public EthernetServer {
public:
    WeidosEthernetServer(uint16_t port) : EthernetServer(port) {}
    virtual void begin(uint16_t port) override { EthernetServer::begin(); }
    virtual void begin() override { EthernetServer::begin(); }
};

class MyWebServer {
private:
    WeidosEthernetServer _server;
    fs::FS* _fs;             // Puntero al sistema de archivos (LittleFS)
    //LittleFSFS* _fs ?¿?¿?¿
    bool _ledEncendido;      // Variable interna para guardar el estado del LED
    
    // Métodos privados internos para procesar y responder
    void encriptarYEnviarArchivo(EthernetClient& client, const char* path, const char* contentType, bool sendLength = false);
    void procesarPeticion(EthernetClient& client, String& HTTP_req);

public:
    // Constructor: Recibe el puerto del servidor y el sistema de archivos
    MyWebServer(uint16_t port, fs::FS &fileSystem);
    
    // Inicializa el servidor Ethernet
    void begin();
    
    // Método principal que debe llamarse continuamente en el loop() de Arduino
    void handleClient();
    
    // Métodos Get/Set por si el resto del programa necesita saber o cambiar el estado
    bool getLedStatus() const { return _ledEncendido; }
    void setLedStatus(bool status) { _ledEncendido = status; }
};

#endif