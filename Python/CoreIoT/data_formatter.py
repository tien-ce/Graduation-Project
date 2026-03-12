import time

def format_coreiot_telemetry(global_store):
    """
    Maps internal GlobalStore keys to CoreIoT telemetry keys.
    Uses time-series format for historical analysis[cite: 398].
    """
    return {
        "ts": int(time.time() * 1000),  # CoreIoT expects ms [cite: 397]
        "values": {
            "co": global_store.get("co_level"),
            "pm25": global_store.get("pm_2_5_level"),
            "pm10": global_store.get("pm_10_level"),
            "people_count": global_store.get("people_count"),
            "wifi_status": global_store.get("wifi_status")
        }
    }