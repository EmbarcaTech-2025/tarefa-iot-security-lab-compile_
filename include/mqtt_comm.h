#ifndef MQTT_COMM_H
#define MQTT_COMM_H

void mqtt_setup(const char *client_id, const char *broker_ip, const char *user, const char *pass);
void mqtt_comm_publish(const char *topic, const void *data, int len);
void mqtt_comm_subscribe(const char *topic, void (*on_message)(char*, char*, int));

#endif