#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "paho_mqtt.h"
#include <wlan_mgnt.h>
#include <drv_gpio.h>
#include "uart.h"
#include "spi.h"

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

/* Wi-Fi 配置 */
#define WLAN_SSID      "Redmi"
#define WLAN_PASSWORD  "12345678"

/* MQTT 配置 */
#define MQTT_HOST      "sh-3-mqtt.iot-api.com"
#define MQTT_PORT      1883
#define MQTT_CLIENTID  "spark-rtt"
#define MQTT_USERNAME  "pstyj2j5h1bbw5ew"
#define MQTT_PASSWORD  "6XHGz6TmP8"
#define MQTT_PUB_TOPIC "attributes"

/* 引脚定义 */
#define EMERGENCY_KEY_PIN   GET_PIN(C, 1)

/* 线程句柄 */
static rt_thread_t key_thread = RT_NULL;
static rt_thread_t spi_thread = RT_NULL;

/* MQTT 资源 */
static MQTTClient client;
static char mqtt_uri[128];
static char mqtt_topic[128];
static rt_bool_t mqtt_ready = RT_FALSE;

/* 状态变量 */
static int emergency_state = 0;
static int last_reported_state = -1;

/* 发布 MQTT 数据 */
static void mqtt_publish_emergency(int state)
{
    if (!mqtt_ready || state == last_reported_state)
        return;

    last_reported_state = state;

    char payload[64];
    rt_snprintf(payload, sizeof(payload), "{\"emergency\":%s}", state ? "true" : "false");

    MQTTMessage msg = {
        .qos = QOS0,
        .retained = 0,
        .payload = (void *)payload,
        .payloadlen = strlen(payload),
    };

    MQTTPublish(&client, MQTT_PUB_TOPIC, &msg);
    LOG_I("Emergency state published: %s", payload);
}

/* SPI线程 - 非阻塞方式轮询 */
static void spi_slave_thread(void *parameter)
{
    LOG_I("SPI thread started");
    rt_uint8_t rx_buf[1] = {0};
    rt_uint8_t tx_date1[4] = {0xf4,0xf5,0x01,0xfb};//商品1
    rt_uint8_t tx_date2[4] = {0xf4,0xf5,0x02,0xfb};//商品2
    rt_uint8_t tx_date3[4] = {0xf4,0xf5,0x03,0xfb};//商品3
    rt_uint8_t tx_date4[4] = {0xf4,0xf5,0x04,0xfb};//商品4

    while (1)
    {
        SPI1_Slave_ReceiveData(rx_buf, sizeof(rx_buf));  // 使用你定义的函数阻塞接收5字节
        if(rx_buf[0]==0x01)
        {
            uart3_send_hex(tx_date1, sizeof(tx_date1));
            rt_thread_mdelay(10);
        }
        else if (rx_buf[0]==0x02)
        {
            uart3_send_hex(tx_date2, sizeof(tx_date2));
            rt_thread_mdelay(10);
        }
        else if (rx_buf[0]==0x03)
        {
            uart3_send_hex(tx_date3, sizeof(tx_date2));
            rt_thread_mdelay(10);
        }
        else if (rx_buf[0]==0x04)
        {
          uart3_send_hex(tx_date4, sizeof(tx_date2));
          rt_thread_mdelay(10);
        }
        rt_thread_mdelay(50);
    }
}

/* 按键线程 - 消抖检测并发布 */
static void key_monitor_thread(void *parameter)
{
    rt_bool_t last_state = RT_TRUE;
    rt_tick_t last_tick = 0;
    const rt_tick_t debounce = rt_tick_from_millisecond(200);

    while (1)
    {
        rt_bool_t curr = rt_pin_read(EMERGENCY_KEY_PIN);
        rt_tick_t now = rt_tick_get();

        if (last_state == RT_TRUE && curr == RT_FALSE && now - last_tick > debounce)
        {
            emergency_state = !emergency_state;
            mqtt_publish_emergency(emergency_state);
            last_tick = now;
        }

        last_state = curr;
        rt_thread_mdelay(10);
    }
}

/* MQTT 连接回调 */
static void mqtt_online_callback(MQTTClient *c)
{
    LOG_I("MQTT connected");
    mqtt_ready = RT_TRUE;
    paho_mqtt_subscribe(&client, QOS1, "attributes/push", RT_NULL);
    paho_mqtt_subscribe(&client, QOS1, "command/send/+", RT_NULL);
}

static void mqtt_offline_callback(MQTTClient *c)
{
    mqtt_ready = RT_FALSE;
    LOG_W("MQTT disconnected");
}

/* MQTT 初始化 */
static void mqtt_client_init(void)
{
    static int init = 0;
    if (init++) return;

    MQTTPacket_connectData conn = MQTTPacket_connectData_initializer;
    rt_snprintf(mqtt_uri, sizeof(mqtt_uri), "tcp://%s:%d", MQTT_HOST, MQTT_PORT);
    rt_snprintf(mqtt_topic, sizeof(mqtt_topic), "%s", MQTT_PUB_TOPIC);

    client.uri = mqtt_uri;
    client.condata = conn;
    client.condata.clientID.cstring = MQTT_CLIENTID;
    client.condata.username.cstring = MQTT_USERNAME;
    client.condata.password.cstring = MQTT_PASSWORD;
    client.condata.keepAliveInterval = 60;
    client.condata.cleansession = 1;

    client.buf_size = client.readbuf_size = 2048;
    client.buf = malloc(client.buf_size);
    client.readbuf = malloc(client.readbuf_size);

    if (!client.buf || !client.readbuf)
    {
        LOG_E("MQTT malloc failed");
        return;
    }

    client.connect_callback = RT_NULL;
    client.online_callback = mqtt_online_callback;
    client.offline_callback = mqtt_offline_callback;

    paho_mqtt_start(&client);
}

/* Wi-Fi 连接成功 */
static void wifi_ready(int event, struct rt_wlan_buff *buff, void *p)
{
    LOG_I("Wi-Fi connected");
    rt_thread_mdelay(1000);
    mqtt_client_init();

    // 启动 SPI 线程
    SPI1_Slave_Init();
    spi_thread = rt_thread_create("spi_thread", spi_slave_thread, RT_NULL, 1024, 22, 10);
    if (spi_thread != RT_NULL) rt_thread_startup(spi_thread);

    // 启动按键线程
    rt_pin_mode(EMERGENCY_KEY_PIN, PIN_MODE_INPUT_PULLUP);
    key_thread = rt_thread_create("key_mon", key_monitor_thread, RT_NULL, 512, 15, 10);
    if (key_thread != RT_NULL) rt_thread_startup(key_thread);
}

/* Wi-Fi 连接失败 */
static void wifi_fail(int event, struct rt_wlan_buff *buff, void *p)
{
    LOG_W("Wi-Fi failed");
}

/* 启动Wi-Fi连接 */
void wifi_init(void)
{
    rt_wlan_register_event_handler(RT_WLAN_EVT_READY, wifi_ready, RT_NULL);
    rt_wlan_register_event_handler(RT_WLAN_EVT_STA_CONNECTED_FAIL, wifi_fail, RT_NULL);
    rt_wlan_set_mode(RT_WLAN_DEVICE_STA_NAME, RT_WLAN_STATION);
    rt_wlan_connect(WLAN_SSID, WLAN_PASSWORD);
}

/* 主函数 */
int main(void)
{
    uart3_init(9600);
    wifi_init();

    return 0;
}


