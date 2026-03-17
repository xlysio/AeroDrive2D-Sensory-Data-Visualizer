# AeroDrive2D-Sensory-Data-Visualizer
AeroDrive2D is an interactive 2D racing game driven by a custom-built, gesture-based physical controller. This project bridges hardware sensor integration with real-time software rendering and data telemetry.

🎮 How It Works

The system discards traditional gamepads in favor of a custom spherical controller resting on a desk, operated with both hands:

    Steering (Left/Right): Achieved by physically tilting the spherical base. The tilt angle is calculated using the built-in IMU (Gyroscope/Accelerometer) of the Arduino Nano 33 IoT.

    Acceleration (Speed): Controlled contactlessly by the vertical distance of the user's hand from a VL53L0X Time-of-Flight (ToF) laser sensor. The closer the hand, the faster the car goes.

The objective is simple: complete a 5-lap time trial as fast as possible.

⚙️ Key Technical Features

    Real-Time Telemetry: Live plotting of incoming sensor data (distance and tilt angle) using Qt Charts (strip chart format).

    Robust Serial Communication: Seamless data parsing from the microcontroller via a virtual COM port over USB (QSerialPort).

    Bilingual GUI: Fully internationalized user interface (English/Polish) implemented via Qt Linguist (QTranslator).

    Scalable Architecture: Responsive and geometry-independent layout built with Qt Layouts (QVBoxLayout, QHBoxLayout).

    Academic-Grade Documentation: Fully documented C++ source code generated using Doxygen.

🛠️ Tech Stack

    Hardware: Arduino Nano 33 IoT, VL53L0X (Pololu), Custom 3D-printed spherical base (Autodesk Fusion).

    Software: C++17, Qt Framework (QtWidgets, Qt Charts, Qt SerialPort), Arduino IDE, CMake.