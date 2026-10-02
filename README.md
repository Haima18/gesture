# Gesture Control Robot 🤖✋

A gesture-controlled robotic system that allows a robot to be operated using hand gestures.

## 📌 Project Overview

This project demonstrates a wireless gesture-based control system for a robot.

The system consists of two main parts:

- **Gesture Controller** – reads the user's hand gestures and sends the corresponding control commands.
- **Robot Receiver** – receives the commands and controls the robot accordingly.

This project was developed as a practical robotics project to explore wireless communication, embedded systems, and gesture-based human-robot interaction.

## ⚙️ Working Principle

The system works in the following sequence:

1. The user performs a specific hand gesture.
2. The controller identifies the gesture.
3. A corresponding command is transmitted wirelessly.
4. The receiver on the robot receives the command.
5. The robot performs the required movement.

### Basic Flow

```text
Hand Gesture
     ↓
Gesture Controller
     ↓
Wireless Communication
     ↓
Robot Receiver
     ↓
Motor Control
     ↓
Robot Movement