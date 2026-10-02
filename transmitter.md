# Gesture Control Robot – Transmitter Code Explanation

The `gesture_control.ino` program runs on the ESP32 connected to the MPU6050 sensor. Its purpose is to detect the user's hand movement or tilt, convert that movement into a robot command, and wirelessly send the command to the second ESP32 mounted on the robot using ESP-NOW.

The program begins by including the required libraries. `Wire.h` is used for I2C communication between the ESP32 and MPU6050. `WiFi.h` provides the ESP32 wireless functionality required by ESP-NOW, while `esp_now.h` provides the ESP-NOW communication functions. `Adafruit_MPU6050.h` provides functions for communicating with the MPU6050 sensor, and `Adafruit_Sensor.h` provides the common sensor interface used to obtain sensor measurements.

The MPU6050 is connected to the ESP32 using I2C. The code defines GPIO 25 as the SDA pin using `#define SDA_PIN 25` and GPIO 26 as the SCL pin using `#define SCL_PIN 26`. SDA is responsible for carrying the data between the ESP32 and MPU6050, while SCL provides the I2C clock signal.

The code then stores the MAC address of the robot ESP32 in the array `robotMAC`. The MAC address used in this project is `98:F4:AB:09:61:B4`. This address identifies the receiving ESP32, allowing the transmitter to send the commands to the correct robot.

Two settings are then defined. `THRESHOLD` is set to `3.0` and determines how much acceleration is required before a movement is recognized as a gesture. `READ_DELAY` is set to `150` milliseconds and controls the delay between successive sensor readings and command transmissions.

An object called `mpu` is created from the `Adafruit_MPU6050` class. This object is used throughout the program to initialize the MPU6050 and read its sensor data.

The program then defines a structure called `Message`. This structure contains a single character variable named `command`. The command represents the movement that should be performed by the robot. The available commands are `F` for Forward, `B` for Backward, `L` for Left, `R` for Right, and `S` for Stop. A variable called `message` is then created using this structure so that the detected command can be stored before transmission.

The `setup()` function runs once whenever the ESP32 is powered on or reset. First, `Serial.begin(115200)` starts serial communication at 115200 baud so that information and debugging messages can be displayed in the Serial Monitor. A one-second delay is then provided using `delay(1000)` to allow the system to initialize.

The I2C communication is started using `Wire.begin(SDA_PIN, SCL_PIN)`. Since the previously defined pins are GPIO 25 and GPIO 26, the ESP32 communicates with the MPU6050 through those pins.

The program then attempts to initialize the MPU6050 using `mpu.begin()`. If the sensor is not detected, the program prints `MPU6050 NOT FOUND - check wiring` to the Serial Monitor and enters an infinite loop. This prevents the program from continuing when the sensor is not properly connected.

After successful initialization, the accelerometer range is set to ±8 G using `mpu.setAccelerometerRange(MPU6050_RANGE_8_G)`. The gyroscope range is set to ±500 degrees per second using `mpu.setGyroRange(MPU6050_RANGE_500_DEG)`. The MPU6050 filter bandwidth is then set to 21 Hz using `mpu.setFilterBandwidth(MPU6050_BAND_21_HZ)`. These settings configure how the sensor measures and processes movement.

Next, the ESP32 is configured in Wi-Fi Station mode using `WiFi.mode(WIFI_STA)`. This configuration is required for ESP-NOW communication.

The program then initializes ESP-NOW using `esp_now_init()`. If initialization fails, the message `ESP-NOW INIT FAILED` is printed to the Serial Monitor and the program stops in an infinite loop.

An ESP-NOW peer information structure called `peerInfo` is then created. The robot's six-byte MAC address is copied into `peerInfo.peer_addr` using `memcpy()`. The communication channel is set to `0`, and encryption is disabled using `peerInfo.encrypt = false`. The robot is then added as an ESP-NOW peer using `esp_now_add_peer(&peerInfo)`. If the robot cannot be added, the program displays `FAILED TO ADD ROBOT` and stops.

After all initialization steps are completed successfully, the program prints `READY` to the Serial Monitor, indicating that the transmitter is ready to detect gestures and communicate with the robot.

The `loop()` function then starts running repeatedly. First, three sensor event variables are created: `a` for acceleration, `g` for gyroscope data, and `t` for temperature. The command `mpu.getEvent(&a, &g, &t)` reads the latest measurements from the MPU6050 and stores them in these variables.

The program then extracts the acceleration values from the X and Y axes. `float X = a.acceleration.x` stores the X-axis acceleration, while `float Y = a.acceleration.y` stores the Y-axis acceleration. These two values are used to determine the direction in which the controller has been tilted.

A character variable called `command` is initially set to `S`, meaning Stop. The program then checks the acceleration values against the defined threshold.

If `Y` is greater than `THRESHOLD`, the command is changed to `F`, meaning Forward. If `Y` is less than negative `THRESHOLD`, the command becomes `B`, meaning Backward. If `X` is greater than `THRESHOLD`, the command becomes `R`, meaning Right. If `X` is less than negative `THRESHOLD`, the command becomes `L`, meaning Left.

Therefore, the gesture detection logic can be summarized as follows:

- `Y > 3.0` → `F` → Forward
- `Y < -3.0` → `B` → Backward
- `X > 3.0` → `R` → Right
- `X < -3.0` → `L` → Left
- No threshold crossed → `S` → Stop

The detected command is then stored inside the message structure using `message.command = command`.

The program sends this message to the robot ESP32 using `esp_now_send()`. The robot MAC address is supplied as the destination, the message is converted into a byte array, and `sizeof(message)` specifies the amount of data that needs to be transmitted. The result of the transmission is stored in the variable `result`.

The transmitter then displays useful information in the Serial Monitor. It prints the current X acceleration, Y acceleration, detected command, and whether the ESP-NOW transmission was successful. For example, an output such as `X: 0.25 | Y: 3.42 | Command: F | Sent` means that the Y-axis acceleration exceeded the threshold, the transmitter detected a Forward command, and the command was successfully sent to the robot.

The expression `result == ESP_OK ? " | Sent" : " | Send FAILED"` uses the conditional operator to display `Sent` when the ESP-NOW transmission is successful and `Send FAILED` when the transmission fails.

Finally, `delay(READ_DELAY)` pauses the program for 150 milliseconds before the next sensor reading. The `loop()` function then starts again, continuously reading the MPU6050, detecting the gesture, creating a command, transmitting it through ESP-NOW, and displaying the result.

Overall, the transmitter acts as the human interface of the robot. The user's hand movement changes the orientation of the MPU6050, the sensor provides X and Y acceleration values, the ESP32 interprets those values as a movement command, and ESP-NOW sends the command wirelessly to the robot ESP32. The transmitter itself does not control the motors; it only detects gestures and sends the corresponding commands to the receiver.