#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "inx_mcp2003b.h"

struct inx_mcp2003b_dev {
    UART_HandleTypeDef *uart_handle;
    uint32_t flags;
    uint32_t last_request_id;
    uint8_t last_request_pid;   /* protected ID of the last header sent,
                                   needed for the enhanced checksum */
    
};

static uint8_t lin_compute_protected_id(uint8_t id)
{
    uint8_t p0 = ((id >> 0) ^ (id >> 1) ^ (id >> 2) ^ (id >> 4)) & 0x01;
    uint8_t p1 = (~((id >> 1) ^ (id >> 3) ^ (id >> 4) ^ (id >> 5)) & 0x01);
    return (uint8_t)(id | (p0 << 6) | (p1 << 7));
}

static bool lin_validate_protected_id(uint8_t pid, uint8_t *id)
{
    uint8_t raw = pid & 0x3FU;
    uint8_t expected = lin_compute_protected_id(raw);
    if (expected != pid)
        return false;
    *id = raw;
    return true;
}

static uint8_t lin_compute_checksum(uint8_t id, const uint8_t *data, uint32_t len, bool enhanced)
{
    uint16_t sum = 0;

    if (enhanced)
        sum += id;

    for (uint32_t i = 0; i < len; i++)
        sum += data[i];

    sum = (uint8_t)(sum & 0xFF) + (uint8_t)(sum >> 8);
    sum = (uint8_t)(sum & 0xFF) + (uint8_t)(sum >> 8);

    return (uint8_t)(~sum);
}

static inx_mcp2003b_status_t lin_send_break(UART_HandleTypeDef *uart)
{
    if (!uart)
        return INX_MCP2003B_STATUS_ERR;

    __HAL_UART_CLEAR_FLAG(uart, UART_FLAG_TC);
    if (HAL_LIN_SendBreak(uart) != HAL_OK)
        return INX_MCP2003B_STATUS_ERR;

    uint32_t polls = 100000U;
    while (!__HAL_UART_GET_FLAG(uart, UART_FLAG_TC) && polls > 0U) {
        polls--;
    }

    if (!__HAL_UART_GET_FLAG(uart, UART_FLAG_TC))
        return INX_MCP2003B_STATUS_ERR;

    return INX_MCP2003B_STATUS_OK;
}

static inx_mcp2003b_status_t lin_detect_break(UART_HandleTypeDef *uart, uint32_t timeout_ms)
{
    if (!uart)
        return INX_MCP2003B_STATUS_ERR;

    uint32_t start = HAL_GetTick();
    while (!__HAL_UART_GET_FLAG(uart, UART_FLAG_LBD)) {
        if (timeout_ms != HAL_MAX_DELAY && (HAL_GetTick() - start) >= timeout_ms)
            return INX_MCP2003B_STATUS_ERR;
        __NOP();
    }
    __HAL_UART_CLEAR_FLAG(uart, UART_FLAG_LBD);

    return INX_MCP2003B_STATUS_OK;
}

static inx_mcp2003b_status_t lin_send_header(inx_mcp2003b_dev_t *dev, uint32_t id, uint32_t timeout)
{
    if (!dev || id > 0x3FU)
        return INX_MCP2003B_STATUS_ERR;

    uint8_t header[2] = {
        0x55,
        lin_compute_protected_id((uint8_t)id)
    };

    if (HAL_UART_Transmit(dev->uart_handle, header, (uint16_t)sizeof(header), timeout) != HAL_OK)
        return INX_MCP2003B_STATUS_ERR;

    dev->last_request_pid = header[1];
    return INX_MCP2003B_STATUS_OK;
}

static int lin_read_bytes_wait(UART_HandleTypeDef *uart, uint8_t *buf, uint32_t len, uint32_t timeout)
{
    if (HAL_UART_Receive(uart, buf, (uint16_t)len, timeout) != HAL_OK) {
        return -1;
    }

    return 0;
}

static inx_mcp2003b_status_t lin_read_header(inx_mcp2003b_dev_t *dev, uint8_t *id, uint32_t *flags, uint32_t timeout)
{
    if (!dev || !id || !flags)
        return INX_MCP2003B_STATUS_ERR;

    uint8_t byte = 0;
    uint32_t sync_timeout = 0xFF;
    while(--sync_timeout) {
      if(lin_read_bytes_wait(dev->uart_handle, &byte, 1, timeout) != 0)
    	  return INX_MCP2003B_STATUS_ERR;

      if (byte != 0x55 && byte != 0U) {
          (*flags) |= INX_MCP2003B_SYNCH_ERROR;
    	  break;
      }
      if(byte == 0x55){
    	/* Sync Received */
    	break;
      }
    }

    uint8_t pid = 0;
    if (lin_read_bytes_wait(dev->uart_handle, &pid, 1, timeout) != 0)
        return INX_MCP2003B_STATUS_ERR;

    uint8_t raw_id = 0;
    if (!lin_validate_protected_id(pid, &raw_id)){
    	 *flags |= INX_MCP2003B_PARITY_ERROR;
        return INX_MCP2003B_STATUS_ERR;
    }

    dev->last_request_id  = raw_id;
    dev->last_request_pid = pid;

    *id    = raw_id;
    *flags |= INX_MCP2003B_NODATA;

    return INX_MCP2003B_STATUS_OK;
}

static inx_mcp2003b_status_t lin_read_response(inx_mcp2003b_dev_t *dev, uint8_t *id, uint8_t *msg, uint8_t dlc, uint32_t *flags, uint32_t timeout)
{
    if (!dev || !id || !msg || !flags)
        return INX_MCP2003B_STATUS_ERR;

    uint8_t buffer[9];
    if (HAL_UART_Receive(dev->uart_handle, buffer, (uint16_t)(dlc + 1U), timeout) != HAL_OK) {
        return INX_MCP2003B_STATUS_ERR;
    }

    *flags &= ~(INX_MCP2003B_NODATA);

    uint8_t checksum = buffer[dlc];

    uint8_t expected = lin_compute_checksum(dev->last_request_pid, buffer, dlc, (dev->flags & INX_MCP2003B_ENHANCED_CHECKSUM) != 0);
    if (checksum != expected) {
        *flags |= INX_MCP2003B_CSUM_ERROR;
        return INX_MCP2003B_STATUS_ERR;
    }

    memcpy(msg, buffer, dlc);
    *id    = dev->last_request_id;
    *flags = 0;

    return INX_MCP2003B_STATUS_OK;
}

inx_mcp2003b_dev_t *inx_mcp2003b_open (const inx_mcp2003b_hw_desc_t *hw)
{
    if (!hw)
        return NULL;

    inx_mcp2003b_dev_t *dev = (inx_mcp2003b_dev_t *)calloc(1, sizeof(*dev));
    if (!dev)
        return NULL;

    /* Get the UART handle */
    dev->uart_handle = hw->uart_handle;

    if (!dev->uart_handle || !dev->uart_handle->Instance) {
        free(dev);
        return NULL;
    }

    return dev;
}

void inx_mcp2003b_close(inx_mcp2003b_dev_t *dev)
{
    if (!dev)
        return;

    free(dev);
}

inx_mcp2003b_status_t inx_mcp2003b_config(inx_mcp2003b_dev_t *dev, inx_mcp2003b_config_t *cfg)
{
    if (!dev)
        return INX_MCP2003B_STATUS_NOT_INITIALIZED;

    if (!cfg)
        return INX_MCP2003B_STATUS_ERR;

    if (cfg->lin_baudrate == 0U)
        return INX_MCP2003B_STATUS_ERR;

    dev->uart_handle->Init.BaudRate = cfg->lin_baudrate;
    dev->uart_handle->Init.WordLength = UART_WORDLENGTH_8B;
    dev->uart_handle->Init.StopBits = UART_STOPBITS_1;
    dev->uart_handle->Init.Parity = UART_PARITY_NONE;
    dev->uart_handle->Init.Mode = UART_MODE_TX_RX;
    dev->uart_handle->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    dev->uart_handle->Init.OverSampling = UART_OVERSAMPLING_16;

    uint32_t break_length = (cfg->break_width >= 11U) ?
                            UART_LINBREAKDETECTLENGTH_11B :
                            UART_LINBREAKDETECTLENGTH_10B;
    if (HAL_LIN_Init(dev->uart_handle, break_length) != HAL_OK)
        return INX_MCP2003B_STATUS_ERR;

    dev->flags = cfg->flags;
    dev->last_request_id  = 0;
    dev->last_request_pid = 0;

    return INX_MCP2003B_STATUS_OK;
}

inx_mcp2003b_status_t inx_mcp2003b_start(inx_mcp2003b_dev_t *dev)
{
    if (!dev)
        return INX_MCP2003B_STATUS_NOT_INITIALIZED;

    SET_BIT(dev->uart_handle->Instance->CR1, USART_CR1_TE | USART_CR1_RE);
    __HAL_UART_ENABLE(dev->uart_handle);

    return INX_MCP2003B_STATUS_OK;
}

inx_mcp2003b_status_t inx_mcp2003b_stop(inx_mcp2003b_dev_t *dev)
{
    if (!dev)
        return INX_MCP2003B_STATUS_NOT_INITIALIZED;

    CLEAR_BIT(dev->uart_handle->Instance->CR1, USART_CR1_TE | USART_CR1_RE);
    __HAL_UART_DISABLE(dev->uart_handle);

    return INX_MCP2003B_STATUS_OK;
}

inx_mcp2003b_status_t inx_mcp2003b_write_message(inx_mcp2003b_dev_t *dev, uint8_t id, uint8_t *msg, uint32_t dlc, uint32_t timeout)
{
    if (!dev)
        return INX_MCP2003B_STATUS_NOT_INITIALIZED;

    if (dlc > 8)
        return INX_MCP2003B_STATUS_ERR;

    if (dev->flags & INX_MCP2003B_MASTER) {
        if (!msg || dlc == 0 || id > 0x3FU)
            return INX_MCP2003B_STATUS_ERR;

        if (lin_send_break(dev->uart_handle) != INX_MCP2003B_STATUS_OK)
            return INX_MCP2003B_STATUS_ERR;

        if (lin_send_header(dev, id, timeout) != INX_MCP2003B_STATUS_OK)
            return INX_MCP2003B_STATUS_ERR;

        if (HAL_UART_Transmit(dev->uart_handle, msg, dlc, timeout) != HAL_OK)
            return INX_MCP2003B_STATUS_ERR;

        uint8_t checksum = lin_compute_checksum(dev->last_request_pid, msg, dlc, (dev->flags & INX_MCP2003B_ENHANCED_CHECKSUM) != 0);
        if (HAL_UART_Transmit(dev->uart_handle, &checksum, 1U, timeout) != HAL_OK)
            return INX_MCP2003B_STATUS_ERR;

        dev->last_request_id = id;
        return INX_MCP2003B_STATUS_OK;
    }

    else { /* SLAVE */
        if (!msg)
            return INX_MCP2003B_STATUS_ERR;

        if (dlc == 0)
            return INX_MCP2003B_STATUS_OK;

        if (HAL_UART_Transmit(dev->uart_handle, msg, dlc, timeout) != HAL_OK)
            return INX_MCP2003B_STATUS_ERR;

        uint8_t checksum = lin_compute_checksum(dev->last_request_pid, msg, dlc, (dev->flags & INX_MCP2003B_ENHANCED_CHECKSUM) != 0);
        if (HAL_UART_Transmit(dev->uart_handle, &checksum, 1U, timeout) != HAL_OK)
            return INX_MCP2003B_STATUS_ERR;

        return INX_MCP2003B_STATUS_OK;
    }

    return INX_MCP2003B_STATUS_ERR;
}

inx_mcp2003b_status_t inx_mcp2003b_request_message(inx_mcp2003b_dev_t *dev, uint8_t id, uint32_t timeout)
{
    if (!dev)
        return INX_MCP2003B_STATUS_NOT_INITIALIZED;

    if (!(dev->flags & INX_MCP2003B_MASTER))
        return INX_MCP2003B_STATUS_ERR;

    if (id > 0x3FU)
        return INX_MCP2003B_STATUS_ERR;

    if (lin_send_break(dev->uart_handle) != INX_MCP2003B_STATUS_OK)
        return INX_MCP2003B_STATUS_ERR;

    if (lin_send_header(dev, id, timeout) != INX_MCP2003B_STATUS_OK)
        return INX_MCP2003B_STATUS_ERR;

    dev->last_request_id = id;
    return INX_MCP2003B_STATUS_OK;
}

inx_mcp2003b_status_t inx_mcp2003b_read_message(inx_mcp2003b_dev_t *dev, uint8_t *id, uint8_t *msg, uint8_t dlc, uint32_t *flags)
{
    return inx_mcp2003b_read_message_wait(dev, id, msg, dlc, flags, 0);
}

inx_mcp2003b_status_t inx_mcp2003b_read_message_wait(inx_mcp2003b_dev_t *dev, uint8_t *id, uint8_t *msg, uint8_t dlc, uint32_t *flags, uint32_t timeout)
{
    if (!dev)
        return INX_MCP2003B_STATUS_NOT_INITIALIZED;

    if (!id || !dlc || !flags)
        return INX_MCP2003B_STATUS_ERR;
    
    if (dlc > 8)
        return INX_MCP2003B_STATUS_ERR;

    if (dev->flags & INX_MCP2003B_MASTER)
        return lin_read_response(dev, id, msg, dlc, flags, timeout);
    else { /* SLAVE */
    	if(lin_detect_break(dev->uart_handle, timeout) != INX_MCP2003B_STATUS_OK)
            return INX_MCP2003B_STATUS_ERR;

        if(lin_read_header(dev, id, flags, timeout) != INX_MCP2003B_STATUS_OK)
            return INX_MCP2003B_STATUS_ERR;

        if(lin_read_response(dev, id, msg, dlc, flags, timeout) != INX_MCP2003B_STATUS_OK)
            return INX_MCP2003B_STATUS_ERR;

        return INX_MCP2003B_STATUS_OK;
    }
}
