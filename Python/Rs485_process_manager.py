import threading
import time
import logging
from RS485.rs485_sensor_manager import SensorManager, COSensor, PMSensor
from Alert.alert_manager import Alert, AlertType, check_and_trigger_alert, turn_off_alert

CO_SLAVE_ID_ADDRESS = 0x01  # Slave ID address for CO sensor
PM_SLAVE_ID_ADDRESS = 0x24  # Slave ID address for PM sensor
"""
This module defines the RS485ProcessManager class, which manages the RS485 sensor polling, data processing, and alerting logic. It runs as a separate process and contains internal threads for continuous sensor monitoring. The manager interacts with the SensorManager to read sensor data, applies filtering and calibration, updates a global store for inter-process communication, and checks alert conditions to trigger notifications. It also ensures clean shutdown of hardware resources and alerts when the process is terminated.
"""

log = logging.getLogger("rs485-manager")

class RS485ProcessManager:
    def __init__(self, stop_signal, ready_signal, global_store):
        self._stop_event = stop_signal
        self._ready_event = ready_signal
        self.global_store = global_store
        
        # Initialize Hardware Managers
        self.sensors = SensorManager(device_path="/dev/ttyUSB0", baud=9600)
        
        # Define specific sensors
        self.co_sensor = COSensor(slave_id=CO_SLAVE_ID_ADDRESS, name="CO_Sensor")
        self.pm_sensor = PMSensor(slave_id=PM_SLAVE_ID_ADDRESS, name="PM_Sensor")
        
        # Define Alerts
        self.co_alert = Alert("High CO", threshold=50.0, persistence=3, 
                              log_file="/var/log/alerts.log", type_alert=AlertType.high_threshold)
        
        self.pm_2_5_alert = Alert("High PM2.5", threshold=35.0, persistence=3, 
                                  log_file="/var/log/alerts.log", type_alert=AlertType.high_threshold)
        
        self.pm_10_alert = Alert("High PM10", threshold=50.0, persistence=3, 
                                  log_file="/var/log/alerts.log", type_alert=AlertType.high_threshold)

    def _sensor_thread(self):
        """Thread 1: Constant Polling and Data Processing"""
        log.info("RS485 Sensor Polling Thread Started")
        while not self._stop_event.is_set():
            try:
                # 1. Read Raw Values
                co_raw = self.co_sensor.read_raw_value(self.sensors._SensorManager__ctx)
                pm_raw = self.pm_sensor.read_raw_value(self.sensors._SensorManager__ctx)
                # 2. Process (Filtering/Moving Average)
                co_val = self.co_sensor.process_raw_value(co_raw)
                pm_2_5_val, pm_10_val = self.pm_sensor.process_raw_value(pm_raw)

                # 3. Update Global Store (for Webserver/Other Processes)
                self.global_store.set("co_level", co_val)
                self.global_store.set("pm_2_5_level", pm_2_5_val)
                self.global_store.set("pm_10_level", pm_10_val)

                # 4. Check Alerts immediately after reading
                check_and_trigger_alert(self.co_alert, co_val)
                check_and_trigger_alert(self.pm_2_5_alert, self.global_store.get("pm_2_5_level"))
                check_and_trigger_alert(self.pm_10_alert, self.global_store.get("pm_10_level"))

            except Exception as e:
                log.error(f"Sensor Thread Error: {e}")
            
            time.sleep(5)

    def start_rs485_process(self):
        """Entry point called by app.py multiprocessing.Process"""
        log.info("========== STARTING INTEGRATED RS485 PROCESS ==========")
        
        # Start the internal threads
        t1 = threading.Thread(target=self._sensor_thread, daemon=True)

        t1.start()

        self._ready_event.set() # Notify app.py that initialization is complete
        self._stop_event.wait() # Keep process alive until system shutdown
        
        # Cleanup hardware on exit
        self.sensors.shutdown()
        turn_off_alert(self.co_alert)
        log.info("RS485 Process Shutdown Cleanly")
    
    def stop_rs485_process(self):
        """Signal handler to stop the process gracefully."""
        log.info("Received stop signal for RS485 Process")
        self._stop_event.set()