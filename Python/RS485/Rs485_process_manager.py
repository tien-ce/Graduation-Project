import threading
import time
import logging
import random  # For simulating sensor data in testing
from .RS485_Data.rs485_sensor_manager import SensorManager, COSensor, PMSensor
from .RS485_Alert.alert_manager import Alert, AlertType, check_and_trigger_alert, turn_off_alert

CO_SLAVE_ID_ADDRESS = 0x01  # Slave ID address for CO sensor
PM_SLAVE_ID_ADDRESS = 0x24  # Slave ID address for PM sensor
"""
This module defines the RS485ProcessManager class, which manages the RS485 sensor polling, data processing, and alerting logic. It runs as a separate process and contains internal threads for continuous sensor monitoring. The manager interacts with the SensorManager to read sensor data, applies filtering and calibration, updates a global store for inter-process communication, and checks alert conditions to trigger notifications. It also ensures clean shutdown of hardware resources and alerts when the process is terminated.
"""
 
log = logging.getLogger("rs485-manager")

class RS485ProcessManager:
    def __init__(self, stop_signal, ready_signal, global_store, rs485_data_ready_cv):
        self._stop_event = stop_signal
        self._ready_event = ready_signal
        self.global_store = global_store
        self.rs485_data_ready_cv = rs485_data_ready_cv

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
                # 1. Read Raw Values and Convert to Physical Values
                # co_raw = self.co_sensor.read_raw_value(self.sensors._SensorManager__ctx)
                pm_raw = self.pm_sensor.read_raw_value(self.sensors._SensorManager__ctx)
                log.debug(f"Raw CO: {co_raw}, Raw PM2.5: {pm_raw[0]}, Raw PM10: {pm_raw[1]}")
                co_raw = random.uniform(0, 100)  # Simulated raw value for testing
                # pm_raw = [random.uniform(0, 100), random.uniform(0, 100)]  # Simulated raw value for testing
                co_physical = self.co_sensor._raw_to_physical(co_raw)
                pm_2_5_physical, pm_10_physical = self.pm_sensor._raw_to_physical(pm_raw)


                # 2. Process (Filtering/Moving Average)
                co_val = self.co_sensor.process_physical_value(co_physical)
                pm_2_5_val = self.pm_sensor.process_physical_value(pm_2_5_physical)
                pm_10_val = self.pm_sensor.process_physical_value(pm_10_physical)

                # 3. Update Global Store (for Webserver/Other Processes)
                self.global_store.set("co_level", co_val)
                self.global_store.set("pm_2_5_level", pm_2_5_val)
                self.global_store.set("pm_10_level", pm_10_val)

                # 4. Notify Alert Thread
                with self.rs485_data_ready_cv:
                    self.rs485_data_ready_cv.notify_all()

            except Exception as e:
                log.error(f"Sensor Thread Error: {e}")
            
            time.sleep(5)
    def _alert_thread(self):
        """Thread 2: Alert Checking and Triggering"""
        log.info("RS485 Alert Monitoring Thread Started")
        while not self._stop_event.is_set():
            try:
            # 1. Wait for the signal (with a timeout so we can check _stop_event)
                with self.rs485_data_ready_cv:
                    # Wait for notification or 5s timeout to re-check stop_event
                    signaled = self.rs485_data_ready_cv.wait(timeout=5.0)
                if signaled:
                    log.debug ("Alert Thread Notified of New Sensor Data")
                    # Check CO Alert
                    check_and_trigger_alert(self.co_alert, self.global_store.get("co_level", 0.0))
                    # Check PM2.5 Alert
                    check_and_trigger_alert(self.pm_2_5_alert, self.global_store.get("pm_2_5_level", 0.0))
                    # Check PM10 Alert
                    check_and_trigger_alert(self.pm_10_alert, self.global_store.get("pm_10_level", 0.0))

            except Exception as e:
                log.error(f"Alert Thread Error: {e}")
            
            time.sleep(10)
    
    
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
    
    def stop(self):
        log.info("========== STOPPING RS485 PROCESS ==========")
        self._stop_event.set()
        self._ready_event.clear()