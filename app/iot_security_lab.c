#include <stdio.h>
#include <string.h>
#include <time.h>
#include "include/wifi_conn.h"
#include "include/mqtt_comm.h"
#include "include/xor_cipher.h"
#include "pico/stdlib.h"
#include <time.h>

// Configurações Wi-Fi e MQTT
#define WIFI_SSID     "JOAO_2.4G"
#define WIFI_PASS     "30226280!"
#define MQTT_BROKER   "192.168.35.207"
#define MQTT_USER     "aluno"
#define MQTT_PASS     "senha123"
#define MQTT_TOPIC    "escola/sala1/temperatura"
#define XOR_KEY       42

// Variável global para controle de replay
uint32_t ultima_timestamp_recebida = 0;

// Callback para mensagens recebidas (subscriber)
void on_message(char* topic, char* msg, int len) {
    // Descriptografa a mensagem
    char decifrada[64] = {0};
    xor_encrypt((uint8_t*)msg, (uint8_t*)decifrada, len, XOR_KEY);

    // Extrai valor e timestamp
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
    // Etapa 1: Conectar ao Wi-Fi
    connect_to_wifi(WIFI_SSID, WIFI_PASS);

    // Etapa 2: Configurar MQTT com autenticação
    mqtt_setup("bitdog1", MQTT_BROKER, MQTT_USER, MQTT_PASS);

    // Etapa 3, 5 e 6: Publicar mensagem criptografada com timestamp
    float temperatura = 26.5;
    char buffer[64];
    sprintf(buffer, "{\"valor\":%.2f,\"ts\":%lu}", temperatura, (unsigned long)time(NULL));

    // Criptografa a mensagem
    uint8_t criptografada[64];
    xor_encrypt((uint8_t*)buffer, criptografada, strlen(buffer), XOR_KEY);

    // Publica a mensagem criptografada
    mqtt_comm_publish(MQTT_TOPIC, criptografada, strlen(buffer));

    // (Opcional) Subscreve para receber mensagens e testar replay
    // mqtt_comm_subscribe(MQTT_TOPIC, on_message);

    // Aguarda para manter o programa rodando (ajuste conforme necessário)
    while (1) {
        sleep_ms(5);
    }

    return 0;
}