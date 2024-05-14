# Examen Parcial 3 de Diseño Electrónico

## Link al video de YouTube

[Video demosrtrativo](URL)

## Explicación

Este proyecto implementa una calculadora básica utilizando un microcontrolador STM32, con entrada mediante un teclado matricial y salida en una pantalla de 7 segmentos. Además, incluye la funcionalidad de comunicación USART para interacción adicional. El código es similar al mostrado en el Examen Parcial 2, sin embargo ahora se incluyen las siguientes funciones que permiten ingresar y visualizar las operaciones y resultados en una terminal serial.

### Función `process_console_input()`

```
void process_console_input(void) {
    uint32_t current_number = 0;  // Almacena el número que se está introduciendo
    uint8_t new_number_flag = 0;  // Indica si se ha completado la introducción de un nuevo número

    while(USART2->ISR & 0x0020) {  // Mientras haya datos en el registro de recepción
        uint8_t char_received = USART2->RDR;  // Leer el caracter recibido

        if (char_received >= '0' && char_received <= '9') {
            if (new_number_flag) {  // Si es un nuevo número, resetear el valor actual
                current_number = 0;
                new_number_flag = 0;
            }
            current_number = current_number * 10 + (char_received - '0');  // Acumular dígito
            decimal_decoder(char_received - '0');  // Agregar el dígito a los displays
        }
        else {
            break;
        }
    }
    update_display_values();  // Actualizar los displays después de procesar el input
}
```

#### Descripción
La función `process_console_input()` está diseñada para procesar entradas numéricas desde la consola a través de la interfaz USART2. Esta función es útil en aplicaciones donde se necesita capturar y procesar números ingresados por el usuario en tiempo real.

#### Detalles de Implementación
- **Variables:**
  - `current_number`: Almacena el número que se está introduciendo.
  - `new_number_flag`: Bandera que indica si se ha completado la introducción de un nuevo número.

### Función `USART2_write(uint8_t ch)`
```
void USART2_write(uint8_t ch) {

	while(!(USART2->ISR & 0x0080)){}
	USART2->TDR = ch;
}
```
#### Descripción
La función `USART2_write(uint8_t ch)` está diseñada para enviar un carácter a través de la interfaz USART2. Esta función maneja la transmisión de caracteres uno a uno, asegurándose de que el registro de transmisión esté listo antes de enviar un nuevo carácter.

