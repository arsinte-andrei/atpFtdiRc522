#ifndef ATPFT232SPI_H
#define ATPFT232SPI_H

#include <QObject>
#include <QMap>
#include <QVector>

#include <Windows.h>
/* Include D2XX header*/
#include "ftd2xx.h"
/* Include libMPSSE header */
#include "libMPSSE_spi.h"

class atpFt232Spi : public QObject {
    Q_OBJECT
public:
    explicit atpFt232Spi(QObject *parent = 0);
    ~atpFt232Spi();

    /* Application specific macro definations */
    #define SPI_DEVICE_BUFFER_SIZE		256
    #define SPI_WRITE_COMPLETION_RETRY		10
    #define START_ADDRESS_EEPROM 	0x00 /*read/write start address inside the EEPROM*/
    #define END_ADDRESS_EEPROM		0x10
    #define RETRY_COUNT_EEPROM		10	/* number of retries if read/write fails */
    #define CHANNEL_TO_OPEN			0	/*0 for first available channel, 1 for next...  FT232H has only 1 chanel so it has to be chanel 0*/
    #define SPI_SLAVE_0				0
    #define SPI_SLAVE_1				1
    #define SPI_SLAVE_2				2
    #define DATA_OFFSET				4
    #define USE_WRITEREAD			0

    /*FT232H pins*/
    #define FT_PIN_C01 1 // PIN C0 of the phisical FT232H board from adafruit
    #define FT_PIN_C02 2 // PIN C1 of the phisical FT232H board from adafruit
    #define FT_PIN_C03 3 // PIN C2 of the phisical FT232H board from adafruit
    #define FT_PIN_C04 4 // PIN C3 of the phisical FT232H board from adafruit
    #define FT_PIN_C05 5 // PIN C4 of the phisical FT232H board from adafruit
    #define FT_PIN_C06 6 // PIN C5 of the phisical FT232H board from adafruit
    #define FT_PIN_C07 7 // PIN C6 of the phisical FT232H board from adafruit
    #define FT_PIN_C08 8 // PIN C7 of the phisical FT232H board from adafruit

    /*FT232H pins value*/
    #define FT_PIN_LOW 0
    #define FT_PIN_HI  1
    #define FT_PIN_OFF 3

    /******************************************************************************/
    /*                                  	SPI         						  */
    /******************************************************************************/

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

    typedef FT_STATUS (*pfunc_SPI_ReadWrite)(FT_HANDLE handle, uint8 *inBuffer, uint8 *outBuffer, uint32 sizeToTransfer, uint32 *sizeTransferred, uint32 transferOptions);
    pfunc_SPI_ReadWrite p_SPI_ReadWrite;

    typedef FT_STATUS (*pfunc_SPI_IsBusy)(FT_HANDLE handle, bool *state);
    pfunc_SPI_IsBusy p_SPI_IsBusy;

    typedef FT_STATUS (*pfunc_SPI_ChangeCS)(FT_HANDLE handle, uint32 configOptions);
    pfunc_SPI_ChangeCS p_SPI_ChangeCS;

    /******************************************************************************/
    /*                                  	GPIO         						  */
    /******************************************************************************/

    typedef FT_STATUS (*pfunc_FT_WriteGPIO)(FT_HANDLE handle, uint8 dir, uint8 value);
    pfunc_FT_WriteGPIO p_FT_WriteGPIO;

    typedef FT_STATUS (*pfunc_FT_ReadGPIO)(FT_HANDLE handle, uint8 *value);
    pfunc_FT_ReadGPIO p_FT_ReadGPIO;

    /******************************************************************************/
    /*								Global variables							  */
    /******************************************************************************/
    FT_HANDLE ftHandle;
    uint8 buffer[SPI_DEVICE_BUFFER_SIZE] = {0};


public slots:
    FT_STATUS read_byte(uint8 slaveAddress, uint8 address, uint16 *data);
    FT_STATUS write_byte(uint8 slaveAddress, uint8 address, uint16 data);

    void delay(int millisecondsToWait);
    void writeGpioPin(int pinNo, int lowHiState);

    void initDllLyb();
    void initSpiComunication();
    bool isDllLybLoaded();
    int getChannels();
    int getPinStatus(int pinNo);
    QVector<uint8> atptransferN(uint8 *buf, uint32 len);
signals:

private:
    uint32 channels;
    bool libDllLoaded; // hold the status of loading the dll library
    QMap<int, int> pinStateValue;

private slots:
    void resetPinstateValue();
    uint8 editPinStateValue(int pinNo, int value);

};

#endif // ATPFT232SPI_H
