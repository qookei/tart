// Taken from pico-sdk (https://github.com/raspberrypi/pico-sdk)
// Copyright: BSD-3-Clause held by Raspberry Pi (Trading) Ltd.

#include "asmincludes/pads_qspi.h"
#include "asmincludes/addressmap.h"
#include "asmincludes/ssi.h"

#define PICO_FLASH_SPI_CLKDIV 2
#define FRAME_FORMAT SSI_CTRLR0_SPI_FRF_VALUE_QUAD
#define CMD_READ 0xeb
#define MODE_CONTINUOUS_READ 0xa0
#define ADDR_L 8
#define WAIT_CYCLES 4
#define PROGRAM_STATUS_REG
#define CMD_WRITE_ENABLE 0x06
#define CMD_READ_STATUS 0x05
#define CMD_READ_STATUS2 0x35
#define CMD_WRITE_STATUS 0x01
#define SREG_DATA 0x02

.syntax unified
.cpu cortex-m0plus
.thumb

.section .text
.global _start
_start:
    push {lr}

    ldr r3, =PADS_QSPI_BASE
    movs r0, #(2 << PADS_QSPI_GPIO_QSPI_SCLK_DRIVE_LSB | PADS_QSPI_GPIO_QSPI_SCLK_SLEWFAST_BITS)
    str r0, [r3, #PADS_QSPI_GPIO_QSPI_SCLK_OFFSET]
    ldr r0, [r3, #PADS_QSPI_GPIO_QSPI_SD0_OFFSET]
    movs r1, #PADS_QSPI_GPIO_QSPI_SD0_SCHMITT_BITS
    bics r0, r1
    str r0, [r3, #PADS_QSPI_GPIO_QSPI_SD0_OFFSET]
    str r0, [r3, #PADS_QSPI_GPIO_QSPI_SD1_OFFSET]
    str r0, [r3, #PADS_QSPI_GPIO_QSPI_SD2_OFFSET]
    str r0, [r3, #PADS_QSPI_GPIO_QSPI_SD3_OFFSET]

    ldr r3, =XIP_SSI_BASE

    movs r1, #0
    str r1, [r3, #SSI_SSIENR_OFFSET]

    movs r1, #PICO_FLASH_SPI_CLKDIV
    str r1, [r3, #SSI_BAUDR_OFFSET]

    movs r1, #1
    movs r2, #SSI_RX_SAMPLE_DLY_OFFSET
    str r1, [r3, r2]

program_sregs:
#define CTRL0_SPI_TXRX \
    (7 << SSI_CTRLR0_DFS_32_LSB) | /* 8 bits per data frame */ \
    (SSI_CTRLR0_TMOD_VALUE_TX_AND_RX << SSI_CTRLR0_TMOD_LSB)

    ldr r1, =(CTRL0_SPI_TXRX)
    str r1, [r3, #SSI_CTRLR0_OFFSET]

    movs r1, #1
    str r1, [r3, #SSI_SSIENR_OFFSET]

    movs r0, #CMD_READ_STATUS2
    bl read_flash_sreg
    movs r2, #SREG_DATA
    cmp r0, r2
    beq skip_sreg_programming

    movs r1, #CMD_WRITE_ENABLE
    str r1, [r3, #SSI_DR0_OFFSET]

    bl wait_ssi_ready
    ldr r1, [r3, #SSI_DR0_OFFSET]

    movs r1, #CMD_WRITE_STATUS
    str r1, [r3, #SSI_DR0_OFFSET]
    movs r0, #0
    str r0, [r3, #SSI_DR0_OFFSET]
    str r2, [r3, #SSI_DR0_OFFSET]

    bl wait_ssi_ready
    ldr r1, [r3, #SSI_DR0_OFFSET]
    ldr r1, [r3, #SSI_DR0_OFFSET]
    ldr r1, [r3, #SSI_DR0_OFFSET]

1:
    movs r0, #CMD_READ_STATUS
    bl read_flash_sreg
    movs r1, #1
    tst r0, r1
    bne 1b

skip_sreg_programming:
    movs r1, #0
    str r1, [r3, #SSI_SSIENR_OFFSET]

dummy_read:
#define CTRLR0_ENTER_XIP \
    (FRAME_FORMAT \
        << SSI_CTRLR0_SPI_FRF_LSB) | \
    (31 << SSI_CTRLR0_DFS_32_LSB)  | \
    (SSI_CTRLR0_TMOD_VALUE_EEPROM_READ \
        << SSI_CTRLR0_TMOD_LSB)

    ldr r1, =(CTRLR0_ENTER_XIP)
    str r1, [r3, #SSI_CTRLR0_OFFSET]

    movs r1, #0x0
    str r1, [r3, #SSI_CTRLR1_OFFSET]

#define SPI_CTRLR0_ENTER_XIP \
    (ADDR_L << SSI_SPI_CTRLR0_ADDR_L_LSB) | \
    (WAIT_CYCLES << SSI_SPI_CTRLR0_WAIT_CYCLES_LSB) | \
    (SSI_SPI_CTRLR0_INST_L_VALUE_8B \
        << SSI_SPI_CTRLR0_INST_L_LSB) | \
    (SSI_SPI_CTRLR0_TRANS_TYPE_VALUE_1C2A \
        << SSI_SPI_CTRLR0_TRANS_TYPE_LSB)

    ldr r1, =(SPI_CTRLR0_ENTER_XIP)
    ldr r0, =(XIP_SSI_BASE + SSI_SPI_CTRLR0_OFFSET)
    str r1, [r0]

    movs r1, #1
    str r1, [r3, #SSI_SSIENR_OFFSET]

    movs r1, #CMD_READ
    str r1, [r3, #SSI_DR0_OFFSET]
    movs r1, #MODE_CONTINUOUS_READ
    str r1, [r3, #SSI_DR0_OFFSET]

    bl wait_ssi_ready

    movs r1, #0
    str r1, [r3, #SSI_SSIENR_OFFSET]

configure_ssi:
#define SPI_CTRLR0_XIP \
    (MODE_CONTINUOUS_READ \
        << SSI_SPI_CTRLR0_XIP_CMD_LSB) | \
    (ADDR_L << SSI_SPI_CTRLR0_ADDR_L_LSB) | \
    (WAIT_CYCLES << SSI_SPI_CTRLR0_WAIT_CYCLES_LSB) | \
    (SSI_SPI_CTRLR0_INST_L_VALUE_NONE \
        << SSI_SPI_CTRLR0_INST_L_LSB) | \
    (SSI_SPI_CTRLR0_TRANS_TYPE_VALUE_2C2A \
        << SSI_SPI_CTRLR0_TRANS_TYPE_LSB)

    ldr r1, =(SPI_CTRLR0_XIP)
    ldr r0, =(XIP_SSI_BASE + SSI_SPI_CTRLR0_OFFSET)
    str r1, [r0]

    movs r1, #1
    str r1, [r3, #SSI_SSIENR_OFFSET]

#include "asmincludes/helpers.h"

.ltorg

.end
