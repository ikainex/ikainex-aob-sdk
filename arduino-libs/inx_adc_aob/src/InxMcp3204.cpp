#include "InxMcp3204.h"

// ADC SPI Protocol Constants
#define INX_ADC_START_BIT      0x04
#define INX_ADC_SINGLE_ENDED   0x02

/**
 * @brief Constructor - Initialize InxMcp3204 object
 * @param cs_pin   Chip Select pin number
 * @param vref_mv  Reference voltage in millivolts (default: 5000mV)
 * @param spi_freq SPI frequency in Hz (default: 2MHz)
 */
InxMcp3204::InxMcp3204(uint8_t cs_pin, uint16_t vref_mv, uint32_t spi_freq)
    : _cs_pin(cs_pin),
      _vref_mv(vref_mv),
      _spi_freq(spi_freq),
      _initialized(false) {
}

/**
 * @brief Initialize the ADC driver
 * Sets up SPI bus and configures the CS pin
 * @return INX_STATUS_OK on success, INX_STATUS_ERR otherwise
 */
Inx_Mcp3204_Status InxMcp3204::begin() {
    if (_initialized) {
        return INX_STATUS_OK;
    }

    // Configure CS pin as output
    pinMode(_cs_pin, OUTPUT);
    digitalWrite(_cs_pin, HIGH);

    // Initialize SPI with specified frequency
    SPI.begin();
    setSPIFrequency(_spi_freq);

    _initialized = true;
    return INX_STATUS_OK;
}

/**
 * @brief Deinitialize the ADC driver
 */
void InxMcp3204::end() {
    if (_initialized) {
        SPI.end();
        _initialized = false;
    }
}

/**
 * @brief Set SPI frequency
 * @param freq Frequency in Hz
 */
void InxMcp3204::setSPIFrequency(uint32_t freq) {
    _spi_freq = freq;
    if (_initialized) {
        SPI.setClockDivider(SPI_CLOCK_DIV2);  // Adjust based on desired frequency
    }
}

/**
 * @brief Get reference voltage
 * @return Reference voltage in millivolts
 */
uint16_t InxMcp3204::getVref() const {
    return _vref_mv;
}

/**
 * @brief Internal SPI transfer - performs 3-byte SPI transaction
 * @param tx_data Pointer to 3-byte transmit buffer
 * @param rx_data Pointer to 3-byte receive buffer
 * @return Number of bytes transferred (3 on success)
 */
int InxMcp3204::spiTransfer(uint8_t *tx_data, uint8_t *rx_data) {
    if (!_initialized || !tx_data || !rx_data) {
        return 0;
    }

    // Assert CS (pull LOW)
    digitalWrite(_cs_pin, LOW);

    // Transfer 3 bytes
    for (int i = 0; i < 3; i++) {
        rx_data[i] = SPI.transfer(tx_data[i]);
    }

    // Deassert CS (pull HIGH)
    digitalWrite(_cs_pin, HIGH);

    return 3;
}

/**
 * @brief Extract 12-bit ADC value from SPI response
 * @param rx_data SPI response buffer (3 bytes)
 * @return 12-bit ADC value (bits 11-0)
 */
uint16_t InxMcp3204::extractADCValue(uint8_t *rx_data) {
    // ADC value is in bits 11-0 of rx_data[1] and rx_data[2]
    // rx_data[1] contains bits 11-8 (in lower 4 bits)
    // rx_data[2] contains bits 7-0
    return ((rx_data[1] & 0x0F) << 8) | rx_data[2];
}

/**
 * @brief Read raw ADC value from a single-ended channel
 * @param channel Channel to read (INX_MCP3204_CH0 to INX_MCP3204_CH3)
 * @param raw_value Pointer to store the raw ADC value (0-4095)
 * @return INX_STATUS_OK on success, INX_STATUS_ERR otherwise
 */
Inx_Mcp3204_Status InxMcp3204::readRaw(Inx_Mcp3204_Ch channel, uint16_t *raw_value) {
    if (!_initialized || !raw_value || channel > INX_MCP3204_CH3) {
        return INX_STATUS_INVALID_PARAM;
    }

    uint8_t tx_data[3] = {0};
    uint8_t rx_data[3] = {0};

    // Prepare SPI packet for single-ended read
    tx_data[0] = INX_ADC_START_BIT | INX_ADC_SINGLE_ENDED;
    tx_data[1] = ((uint8_t)channel << 6);
    tx_data[2] = 0x00;

    // Perform SPI transfer
    int transfer_length = spiTransfer(tx_data, rx_data);

    if (transfer_length != 3) {
        return INX_STATUS_ERR;
    }

    // Extract and store the 12-bit ADC value
    *raw_value = extractADCValue(rx_data);

    return INX_STATUS_OK;
}

/**
 * @brief Read ADC value in millivolts from a single-ended channel
 * @param channel Channel to read (INX_MCP3204_CH0 to INX_MCP3204_CH3)
 * @param voltage_mv Pointer to store the voltage in millivolts
 * @return INX_STATUS_OK on success, INX_STATUS_ERR otherwise
 */
Inx_Mcp3204_Status InxMcp3204::readVoltage(Inx_Mcp3204_Ch channel, uint16_t *voltage_mv) {
    if (!_initialized || !voltage_mv || channel > INX_MCP3204_CH3) {
        return INX_STATUS_INVALID_PARAM;
    }

    uint16_t adc_raw = 0;

    // Read raw ADC value
    Inx_Mcp3204_Status status = readRaw(channel, &adc_raw);
    if (status != INX_STATUS_OK) {
        return status;
    }

    // Convert raw value to millivolts using reference voltage
    *voltage_mv = (uint32_t)adc_raw * _vref_mv / INX_MCP3204_RESOLUTION;

    return INX_STATUS_OK;
}

/**
 * @brief Read differential ADC value in millivolts
 * @param channel Differential channel pair to read
 * @param voltage_mv Pointer to store the differential voltage in millivolts
 * @return INX_STATUS_OK on success, INX_STATUS_ERR otherwise
 */
Inx_Mcp3204_Status InxMcp3204::readDifferentialVoltage(Inx_Mcp3204_DiffCh channel, uint16_t *voltage_mv) {
    if (!_initialized || !voltage_mv || channel > INX_MCP3204_DIFF_CH32) {
        return INX_STATUS_INVALID_PARAM;
    }

    uint8_t tx_data[3] = {0};
    uint8_t rx_data[3] = {0};

    // Prepare SPI packet for differential read (no INX_ADC_SINGLE_ENDED flag)
    tx_data[0] = INX_ADC_START_BIT;
    tx_data[1] = ((uint8_t)channel << 6);
    tx_data[2] = 0x00;

    // Perform SPI transfer
    int transfer_length = spiTransfer(tx_data, rx_data);

    if (transfer_length != 3) {
        return INX_STATUS_ERR;
    }

    // Extract 12-bit ADC value
    uint16_t adc_raw = extractADCValue(rx_data);

    // Convert raw value to millivolts using reference voltage
    *voltage_mv = (uint32_t)adc_raw * _vref_mv / INX_MCP3204_RESOLUTION;

    return INX_STATUS_OK;
}
