
const miInterruptor = document.getElementById('interruptor');
const miBombilla = document.getElementById('bombilla');

// Volvemos a 'change' que es el evento nativo y correcto para los interruptores
miInterruptor.addEventListener('change', function() {
    
    // Guardamos el estado en el que acaba de quedar el interruptor tras el clic
    let estadoActual = miInterruptor.checked; 
    let valorEnviar = estadoActual ? "1" : "0";

    console.log("Enviando petición al ESP32... Estado solicitado: " + valorEnviar);

    // Enviamos la petición al ESP32
    fetch('/api/led?status=' + valorEnviar)
    .then(response => {
        if (!response.ok) throw new Error("Error de red");
        return response.text();
    })
    .then(textoDelEsp32 => {
        
        // Evaluamos lo que el ESP32 nos respondió de verdad
        if (textoDelEsp32.includes("OK_ENCENDIDO")) {
            console.log("El ESP32 confirma: Encendido.");
            miInterruptor.checked = true; // Aseguramos que esté en ON
            miBombilla.classList.add('bombilla-encendida'); // Encendemos la bombilla
        } 
        else if (textoDelEsp32.includes("OK_APAGADO")) {
            console.log("El ESP32 confirma: Apagado.");
            miInterruptor.checked = false; // Aseguramos que esté en OFF
            miBombilla.classList.remove('bombilla-encendida'); // Apagamos la bombilla
        } 
        else {
            // Si el ESP32 responde algo inesperado, deshacemos el cambio del usuario
            console.warn("Respuesta inesperada, revirtiendo interruptor.");
            miInterruptor.checked = !estadoActual;
            miBombilla.classList.remove('bombilla-encendida');
        }
    })
    .catch(error => {
        console.error("Error al conectar con el hardware:", error);
        alert("El hardware no respondió. Revirtiendo control.");
        // Si falla la red, regresamos el interruptor a donde estaba para no mentir al usuario
        miInterruptor.checked = !estadoActual;
        if (miInterruptor.checked) {
            miBombilla.classList.add('bombilla-encendida');
        } else {
            miBombilla.classList.remove('bombilla-encendida');
        }
    });
});

    function sincronizarEstadoInicial() {
        console.log("Cargando estado inicial desde el ESP32...");
    
        fetch('/api/estado')
        .then(response => {
            if (!response.ok) throw new Error("No se pudo obtener el estado inicial");
            return response.json(); // <-- Convertimos la respuesta directamente en un objeto JSON
        })
        .then(objetoJson => {
            console.log("Estado inicial recibido:", objetoJson);
        
            // Si el JSON dice status: 1, encendemos todo visualmente
            if (objetoJson.status === 1) {
                miInterruptor.checked = true;
                miBombilla.classList.add('bombilla-encendida');
            } else {
                miInterruptor.checked = false;
                miBombilla.classList.remove('bombilla-encendida');
            }
        })
        .catch(error => {
            console.error("Error al sincronizar al inicio:", error);
        });
    }

    sincronizarEstadoInicial();
    