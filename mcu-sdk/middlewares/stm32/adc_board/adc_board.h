#ifndef INX_ADC_BOARD_H
#define INX_ADC_BOARD_H

#include <stdint.h>
#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Annex reference voltage definition.
 */
#define INX_ADC_BOARD_VREF_3300MV     3300
#define INX_ADC_BOARD_VREF_4096MV     4096
#define INX_ADC_BOARD_VREF_5000MV     5000

/**
  * @brief  Annex Status structures definition
  */
typedef enum {
  INX_STATUS_OK           = 0x00U,
  INX_STATUS_ERR          = 0x01U,
  INX_ADC_BOARD_STATUS_SPI_BUSY     = 0x02U,
  INX_ADC_BOARD_STATUS_SPI_TIMEOUT  = 0x03U
} inx_adc_board_status_t;

typedef struct {
    SPI_HandleTypeDef *spi_handle;
    GPIO_TypeDef *cs_port;
    uint16_t cs_pin;
    uint32_t spi_baud_rate_hz;
    uint16_t   vref_mv;             /* Voltage reference in mv         */
} inx_adc_board_hw_desc_t;

typedef enum {
    INX_ADC_BOARD_CH0 = 0,
    INX_ADC_BOARD_CH1 = 1,
    INX_ADC_BOARD_CH2 = 2,
    INX_ADC_BOARD_CH3 = 3
} inx_adc_board_ch_t;

typedef enum {
    INX_ADC_BOARD_DIFF_CH01 = 0,
    INX_ADC_BOARD_DIFF_CH10 = 1,
    INX_ADC_BOARD_DIFF_CH23 = 2,
    INX_ADC_BOARD_DIFF_CH32 = 3
} inx_adc_board_diff_ch_t;

typedef struct inx_adc_board_dev inx_adc_board_dev_t;

inx_adc_board_dev_t *inx_adc_board_open(const inx_adc_board_hw_desc_t *hw);
void inx_adc_board_close(inx_adc_board_dev_t *dev);
inx_adc_board_status_t inx_adc_board_init(inx_adc_board_dev_t *dev);
inx_adc_board_status_t inx_adc_board_read_raw(inx_adc_board_dev_t *dev, inx_adc_board_ch_t channel, uint16_t *adc_value);
inx_adc_board_status_t inx_adc_board_read_mv(inx_adc_board_dev_t *dev, inx_adc_board_ch_t channel, uint16_t *mv);
inx_adc_board_status_t inx_adc_board_read_differential_mv(inx_adc_board_dev_t *dev, inx_adc_board_diff_ch_t channel, uint16_t *mv);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* INX_ADC_BOARD_H */
