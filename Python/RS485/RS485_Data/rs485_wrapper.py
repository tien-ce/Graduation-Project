import ctypes
import os

# --- Configuration and Initialization ---
SENS_LIB_PATH = "/usr/lib/libair_485.so"
DATA_HANDLE_PATH = "/usr/lib/libdatahandle.so"

# Check if libraries exist
if not os.path.exists(SENS_LIB_PATH) or not os.path.exists(DATA_HANDLE_PATH):
    raise FileNotFoundError("One or more required shared libraries (.so) are missing.")

# Load the shared libraries
lib_air = ctypes.CDLL(SENS_LIB_PATH)
lib_data_handle = ctypes.CDLL(DATA_HANDLE_PATH)

# --- Define C Signatures (Stateless Functional Logic) ---

# RS485 initialization 
lib_air.rs485_init.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_char, ctypes.c_int, ctypes.c_int]
lib_air.rs485_init.restype = ctypes.c_void_p

# RS485 read raw register
lib_air.rs485_read_raw.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_uint16)]
lib_air.rs485_read_raw.restype = ctypes.c_int

# RS485 close
lib_air.rs485_close.argtypes = [ctypes.c_void_p]
lib_air.rs485_close.restype = None

# Math functions from data_handle
lib_data_handle.calculate_median.argtypes = [ctypes.POINTER(ctypes.c_float), ctypes.c_int]
lib_data_handle.calculate_median.restype = ctypes.c_float

lib_data_handle.calculate_average.argtypes = [ctypes.POINTER(ctypes.c_float), ctypes.c_int]
lib_data_handle.calculate_average.restype = ctypes.c_float

# --- Exported Functions ---

def init_bus(device="/dev/ttyS0", baud=9600):
    """Initializes the RS485 bus via C library."""
    device_bytes = device.encode('utf-8')
    ctx = lib_air.rs485_init(device_bytes, baud, b'N', 8, 1)
    return ctx

def read_data(ctx, slave_id, address):
    """Reads raw data from a Modbus slave."""
    if not ctx:
        return None
    raw_val = ctypes.c_uint16(0)
    result = lib_air.rs485_read_raw(ctx, slave_id, address, ctypes.byref(raw_val))
    return raw_val.value if result == 0 else None

def close_bus(ctx):
    """Closes the Modbus context."""
    if ctx:
        lib_air.rs485_close(ctx)

def calculate_median(values):
    """Calculates median using the O(n log n) C library implementation."""
    size = len(values)
    if size <= 0: return 0.0
    c_array = (ctypes.c_float * size)(*values)
    return lib_data_handle.calculate_median(c_array, size)

def calculate_average(values):
    """Calculates moving average via C library."""
    size = len(values)
    if size <= 0: return 0.0
    c_array = (ctypes.c_float * size)(*values)
    return lib_data_handle.calculate_average(c_array, size)