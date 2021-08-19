// Taken from pico-sdk (https://github.com/raspberrypi/pico-sdk)
// Copyright: BSD-3-Clause held by Raspberry Pi (Trading) Ltd.

check_return:
    pop {r0}
    cmp r0, #0
    beq vector_into_flash
    bx r0
vector_into_flash:
    ldr r0, =(0x10000100)
    ldr r1, =(0xe000ed08)
    str r0, [r1]
    ldmia r0, {r0, r1}
    msr msp, r0
    bx r1

read_flash_sreg:
    push {r1, lr}
    str r0, [r3, #SSI_DR0_OFFSET]
    str r0, [r3, #SSI_DR0_OFFSET]
    
    bl wait_ssi_ready
    ldr r0, [r3, #SSI_DR0_OFFSET]
    ldr r0, [r3, #SSI_DR0_OFFSET]

    pop {r1, pc}

wait_ssi_ready:
    push {r0, r1, lr}

1:
    ldr r1, [r3, #SSI_SR_OFFSET]
    movs r0, #SSI_SR_TFE_BITS
    tst r1, r0
    beq 1b
    movs r0, #SSI_SR_BUSY_BITS
    tst r1, r0
    bne 1b

    pop {r0, r1, pc}
