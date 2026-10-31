#ifndef ANNEX_CAN_H
#define ANNEX_CAN_H

#include <stdint.h>

typedef struct {
    uint32_t id;
    uint8_t dlc;
    uint8_t data[8];
} annex_can_frame_t;

void annex_can_init(void);
void annex_can_transmit(const annex_can_frame_t *frame);
void annex_can_receive(void);

#endif /* ANNEX_CAN_H */
