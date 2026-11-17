/*
 * LIN Add-on Board (Slave) Example - Listen for a Master Frame
 *
 * This example waits for a LIN message from the master. When the received
 * payload matches the expected sequence, the onboard UNO LED is turned on.
 *
 * Hardware Setup:
 * - TX pin: 1 (Arduino Uno hardware UART)
 * - RX pin: 0 (Arduino Uno hardware UART)
 * - LIN transceiver connected to TX/RX
 */

#include <InxMcp2003b.h>
#include <SoftwareSerial.h>

SoftwareSerial linBoardSerial(10, 11); // RX, TX -> For prints

// Create LIN slave instance with Serial, tx pin 1, break width 13 bits
InxMcp2003b lin(Serial, 1, 0, 13);

// Message ID used by the master example
#define MSG_COMMAND 0x17U

// Expected payload from the master
static const uint8_t expected_data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

void setup() {

    linBoardSerial.begin(115200);

    linBoardSerial.println("Lin Responder Start");
    uint16_t status = lin.beginSlave(10000);
    if (status != INX_MCP2003B_STATUS_OK) {
        linBoardSerial.println("Lin Init Failed");
    }

    lin.setEnhancedChecksum(false);
}

void loop() {
    Inx_Mcp2003b_Msg msg;
    uint16_t result = lin.readMessageWait(&msg, 8, 2000);

    if (result == INX_MCP2003B_STATUS_OK && msg.id == MSG_COMMAND && msg.length == sizeof(expected_data)) {
        bool match = true;
        for (uint8_t i = 0; i < msg.length; ++i) {
            if (msg.data[i] != expected_data[i]) {
                match = false;
                break;
            }
        }

        linBoardSerial.print("Received Data: ");
        for (uint8_t i = 0; i < msg.length; i++) {
            linBoardSerial.print(msg.data[i], HEX); // Print the number without starting a new line
            if (i < msg.length - 1) {
                linBoardSerial.print(", ");
            }
        }
        linBoardSerial.println();
        if(match) {
            linBoardSerial.println("Received Data Matched: OK");
        } else {
            linBoardSerial.println("Received Data not Matched: FAIL");
        }
    }
}
