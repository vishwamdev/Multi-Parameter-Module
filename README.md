# IoT Based Multi-Parameter Module (MPM)
This project involves the development of an IoT-based real-time monitoring system using an Arduino UNO microcontroller. The system is designed to measure various environmental and electrical parameters and send the data to ThingSpeak for visualization with real-time graphs.

# Components Used
* __Microcontroller__: Arduino UNO
* **Display**: HX1230 LCD
* __Sensors__:
  * BMP280 Sensor _(Temperature, Humidity, Pressure, Altitude)_
  * Voltage Sensor
  * ACS712 Current Sensor
  * INA219 Sensor _(Voltage and Current Measurement)_
  * NEO 6M GPS Module _(Future Integration for Real-Time Location)_
# Key Features
* **IoT-Based Monitoring**: The system sends measured data to ThingSpeak, allowing for real-time monitoring and visualization through graphs.
* **Real-Time Monitoring**: Users can observe real-time data such as temperature, humidity, pressure, altitude, and voltage.
* **User-Friendly Interface**: Two push buttons are included for shifting between the displayed parameters on the HX1230 LCD screen.
# Measured Parameters
* **Temperature**: Measured using the BMP280 sensor.
* **Humidity**: Measured using the BMP280 sensor.
* **Pressure**: Measured using the BMP280 sensor.
* **Altitude**: Measured using the BMP280 sensor.
* **Voltage**: Measured using the Voltage Sensor and INA219 sensor.
* **Current _(Future Improvement)_**: Will be measured using the ACS712 Current Sensor and INA219 sensor.
* **Location _(Future Improvement)_**: Will be integrated using the NEO 6M GPS Module.
* **Date & Time _(Future Improvement)_**: Will be integrated with real-time clock modules or GPS data.
# Future Improvements
* **Real-Time Location Tracking**: Integration of the NEO 6M GPS module for location tracking.
* **Date & Time Display**: Adding functionality to display the current date and time.
* **Current Measurement**: Additional sensors for accurate current measurements.
# Project Mechanism
The system uses an Arduino UNO as the core microcontroller. Various sensors are connected to the Arduino to measure parameters like temperature, humidity, pressure, altitude, voltage, and current. The HX1230 LCD is used to display these parameters, and two push buttons are provided to shift among the different displayed values. Data is transmitted to ThingSpeak for visualization, providing users with real-time graphs and insights.


# How to Use
1) Connect the Components: Follow the circuit diagram provided in the repository to connect all sensors and the display to the Arduino UNO.
2) Upload the Code: Use the Arduino IDE to upload the code to the Arduino UNO board.
3) View Real-Time Data: Monitor the parameters on the HX1230 LCD and observe the real-time graphs on ThingSpeak.

**You can view real-time data from the system on the ThingSpeak channel here**: _https://thingspeak.com/channels/2624710_
# Contribution
_Contributions are welcome! If you have ideas for improvement or want to fix bugs, feel free to fork the repository and create a pull request._

_Feel free to explore the code and contribute to further improvements!_
