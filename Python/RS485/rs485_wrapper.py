import ctypes
import os
from enum import Enum

class RS485Wrapper:
    def __init__(self, sens_lib_path="/usr/lib/libair_485.so", data_corect_path = "/usr/lib/lib_data_handle.so"):
        """
        Initializes the ctypes interface for the RS485 C library.
        """
        if not os.path.exists(sens_lib_path):
            raise FileNotFoundError(f"Shared library not found at {sens_lib_path}")

        self.lib_air = ctypes.CDLL(sens_lib_path)
        self.lib_data_handle = ctypes.CDLL(data_corect_path)
        self._declare_signatures()

    def _declare_signatures(self):
        """
        Defines argument and return types for the C functions to ensure memory safety.
        """
        """
            Read data function signature:
        """
        # 1. Initialize Modbus: modbus_t* rs485_init(const char* device, int baud, char parity, int data_bit, int stop_bit)
        self.lib_air.rs485_init.argtypes = [
            ctypes.c_char_p, ctypes.c_int, ctypes.c_char, ctypes.c_int, ctypes.c_int
        ]
        self.lib_air.rs485_init.restype = ctypes.c_void_p  # Returns the modbus_t pointer

        # 2. Read Register: int rs485_read_raw(modbus_t *ctx, int slave_id, int reg_addr, uint16_t *out_value)
        self.lib_air.rs485_read_raw.argtypes = [
            ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_uint16)
        ]
        self.lib_air.rs485_read_raw.restype = ctypes.c_int

        # 3. Close: void rs485_close(modbus_t *ctx)
        self.lib_air.rs485_close.argtypes = [ctypes.c_void_p]
        self.lib_air.rs485_close.restype = None
        """
            Data handle function signature:
        """
        #4. Calculate median: float calculate_median(float* values, int size)
        self.lib_data_handle.calculate_median.argtypes = [ctypes.POINTER(ctypes.c_float), ctypes.c_int]
        self.lib_data_handle.calculate_median.restype = ctypes.c_float 

        #5. Calculate average: float calculate_average(float* values, int size)
        self.lib_data_handle.calculate_average.argtypes = [ctypes.POINTER(ctypes.c_float), ctypes.c_int]
        self.lib_data_handle.calculate_average.restype = ctypes.c_float

    def init_bus(self, device="/dev/ttyS0", baud=9600):
        """
        Wraps the C init function. Converts Python strings to C-style char pointers.
        """
        device_bytes = device.encode('utf-8')
        ctx = self.lib_air.rs485_init(device_bytes, baud, b'N', 8, 1)
        if not ctx:
            print(f"Failed to initialize RS485 on {device}")
        return ctx

    def read_data(self, ctx, slave_id, address):
        """
        Wraps the C read function. Handles the pointer for the output value.
        """
        if not ctx:
            return None

        # Create the uint16 buffer for C to write into
        raw_val = ctypes.c_uint16(0)
        
        # Pass by reference using ctypes.byref()
        result = self.lib_air.rs485_read_raw(ctx, slave_id, address, ctypes.byref(raw_val))
        
        if result == 0:
            return raw_val.value
        return None

    def close_bus(self, ctx):
        if ctx:
            self.lib_air.rs485_close(ctx)
    
    def calculate_median(self, values):
        """
        Wraps the C median calculation function. Converts Python lists to C arrays.
        """
        size = len(values)
        if size <= 0:
            return 0.0
        
        # Create a C array of floats
        c_array = (ctypes.c_float * size)(*values)
        
        # Call the C function
        median = self.lib_data_handle.calculate_median(c_array, size)
        return median

    def calculate_average(self, values):
        """
        Wraps the C average calculation function. Converts Python lists to C arrays.
        """
        size = len(values)
        if size <= 0:
            return 0.0

        # Create a C array of floats
        c_array = (ctypes.c_float * size)(*values)

        # Call the C function
        average = self.lib_data_handle.calculate_average(c_array, size)
        return average