#ifndef INX_LIN_BOARD_H
#define INX_LIN_BOARD_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif


/* LIN Status flags */
#define INX_LIN_BOARD_BIT_ERROR 128 /* Bit error when transmitting */
#define INX_LIN_BOARD_SYNCH_ERROR 64 /* A synch error */
#define INX_LIN_BOARD_PARITY_ERROR 32 /* ID parity error */
#define INX_LIN_BOARD_CSUM_ERROR 16 /* Checksum error */
#define INX_LIN_BOARD_WAKEUP_FRAME 4 /* A wake up frame was received. */
#define INX_LIN_BOARD_NODATA 8 /* No data, only a header */

/* LIN Config flags */
#define INX_LIN_BOARD_MASTER 1 /* The LIN interface will be a LIN MASTER, (Default will be SLAVE)*/
#define INX_LIN_BOARD_VARIABLE_DLC 2 /* When specified, turns variable message length ON so the \
                                    message length will depend on the message ID */
#define INX_LIN_BOARD_ENHANCED_CHECKSUM 4 /* LIN interface will use the "enhanced checksum" according to LIN 2.0 */

typedef enum {
    INX_LIN_BOARD_STATUS_OK = 0,
    INX_LIN_BOARD_STATUS_ERR = -1,
    INX_LIN_BOARD_STATUS_NOT_INITIALIZED = -2
} inx_lin_board_status_t;

typedef struct {
    uint32_t lin_baudrate;
    uint8_t break_width;
    uint8_t flags;
} inx_lin_board_config_t;

typedef struct {
    UART_HandleTypeDef *uart_handle;
} inx_lin_board_hw_desc_t;

typedef struct inx_lin_board_dev inx_lin_board_dev_t;

inx_lin_board_dev_t *inx_lin_board_open (const inx_lin_board_hw_desc_t *hw);
void inx_lin_board_close(inx_lin_board_dev_t *dev);
inx_lin_board_status_t inx_lin_board_start(inx_lin_board_dev_t *dev);
inx_lin_board_status_t inx_lin_board_config(inx_lin_board_dev_t *dev, inx_lin_board_config_t *cfg);
inx_lin_board_status_t inx_lin_board_write_message(inx_lin_board_dev_t *dev, uint8_t id, uint8_t *msg, uint32_t dlc, uint32_t timeout);
inx_lin_board_status_t inx_lin_board_request_message(inx_lin_board_dev_t *dev, uint8_t id, uint32_t timeout);
inx_lin_board_status_t inx_lin_board_read_message(inx_lin_board_dev_t *dev, uint8_t *id, uint8_t *msg, uint8_t dlc, uint32_t *flags);
inx_lin_board_status_t inx_lin_board_read_message_wait(inx_lin_board_dev_t *dev, uint8_t *id, uint8_t *msg, uint8_t dlc, uint32_t *flags, uint32_t timeout);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* INX_LIN_BOARD_H */
