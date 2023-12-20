#include <WiFi.h>
#include <M5Cardputer.h>
#include "libssh_esp32.h"
#include <libssh/libssh.h>

const char* ssid = "Your_SSID";
const char* password = "Your_Password";

// Function declarations
ssh_session connect_ssh(const char *host, const char *user, int verbosity);
int authenticate_console(ssh_session session, const char *password);
int verify_knownhost(ssh_session session);

// SSH Task Declaration
void sshTask(void *pvParameters);

void setup() {
    M5.begin(); 
    M5.Lcd.setRotation(1);
    M5.Lcd.print("Connecting to WiFi...\n");

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        M5.Lcd.print(".");
    }

    M5.Lcd.println("\nWiFi connected.");
    M5.Lcd.print("IP Address: ");
    M5.Lcd.println(WiFi.localIP());

    TaskHandle_t sshTaskHandle = NULL;
    xTaskCreatePinnedToCore(sshTask, "SSH Task", 20000, NULL, 1, &sshTaskHandle, 1); // Increased stack size

    if (sshTaskHandle == NULL) {
        Serial.println("Failed to create SSH Task");
    }
}

void loop() {
    M5.update();
    // The loop does not perform any operation in this case.
}

void sshTask(void *pvParameters) {
    ssh_session my_ssh_session = connect_ssh("192.168.0.188", "user", SSH_LOG_PROTOCOL);
    if (my_ssh_session != NULL) {
        M5.Lcd.println("SSH Connection established.");

        if (authenticate_console(my_ssh_session, "password") == SSH_OK) {
            M5.Lcd.println("SSH Authentication succeeded.");
            // Authentication successful
        } else {
            M5.Lcd.println("SSH Authentication failed.");
        }

        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
    } else {
        M5.Lcd.println("SSH Connection failed.");
    }

    vTaskDelete(NULL); // Delete the task when done
}

ssh_session connect_ssh(const char *host, const char *user, int verbosity) {
    ssh_session session = ssh_new();
    if (session == NULL) {
        Serial.println("Failed to create SSH session");
        return NULL;
    }

    ssh_options_set(session, SSH_OPTIONS_HOST, host);
    ssh_options_set(session, SSH_OPTIONS_USER, user);
    ssh_options_set(session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);

    if (ssh_connect(session) != SSH_OK) {
        Serial.print("Error connecting to host: ");
        Serial.println(ssh_get_error(session));
        ssh_free(session);
        return NULL;
    }

    return session;
}

int authenticate_console(ssh_session session, const char *password) {
    int rc = ssh_userauth_password(session, NULL, password);
    if (rc != SSH_AUTH_SUCCESS) {
        Serial.print("Error authenticating with password: ");
        Serial.println(ssh_get_error(session));
        return rc;
    }
    return SSH_OK;
}

int verify_knownhost(ssh_session session) {
    // Placeholder implementation for host verification
    return SSH_OK;
}
