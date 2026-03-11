# LSMY Lab Safety Monitoring System - RS485 & Alert Subsystem

This repository contains the source code for environmental monitoring and safety alert subsystems of the LSMY system. These components are specifically designed for cross-compilation via the Yocto Project and integration into the meta-lsmy layer.

The system utilizes a modular, multi-threaded architecture to ensure real-time responsiveness for industrial sensors and safety hardware.

## System Architecture

The project is structured to separate low-level hardware communication from high-level application logic:

- **Hardware Interfacing**: Native C wrappers for high-speed RS485 Modbus RTU and GPIO-based alert modules.
- **Process Management**: A Python-based manager that orchestrates sensor polling and alert checking in isolated threads.
- **Inter-Process Communication (IPC)**: Synchronized data sharing via a centralized Global Store and Condition Variables (CV) for efficient, interrupt-like signaling.

## Project Structure

| Directory         | Purpose                                                                |
| :---------------- | :--------------------------------------------------------------------- |
| **Python/**       | Main application logic including the Rs485_process_manager.py.         |
| **Python/RS485/** | Sensor-specific classes (CO, PM2.5/PM10) and Modbus protocol handlers. |
| **Python/Alert/** | Safety alert logic and threshold monitoring modules.                   |
| **Components/**   | Native C source code for libalert and librs485 wrappers.               |
| **rs485/**        | Low-level C implementations for Modbus communication.                  |
| **Example/**      | Example for using library developed in Component.                      |

## Integration with Yocto (meta-lsmy)

These components are deployed to the Raspberry Pi 4 using custom Bitbake recipes within the meta-lsmy layer.

### Building and Deployment

To build and deploy these components within your Yocto environment, use the provided automation scripts or devtool:

```bash
# Using the custom automation script
./build_deploy.sh rs485-app

# Manual devtool workflow
devtool modify rs485-app
devtool build rs485-app
devtool deploy-target rs485-app root@<TARGET_IP>
```

If you don't use yocto, you can use the component folder as library (Using Cmake, you can indicate use for navtive machine or cross machine.)

## Hardware Specifications

- **Protocol**: Modbus RTU over RS485.

- **Supported Sensors**: Carbon Monoxide (CO), Particulate Matter (PM2.5, PM10).

- **Alert Output**: Visual and Audible indicators (LED/Buzzer) managed via libgpiod.

## Manual Build (Without Yocto)

If you are developing on a standard Linux distribution (Raspberry Pi OS, Ubuntu, etc.) without using Yocto, you can build the native C components manually using CMake.

### Prerequisites

- **Build Tools**: `gcc`, `g++`, `make`, `cmake` (version 3.28 or higher).
- **System Libraries**: `libgpiod` (for Alert components), `libmodbus` (for rs485 components) and standard POSIX real-time libraries (`-lrt`).
- **Python**: Python 3.12+ for running the application logic.

### Building Native Components

The project uses an out-of-source build approach. Each component in the `Components/` directory contains its own `CMakeLists.txt`.

```bash
# 1. Navigate to the component directory
cd Components/<COMPONENT_NAME>

# 2. Create and enter build directory
mkdir -p build && cd build

# 3. Configure and compile
cmake ..
make
```
