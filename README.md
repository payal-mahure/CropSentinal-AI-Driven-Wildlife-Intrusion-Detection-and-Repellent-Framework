# CropSentinal:AI_Driven_Wildlife_Intrusion_Detection_and_Repellent_Framework
A solar-powered, AI-enabled wildlife monitoring system built using ESP32-CAM that performs real-time motion detection, distance measurement, ultrasonic deterrence,  and  animal image classification using TinyML. Ideal for farms and remote fields, the system logs and reacts to animal intrusion without human intervention.

Features:
🎯 Motion Detection using PIR sensor.
📏 Object Distance Estimation via HC-SR04 ultrasonic sensor.
🔊 Ultrasonic Sound-Based Deterrence using active transducer.
📷 ESP32-CAM Image Capture on intrusion detection.
💾 Image Storage on SD card.
🤖 TinyML-based Animal Classification from captured images.
☀️ Fully Solar-Powered Setup using L7805CV and TP4056.

🔧 Hardware Technologies:
ESP32-CAM – For image capture, processing, and running logic.
PIR Motion Sensor (HC-SR501) – Detects motion.
Ultrasonic Sensor (HC-SR04) – Measures distance from detected object.
Ultrasonic Transducer (e.g., 40kHz) – Emits high-frequency sound to deter animals.
Micro SD Card – To store captured images locally.
Solar Panel + Battery  – Power supply for off-grid usage.

Software / Programming Technologies:
Arduino IDE – For programming the ESP32-CAM.
C / C++ – Programming language used in Arduino environment.
TinyML – For animal classification using machine learning.
Serial Monitor – For debugging sensor data and motion/distance outputs.

AI / ML (TinyML) Technologies:
TensorFlow Lite for Microcontrollers – To train & deploy tiny ML models.
Image Classification Model – Trained on wildlife images (binary or multi-class).
Model Deployment on ESP32-CAM – Using quantized .tflite model embedded in Arduino sketch.

