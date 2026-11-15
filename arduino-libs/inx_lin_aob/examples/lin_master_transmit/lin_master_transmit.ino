/*
 * LIN Add-on Board (Master) Example - Send and Receive Messages
 * 
 * This example demonstrates LIN master mode operation:
 * - Send messages to slave nodes
 * - Request data from slaves
 * - Process responses
 * 
 * Hardware Setup:
 * - TX pin: 1 (Arduino Uno hardware UART)
 * - RX pin: 0 (Arduino Uno hardware UART)
 * - LIN transceiver connected to TX/RX
 * 
 * For prints, Software Serial is used 
 * You would need a USB to TTL serial converter.
 * Connect D11 to RX pin of the serial converter.
 *
 * Note: Connect a LIN slave device or another Arduino in slave mode
 */

#include <InxMcp2003b.h>

#include <SoftwareSerial.h>

SoftwareSerial linBoardSerial(10, 11); // RX, TX -> For prints

// Create LIN master instance with Serial, tx pin 1, break width 13 bits
InxMcp2003b lin(Serial, 1, 0, 13);

// Message IDs used in this example
#define MSG_COMMAND     0x17U  // Master sends commands
#define MSG_STATUS      0x20  // Master requests status
#define MSG_CONTROL     0x30  // Master control message
#define WK_PIN          6
#define EN_PIN          10 
void setup() {

    linBoardSerial.begin(115200);
    
    digitalWrite(EN_PIN, HIGH);
    linBoardSerial.println("Lin Commander Start");
    uint16_t status = lin.beginMaster(10000);
    if (status != INX_MCP2003B_STATUS_OK) {
        while (1);  // Halt on initialization failure
    }

    // Enable LIN 2.0 enhanced checksum
    lin.setEnhancedChecksum(false);

    sendCommand();
}

void loop() {
    // // Example 1: Send a command message
    // sendCommand();
    delay(500);
    
    // // Example 2: Request status from slave
    // requestStatus();
    // delay(500);
    
    // // Example 3: Send control message
    // sendControl();
    // delay(500);
}

/**
 * Send a command message (master as transmitter)
 */
void sendCommand() {
    uint8_t cmd_data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    uint8_t dlc = sizeof(cmd_data);
    uint16_t result = lin.writeMessage(MSG_COMMAND, cmd_data, dlc);

    if (result != INX_MCP2003B_STATUS_OK) {
        linBoardSerial.println("Lin write message failed");
    }
    linBoardSerial.print("Sent Data: ");
    for (int i = 0; i < dlc; i++) {
        linBoardSerial.print(cmd_data[i], HEX); // Print the number without starting a new line
        if (i < dlc - 1) {
            linBoardSerial.print(", ");
        }
    }
    linBoardSerial.println();
    linBoardSerial.println("Lin write message Success");
}

/**
 * Request a status message from slave
 */

#if 0
void requestStatus() {
    // Send request header
    uint16_t result = lin.requestMessage(MSG_STATUS);
    if (result != INX_MCP2003B_STATUS_OK) {
        return;
    }

    // Wait for slave response
    Inx_Mcp2003b_Msg msg;
    result = lin.readMessageWait(&msg, 100);  // 100ms timeout

    if (result == INX_MCP2003B_STATUS_OK) {
        /* TODO: User can do application specific handling */
    }
}

/**
 * Send a control message with multiple parameters
 */
void sendControl() {
    // Prepare control data (e.g., PWM levels, on/off states, etc.)
    uint8_t ctrl_data[] = {
        0x50,        // Mode: 80
        0x75,        // PWM: 117 (75%)
        0x01,        // Enable: true
        0x00         // Reserved
    };

    uint16_t result = lin.writeMessage(MSG_CONTROL, ctrl_data, sizeof(ctrl_data));

    if (result != INX_MCP2003B_STATUS_OK) {
        // Handle transmit failure silently to avoid interfering with LIN timing.
    }
}
#endif