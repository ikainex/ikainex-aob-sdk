#ifndef INX_MCP3204_H
#define INX_MCP3204_H

#include <stdint.h>
#include <SPI.h>

/**
 * @brief ADC Reference voltage definitions (in millivolts)
 */
#define INX_MCP3204_VREF_3300MV     3300
#define INX_MCP3204_VREF_4096MV     4096
#define INX_MCP3204_VREF_5000MV     5000

/**
 * @brief ADC Resolution (12-bit)
 */
#define INX_MCP3204_RESOLUTION      4095

/**
 * @brief ADC Status codes
 */
typedef enum {
    INX_STATUS_OK            = 0x00,
    INX_STATUS_ERR           = 0x01,
    INX_STATUS_INVALID_CH    = 0x02,
    INX_STATUS_INVALID_PARAM = 0x03
} Inx_Mcp3204_Status;

/**
 * @brief Single-ended channel selection
 */
typedef enum {
    INX_MCP3204_CH0 = 0,
    INX_MCP3204_CH1 = 1,
    INX_MCP3204_CH2 = 2,
    INX_MCP3204_CH3 = 3
} Inx_Mcp3204_Ch;

/**
 * @brief Differential channel selection
 */
typedef enum {
    INX_MCP3204_DIFF_CH01 = 0,  // CH0 - CH1
    INX_MCP3204_DIFF_CH10 = 1,  // CH1 - CH0
    INX_MCP3204_DIFF_CH23 = 2,  // CH2 - CH3
    INX_MCP3204_DIFF_CH32 = 3   // CH3 - CH2
} Inx_Mcp3204_DiffCh;

/**
 * @class InxMcp3204
 * @brief Arduino-compatible ADC Annex driver using SPI communication
 * 
 * This class provides interface to read analog values from an ADC chip
 * connected via SPI bus. It supports both single-ended and differential readings.
 */
class InxMcp3204 {
public:
    /**
     * @brief Constructor for InxMcp3204
     * @param cs_pin   Chip Select pin number
     * @param vref_mv  Reference voltage in millivolts
     * @param spi_freq SPI frequency in Hz (default: 2MHz)
     */
    InxMcp3204(uint8_t cs_pin, uint16_t vref_mv = INX_MCP3204_VREF_5000MV, uint32_t spi_freq = 2000000);

    /**
     * @brief Initialize the ADC driver
     * @return INX_STATUS_OK on success, INX_STATUS_ERR otherwise
     */
    Inx_Mcp3204_Status begin();

    /**
     * @brief Close/deinitialize the ADC driver
     */
    void end();

    /**
     * @brief Read raw ADC value from a single-ended channel
     * @param channel Channel to read (INX_MCP3204_CH0 to INX_MCP3204_CH3)
     * @param raw_value Pointer to store the raw ADC value (0-4095)
     * @return INX_STATUS_OK on success, INX_STATUS_ERR otherwise
     */
    Inx_Mcp3204_Status readRaw(Inx_Mcp3204_Ch channel, uint16_t *raw_value);

    /**
     * @brief Read ADC value in millivolts from a single-ended channel
     * @param channel Channel to read (INX_MCP3204_CH0 to INX_MCP3204_CH3)
     * @param voltage_mv Pointer to store the voltage in millivolts
     * @return INX_STATUS_OK on success, INX_STATUS_ERR otherwise
     */
    Inx_Mcp3204_Status readVoltage(Inx_Mcp3204_Ch channel, uint16_t *voltage_mv);

    /**
     * @brief Read differential ADC value in millivolts
     * @param channel Differential channel pair to read
     * @param voltage_mv Pointer to store the differential voltage in millivolts
     * @return INX_STATUS_OK on success, INX_STATUS_ERR otherwise
     */
    Inx_Mcp3204_Status readDifferentialVoltage(Inx_Mcp3204_DiffCh channel, uint16_t *voltage_mv);

    /**
     * @brief Set SPI frequency
     * @param freq SPI frequency in Hz
     */
    void setSPIFrequency(uint32_t freq);

    /**
     * @brief Get reference voltage
     * @return Reference voltage in millivolts
     */
    uint16_t getVref() const;

private:
    uint8_t _cs_pin;
    uint16_t _vref_mv;
    uint32_t _spi_freq;
    bool _initialized;

    /**
     * @brief Internal method to perform SPI transfer for ADC read
     * @param tx_data Pointer to transmit data (3 bytes)
     * @param rx_data Pointer to receive buffer (3 bytes)
     * @return Number of bytes transferred (3 on success)
     */
    int spiTransfer(uint8_t *tx_data, uint8_t *rx_data);

    /**
     * @brief Extract 12-bit ADC value from SPI response
     * @param rx_data SPI response buffer
     * @return 12-bit ADC value (0-4095)
     */
    uint16_t extractADCValue(uint8_t *rx_data);
};

#endif /* INX_MCP3204_H */
