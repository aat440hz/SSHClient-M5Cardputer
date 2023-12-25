# SSH Terminal with M5Cardputer and ESP32

This project demonstrates how to create an SSH terminal using an M5Cardputer device and an ESP32 microcontroller. With this setup, you can remotely connect to an SSH server and interact with it using the M5Cardputer's keyboard and display.

## Prerequisites

Before you get started, make sure you have the following hardware and software installed:

- [M5Cardputer](https://m5stack.com/)
- ESP32 development environment
- [Arduino IDE](https://www.arduino.cc/en/software)
- Libraries:
  - M5Cardputer library
  - [libssh_esp32](https://github.com/ewpa/LibSSH-ESP32)
  - [libssh](https://www.libssh.org/)

## Installation

1. Clone or download this repository to your local machine.

2. Open the Arduino IDE, go to **File > Preferences**, and add the following URL to the "Additional Boards Manager URLs" field:

   https://dl.espressif.com/dl/package_esp32_index.json

3. Go to **Tools > Board > Boards Manager**, search for "esp32," and install the ESP32 board manager.

4. Select your M5Cardputer device from the **Tools > Board** menu.

5. Install the required libraries:
   - M5Cardputer library: Go to **Sketch > Include Library > M5Cardputer**
   - `libssh_esp32` and `libssh`: Download the libraries from their respective repositories and install them using **Sketch > Include Library > Add .ZIP Library**.

6. Configure your WiFi settings in the Arduino sketch:
   - Set your WiFi SSID and password.

7. Connect your M5Cardputer device to your computer, select the correct COM port, and upload the sketch.

## Usage

1. Power on your M5Cardputer device.

2. You should see the M5Cardputer screen display a terminal-like interface. Use the keyboard to type commands.

3. Connect to your SSH server by pressing Enter after typing the server address, username, and password.

4. You can now interact with your SSH server through the M5Cardputer terminal.

5. The terminal supports basic keyboard input, and you can send commands to the SSH server.

## Features

- Basic SSH terminal interface with M5Cardputer.
- Keyboard input and display for interacting with the SSH server.
- Ability to configure WiFi settings in the Arduino sketch.

## Contributing

If you'd like to contribute to this project, please open an issue or pull request on the GitHub repository.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- [M5Cardputer](https://m5stack.com/)
- [libssh_esp32](https://github.com/jonblack/libssh_esp32)
- [libssh](https://www.libssh.org/)
