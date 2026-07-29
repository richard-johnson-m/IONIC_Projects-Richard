/*
 * usb_log.h
 *
 * Created on: 10-Jul-2026
 * Author: richa
 */

#ifndef INC_USB_LOG_H_
#define INC_USB_LOG_H_

#include "main.h"
#include "fatfs.h"
#include "usbh_msc.h"


/* =====================================================
 * USB INFORMATION VARIABLES
 * ===================================================== */


/*
 * Becomes 1 after USB information is successfully read
 */
extern volatile uint8_t USB_Info_Ready;

/*
 * Pendrive volume label
 */
extern volatile char USB_Pendrive_Name[32];


/*
 * FAT serial number
 */
extern volatile uint32_t USB_Serial_Number;


/*
 * FAT filesystem size
 */
extern volatile uint32_t USB_Total_MB;


/*
 * Free space available
 */
extern volatile uint32_t USB_Free_MB;


/*
 * USB MSC raw block information
 */
extern volatile uint32_t USB_Block_Count;

extern volatile uint32_t USB_Block_Size;


/*
 * Calculated pendrive capacity
 */
extern volatile uint32_t USB_Capacity_MB;



/* =====================================================
 * CSV LOGGER FUNCTIONS
 * ===================================================== */


/**
 * @brief
 * Mount USB drive and create/open log.csv
 *
 * @return FatFs result
 */
FRESULT USB_CSV_Init(void);



/**
 * @brief
 * Write one data row into CSV file
 *
 * @param voltage Voltage value
 * @param current Current value
 *
 * @return FatFs result
 */
FRESULT USB_CSV_Write(float voltage, float current);



/**
 * @brief
 * Close CSV file safely before removing USB
 */
void USB_CSV_Close(void);



/**
 * @brief
 * Read USB pendrive information
 *
 * Reads:
 *  - Pendrive name
 *  - Serial number
 *  - Total FAT size
 *  - Free space
 *  - Block count
 *  - Block size
 *  - Total capacity
 */
void USB_Get_Info(void);



#endif /* INC_USB_LOG_H_ */
