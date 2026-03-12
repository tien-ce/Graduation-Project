import logging
from .mqtt_client import CoreIoTMQTTClient
from .data_formatter import format_coreiot_telemetry

log = logging.getLogger(__name__)

class CoreIoTProcessManager:
    def __init__(self, global_store, stop_signal, data_ready_cv, token):
        self.global_store = global_store
        self.stop_signal = stop_signal
        self.data_ready_cv = data_ready_cv
        self.client = CoreIoTMQTTClient(token=token)

    def run_main_process(self):
        """FSM Implementation for Cloud Connection"""
        log.info("CoreIoT Process Started")
        
        # Initial Connection (CHECK_CONNECTION state)
        if not self.client.connect():
            log.warning("Starting with offline mode, will retry later")

        while not self.stop_signal.is_set():
            # State: WAITING_DATA
            with self.data_ready_cv:
                # Wait for sensor thread to signal that new data is in GlobalStore
                data_update = self.data_ready_cv.wait(timeout=10.0) 

            if data_update:
                # State: DATA_PROCESSING 
                telemetry = format_coreiot_telemetry(self.global_store)

                # State: SEND_TO_SERVER
                success = self.client.publish_telemetry(telemetry)
                if not success:
                    log.error("Failed to send telemetry to CoreIoT")
            
        self.client.disconnect()
        log.info("CoreIoT Process Terminated")