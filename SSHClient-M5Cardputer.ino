#include <WiFi.h>
#include <M5Cardputer.h>
#include "libssh_esp32.h"
#include <libssh/libssh.h>

const char* ssid = "SETUP-8CD3"; // Replace with your WiFi SSID
const char* password = "career6174brace"; // Replace with your WiFi password

// SSH server configuration (initialize as empty strings)
String ssh_host = "";
String ssh_user = "";
String ssh_password = "";

// M5Cardputer setup
M5Canvas canvas(&M5Cardputer.Display);
String commandBuffer = "> ";
int cursorY = 0;
const int lineHeight = 32;
unsigned long lastKeyPressMillis = 0;
const unsigned long debounceDelay = 200; // Adjust debounce delay as needed

ssh_session my_ssh_session;
ssh_channel channel;

void setup() {
    Serial.begin(115200); // Initialize serial communication for debugging
    Serial.println("Starting Setup");

    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true);
    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.setTextSize(1); // Set text size

    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected");

    // Initialize the cursor Y position
    cursorY = M5Cardputer.Display.getCursorY();

    // Prompt for SSH host, username, and password
    M5Cardputer.Display.print("SSH Host: ");
    waitForInput(ssh_host);
    M5Cardputer.Display.print("\nSSH Username: ");
    waitForInput(ssh_user);
    M5Cardputer.Display.print("\nSSH Password: ");
    waitForInput(ssh_password);

    // Connect and authenticate with SSH server
    my_ssh_session = ssh_new();
    if (my_ssh_session == NULL) {
        Serial.println("SSH Session creation failed.");
        return;
    }
    ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, ssh_host.c_str());
    ssh_options_set(my_ssh_session, SSH_OPTIONS_USER, ssh_user.c_str());

    if (ssh_connect(my_ssh_session) != SSH_OK) {
        Serial.println("SSH Connect error.");
        ssh_free(my_ssh_session);
        return;
    }

    if (ssh_userauth_password(my_ssh_session, NULL, ssh_password.c_str()) != SSH_AUTH_SUCCESS) {
        Serial.println("SSH Authentication error.");
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        return;
    }

    channel = ssh_channel_new(my_ssh_session);
    if (channel == NULL || ssh_channel_open_session(channel) != SSH_OK) {
        Serial.println("SSH Channel open error.");
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        return;
    }

    if (ssh_channel_request_pty(channel) != SSH_OK) {
        Serial.println("SSH PTY request error.");
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        return;
    }

    if (ssh_channel_request_shell(channel) != SSH_OK) {
        Serial.println("SSH Shell request error.");
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        return;
    }

    Serial.println("SSH setup completed.");
}

void loop() {
    M5Cardputer.update();

    // Handle keyboard input with debounce
    if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
        unsigned long currentMillis = millis();
        if (currentMillis - lastKeyPressMillis >= debounceDelay) {
            lastKeyPressMillis = currentMillis;
            Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

            for (auto i : status.word) {
                commandBuffer += i;
                M5Cardputer.Display.print(i);
                cursorY = M5Cardputer.Display.getCursorY();
            }

            if (status.del && commandBuffer.length() > 2) {
                commandBuffer.remove(commandBuffer.length() - 1);
                M5Cardputer.Display.setCursor(M5Cardputer.Display.getCursorX() - 6, M5Cardputer.Display.getCursorY());
                M5Cardputer.Display.print(" ");
                M5Cardputer.Display.setCursor(M5Cardputer.Display.getCursorX() - 6, M5Cardputer.Display.getCursorY());
                cursorY = M5Cardputer.Display.getCursorY();
            }

            if (status.enter) {
                String message = commandBuffer.substring(2) + "\r\n";
                ssh_channel_write(channel, message.c_str(), message.length());

                commandBuffer = "> ";
                M5Cardputer.Display.print('\n');
                cursorY = M5Cardputer.Display.getCursorY();
            }
        }
    }

    // Check if the cursor has reached the bottom of the display
    if (cursorY > M5Cardputer.Display.height() - lineHeight) {
        M5Cardputer.Display.scroll(0, -lineHeight);
        cursorY -= lineHeight;
        M5Cardputer.Display.setCursor(M5Cardputer.Display.getCursorX(), cursorY);
    }

    // Read data from SSH server and display it
    char buffer[128]; // Reduced buffer size for less memory usage
    int nbytes = ssh_channel_read_nonblocking(channel, buffer, sizeof(buffer), 0);
    if (nbytes > 0) {
        for (int i = 0; i < nbytes; ++i) {
            if (buffer[i] == '\r') {
                continue; // Ignore carriage return
            }
            M5Cardputer.Display.write(buffer[i]);
            cursorY = M5Cardputer.Display.getCursorY();
        }
    }

    // Handle channel closure and other conditions
    if (nbytes < 0 || ssh_channel_is_closed(channel)) {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        M5Cardputer.Display.println("\nSSH session closed.");
        return; // Exit the loop upon session closure
    }
}

void waitForInput(String& input) {
    unsigned long startTime = millis();
    unsigned long lastKeyPressMillis = 0;
    const unsigned long debounceDelay = 200; // Adjust debounce delay as needed
    String currentInput = input;

    while (true) {
        M5Cardputer.update();
        if (M5Cardputer.Keyboard.isChange()) {
            Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
            
            if (status.del && currentInput.length() > 0) {
                // Handle backspace key
                currentInput.remove(currentInput.length() - 1);
                M5Cardputer.Display.setCursor(M5Cardputer.Display.getCursorX() - 6, M5Cardputer.Display.getCursorY());
                M5Cardputer.Display.print(" ");
                M5Cardputer.Display.setCursor(M5Cardputer.Display.getCursorX() - 6, M5Cardputer.Display.getCursorY());
                cursorY = M5Cardputer.Display.getCursorY();
                lastKeyPressMillis = millis();
            }

            for (auto i : status.word) {
                currentInput += i;
                M5Cardputer.Display.print(i);
                cursorY = M5Cardputer.Display.getCursorY();
                lastKeyPressMillis = millis();
            }

            if (status.enter) {
                M5Cardputer.Display.println(); // Move to the next line
                input = currentInput;
                break;
            }
        }

        if (millis() - startTime > 60000) { // Timeout after 60 seconds
            M5Cardputer.Display.println("\nInput timeout. Rebooting...");
            delay(1000); // Delay for 1 second to allow the message to be displayed
            ESP.restart(); // Reboot the ESP32
        }

        // Debounce delay for the keyboard input
        if (millis() - lastKeyPressMillis >= debounceDelay) {
            lastKeyPressMillis = millis();
        }
    }
}
