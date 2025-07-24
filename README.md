# PSA - Vision-Based Tracking and Unlocking System for an E-Bike

Automation Systems Project (**P**rojeto em **S**istemas de **A**utomação) - Group 2 - Class P4 - University of Aveiro - 2023/24

## Table of Contents

- [Introduction](#introduction)
- [Components Used](#components-used)
- [Protocols Used](#protocols-used)
- [Programming Languages Used](#programming-languages-used)
- [Software Used](#software-used)
- [Authors](#authors)



---
## Introduction

<p align="justify">
In the current context of rapid urbanization and growing concern for environmental sustainability, electric bicycles, also known as e-bikes, emerge as an efficient and eco-friendly alternative for urban transportation. However, managing and securing these e-bikes presents significant challenges, such as real-time tracking and access control limited to authorized users. To address these issues, this project proposes the development of an integrated tracking and unlocking system for e-bikes, using technologies related to the Internet of Things (IoT). In this way, the project's objectives were the development of:</p>

- A computer vision system capable of reading and decoding a QR code;
- A system to collect and store variables related to GPS location (latitude, longitude, and speed);
- A system to send the relevant information to a remote server;
- A system for processing and storing the e-bike’s data;
- A graphical interface (dashboard) for information monitoring.

---
## Components Used

<p align="justify">
To develop the solution for this project, the following components were used:</p>

- **ESP32** as the main processing unit on the _e-bike_;
- **ESP32-CAM** for the project's vision module;
- **NEO-6M** GPS module to obtain the _e-bike_ coordinates, as well as other relevant information such as speed;
- **SIM900 GPRS** modem for remote communication;
- **Remote server** to integrate the IoT (_Internet of Things_) interface, MQTT _broker_, and database;
- **Opto-relay** to unlock the _e-bike_ lock.


![Prototipo_Compressed](https://github.com/RBastos36/PSA-eBike-Project/assets/145439743/723f937c-b5d7-47ad-88e4-f71a5076ce88)

<p align="center">
Prototype of the hardware used.
</p>

[![Demonstração do protótipo em funcionamento](https://img.youtube.com/vi/8IhwF9_wa2A/0.jpg)](https://www.youtube.com/watch?v=8IhwF9_wa2A)
<p align="center">
Demonstration of the working prototype.
</p>


---
## Protocols Used

The following protocols were used in the development of this project:

- **MQTT**
  - Description: a lightweight, publish-subscribe communication protocol designed for remote connections where bandwidth is limited. Widely used in *Internet of Things* (IoT) applications due to its efficiency in transmitting messages between devices over unstable or low-bandwidth networks.

- **TCP/IP**
  - Description: a suite of communication protocols used to interconnect devices on computer networks. TCP ensures reliable and ordered delivery of a stream of bytes between network applications, while IP handles the addressing and routing of data packets across interconnected networks, forming the foundation of the *Internet*.

- **UART**
  - Description: an asynchronous serial communication protocol used for data transmission between electronic devices. It converts parallel data from a device into serial format and vice versa, enabling point-to-point communication without the need for a synchronized clock.


---
## Programming Languages Used

The following programming languages were used in this project:

- **C/C++** used in the **Arduino IDE** to program the ESP microcontrollers;
- **Node-Red** to create the graphical interface and process the data received by the server;
- **Python** to explore alternative approaches for computer vision using microcontrollers.


---
## Software Used

The following software was used to carry out this project, in order to streamline the process:

- **XAMPP** to run the database, the broker, and the IoT dashboard on the remote server;
- **Mosquitto** as the MQTT broker on the remote server;
- **Arduino IDE** to program the microcontrollers.

---
## Authors

- **[Adriano Figueiredo](https://github.com/AdrianoFF10)**
  - Contribution: Camera Recognition and Database Communication
  - Information:
    - Email: adrianofigueiredo7@ua.pt

- **[Ricardo Bastos](https://github.com/RBastos36)**
  - Contribution: Sensoring and Communication Protocols
  - Information:
    - Email: r.bastos@ua.pt
   
- **Tiago Oliveira**
  - Contribution: Node-Red Interface
  - Information:
    - Email: tiago.oliv@ua.pt
