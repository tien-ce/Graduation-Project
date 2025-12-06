# Raspberry Pi 4 Component Library: Lab Safety Monitoring System

This repository hosts the source code and documentation for the **System for Monitoring Environment and Safety in the Lab**, developed using a modular architecture for embedded systems (Raspberry Pi 4) and custom Linux kernel optimization via **Yocto Project**.

[cite_start]The goal is to provide a fast, secure, and resource-optimized solution leveraging **Edge AI** and **IoT**[cite: 6, 44].

## 💡 Project Architecture and Approach

[cite_start]The system combines industrial sensors, a custom embedded operating system (Yocto), and Edge AI, managed by a centralized platform (CoreIoT)[cite: 44].

### Key Components

| Component                    | Function                                                                                                                          | Technology                                                                  |
| :--------------------------- | :-------------------------------------------------------------------------------------------------------------------------------- | :-------------------------------------------------------------------------- |
| **Edge Computing (Gateway)** | [cite_start]Handles real-time processing, data correctness checks, human detection (AI), and local alerting[cite: 163, 166, 167]. | [cite_start]Custom Yocto OS, Embedded AI (TFLite/ONNX Runtime) [cite: 95]   |
| **IoT End-Device**           | [cite_start]Collects raw environmental data (Temperature, Humidity, $CO_2$, VOC)[cite: 160, 36].                                  | [cite_start]Sensors (Modbus RS-485 likely) [cite: 38]                       |
| **IoT Server**               | [cite_start]Centralized data storage, history analysis, dashboard display, and remote alerts for end-users[cite: 179, 181].       | [cite_start]CoreIoT Platform [cite: 44, 90]                                 |
| **Human Safety**             | [cite_start]Uses computer vision models to count people and detect signs of fatigue or non-safe behavior[cite: 37, 103].          | [cite_start]Camera, Facial Analysis (EAR), Pose Estimation [cite: 108, 111] |

---

## 📁 Project Structure

The project is divided into **Components** (reusable libraries) and **Examples** (application entry points). This structure allows for easy integration of multiple languages (C/C++, Python) handled by CMake.
| Directory | Purpose |
| :--- | :--- |
| **Components/Message_Passing/** | Core reusable library for POSIX Message Queue IPC. |
| **Components/Message_Passing/Include/** | Header files for the Message_Passing component. |
| **Example/Message_passing/** | A demonstration application to test the Message_Passing component. |

---

## 🛠️ Building and Development

The project uses an **out-of-source** build method via CMake, ensuring the source directory remains clean.

### For C files

#### Prerequisites

- **Compiler Toolchain:** GCC/G++ or other required language compilers (e.g., Python environment for Edge AI models).
- **CMake:** Version 3.28.3 or higher.
- **System Libraries:** Dependencies like the POSIX real-time library (`-lrt`) are managed by the component's CMake file.

#### Step 1: Build the Component Library

Any required component library must be built first. This step creates the necessary library file (`.a` or `.so`) in the component's `build` directory.

```bash
# 1. Navigate to the desired component's build directory
cd Components/<COMPONENT_NAME>/build

# 2. Configure CMake (Initial setup, finds source and dependencies)
cmake ..

# 3. Build the component library
make
```

### Step 2: Build the Example Executable

The example application is built next, linking against the artifact created in Step 1.

```bash
# 1. Navigate to the example's build directory
cd ../../../Example/<EXAMPLE_NAME>/build

# 2. Configure CMake (This resolves all component dependencies using GLOB)
cmake ..

# 3. Build the example executable
make
```

### For python file (Will be added)
