# Gesture Control Robot – Receiver/Robot Code Explanation

The `gesture_control_receiver.ino` program runs on the ESP32 mounted on the robot. Its main purpose is to receive movement commands wirelessly from the transmitter ESP32 using ESP-NOW and control the two DC motors through the L298N motor driver. The receiver converts the commands `F`, `B`, `L`, `R`, and `S` into the corresponding robot movements.

The program begins by including `WiFi.h`, which provides the Wi-Fi functionality required by ESP-NOW, and `esp_now.h`, which provides the functions required for ESP-NOW wireless communication.

The next section defines the motor control pins. The left motor is connected to the L298N Motor A output and uses GPIO 25 as its enable/PWM pin, GPIO 26 as `IN1`, and GPIO 27 as `IN2`. The right motor is connected to Motor B and uses GPIO 13 as its enable/PWM pin, GPIO 14 as `IN3`, and GPIO 12 as `IN4`.

The enable pins `ENA` and `ENB` control the speed of the motors using PWM. The direction pins control the direction in which each motor rotates. By changing the HIGH and LOW states of the direction pins, the ESP32 can make each motor rotate in either direction.

The program then defines two settings. `SPEED` is set to `170`. Since the ESP32 uses 8-bit PWM in this program, the possible PWM duty-cycle values range from 0 to 255. Therefore, a value of 170 determines the motor speed used for the robot's movements. `COMMAND_TIMEOUT` is set to 1000 milliseconds, which provides a safety mechanism. If the robot does not receive a new command for more than one second, it automatically stops.

The same `Message` structure used by the transmitter is defined on the receiver. It contains a single character variable called `command`. This is important because both ESP32 boards must use the same data structure when communicating with each other.

A `Message` variable called `message` is then created and initialized with the command `S`. This means the robot starts in the Stop state.

The variable `lastCommandTime` stores the time at which the most recent valid command was received. It is used later to implement the communication safety timeout.

The program then defines individual functions for controlling the motors. The `leftCW()` function makes the left motor rotate in the direction defined as clockwise by the wiring and then applies the PWM speed value using `ledcWrite(ENA, SPEED)`. The `leftCCW()` function reverses the direction of the left motor by changing the states of `IN1` and `IN2`.

Similarly, `rightCW()` controls the right motor in one direction and `rightCCW()` controls it in the opposite direction. The actual physical forward direction depends on how the motors are wired to the L298N, but these functions provide the two possible rotation directions for each motor.

The `stopRobot()` function is responsible for completely stopping the robot. It sets all four motor direction pins to `LOW`, which removes the direction signal from both motors. It then sets the PWM value of both enable pins to zero using `ledcWrite(ENA, 0)` and `ledcWrite(ENB, 0)`. Therefore, both motors stop.

The program then combines the individual motor functions to create complete robot movements. `forwardRobot()` calls `leftCW()` and `rightCW()`, causing both motors to rotate in their forward direction. `backwardRobot()` calls `leftCCW()` and `rightCCW()`, causing both motors to rotate in the opposite direction.

For turning left, `leftRobot()` makes the left motor rotate in one direction and the right motor rotate in the opposite direction. This creates a turning motion for the robot. Similarly, `rightRobot()` reverses the motor directions to create a right turn.

The next section handles ESP-NOW data reception. The `OnDataRecv()` function is a callback function that is automatically called by the ESP-NOW system whenever data is received from the transmitter ESP32.

The function receives three important parameters. `info` contains information about the ESP-NOW sender, `incomingData` contains the received data, and `len` specifies the length of the received data. The `info` parameter is included as part of the ESP-NOW callback interface, although it is not directly used in this program.

The first check inside the callback is `if (len != sizeof(Message)) return;`. This verifies that the received data has exactly the expected size. If the size does not match the `Message` structure, the function immediately returns and ignores the data. This helps prevent the robot from processing unexpected data.

The received bytes are then copied into the `message` variable using `memcpy()`. This converts the received data back into the `Message` structure so that the robot can access the command character.

The program then updates `lastCommandTime` using `millis()`. This records the time at which the latest command was received and is later used by the safety timeout.

For debugging, the received command is also printed to the Serial Monitor. For example, if the transmitter sends `F`, the receiver will display `Received: F`.

The `setup()` function runs once when the robot ESP32 starts. It begins serial communication at 115200 baud using `Serial.begin(115200)`.

The four motor direction pins are then configured as outputs using `pinMode()`. This allows the ESP32 to control the L298N direction inputs.

The PWM channels for the two motor enable pins are configured using `ledcAttach(ENA, 1000, 8)` and `ledcAttach(ENB, 1000, 8)`. The PWM frequency is set to 1000 Hz and the resolution is set to 8 bits. With 8-bit resolution, the PWM duty-cycle range is 0 to 255.

The `stopRobot()` function is called immediately after configuring the motors. This ensures that the robot starts in a stopped condition rather than accidentally moving when powered on.

The ESP32 is then configured in Wi-Fi Station mode using `WiFi.mode(WIFI_STA)`, which is required for ESP-NOW communication.

The program initializes ESP-NOW using `esp_now_init()`. If initialization fails, the message `ESP-NOW INIT FAILED!` is printed to the Serial Monitor and the setup function returns.

The receiver then registers the `OnDataRecv()` function as the ESP-NOW receive callback using `esp_now_register_recv_cb(OnDataRecv)`. From this point onward, whenever a valid ESP-NOW packet arrives, the `OnDataRecv()` function is automatically executed.

The message `ROBOT READY` is then printed to the Serial Monitor to indicate that the robot ESP32 has successfully initialized.

`lastCommandTime` is initialized using `millis()` so that the timeout system starts counting from the moment the robot becomes ready.

The `loop()` function then runs continuously. The first thing it does is check whether the robot has received a command recently.

The condition `if (millis() - lastCommandTime > COMMAND_TIMEOUT)` calculates how much time has passed since the last received command. If more than 1000 milliseconds have passed, `stopRobot()` is called and the robot stops. The `return` statement then exits the current loop iteration. This safety feature prevents the robot from continuing to move if the wireless connection is interrupted or the transmitter stops sending commands.

If a recent command has been received, the current command is copied from `message.command` into the variable `cmd`.

The program then checks the value of `cmd`. If it is `F`, the `forwardRobot()` function is called. If it is `B`, `backwardRobot()` is called. If it is `L`, `leftRobot()` is called. If it is `R`, `rightRobot()` is called. If the command does not match any of these values, `stopRobot()` is called.

A 20-millisecond delay is then used before the loop repeats. This allows the robot to continuously process the most recently received command while keeping the motor control responsive.

Overall, the receiver acts as the control unit of the robot. It receives the wireless command generated by the transmitter, checks that the received data is valid, updates the latest command time, and converts the command into motor movements through the L298N motor driver.

The complete communication flow is:

**MPU6050 → Transmitter ESP32 → ESP-NOW → Receiver ESP32 → L298N Motor Driver → DC Motors → Robot Movement**

The transmitter detects the user's hand movement and sends a single-character command. The receiver interprets that command and controls the left and right motors accordingly. The one-second command timeout provides an additional safety mechanism by automatically stopping the robot when communication is lost.