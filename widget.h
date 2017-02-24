#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <Windows.h>

/* Include D2XX header*/
#include "ftd2xx.h"

/* Include libMPSSE header */
#include "libMPSSE_spi.h"

namespace Ui {
    class Widget;
}

class Widget : public QWidget {
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    /* Application specific macro definations */
    #define SPI_DEVICE_BUFFER_SIZE		256
    #define SPI_WRITE_COMPLETION_RETRY		10
    #define START_ADDRESS_EEPROM 	0x00 /*read/write start address inside the EEPROM*/
    #define END_ADDRESS_EEPROM		0x10
    #define RETRY_COUNT_EEPROM		10	/* number of retries if read/write fails */
    #define CHANNEL_TO_OPEN			0	/*0 for first available channel, 1 for next... */
    #define SPI_SLAVE_0				0
    #define SPI_SLAVE_1				1
    #define SPI_SLAVE_2				2
    #define DATA_OFFSET				4
    #define USE_WRITEREAD			0

    typedef FT_STATUS (*pfunc_SPI_GetNumChannels)(uint32 *numChannels);
    pfunc_SPI_GetNumChannels p_SPI_GetNumChannels;
    typedef FT_STATUS (*pfunc_SPI_GetChannelInfo)(uint32 index, FT_DEVICE_LIST_INFO_NODE *chanInfo);
    pfunc_SPI_GetChannelInfo p_SPI_GetChannelInfo;
    typedef FT_STATUS (*pfunc_SPI_OpenChannel)(uint32 index, FT_HANDLE *handle);
    pfunc_SPI_OpenChannel p_SPI_OpenChannel;
    typedef FT_STATUS (*pfunc_SPI_InitChannel)(FT_HANDLE handle, ChannelConfig *config);
    pfunc_SPI_InitChannel p_SPI_InitChannel;
    typedef FT_STATUS (*pfunc_SPI_CloseChannel)(FT_HANDLE handle);
    pfunc_SPI_CloseChannel p_SPI_CloseChannel;
    typedef FT_STATUS (*pfunc_SPI_Read)(FT_HANDLE handle, uint8 *buffer, uint32 sizeToTransfer, uint32 *sizeTransfered, uint32 options);
    pfunc_SPI_Read p_SPI_Read;
    typedef FT_STATUS (*pfunc_SPI_Write)(FT_HANDLE handle, uint8 *buffer, uint32 sizeToTransfer, uint32 *sizeTransfered, uint32 options);
    pfunc_SPI_Write p_SPI_Write;
    typedef FT_STATUS (*pfunc_SPI_IsBusy)(FT_HANDLE handle, bool *state);
    pfunc_SPI_IsBusy p_SPI_IsBusy;
    typedef FT_STATUS (*pfunc_SPI_ReadWrite)(FT_HANDLE handle, uint8 *inBuffer, uint8 *outBuffer, uint32 sizeToTransfer, uint32 *sizeTransferred, uint32 transferOptions);
    pfunc_SPI_ReadWrite p_SPI_ReadWrite;

    /******************************************************************************/
    /*								Global variables							  	    */
    /******************************************************************************/
    FT_HANDLE ftHandle;
    uint8 buffer[SPI_DEVICE_BUFFER_SIZE] = {0};


public slots:
    FT_STATUS read_byte(uint8 slaveAddress, uint8 address, uint16 *data);
    FT_STATUS write_byte(uint8 slaveAddress, uint8 address, uint16 data);
private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
