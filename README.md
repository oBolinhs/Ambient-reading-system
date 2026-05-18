This project is a simple test of an **Ambient reading system** which covers medium difficulty ESP32 concepts such as 
// **Libraries:** The DHT11 library is used to read the humididty and temperature of the room, and the u8g2 library is used to display data on an external display, in this case an OLED
//**RTOS tasks** There are two tasks present in this code for the system to run independently, one for DHT11 reading (priority 1) and another one to print on the OLED screen (priority 2), both in the same core for modularity purposes
//**RTOS queues** Since the data type for the read and printed variables are the same I used a queue to pass the variables from one task to another.
