#include <Arduino.h>
/*
    SPI Slave Demo Sketch
    Connect the SPI Master device to the following pins on the esp8266:
    GPIO    NodeMCU   Name  |   Uno
  ===================================
     15       D8       SS   |   D10
     13       D7      MOSI  |   D11
     12       D6      MISO  |   D12
     14       D5      SCK   |   D13
    Note: If the ESP is booting at a moment when the SPI Master has the Select line HIGH (deselected)
    the ESP8266 WILL FAIL to boot!
    See SPISlave_SafeMaster example for possible workaround
*/

#include "SPISlave.h"
#define _DEBUG_SPI_SLAVE // comment this to disable debug output
#define _DEBUG_SPI_SLAVE_MAX_PACKAGE_LEN 32  // real package is 32 bytes long, so max value for this is 32, should not be too big or it will block Rx process
void setup() {
  Serial.begin(115200);
  Serial.println("Start ESP");
  Serial.setDebugOutput(true);

  // data has been received from the master. Beware that len is always 32
  // and the buffer is autofilled with zeroes if data is less than 32 bytes long
  // It's up to the user to implement protocol for handling data length
  SPISlave.onData([](uint8_t * data, size_t len) {
    String message = String((char *)data);
    (void) len;
    #ifdef _DEBUG_SPI_SLAVE
    Serial.println("------------Data------------");
    Serial.println("CHAR: ");
    for (unsigned long i = 0; i < _DEBUG_SPI_SLAVE_MAX_PACKAGE_LEN; i++)
    {
      Serial.printf("%c  ", data[i]);
    }
    Serial.println();
    Serial.println("HEX:  ");
    for (unsigned long i = 0; i < _DEBUG_SPI_SLAVE_MAX_PACKAGE_LEN; i++)
    {
      Serial.printf("%2x ", data[i]);
    }
    Serial.println();
    #endif

  });

  // The master has read out outgoing data buffer
  // that buffer can be set with SPISlave.setData
  SPISlave.onDataSent([]() {
    #ifdef _DEBUG_SPI_SLAVE
    Serial.println("Answer Sent");
    #endif
  });

  // status has been received from the master.
  // The status register is a special register that bot the slave and the master can write to and read from.
  // Can be used to exchange small data or status information
  SPISlave.onStatus([](uint32_t data) {
    #ifdef _DEBUG_SPI_SLAVE
    Serial.printf("Status: %u\n", data);
    Serial.printf("Status(HEX): %x\n", data);
    SPISlave.setStatus(millis()); //set next status
    #endif
  });

  // The master has read the status register
  SPISlave.onStatusSent([]() {
    #ifdef _DEBUG_SPI_SLAVE
    Serial.println("Status Sent");
    #endif
  });

  // Setup SPI Slave registers and pins
  SPISlave.begin();

  // Set the status register (if the master reads it, it will read this value)
  SPISlave.setStatus(millis());

  // Sets the data registers. Limited to 32 bytes at a time.
  // SPISlave.setData(uint8_t * data, size_t len); is also available with the same limitation
  SPISlave.setData("Ask me a question!");
}

void loop() {}