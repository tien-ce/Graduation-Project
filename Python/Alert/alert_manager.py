import alert_wrapper 
import time
from enum import Enum
class AlertType(Enum):
    low_threshold = 1
    high_threshold = 2

class Alert:
    def __init__(self, name, threshold, persistence, log_file, type_alert):
        """
        Initializes an Alert object with the specified parameters.
            @param name: A string representing the name of the alert.
            @param threshold: A numeric value that defines the threshold for triggering the alert.
            @param persistence: An integer representing how many consecutive readings must meet the condition before triggering the alert.
            @param log_file: A string representing the file path where alert logs should be recorded.
            @param type_alert: An instance of AlertType enum indicating whether the alert is for a low threshold or a high threshold condition.
        """
        self.name = name
        self.threshold = threshold
        self.persistence = persistence
        self.log_file = log_file
        self.alert_count = 0
        self.last_alert_time = None
        self.type_alert = type_alert
        self.triggered_status = False

def check_and_trigger_alert (alert, value):
    """Checks if the alert condition is met and triggers the alert if necessary."""
    if (alert.triggered_status):
        return  # Alert already triggered, do nothing
    else:
        if (alert.type_alert == AlertType.low_threshold and value < alert.threshold) or \
           (alert.type_alert == AlertType.high_threshold and value > alert.threshold):
            alert.alert_count += 1
            if alert.alert_count >= alert.persistence:
                # Trigger the alert
                alert_wrapper.set_led(1)  # Turn on LED
                # alert_wrapper.set_buzzer(1)  # Turn on buzzer (if implemented)
                alert.last_alert_time = time.time()
                alert.triggered_status = True
                message = f"Alert '{alert.name}' triggered with value {value}, exceeded {alert.type_alert} of {alert.threshold}"
                log_alert(alert, message) # Log the alert event
                core_iot_send_alert(message) # Send alert to Core IoT connection module 
        else:
            # Reset alert count if condition is not met
            alert.alert_count = 0   

def turn_off_alert(alert):
    """Turns off the alert and resets its status."""
    alert_wrapper.set_led(0)  # Turn off LED
    # alert_wrapper.set_buzzer(0)  # Turn off buzzer (if implemented)
    alert.triggered_status = False
    alert.alert_count = 0

def log_alert(alert, message):
    """Logs the alert event to the specified log file."""
    with open(alert.log_file, 'a') as f:
        f.write(f"{time.ctime()}: {message}\n")

def core_iot_send_alert(message):
    """Sends the alert message to the Core IoT connection module (placeholder function)."""
    # This function would contain logic to send the alert message to the Core IoT system.
    print(f"Sending alert to Core IoT: {message}")