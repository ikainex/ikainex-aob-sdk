#include "adc_board.h"

#include <stdlib.h>

#define INX_ADC_BOARD_START_BIT      0x04
#define INX_ADC_BOARD_SINGLE_ENDED   0x02
#define INX_ADC_BOARD_RESOLUTION     4095

struct inx_adc_board_dev {
    SPI_HandleTypeDef *spi_handle;
    GPIO_TypeDef *cs_port;
    uint16_t cs_pin;
    uint16_t vref_mv;
};

static uint32_t stm32_spi_prescaler(SPI_TypeDef *instance, uint32_t baud_rate_hz)
{
    uint32_t peripheral_clock_hz =
        (instance == SPI2 || instance == SPI3) ?
        HAL_RCC_GetPCLK1Freq() : HAL_RCC_GetPCLK2Freq();

    if (peripheral_clock_hz == 0U || baud_rate_hz == 0U)
        return 0U;

    if (peripheral_clock_hz / 2U <= baud_rate_hz) return SPI_BAUDRATEPRESCALER_2;
    if (peripheral_clock_hz / 4U <= baud_rate_hz) return SPI_BAUDRATEPRESCALER_4;
    if (peripheral_clock_hz / 8U <= baud_rate_hz) return SPI_BAUDRATEPRESCALER_8;
    if (peripheral_clock_hz / 16U <= baud_rate_hz) return SPI_BAUDRATEPRESCALER_16;
    if (peripheral_clock_hz / 32U <= baud_rate_hz) return SPI_BAUDRATEPRESCALER_32;
    if (peripheral_clock_hz / 64U <= baud_rate_hz) return SPI_BAUDRATEPRESCALER_64;
    if (peripheral_clock_hz / 128U <= baud_rate_hz) return SPI_BAUDRATEPRESCALER_128;
    return SPI_BAUDRATEPRESCALER_256;
}

static HAL_StatusTypeDef adc_board_transfer(inx_adc_board_dev_t *dev,
                                             uint8_t tx_data[3],
                                             uint8_t rx_data[3])
{
    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_RESET);
    HAL_StatusTypeDef status =
        HAL_SPI_TransmitReceive(dev->spi_handle, tx_data, rx_data, 3U, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_SET);
    return status;
}

inx_adc_board_dev_t *inx_adc_board_open(const inx_adc_board_hw_desc_t *hw)
{
    if (!hw || !hw->spi_handle || !hw->spi_handle->Instance ||
        !hw->cs_port || !hw->cs_pin || (hw->cs_pin & (hw->cs_pin - 1U)) != 0U ||
        hw->spi_baud_rate_hz == 0U)
        return NULL;

    inx_adc_board_dev_t *dev = (inx_adc_board_dev_t *)calloc(1, sizeof(*dev));
    if (!dev)
        return NULL;

    dev->spi_handle = hw->spi_handle;
    dev->cs_port = hw->cs_port;
    dev->cs_pin = hw->cs_pin;
    dev->vref_mv = hw->vref_mv;

    SPI_HandleTypeDef *spi = dev->spi_handle;
    spi->Init.Mode = SPI_MODE_MASTER;
    spi->Init.Direction = SPI_DIRECTION_2LINES;
    spi->Init.DataSize = SPI_DATASIZE_8BIT;
    spi->Init.CLKPolarity = SPI_POLARITY_LOW;
    spi->Init.CLKPhase = SPI_PHASE_1EDGE;
    spi->Init.NSS = SPI_NSS_SOFT;
    spi->Init.BaudRatePrescaler =
        stm32_spi_prescaler(spi->Instance, hw->spi_baud_rate_hz);
    spi->Init.FirstBit = SPI_FIRSTBIT_MSB;
    spi->Init.TIMode = SPI_TIMODE_DISABLE;
    spi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    spi->Init.CRCPolynomial = 10U;

    if (spi->Init.BaudRatePrescaler == 0U || HAL_SPI_Init(spi) != HAL_OK) {
        free(dev);
        return NULL;
    }

    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_SET);
    GPIO_InitTypeDef gpio_init = {0};
    gpio_init.Pin = dev->cs_pin;
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(dev->cs_port, &gpio_init);

    return dev;
}

void inx_adc_board_close(inx_adc_board_dev_t *dev)
{
    if (!dev)
        return;

    HAL_GPIO_WritePin(dev->cs_port, dev->cs_pin, GPIO_PIN_SET);
    free(dev);
}

inx_adc_board_status_t inx_adc_board_read_raw(inx_adc_board_dev_t *dev,
                                               inx_adc_board_ch_t channel,
                                               uint16_t *adc_value)
{
    if (!dev || !adc_value || (uint32_t)channel > INX_ADC_BOARD_CH3)
        return INX_STATUS_ERR;

    uint8_t tx_data[3] = {
        INX_ADC_BOARD_START_BIT | INX_ADC_BOARD_SINGLE_ENDED,
        (uint8_t)((uint8_t)channel << 6),
        0x00
    };
    uint8_t rx_data[3] = {0};

    if (adc_board_transfer(dev, tx_data, rx_data) != HAL_OK)
        return INX_STATUS_ERR;

    *adc_value = (uint16_t)(((uint16_t)(rx_data[1] & 0x0F) << 8) | rx_data[2]);
    return INX_STATUS_OK;
}

inx_adc_board_status_t inx_adc_board_read_mv(inx_adc_board_dev_t *dev,
                                              inx_adc_board_ch_t channel,
                                              uint16_t *mv)
{
    if (!dev || !mv)
        return INX_STATUS_ERR;

    uint16_t adc_raw;
    inx_adc_board_status_t status = inx_adc_board_read_raw(dev, channel, &adc_raw);
    if (status != INX_STATUS_OK)
        return status;

    *mv = (uint16_t)((uint32_t)adc_raw * dev->vref_mv / INX_ADC_BOARD_RESOLUTION);
    return INX_STATUS_OK;
}

inx_adc_board_status_t inx_adc_board_read_differential_mv(inx_adc_board_dev_t *dev,
                                                           inx_adc_board_diff_ch_t channel,
                                                           uint16_t *mv)
{
    if (!dev || !mv || (uint32_t)channel > INX_ADC_BOARD_DIFF_CH32)
        return INX_STATUS_ERR;

    uint8_t tx_data[3] = {
        INX_ADC_BOARD_START_BIT,
        (uint8_t)((uint8_t)channel << 6),
        0x00
    };
    uint8_t rx_data[3] = {0};

    if (adc_board_transfer(dev, tx_data, rx_data) != HAL_OK)
        return INX_STATUS_ERR;

    uint16_t adc_raw = (uint16_t)(((uint16_t)(rx_data[1] & 0x0F) << 8) | rx_data[2]);
    *mv = (uint16_t)((uint32_t)adc_raw * dev->vref_mv / INX_ADC_BOARD_RESOLUTION);
    return INX_STATUS_OK;
}
