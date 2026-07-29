/*
 * usb_log.c
 */

#include "usb_log.h"
#include "string.h"
#include "stdio.h"


extern FATFS USBHFatFS;
extern FIL USBHFile;
extern char USBHPath[4];

extern USBH_HandleTypeDef hUSB_Host;

/* CSV status */
static uint8_t csv_created = 0;


/* USB information variables */
volatile uint8_t USB_Info_Ready = 0;

volatile char USB_Pendrive_Name[32] = "NO USB";

volatile uint32_t USB_Serial_Number = 0;

volatile uint32_t USB_Total_MB = 0;

volatile uint32_t USB_Free_MB = 0;

volatile uint32_t USB_Block_Count = 0;

volatile uint32_t USB_Block_Size = 0;

volatile uint32_t USB_Capacity_MB = 0;
volatile uint64_t USB_Capacity_Bytes = 0;


FRESULT USB_CSV_Init(void)
{
    FRESULT res;


    if(csv_created)
        return FR_OK;


    res = f_mount(&USBHFatFS, USBHPath, 1);

    if(res != FR_OK)
    {
        return res;
    }



    res = f_open(&USBHFile,
                 "log.csv",
                 FA_OPEN_ALWAYS | FA_WRITE);


    if(res != FR_OK)
    {
        return res;
    }



    f_lseek(&USBHFile,
            f_size(&USBHFile));


    if(f_size(&USBHFile)==0)
    {
        char header[] =
        "Time,Voltage,Current\r\n";

        UINT bw;

        f_write(&USBHFile,
                header,
                strlen(header),
                &bw);

        f_sync(&USBHFile);
    }


    csv_created = 1;


    return FR_OK;
}




FRESULT USB_CSV_Write(float voltage,float current)
{

    if(csv_created==0)
        return FR_NOT_READY;


    char data[128];

    UINT bw;


    sprintf(data,
            "%lu,%.2f,%.2f\r\n",
            HAL_GetTick(),
            voltage,
            current);



    FRESULT res;

    res=f_write(&USBHFile,
                data,
                strlen(data),
                &bw);


    f_sync(&USBHFile);


    return res;

}





void USB_CSV_Close(void)
{

    if(csv_created)
    {
        f_sync(&USBHFile);

        f_close(&USBHFile);

        csv_created=0;
    }

}




void USB_Get_Info(void)
{
    FATFS *fs;
    DWORD free_clusters;
    DWORD total_sect;
    DWORD free_sect;

    MSC_LUNTypeDef info;


    USB_Info_Ready = 0;


    /* Get pendrive label */
    if(f_getlabel(USBHPath,
                  USB_Pendrive_Name,
                  &USB_Serial_Number) != FR_OK)
    {
        strcpy(USB_Pendrive_Name, "NO_LABEL");
    }



    /* Get FAT storage information */
    if(f_getfree(USBHPath,
                 &free_clusters,
                 &fs) == FR_OK)
    {

        total_sect = (fs->n_fatent - 2) * fs->csize;

        free_sect = free_clusters * fs->csize;


        /*
         * Convert sectors to MB
         * sector size = 512 bytes
         */
        USB_Total_MB = (total_sect * 512) / (1024 * 1024);

        USB_Free_MB = (free_sect * 512) / (1024 * 1024);
    }



    /* Get USB MSC information */
    if(USBH_MSC_GetLUNInfo(&hUSB_Host,
                           0,
                           &info) == USBH_OK)
    {

        USB_Block_Count = info.capacity.block_nbr;

        USB_Block_Size = info.capacity.block_size;

        uint64_t capacity_bytes;

        capacity_bytes = (uint64_t)USB_Block_Count * USB_Block_Size;

        USB_Capacity_MB = capacity_bytes / (1024 * 1024);

    }


    USB_Info_Ready = 1;
}
