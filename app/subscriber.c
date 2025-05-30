#include <stdio.h>
#include <string.h>
#include <stdint.h>
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

uint32_t ultima_timestamp_recebida = 0;

void on_message(char* topic, char* msg, int len) {
    char decifrada[64] = {0};
    xor_encrypt((uint8_t*)msg, (uint8_t*)decifrada, len, XOR_KEY);
    float valor;
    uint32_t nova_timestamp;
    if (sscanf(decifrada, "{\"valor\":%f,\"ts\":%lu}", &valor, &nova_timestamp) != 2) {
        printf("Erro no parse da mensagem!\n");
        return;
    }
    if (nova_timestamp > ultima_timestamp_recebida) {
        ultima_timestamp_recebida = nova_timestamp;
        printf("Nova leitura: %.2f (ts: %lu)\n", valor, nova_timestamp);
    } else {
        printf("Replay detectado (ts: %lu <= %lu)\n", nova_timestamp, ultima_timestamp_recebida);
    }
}

int main() {
    stdio_init_all();
    connect_to_wifi(WIFI_SSID, WIFI_PASS);
    mqtt_setup("bitdog2", MQTT_BROKER, MQTT_USER, MQTT_PASS);
    mqtt_comm_subscribe(MQTT_TOPIC, on_message);

    while (1) {
        sleep_ms(5000);
    }
    return 0;
}