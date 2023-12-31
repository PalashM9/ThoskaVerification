# ESP32 OCR Project

## Overview
This project explores the implementation of OCR on the ESP32 microcontroller. The goal is to develop a system capable of recognizing and extracting text from images, specifically matriculation numbers on ID cards. The document provides a comprehensive analysis of different approaches to OCR on ESP32, discussing their feasibility, limitations, and technical details.

<img width="415" alt="image" src="https://github.com/PalashM9/ThoskaVerification/assets/100582448/da8a8e5c-12e3-438d-8d46-ab6060891706">

## Project Structure
- **OCR on Microcontrollers**: Discusses general requirements, feasibility, and challenges.
- **Implementation Research**: Various approaches, including PyTesseract, Cloud-Based OCR, Keras Model Training, Compiled Libraries, and Local Models.
- **Final Implementation**: Details the selected approach, tools used, architecture, and steps involved in the project.

## Technologies Used
- Arduino IDE
- EdgeImpulse SDK
- ESP32-OV2640 Camera Module
- SD-Card for data storage

## Development and Deployment
The project underwent several stages:
1. Creating a custom frame for the ESP32 and Thoska card.
2. Collecting and labeling a dataset.
3. Feature extraction and model training.
4. Model evaluation and refinement.
5. Deployment on ESP32 for real-time digit recognition.

## Challenges and Solutions
The document highlights various challenges encountered during implementation and how they were addressed, particularly focusing on the limitations of the ESP32 microcontroller.

## Applications
Potential applications of this project include access control, attendance tracking, and identity verification. It demonstrates the capabilities of ESP32 microcontrollers in computer vision.

For more information on the project's progress and insights, refer to the detailed document provided.
