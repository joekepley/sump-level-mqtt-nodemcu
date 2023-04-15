# Liquid Level monitoring with an ESP8266-powered NodeMCU WEMOS D1 Mini

This project was initially developed to feed the water level in a sump hole to MQTT. Boat fuel sensors are relatively inexpensive and available in many lengths. Most are simply a stainless steel rod with an attached float that acts as a variable resistor. By connecting this to the analog read pin of an ESP8266 chip, you can easily read the value, map it to physical height, and send the result to MQTT.

I've built this using a [100Tech Fuel Sending Unit][1], which has a resistance range of 33-240 ohms and comes in a number of sizes, but nearly any resistive level gauge should work. You will need to select different values for the resistors in the circuit if your sender uses different resistance values, in order to get the input voltage into a range that the ESP8266 can read properly.

[1]: https://a.co/d/7ozJsie
