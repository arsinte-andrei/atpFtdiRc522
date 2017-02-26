#include "atpft232spi.h"
#include <QLibrary>
#include <QDebug>
#include <QTime>
#include <QCoreApplication>

#define  DEFAULT_SPI_SPEED 5000L

atpFt232Spi::atpFt232Spi(QObject *parent) : QObject(parent) {

    libDllLoaded = false;
    resetPinstateValue();
    initDllLyb();
    initSpiComunication();
}

atpFt232Spi::~atpFt232Spi() {
    if(channels>0){
        p_SPI_CloseChannel(ftHandle);
    }
}

/******************************************************************************/
/*						Public function definitions						  		   */
/******************************************************************************/
/*!
 * \brief Writes to EEPROM
 *
 * This function writes a byte to a specified address within the 93LC56B EEPROM
 *
 * \param[in] slaveAddress Address of the I2C slave (EEPROM)
 * \param[in] registerAddress Address of the memory location inside the slave to where the byte
 *			is to be written
 * \param[in] data The byte that is to be written
 * \return Returns status code of type FT_STATUS(see D2XX Programmer's Guide)
 * \sa Datasheet of 93LC56B http://ww1.microchip.com/downloads/en/DeviceDoc/21794F.pdf
 * \note
 * \warning
 */
FT_STATUS atpFt232Spi::read_byte(uint8 slaveAddress, uint8 address, uint16 *data) {
    uint32 sizeToTransfer = 0;
    uint32 sizeTransfered;
    uint8 writeComplete=0;
    uint32 retry=0;
    FT_STATUS status;

    /* CS_High + Write command + Address */
    sizeToTransfer=1;
    sizeTransfered=0;
    buffer[0] = 0xC0;/* Write command (3bits)*/
    buffer[0] = buffer[0] | ( ( address >> 3) & 0x0F );/*5 most significant add bits*/
    status = p_SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
        SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES|
        SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);


    /*Write partial address bits */
    sizeToTransfer=4;
    sizeTransfered=0;
    buffer[0] = ( address & 0x07 ) << 5; /* least significant 3 address bits */
    status = p_SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
        SPI_TRANSFER_OPTIONS_SIZE_IN_BITS);

    /*Read 2 bytes*/
    sizeToTransfer=2;
    sizeTransfered=0;
    status = p_SPI_Read(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
        SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES|
        SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);

    *data = (uint16)(buffer[1]<<8);
    *data = (*data & 0xFF00) | (0x00FF & (uint16)buffer[0]);

    return status;
}

/*!
 * \brief Reads from EEPROM
 *
 * This function reads a byte from a specified address within the 93LC56B EEPROM
 *
 * \param[in] slaveAddress Address of the I2C slave (EEPROM)
 * \param[in] registerAddress Address of the memory location inside the slave from where the
 *			byte is to be read
 * \param[in] *data Address to where the byte is to be read
 * \return Returns status code of type FT_STATUS(see D2XX Programmer's Guide)
 * \sa Datasheet of 93LC56B http://ww1.microchip.com/downloads/en/DeviceDoc/21794F.pdf
 * \note
 * \warning
 */
FT_STATUS atpFt232Spi::write_byte(uint8 slaveAddress, uint8 address, uint16 data) {
    uint32 sizeToTransfer = 0;
    uint32 sizeTransfered=0;
    uint8 writeComplete=0;
    uint32 retry=0;
    FT_STATUS status;

    /* Write command EWEN(with CS_High -> CS_Low) */
    sizeToTransfer=11;
    sizeTransfered=0;
    buffer[0]=0x9F;/* SPI_EWEN -> binary 10011xxxxxx (11bits) */
    buffer[1]=0xFF;
    status = p_SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
        SPI_TRANSFER_OPTIONS_SIZE_IN_BITS|
        SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE|
        SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);


    /* CS_High + Write command + Address */
    sizeToTransfer=1;
    sizeTransfered=0;
    buffer[0] = 0xA0;/* Write command (3bits) */
    buffer[0] = buffer[0] | ( ( address >> 3) & 0x0F );/*5 most significant add bits*/
    status = p_SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
        SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES|
        SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);

    /*Write 3 least sig address bits */
    sizeToTransfer=3;
    sizeTransfered=0;
    buffer[0] = ( address & 0x07 ) << 5; /* least significant 3 address bits */
    status = p_SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
        SPI_TRANSFER_OPTIONS_SIZE_IN_BITS);

    /* Write 2 byte data + CS_Low */
    sizeToTransfer=2;
    sizeTransfered=0;
    buffer[0] = (uint8)(data & 0xFF);
    buffer[1] = (uint8)((data & 0xFF00)>>8);
    status = p_SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
        SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES|
        SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);

    /* Wait until D0 is high */
#if 1
    /* Strobe Chip Select */
    sizeToTransfer=0;
    sizeTransfered=0;
    status = p_SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
        SPI_TRANSFER_OPTIONS_SIZE_IN_BITS|
        SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);
    sizeToTransfer=0;
    sizeTransfered=0;
    status = p_SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
        SPI_TRANSFER_OPTIONS_SIZE_IN_BITS|
        SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
#else
    retry=0;
    state=FALSE;
    p_SPI_IsBusy(ftHandle,&state);
    while((FALSE==state) && (retry<SPI_WRITE_COMPLETION_RETRY))
    {
        printf("SPI device is busy(%u)\n",(unsigned)retry);
        p_SPI_IsBusy(ftHandle,&state);
        retry++;
    }
#endif
    /* Write command EWEN(with CS_High -> CS_Low) */
    sizeToTransfer=11;
    sizeTransfered=0;
    buffer[0]=0x8F;/* SPI_EWEN -> binary 10011xxxxxx (11bits) */
    buffer[1]=0xFF;
    status = p_SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
        SPI_TRANSFER_OPTIONS_SIZE_IN_BITS|
        SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE|
        SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
    return status;
}

void atpFt232Spi::delay( int millisecondsToWait ) {
    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
    while( QTime::currentTime() < dieTime ) {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

void atpFt232Spi::writeGpioPin(int pinNo, int lowHiState){
    uint8 value;
    p_FT_ReadGPIO(ftHandle, &value);
    qDebug() <<value;

    qDebug() <<"PIN: " <<pinNo <<"LOW - HI: " <<lowHiState;


    if( (pinNo>7) || (lowHiState>1) ){
        return;
    }
    uint8 allPinsToWrite = 0xFF; //primii trei pini
    uint8 allValuesToWrite = editPinStateValue(pinNo, lowHiState);
    qDebug() <<"uint8 returned:" << allValuesToWrite;
    p_FT_WriteGPIO(ftHandle, allPinsToWrite, allValuesToWrite);

     p_FT_ReadGPIO(ftHandle, &value);
     qDebug() <<value;
}

void atpFt232Spi::initDllLyb() { /* load library */
    if (QLibrary::isLibrary("libMPSSE.dll")) {
      QLibrary lib("libMPSSE.dll");
      if(lib.load()){
          /* init function pointers */
          p_SPI_GetNumChannels = (pfunc_SPI_GetNumChannels)lib.resolve("SPI_GetNumChannels");
          p_SPI_GetChannelInfo = (pfunc_SPI_GetChannelInfo)lib.resolve("SPI_GetChannelInfo");
          p_SPI_OpenChannel = (pfunc_SPI_OpenChannel)lib.resolve("SPI_OpenChannel");
          p_SPI_InitChannel = (pfunc_SPI_InitChannel)lib.resolve("SPI_InitChannel");
          p_SPI_CloseChannel = (pfunc_SPI_CloseChannel)lib.resolve("SPI_CloseChannel");
          p_SPI_Read = (pfunc_SPI_Read)lib.resolve("SPI_Read");
          p_SPI_Write = (pfunc_SPI_Write)lib.resolve("SPI_Write");
          p_SPI_ReadWrite = (pfunc_SPI_ReadWrite)lib.resolve("SPI_ReadWrite");
          p_SPI_IsBusy = (pfunc_SPI_IsBusy)lib.resolve("SPI_IsBusy");
          p_SPI_ChangeCS = (pfunc_SPI_ChangeCS)lib.resolve("SPI_ChangeCS");
          p_FT_WriteGPIO = (pfunc_FT_WriteGPIO)lib.resolve("FT_WriteGPIO");
          p_FT_ReadGPIO = (pfunc_FT_ReadGPIO)lib.resolve("FT_ReadGPIO");
      }

      if (!lib.isLoaded()) {
        qDebug() << lib.errorString();
        libDllLoaded = false;
      } else {
          libDllLoaded = true;
      }
    }

}

void atpFt232Spi::initSpiComunication() {
    uint16_t SPI_SPEED;
    SPI_SPEED = (uint16_t)(250000L / DEFAULT_SPI_SPEED);

    FT_STATUS status = FT_OK;
    FT_DEVICE_LIST_INFO_NODE devList = {0};
    ChannelConfig channelConf = {0};
    uint8 address = 0;
    channels = 0;
    uint16 data = 0;
    uint8 i = 0;
    uint8 latency = 255;

//    status = FT_SetLatencyTimer(ftHandle, 1);

    channelConf.ClockRate = SPI_SPEED;
    channelConf.LatencyTimer = latency;
    channelConf.configOptions = SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS3 | SPI_CONFIG_OPTION_CS_ACTIVELOW;
    channelConf.Pin = 0x00000000;/*FinalVal-FinalDir-InitVal-InitDir (for dir 0=in, 1=out)*/

    status = p_SPI_GetNumChannels(&channels);
    qDebug() <<"Number of available SPI channels = " << channels;

    if(channels>0) {
        for(i=0;i<channels;i++) {
            status = p_SPI_GetChannelInfo(i,&devList);
            qDebug() <<"Information on channel number " <<i << ":";
            /* print the dev info */
            qDebug() <<"Flags=" <<devList.Flags;
            qDebug() <<"Type=" <<devList.Type;
            qDebug() <<"ID=" <<devList.ID;
            qDebug() <<"LocId=" <<devList.LocId;
            qDebug() <<"SerialNumber=" <<devList.SerialNumber;
            qDebug() <<"Description=" <<devList.Description;
            qDebug() <<"ftHandle=" <<devList.ftHandle;
/*            printf("		Flags=0x%x\n",devList.Flags);
            printf("		Type=0x%x\n",devList.Type);
            printf("		ID=0x%x\n",devList.ID);
            printf("		LocId=0x%x\n",devList.LocId);
            printf("		SerialNumber=%s\n",devList.SerialNumber);
            printf("		Description=%s\n",devList.Description);
            printf("		ftHandle=0x%x\n",(unsigned int)devList.ftHandle);/*is 0 unless open*/
        }

        /* Open the first available channel */
        status = p_SPI_OpenChannel(CHANNEL_TO_OPEN,&ftHandle);
        printf("\nhandle=0x%x status=0x%x\n",(unsigned int)ftHandle,status);
        status = p_SPI_InitChannel(ftHandle,&channelConf);

#if USE_WRITEREAD
        {
            uint8 k,l;
            uint8 inBuffer[100];
            uint8 outBuffer[]={0x81,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,30,31,32,33,34,35,36,37,38,39};
            uint32 sizeToTransfer,sizeTransferred;
            for(k=0; k<5; k++) {
                printf("LoopCount = %u ",(unsigned)k);
                sizeToTransfer=10;
                sizeTransferred=0;
#if 1 // BYTES
                status = p_SPI_ReadWrite(ftHandle, inBuffer, outBuffer+k, sizeToTransfer, &sizeTransferred,
                    SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES|
                    SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE|
                    SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
#else // BITES
                status = p_SPI_ReadWrite(ftHandle, inBuffer, outBuffer+k, sizeToTransfer*8, &sizeTransferred,
                    SPI_TRANSFER_OPTIONS_SIZE_IN_BITS|
                    SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE|
                    SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
#endif
                printf("status=0x%x sizeTransferred=%u\n", status, sizeTransferred);
                for(l=0;l<sizeToTransfer;l++)
                    printf("0x%x\n",(unsigned)inBuffer[l]);
                printf("\n");
            }
        }
#else // USE_WRITEREAD
        for(address=START_ADDRESS_EEPROM;address<END_ADDRESS_EEPROM;address++) {
            printf("writing address = %02d data = %d\n",address,address+DATA_OFFSET);
            write_byte(SPI_SLAVE_0, address,(uint16)address+DATA_OFFSET);
        }

        for(address=START_ADDRESS_EEPROM;address<END_ADDRESS_EEPROM;address++) {
            read_byte(SPI_SLAVE_0, address,&data);
            printf("reading address = %02d data = %d\n",address,data);
        }
#endif // USE_WRITEREAD

    }

}

bool atpFt232Spi::isDllLybLoaded() {
    return libDllLoaded;
}

void atpFt232Spi::resetPinstateValue() {
    pinStateValue[1] = 0;
    pinStateValue[2] = 0;
    pinStateValue[3] = 0;
    pinStateValue[4] = 0;
    pinStateValue[5] = 0;
    pinStateValue[6] = 0;
    pinStateValue[7] = 0;
    pinStateValue[8] = 0;
}

uint8 atpFt232Spi::editPinStateValue(int pinNo, int value) {
    bool valoare = false;
   pinStateValue[pinNo] = value;
   QString pinValStr ="";
   for (int var = 1; var < 9; var++) {
       qDebug() <<"VAR: " <<var;
       pinValStr.append(QString::number(pinStateValue[var]));
       qDebug() <<"STR: " <<pinValStr;
   }
   qDebug() <<"STR: " <<pinValStr;
   int myVal = pinValStr.toInt(&valoare, 2);
   qDebug() <<"int: " <<myVal;

   return (uint8)myVal;
}
