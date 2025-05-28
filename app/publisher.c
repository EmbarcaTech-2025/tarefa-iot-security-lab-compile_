#include <stdio.h>
#include <string.h>
#include <time.h>
#include "include/wifi_conn.h"
#include "include/mqtt_comm.h"
#include "include/xor_cipher.h"
#include "pico/stdlib.h"

#define WIFI_SSID     "NOME_DA_SUA_REDE_WIFI"
#define WIFI_PASS     "SENHA_DA_SUA_REDE_WIFI"
#define MQTT_BROKER   "192.168.1.100"   // IP do seu computador
#define MQTT_USER     "aluno"
#define MQTT_PASS     "senha123"
#define MQTT_TOPIC    "escola/sala1/temperatura"
#define XOR_KEY       42

int main() {
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