import ctypes
import os

# --- Configuration and Initialization ---
ALERT_LIB_PATH = "/usr/lib/libalert.so"

# Check if library exists 
if not os.path.exists(ALERT_LIB_PATH):
    raise FileNotFoundError("Required shared library (.so) is missing.")

# Load the shared library
lib_alert = ctypes.CDLL(ALERT_LIB_PATH)

# --- Define C Signatures (Stateless Functional Logic) ---

# Alert Init 
lib_alert.alert_init.argtypes = [ctypes.c_int, ctypes.c_int]

# Alert set led
lib_alert.alert_set_led.argtypes = [ctypes.c_int]
lib_alert.alert_set_led.restype = None

# Alert get led status
lib_alert.alert_get_led_state.argtypes = []
lib_alert.alert_get_led_state.restype = ctypes.c_int

# Alert set buzzer
lib_alert.alert_set_buzzer.argtypes = [ctypes.c_int]
lib_alert.alert_set_buzzer.restype = None

# # Alert get buzzer status
# lib_alert.alert_get_buzzer_state.argtypes = []
# lib_alert.alert_get_buzzer_state.restype = ctypes.c_int

# --- Exported Functions ---
def set_led(state):
    """Sets the alert LED state (0 for off, 1 for on)."""
    lib_alert.alert_set_led(state)

def get_led_state():
    """Returns the current state of the alert LED (0 for off, 1 for on)."""
    return lib_alert.alert_get_led_state()

def set_buzzer(state):
    """Sets the alert buzzer state (0 for off, 1 for on)."""
    lib_alert.alert_set_buzzer(state)
# def get_buzzer_state():
#     """Returns the current state of the alert buzzer (0 for off, 1 for on)."""
#     return lib_alert.alert_get_buzzer_state()

def setall():
    """Sets both LED and buzzer to the same state (0 for off, 1 for on)."""
    lib_alert.alert_all_on()

def clearall():
    """Clears both LED and buzzer (turns them off)."""
    lib_alert.alert_all_off()

def init_alerts(led_state=0, buzzer_state=0):
    """Initializes the alert system with specified LED and buzzer states."""
    lib_alert.alert_init(led_state, buzzer_state)   