#ifndef INX_MCP3204_H
#define INX_MCP3204_H

#include <stdint.h>
#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Annex reference voltage definition.
 */
#define INX_MCP3204_VREF_3300MV     3300
#define INX_MCP3204_VREF_4096MV     4096
#define INX_MCP3204_VREF_5000MV     5000

/**
  * @brief  Annex Status structures definition
  */
typedef enum {
  INX_STATUS_OK           = 0x00U,
  INX_STATUS_ERR          = 0x01U,
  INX_MCP3204_STATUS_SPI_BUSY     = 0x02U,
  INX_MCP3204_STATUS_SPI_TIMEOUT  = 0x03U
} inx_mcp3204_status_t;

typedef struct {
    SPI_HandleTypeDef *spi_handle;
    GPIO_TypeDef *cs_port;
    uint16_t cs_pin;
    uint32_t spi_baud_rate_hz;
    uint16_t   vref_mv;             /* Voltage reference in mv         */
} inx_mcp3204_hw_desc_t;

typedef enum {
    INX_MCP3204_CH0 = 0,
    INX_MCP3204_CH1 = 1,
    INX_MCP3204_CH2 = 2,
    INX_MCP3204_CH3 = 3
} inx_mcp3204_ch_t;

typedef enum {
    INX_MCP3204_DIFF_CH01 = 0,
    INX_MCP3204_DIFF_CH10 = 1,
    INX_MCP3204_DIFF_CH23 = 2,
    INX_MCP3204_DIFF_CH32 = 3
} inx_mcp3204_diff_ch_t;

typedef struct inx_mcp3204_dev inx_mcp3204_dev_t;

inx_mcp3204_dev_t *inx_mcp3204_open(const inx_mcp3204_hw_desc_t *hw);
void inx_mcp3204_close(inx_mcp3204_dev_t *dev);
inx_mcp3204_status_t inx_mcp3204_init(inx_mcp3204_dev_t *dev);
inx_mcp3204_status_t inx_mcp3204_read_raw(inx_mcp3204_dev_t *dev, inx_mcp3204_ch_t channel, uint16_t *adc_value);
inx_mcp3204_status_t inx_mcp3204_read_mv(inx_mcp3204_dev_t *dev, inx_mcp3204_ch_t channel, uint16_t *mv);
inx_mcp3204_status_t inx_mcp3204_read_differential_mv(inx_mcp3204_dev_t *dev, inx_mcp3204_diff_ch_t channel, uint16_t *mv);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* INX_MCP3204_H */
