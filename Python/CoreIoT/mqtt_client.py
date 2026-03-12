import paho.mqtt.client as mqtt
import json
import logging

log = logging.getLogger(__name__)

class CoreIoTMQTTClient:
    def __init__(self, broker="app.coreiot.io", port=1883, token=None):
        self.client = mqtt.Client()
        self.broker = broker
        self.port = port
        # CoreIoT uses the Access Token as the username 
        if token:
            self.client.username_pw_set(token)

    def connect(self):
        try:
            self.client.connect(self.broker, self.port, keepalive=60)
            self.client.loop_start() # Start background thread for network traffic
            return True
        except Exception as e:
            log.error(f"CoreIoT Connection Failed: {e}")
            return False

    def publish_telemetry(self, payload):
        """Publishes JSON data to the standard ThingsBoard telemetry topic."""
        topic = "v1/devices/me/telemetry"
        result = self.client.publish(topic, json.dumps(payload))
        return result.rc == mqtt.MQTT_ERR_SUCCESS

    def disconnect(self):
        self.client.loop_stop()
        self.client.disconnect()