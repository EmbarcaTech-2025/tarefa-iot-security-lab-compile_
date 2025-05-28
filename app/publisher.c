#include <stdio.h>
#include <string.h>
#include <time.h>
#include "include/wifi_conn.h"
#include "include/mqtt_comm.h"
#include "include/xor_cipher.h"
#include "pico/stdlib.h"

// Configurações Wi-Fi e MQTT
#define WIFI_SSID     "JOAO_2.4G"
#define WIFI_PASS     "30226280!"
#define MQTT_BROKER   "192.168.1.65"
#define MQTT_USER     "aluno"
#define MQTT_PASS     "1234"
#define MQTT_TOPIC    "escola/sala1/temperatura"
#define XOR_KEY       42

int main() {
    stdio_init_all();
    connect_to_wifi(WIFI_SSID, WIFI_PASS);
    mqtt_setup("bitdog1", MQTT_BROKER, MQTT_USER, MQTT_PASS);

    while (1) {
        float temperatura = 26.5;
        char buffer[64];
        sprintf(buffer, "{\"valor\":%.2f,\"ts\":%lu}", temperatura, (unsigned long)time(NULL));
        uint8_t criptografada[64];
        xor_encrypt((uint8_t*)buffer, criptografada, strlen(buffer), XOR_KEY);
        mqtt_comm_publish(MQTT_TOPIC, criptografada, strlen(buffer));
        printf("Mensagem criptografada publicada!\n");
        sleep_ms(5000); // envia a cada 5 segundos
    }
    return 0;
}