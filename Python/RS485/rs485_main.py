from rs485_sensor_manager import COSensor, PMSensor
import time
import random  
def main():
    co_sensor = COSensor (slave_id=1, register_address=0x01, name="CO Sensor", unit="ppm")
    pm_sensor = PMSensor (slave_id=2, register_address=0x02, name="PM Sensor", unit="µg/m³")
    while True:
        virtual_CO_value = random.randint(300, 400)  # Simulated raw value for CO sensor
        virtual_PM_25_value = random.randint(100, 200)  # Simulated raw value for PM sensor
        virtual_PM_10_value = random.randint(150, 250)  # Simulated raw value for PM sensor
        old_CO_value, clean_CO_value = co_sensor.process_raw_value(virtual_CO_value)
        old_PM_25_value, clean_PM_25_value = pm_sensor.process_raw_value(virtual_PM_25_value)
        old_PM_10_value, clean_PM_10_value = pm_sensor.process_raw_value(virtual_PM_10_value)
        print(f"CO Sensor: Raw={virtual_CO_value}, Clean={clean_CO_value} ppm")
        print(f"PM2.5 Sensor: Raw={virtual_PM_25_value}, Clean={clean_PM_25_value} µg /m³")
        print(f"PM10 Sensor: Raw={virtual_PM_10_value}, Clean={clean_PM_10_value} µg /m³")
        time.sleep(5)  # Simulate delay between readings 
if __name__ == "__main__":
    main()