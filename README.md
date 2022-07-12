# Шлюз для устройств RF24 на основе ESP-NOW для ESP8266 + RF24.
Шлюз для получения данных с устройств RF24 на основе ESP-NOW для ESP8266.

## Функции:

1. Никакого WiFi и сторонних серверов. Всё работает исключительно локально.
2. При подключении к шлюзу периодическая передача своего состояния доступности (Keep Alive). 
3. Управление осуществляется через [ESP-NOW шлюз](https://github.com/aZholtikov/ESP-NOW-MQTT_Gateway) посредством Home Assistant через MQTT брокер.
  
## Примечание:

1. Работает на основе библиотеки [ZHNetwork](https://github.com/aZholtikov/ZHNetwork) и протоколов передачи данных [ZH Smart Home Protocol](https://github.com/aZholtikov/ZH-Smart-Home-Protocol) и [ZH-RF24-Sensor-Protocol](https://github.com/aZholtikov/ZH-RF24-Sensor-Protocol).
2. Для работы в сети необходимо наличие [ESP-NOW - MQTT Gateway](https://github.com/aZholtikov/ESP-NOW-MQTT_Gateway).
3. Для включения режима обновления прошивки необходимо послать команду "update" в корневой топик устройства (пример - "homeassistant/gateway/70039F44BEF7"). Устройство перейдет в режим обновления (подробности в [API](https://github.com/aZholtikov/ZHNetwork/blob/master/src/ZHNetwork.h) библиотеки [ZHNetwork](https://github.com/aZholtikov/ZHNetwork)). Аналогично для перезагрузки послать команду "restart".
4. При возникновении вопросов/пожеланий/замечаний пишите на github@zh.com.ru

## Поддерживаемые устройства:

1. [RF24-Open/Close-Sensor](https://github.com/aZholtikov/RF24-Open-Close-Sensor)
2. [RF24-Climate-Sensor(BME280)](https://github.com/aZholtikov/RF24-Climate-Sensor-BME280)
3. [RF24-Climate-Sensor(BMP280)](https://github.com/aZholtikov/RF24-Climate-Sensor-BMP280)
4. [RF24-Climate-Sensor(BME680)](https://github.com/aZholtikov/RF24-Climate-Sensor-BME680)
5. [RF24-Water-Leakage-Sensor](https://github.com/aZholtikov/RF24-Water-Leakage-Sensor)
6. [RF24-Touch-Switch](https://github.com/aZholtikov/RF24-Touch-Switch)

## Пример полной конфигурации для Home Assistant:

    # BME280
    sensor:
    - platform: mqtt
        name: "NAME"
        device_class: "voltage"
        unit_of_measurement: "V"
        state_topic: "homeassistant/rf24_sensor/bme280/X"
        value_template: "{{ value_json.battery }}"
    - platform: mqtt
        name: "NAME"
        device_class: "humidity"
        unit_of_measurement: "%"
        state_topic: "homeassistant/rf24_sensor/bme280/X"
        value_template: "{{ value_json.humidity }}"
    - platform: mqtt
        name: "NAME"
        device_class: "temperature"
        unit_of_measurement: "°C"
        state_topic: "homeassistant/rf24_sensor/bme280/X"
        value_template: "{{ value_json.temperature }}"
    - platform: mqtt
        name: "NAME"
        device_class: "pressure"
        unit_of_measurement: "мм"
        state_topic: "homeassistant/rf24_sensor/bme280/X"
        value_template: "{{ value_json.pressure }}"

    # BMP280
    sensor:
    - platform: mqtt
        name: "NAME"
        device_class: "voltage"
        unit_of_measurement: "V"
        state_topic: "homeassistant/rf24_sensor/bmp280/X"
        value_template: "{{ value_json.battery }}"
    - platform: mqtt
        name: "NAME"
        device_class: "temperature"
        unit_of_measurement: "°C"
        state_topic: "homeassistant/rf24_sensor/bmp280/X"
        value_template: "{{ value_json.temperature }}"
    - platform: mqtt
        name: "NAME"
        device_class: "pressure"
        unit_of_measurement: "мм"
        state_topic: "homeassistant/rf24_sensor/bmp280/X"
        value_template: "{{ value_json.pressure }}"

    # OPEN_CLOSE
    sensor:
    - platform: mqtt
        name: "NAME"
        device_class: "voltage"
        unit_of_measurement: "V"
        state_topic: "homeassistant/rf24_sensor/open_close/X"
        value_template: "{{ value_json.battery }}"
    binary_sensor:
    - platform: mqtt
        name: "NAME"
        device_class: "door"
        state_topic: "homeassistant/rf24_sensor/open_close/X"
        value_template: "{{ value_json.state }}"
        payload_on: "OPEN"
        payload_off: "CLOSE"
    
    # TOUCH_SWITCH
    sensor:
    - platform: mqtt
        name: "NAME"
        device_class: "voltage"
        unit_of_measurement: "V"
        state_topic: "homeassistant/rf24_sensor/touch_switch/X"
        value_template: "{{ value_json.battery }}"

    # WATER_LEAKAGE
    sensor:
    - platform: mqtt
        name: "NAME"
        device_class: "voltage"
        unit_of_measurement: "V"
        state_topic: "homeassistant/rf24_sensor/water_leakage/X"
        value_template: "{{ value_json.battery }}"
    binary_sensor:
    - platform: mqtt
        name: "NAME"
        device_class: "moisture"
        state_topic: "homeassistant/rf24_sensor/water_leakage/X"
        value_template: "{{ value_json.state }}"
        payload_on: "ALARM"
        payload_off: "DRY"

## Версии:

1. v1.0 Начальная версия.