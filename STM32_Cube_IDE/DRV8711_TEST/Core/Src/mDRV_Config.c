/*
 * mDRV_Config.c
 *
 *  Created on: 13-Jul-2026
 *      Author: richa
 */



#include "mDRV_Config.h"
#include "spi.h"
#include "gpio.h"

/* ============================================================
   Global Variables
   ============================================================ */
#ifdef READ_DRV_REG
unsigned short g_drv_con_reg_val;
unsigned short g_drv_tor_reg_val;
unsigned short g_drv_off_reg_val;
unsigned short g_drv_blk_reg_val;
unsigned short g_drv_dec_reg_val;
unsigned short g_drv_stall_reg_val;
unsigned short g_drv_dri_reg_val;
unsigned short g_drv_sta_reg_val;
#endif

unsigned short g_drv_sta_reg_val;
unsigned short g_drv_status_reg_clr_val = 0;

/* ============================================================
   SPI Read/Write Function - Your STM32 Port
   ============================================================ */
static uint16_t SPI_DRV8711_ReadWrite(uint8_t dataHi, uint8_t dataLo)
{
    uint16_t readData = 0;
    uint8_t rxByte = 0;

    // CS LOW (Active) - matches SET_CS
    HAL_GPIO_WritePin(L_CS_GPIO_Port, L_CS_Pin, GPIO_PIN_SET);

    // Small delay - matches _NOP()
    __NOP();

    // Send first byte - matches UCB0TXBUF = dataHi
    HAL_SPI_Transmit(&hspi1, &dataHi, 1, 1000);

    // Wait for SPI to be ready - matches while (UCB0STAT & BUSY)
    while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY) {
        __NOP();
        __NOP();
        __NOP();
        __NOP();
    }

    // Read first received byte - matches readData = (UCB0RXBUF << 8)
    HAL_SPI_Receive(&hspi1, &rxByte, 1, 1000);
    readData = ((uint16_t)rxByte << 8);

    // Send second byte - matches UCB0TXBUF = dataLo
    HAL_SPI_Transmit(&hspi1, &dataLo, 1, 1000);

    // Wait for SPI to be ready - matches while (UCB0STAT & BUSY)
    while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY) {
        __NOP();
        __NOP();
        __NOP();
        __NOP();
    }

    // Read second received byte - matches readData |= UCB0RXBUF
    HAL_SPI_Receive(&hspi1, &rxByte, 1, 1000);
    readData |= rxByte;

    // Small delay - matches _NOP()
    __NOP();

    // CS HIGH - matches CLR_CS
    HAL_GPIO_WritePin(L_CS_GPIO_Port, L_CS_Pin, GPIO_PIN_RESET);

    return readData;
}

/* ============================================================
   Write All Registers - Initialize DRV8711
   ============================================================ */
void DRV_Write_All_REG(void)
{
    DRV_REG_Write(CONTROL_REG);
    DRV_REG_Write(TORQUE_REG);
    DRV_REG_Write(OFF_REG);
    DRV_REG_Write(BLANK_REG);
    DRV_REG_Write(DECAY_REG);
    DRV_REG_Write(STALL_REG);
    DRV_REG_Write(DRIVE_REG);

    g_drv_status_reg_clr_val = 0;
    DRV_REG_Write(STATUS_REG);
}

/* ============================================================
   Write to Specific Register
   ============================================================ */
void DRV_REG_Write(uint8_t address)
{
    uint8_t datahi, datalo;
    uint8_t step_mod, sdcnt, sdthr;

    switch(address)
    {
        case CONTROL_REG:
            step_mod = 0x01;  // Full step mode for high speed control
            datahi = (REG_WRITE | ((address << 4) & 0x70) | DTIME(3) | ISGAIN(1));
            datalo = (EXSTALL(0) | (step_mod << 3) | RSTEP(0) | RDIR(0) | ENBL(1));
            break;

        case TORQUE_REG:
            datahi = (REG_WRITE | ((address << 4) & 0x70) | SMPLTH(0));
            datalo = 150;  // Default torque value
            break;

        case OFF_REG:
            datahi = (REG_WRITE | ((address << 4) & 0x70) | PWMMODE(0));
            datalo = TOFF(0x30);
            break;

        case BLANK_REG:
            datahi = (REG_WRITE | ((address << 4) & 0x70) | ABT(0x01));
            datalo = TBLANK(0x08);
            break;

        case DECAY_REG:
            datahi = (REG_WRITE | ((address << 4) & 0x70) | DECMOD(0x00));
            datalo = TDECAY(0x10);
            break;

        case STALL_REG:
            sdcnt = 0x00;
            sdthr = 100;
            datahi = (REG_WRITE | ((address << 4) & 0x70) | VDIV(0x01) | sdcnt);
            datalo = sdthr;
            break;

        case DRIVE_REG:
            datahi = (REG_WRITE | ((address << 4) & 0x70) | IDRIVEP(0) | IDRIVEN(0));
            datalo = (TDRIVEP(1) | TDRIVEN(1) | OCPDEG(1) | OCPTH(1));
            break;

        case STATUS_REG:
            datahi = (REG_WRITE | ((address << 4) & 0x70) | (g_drv_status_reg_clr_val & 0x0700));
            datalo = (g_drv_status_reg_clr_val & 0x00FF);
            break;

        default:
            return;  // Invalid address
    }

    SPI_DRV8711_ReadWrite(datahi, datalo);
}

/* ============================================================
   Read from Specific Register
   ============================================================ */
uint16_t DRV_REG_Read(uint8_t address)
{
    uint8_t datahi;
    uint16_t readData;

    datahi = (REG_READ | (address << 4));
    readData = SPI_DRV8711_ReadWrite(datahi, 0);

    return (readData & 0x0FFF);  // Mask to 12 bits
}

/* ============================================================
   Extended Functions for Motor Control
   ============================================================ */

// Enable/Disable Motor
void DRV_SetEnable(uint8_t enable)
{
    uint8_t datahi, datalo;
    uint8_t step_mod = 0x01;

    datahi = (REG_WRITE | (CONTROL_REG << 4) | DTIME(3) | ISGAIN(1));
    datalo = (EXSTALL(0) | (step_mod << 3) | RSTEP(0) | RDIR(0) | ENBL(enable));

    SPI_DRV8711_ReadWrite(datahi, datalo);
}

// Set Motor Direction
void DRV_SetDirection(uint8_t direction)
{
    uint8_t datahi, datalo;
    uint8_t step_mod = 0x01;

    datahi = (REG_WRITE | (CONTROL_REG << 4) | DTIME(3) | ISGAIN(1));
    datalo = (EXSTALL(0) | (step_mod << 3) | RSTEP(0) | RDIR(direction) | ENBL(1));

    SPI_DRV8711_ReadWrite(datahi, datalo);
}

// Step Motor (One Step)
void DRV_StepMotor(void)
{
    uint8_t datahi, datalo;
    uint8_t step_mod = 0x01;

    // Set RSTEP bit to 1
    datahi = (REG_WRITE | (CONTROL_REG << 4) | DTIME(3) | ISGAIN(1));
    datalo = (EXSTALL(0) | (step_mod << 3) | RSTEP(1) | RDIR(0) | ENBL(1));
    SPI_DRV8711_ReadWrite(datahi, datalo);

    // Clear RSTEP bit (auto-clears in hardware, but we do it anyway)
    datalo = (EXSTALL(0) | (step_mod << 3) | RSTEP(0) | RDIR(0) | ENBL(1));
    SPI_DRV8711_ReadWrite(datahi, datalo);
}

// Set Microstepping Mode
void DRV_SetMicrostepping(uint8_t mode)
{
    uint8_t datahi, datalo;

    datahi = (REG_WRITE | (CONTROL_REG << 4) | DTIME(3) | ISGAIN(1));
    datalo = (EXSTALL(0) | (mode << 3) | RSTEP(0) | RDIR(0) | ENBL(1));

    SPI_DRV8711_ReadWrite(datahi, datalo);
}

// Read Status Register
uint16_t DRV_GetStatus(void)
{
    return DRV_REG_Read(STATUS_REG);
}

// Clear Faults
void DRV_ClearFaults(uint16_t fault_bits)
{
    g_drv_status_reg_clr_val = fault_bits;
    DRV_REG_Write(STATUS_REG);
    g_drv_status_reg_clr_val = 0;
}
