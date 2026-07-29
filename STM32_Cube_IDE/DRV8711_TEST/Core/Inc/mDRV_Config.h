/*
 * mDRV_Config.h
 *
 *  Created on: 13-Jul-2026
 *      Author: richa
 */

#ifndef INC_MDRV_CONFIG_H_
#define INC_MDRV_CONFIG_H_


#include "main.h"
#include "spi.h"
#include "gpio.h"

/* ============================================================
   DRV Register Address (Bits 12-14)
   ============================================================ */
#define CONTROL_REG    0x00
#define TORQUE_REG     0x01
#define OFF_REG        0x02
#define BLANK_REG      0x03
#define DECAY_REG      0x04
#define STALL_REG      0x05
#define DRIVE_REG      0x06
#define STATUS_REG     0x07

/* ============================================================
   DRV Register Access (Bit 15)
   ============================================================ */
#define REG_WRITE      0x00    /* Bit15 = 0 for Write */
#define REG_READ       0x80    /* Bit15 = 1 for Read */

/* ============================================================
   DRV Control Register (0x00) - Bits Configuration
   ============================================================ */
#define DTIME(x)        (((x) & 0x03) << 2)   /* Bit 10-11: Dead Time */
#define ISGAIN(x)       (((x) & 0x03) << 0)   /* Bit 8-9: Current Sense Gain */
#define EXSTALL(x)      (((x) & 0x01) << 7)   /* Bit 7: External Stall Detect */
#define RSTEP(x)        (((x) & 0x04) << 2)   /* Bit 2: Step Motor */
#define RDIR(x)         (((x) & 0x02) << 1)   /* Bit 1: Direction */
#define ENBL(x)         ((x) & 0x01)          /* Bit 0: Enable */

/* ============================================================
   DRV Torque Register (0x01) - Bits Configuration
   ============================================================ */
#define SMPLTH(x)       ((x) & 0x07)          /* Bit 8-10: Sample Time */

/* ============================================================
   DRV OFF Register (0x02) - Bits Configuration
   ============================================================ */
#define PWMMODE(x)      ((x) & 0x01)          /* Bit 8: PWM Mode */
#define TOFF(x)         ((x) & 0xFF)          /* Bit 0-7: Off Time */

/* ============================================================
   DRV Blank Register (0x03) - Bits Configuration
   ============================================================ */
#define ABT(x)          ((x) & 0x01)          /* Bit 8: Automatic Blanking Time */
#define TBLANK(x)       ((x) & 0xFF)          /* Bit 0-7: Blanking Time */

/* ============================================================
   DRV Decay Register (0x04) - Bits Configuration
   ============================================================ */
#define DECMOD(x)       ((x) & 0x07)          /* Bit 8-10: Decay Mode */
#define TDECAY(x)       ((x) & 0xFF)          /* Bit 0-7: Decay Time */

/* ============================================================
   DRV Stall Register (0x05) - Bits Configuration
   ============================================================ */
#define VDIV(x)         (((x) & 0x03) << 2)   /* Bit 10-11: Voltage Divider */
#define SDCNT(x)        ((x) & 0x03)          /* Bit 8-9: Stall Count */
#define SDTHR(x)        ((x) & 0xFF)          /* Bit 0-7: Stall Threshold */

/* ============================================================
   DRV Drive Register (0x06) - Bits Configuration
   ============================================================ */
#define IDRIVEP(x)      (((x) & 0x03) << 2)   /* Bit 10-11: Source Current */
#define IDRIVEN(x)      ((x) & 0x03)          /* Bit 8-9: Sink Current */
#define TDRIVEP(x)      (((x) & 0x03) << 6)   /* Bit 6-7: Source Time */
#define TDRIVEN(x)      (((x) & 0x03) << 4)   /* Bit 4-5: Sink Time */
#define OCPDEG(x)       (((x) & 0x03) << 2)   /* Bit 2-3: Overcurrent Deglitch */
#define OCPTH(x)        ((x) & 0x03)          /* Bit 0-1: Overcurrent Threshold */

/* ============================================================
   Global Variables
   ============================================================ */
#ifdef READ_DRV_REG
extern unsigned short g_drv_con_reg_val;
extern unsigned short g_drv_tor_reg_val;
extern unsigned short g_drv_off_reg_val;
extern unsigned short g_drv_blk_reg_val;
extern unsigned short g_drv_dec_reg_val;
extern unsigned short g_drv_stall_reg_val;
extern unsigned short g_drv_dri_reg_val;
extern unsigned short g_drv_sta_reg_val;
#endif

extern unsigned short g_drv_sta_reg_val;
extern unsigned short g_drv_status_reg_clr_val;

/* ============================================================
   Function Prototypes
   ============================================================ */
void DRV_Write_All_REG(void);
void DRV_REG_Write(uint8_t address);
uint16_t DRV_REG_Read(uint8_t address);

// Extended Functions
void DRV_SetEnable(uint8_t enable);
void DRV_SetDirection(uint8_t direction);
void DRV_StepMotor(void);
void DRV_SetMicrostepping(uint8_t mode);
uint16_t DRV_GetStatus(void);
void DRV_ClearFaults(uint16_t fault_bits);


#endif /* INC_MDRV_CONFIG_H_ */
