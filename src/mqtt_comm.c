#include "include/mqtt_comm.h"
#include "lwip/apps/mqtt.h"
#include "lwip/ip_addr.h"
#include <stdio.h>
#include <string.h>

static mqtt_client_t *client = NULL;
static void (*user_on_message)(char*, char*, int) = NULL;

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    if (status == MQTT_CONNECT_ACCEPTED) {
        printf("Conectado ao broker MQTT com sucesso!\n");
    } else {
        printf("Falha ao conectar ao broker, código: %d\n", status);
    }
}

static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len) {
    // Apenas notificação de novo publish, sem payload
}

static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags) {
    if (user_on_message) {
        user_on_message("topico", (char*)data, len);
    }
}

void mqtt_setup(const char *client_id, const char *broker_ip, const char *user, const char *pass) {
    ip_addr_t broker_addr;
    if (!ip4addr_aton(broker_ip, &broker_addr)) {
        printf("Erro no IP\n");
        return;
    }
    client = mqtt_client_new();
    if (client == NULL) {
        printf("Falha ao criar o cliente MQTT\n");
        return;
    }
    struct mqtt_connect_client_info_t ci = {
        .client_id = client_id,
        .client_user = user,
        .client_pass = pass
    };
    mqtt_client_connect(client, &broker_addr, 1883, mqtt_connection_cb, NULL, &ci);
}

void mqtt_comm_publish(const char *topic, const void *data, int len) {
    if (client) {
        mqtt_publish(client, topic, data, len, 0, 0, NULL, NULL);
    }
}

void mqtt_comm_subscribe(const char *topic, void (*on_message)(char*, char*, int)) {
    user_on_message = on_message;
    if (client) {
        mqtt_subscribe(client, topic, 0, NULL, NULL);
        mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, NULL);
    }
}