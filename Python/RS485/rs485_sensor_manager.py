import rs485_wrapper as RS485Wrapper
from collections import deque

class SensorDevice:
    def __init__(self, slave_id, register_address, name, unit=""):
        self._slave_id = slave_id
        self._reg_addr = register_address
        self._name = name
        self._unit = unit
        
        # Private variables for data integrity
        self.__window_size = 5
        self.__history = deque (maxlen=self.__window_size)  # Store last N values for moving average
        self.__filtered_history = deque (maxlen=self.__window_size)  # Store last N values for median filtering
        self.__last_clean_value = 0.0  # Last value that passed integrity checks
        self.__offset = 0.0  # Sensor-specific offset for calibration

    def _raw_to_physical(self, raw_value):
        """
            Virtual method to convert raw sensor data to physical units. Can be overridden for specific sensors.
        """
        return float(raw_value)
    
    def process_raw_value(self, raw_value) -> tuple[float, float]:
        if raw_value is None:
            return self.__last_clean_value, self.__last_clean_value

        physical_value = self._raw_to_physical(raw_value) # Apply sensor-specific conversion
        physical_value += self.__offset  # Apply calibration offset 

        self.__history.append(physical_value)
        # Use moving average for smoothing
        moving_average_value = RS485Wrapper.calculate_average(self.__history)
        self.__filtered_history.append(moving_average_value)

        # Use median filtering to reject outliers
        median_value = RS485Wrapper.calculate_median(self.__filtered_history)
        self.__last_clean_value = median_value

        return self.__last_clean_value, physical_value

    def set_calibration_offset(self, offset) -> None:
        self.__offset = offset
    def get_calibration_offset(self) -> float:
        return self.__offset

class PMSensor(SensorDevice):
    def _raw_to_physical(self, raw_value):
        # Example conversion for PM2.5 sensor
        return float(raw_value) * 0.1  # Placeholder conversion factor

class COSensor(SensorDevice):
    def _raw_to_physical(self, raw_value):
        """Override: CO sensor might have an offset or different scale"""
        return (raw_value / 100.0) - 0.5   # Placeholder conversion factor

class SensorManager:
    def __init__(self, device_path="/dev/ttyUSB0", baud=9600):
        self.__wrapper = RS485Wrapper()
        # The C context pointer is highly sensitive; keep it private
        self.__ctx = self.__wrapper.init_bus(device_path, baud)
        self.__sensors = []

    def add_sensor(self, slave_id, address, name, unit=""):
        new_sensor = SensorDevice(slave_id, address, name, unit)
        self.__sensors.append(new_sensor)

    def read_all_sensors(self) -> dict[str, tuple[float, str]]:
        results = {}
        for sensor in self.__sensors:
            raw_value = self.__wrapper.read_data(self.__ctx, sensor._slave_id, sensor._reg_addr)
            processed_value = sensor.process_raw_value(raw_value)
            results[sensor._name] = (processed_value, sensor._unit)
        return results

    def shutdown(self):
        if self.__ctx:
            self.__wrapper.close_bus(self.__ctx)
            self.__ctx = None