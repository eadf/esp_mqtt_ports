/* main.c -- MQTT client example
 *
 * Copyright (c) 2014-2015, Tuan PM <tuanpm at live dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * * Neither the name of Redis nor the names of its contributors may be used
 * to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "ets_sys.h"
#include "osapi.h"
#include "mqtt.h"
#include "wifi.h"
#include "config.h"
#include "debug.h"
#include "gpio.h"
#include "user_interface.h"
#include "mem.h"
#include "stdout/stdout.h"

MQTT_Client mqttClient;

static volatile os_timer_t setup_timer;
#define user_procTaskPrio        0
#define user_procTaskQueueLen    1
os_event_t user_procTaskQueue[user_procTaskQueueLen];

static void setup(void);

void ICACHE_FLASH_ATTR
wifiConnectCb(uint8_t status) {

  if (status == STATION_GOT_IP) {
    MQTT_Connect(&mqttClient);
  } else {
    MQTT_Disconnect(&mqttClient);
  }
}

void ICACHE_FLASH_ATTR
mqttConnectedCb(uint32_t *args) {

  MQTT_Client* client = (MQTT_Client*) args;
  INFO("MQTT: Connected\n");
  MQTT_Subscribe(client, "/mqtt/topic/0", 0);
  MQTT_Subscribe(client, "/mqtt/topic/1", 1);
  MQTT_Subscribe(client, "/mqtt/topic/2", 2);

  MQTT_Publish(client, "/mqtt/topic/0", "hello0", 6, 0, 0);
  MQTT_Publish(client, "/mqtt/topic/1", "hello1", 6, 1, 0);
  MQTT_Publish(client, "/mqtt/topic/2", "hello2", 6, 2, 0);

}

void ICACHE_FLASH_ATTR
mqttDisconnectedCb(uint32_t *args) {

  MQTT_Client* client = (MQTT_Client*) args;
  INFO("MQTT: Disconnected\n");
}

void ICACHE_FLASH_ATTR
mqttPublishedCb(uint32_t *args) {

  MQTT_Client* client = (MQTT_Client*) args;
  INFO("MQTT: Published\n");
}

void ICACHE_FLASH_ATTR
mqttDataCb(uint32_t *args, const char* topic, uint32_t topic_len, const char *data, uint32_t data_len) {

  char *topicBuf = (char*) os_zalloc(topic_len + 1), *dataBuf =
      (char*) os_zalloc(data_len + 1);

  MQTT_Client* client = (MQTT_Client*) args;

  os_memcpy(topicBuf, topic, topic_len);
  topicBuf[topic_len] = 0;

  os_memcpy(dataBuf, data, data_len);
  dataBuf[data_len] = 0;

  INFO("Receive topic: %s, data: %s \n", topicBuf, dataBuf);
  os_free(topicBuf);
  os_free(dataBuf);
}

//Do nothing function
static void ICACHE_FLASH_ATTR
user_procTask(os_event_t *events) {
  os_delay_us(10);
}

void ICACHE_FLASH_ATTR
setup() {
  CFG_Load();

  MQTT_InitConnection(&mqttClient, sysCfg.mqtt_host, sysCfg.mqtt_port,
      sysCfg.security);
  //MQTT_InitConnection(&mqttClient, "192.168.11.122", 1880, 0);

  MQTT_InitClient(&mqttClient, sysCfg.device_id, sysCfg.mqtt_user,
      sysCfg.mqtt_pass, sysCfg.mqtt_keepalive, 1);
  //MQTT_InitClient(&mqttClient, "client_id", "user", "pass", 120, 1);

  MQTT_InitLWT(&mqttClient, "/lwt", "offline", 0, 0);
  MQTT_OnConnected(&mqttClient, mqttConnectedCb);
  MQTT_OnDisconnected(&mqttClient, mqttDisconnectedCb);
  MQTT_OnPublished(&mqttClient, mqttPublishedCb);
  MQTT_OnData(&mqttClient, mqttDataCb);

  WIFI_Connect(sysCfg.sta_ssid, sysCfg.sta_pwd, wifiConnectCb);
}

void user_init(void) {
  // Initialize the GPIO subsystem.
  gpio_init();

  // Make uart0 work with just the TX pin. Baud:115200,n,8,1
  // The RX pin is now free for GPIO use.
  stdout_init();

  //Set the setup timer
  os_timer_disarm(&setup_timer);
  os_timer_setfn(&setup_timer, (os_timer_func_t *) setup, NULL);
  os_timer_arm(&setup_timer, 1000, false);

  //Start os task
  system_os_task(user_procTask, user_procTaskPrio, user_procTaskQueue,
      user_procTaskQueueLen);

  INFO("\nSystem started ...\n");
}
