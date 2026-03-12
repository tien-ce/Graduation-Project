from . import rs485_wrapper as RS485Wrapper
from collections import deque

## ------------ Register Addresses ------------##
CO_CONCENTRATION_REGISTER_ADDRESS = 0x0006   # Register holding CO concentration data
CO_SLAVEID_REGISTER_ADDRESS = 0x0100  # Register holding CO sensor slave ID (Use this for read or change slave ID)
CO_BAUDRATE_REGISTER_ADDRESS = 0x0101  # Register holding CO sensor baudrate (Use this for read or change baudrate (must be changed to same with baudrate at manager))

PM25_CONCENTRATION_REGISTER_ADDRESS = 0x0004  # Register holding PM2.5 concentration data
PM10_CONCENTRATION_REGISTER_ADDRESS = 0x0009  # Register holding PM10 concentration data
PM_SLAVEID_REGISTER_ADDRESS = 0x0100  # Register holding PM sensor slave ID (Use this for read or change slave ID)
PM_BAUDRATE_REGISTER_ADDRESS = 0x0101  # Register holding PM sensor baudrate (Use this for read or change baudrate (must be changed to same with baudrate at manager))

class SensorDevice:
    def __init__(self, slave_id, num_value, register_data_address, register_config_address, name, unit=""):
        """
        Base class for a Modbus sensor device. Handles raw data processing and integrity checks.
            @param slave_id: Modbus slave ID (address) for the sensor
            @param num_value: Number of values one sensor provides (e.g., PM2.5 and PM10 for a particulate matter sensor)
            @param register_data_address: Modbus register address for reading sensor data
            @param register_config_address: Modbus register address for sensor configuration (Baudrate, ID address, etc.)
            @param name: Human-readable name for the sensor (e.g., "CO Sensor")
            @param unit: Unit of measurement for the sensor data (e.g., "ppm", "µg/m³")
        """
        self._slave_id = slave_id
        self._num_values = num_value
        self.register_data_address = register_data_address
        self.register_config_address = register_config_address

        self._name = name
        self._unit = unit
        
        # Private variables for data integrity
        self.__window_size = 5
        self.__history = deque (maxlen=self.__window_size)  # Store last N values for moving average
        self.__filtered_history = deque (maxlen=self.__window_size)  # Store last N values for median filtering
        self.__last_clean_value = 0.0  # Last value that passed integrity checks
        self.__calibration_offset = 0.0  # Sensor-specific offset for calibration
    
    ##------------ Private Methods for Data Integrity and Processing ------------##
    def _raw_to_physical(self, num_values, raw_value):
        """
            Virtual method to convert raw sensor datas to physical units. Can be overridden for specific sensors.
        """
        
    def _calculate_moving_average(self, new_value):
        """Helper method to calculate moving average for smoothing."""
        self.__history.append(new_value)
        # Use moving average for smoothing
        moving_average_value = RS485Wrapper.calculate_average(self.__history)
        self.__filtered_history.append(moving_average_value)
    
    def _calculate_median(self) -> float: 
        """Helper method to calculate median for outlier rejection."""
        if len(self.__filtered_history) == 0:
            return self.__last_clean_value
        return RS485Wrapper.calculate_median(self.__filtered_history)
    
    ##------------ Public Methods for Sensor Interaction ------------##
    def read_raw_value(self, ctx):
        """
        Virtual method to read raw values from the sensor. Can be overridden for specific sensors if needed.
        """
        raw_value = []
        for i in range(self._num_values):
            raw_value.append (RS485Wrapper.read_data(ctx, self._slave_id, self.register_data_address[i]))
        return raw_value
    
    def process_physical_value(self, physical_value):
        if physical_value is None:
            return self.__last_clean_value
        # Apply calibration offset
        calibrated_value = physical_value + self.__calibration_offset
        # Update moving average and median filtering
        self._calculate_moving_average(calibrated_value)
        # Use median filtering to reject outliers
        median_value = self._calculate_median()
        self.__last_clean_value = median_value
        return self.__last_clean_value

    def set_calibration_offset(self, offset) -> None:
        self.__calibration_offset = offset
    
    def get_calibration_offset(self) -> float:
        return self.__calibration_offset

class PMSensor(SensorDevice):
    def __init__(self, slave_id, name):
        """PM sensor provides two values (PM2.5 and PM10), so we set num_value=2 and provide two register addresses."""
        super().__init__(slave_id, num_value=2, register_data_address=[PM25_CONCENTRATION_REGISTER_ADDRESS, PM10_CONCENTRATION_REGISTER_ADDRESS], register_config_address = [PM_SLAVEID_REGISTER_ADDRESS, PM_BAUDRATE_REGISTER_ADDRESS], name= name, unit= ["µg/m³", "µg/m³"])

    def _raw_to_physical(self, raw_value):
        # Example conversion for PM2.5 and PM10 sensors
        raw_PM10, raw_PM2_5 = raw_value
        return float(raw_PM2_5) * 0.1, float(raw_PM10) * 0.1  # Placeholder conversion factors

class COSensor(SensorDevice):
    def __init__(self, slave_id, name):
        """CO sensor might have different conversion logic, so we override the base class."""
        super().__init__(slave_id, num_value=1, register_data_address=CO_CONCENTRATION_REGISTER_ADDRESS, register_config_address = [CO_SLAVEID_REGISTER_ADDRESS, CO_BAUDRATE_REGISTER_ADDRESS], name= name, unit= "ppm")

    def _raw_to_physical(self, raw_value):
        """Override: CO sensor might have an offset or different scale"""
        return (raw_value / 100.0) - 0.5   # Placeholder conversion factor

class SensorManager:
    def __init__(self, device_path="/dev/ttyUSB0", baud=9600):
        # The C context pointer is highly sensitive; keep it private
        self.__ctx = RS485Wrapper.init_bus(device_path, baud)
        self.__sensors = []
        self.baudrate = baud
    
    def add_sensor(self, slave_id, address, name, unit=""):
        new_sensor = SensorDevice(slave_id, address, name, unit)
        self.__sensors.append(new_sensor)

    def read_all_sensors(self) -> dict[str, float]:
        results = {}
        for sensor in self.__sensors:
            raw_value = RS485Wrapper.read_data(self.__ctx, sensor._slave_id, sensor._reg_addr)
            processed_value = sensor.process_raw_value(raw_value)
            results[sensor._name] = processed_value 
        return results

    def shutdown(self):
        if self.__ctx:
            RS485Wrapper.close_bus(self.__ctx)
            self.__ctx = None