#include <cstring>
#include <cstdio>
#include "sm_hw_bt.h"
#include "sm_hal_rtc.h"
#include "sm_crc.h"
#include "sm_desktop.h"

#ifndef PC_SOFTWARE
#include "sm_hal_gpio.h"
#include "sm_hal_rcc.h"

#define BT_RX_PORT  GPIOA_BASE
#define BT_RX_PIN   10
#define BT_TX_PORT  GPIOA_BASE
#define BT_TX_PIN   9
#define BT_EN_PORT  GPIOB_BASE
#define BT_EN_PIN   4
#define BT_ST_PORT  GPIOA_BASE
#define BT_ST_PIN   3


#define USART_BAUD          115200UL
#define USART_BAUD_PREINIT  9600UL
#define USART_BASE          USART1_BASE

#define USART_WORDLENGTH_8B             0UL
#define USART_STOPBITS_1                0UL
#define USART_PARITY_NO                 0UL
#define USART_FLOWCONTROL_NONE          0UL
#define USART_MODE_RX                   0x04UL
#define USART_MODE_TX                   0x08UL

#define USART_FLAG_CTS                       ((uint16_t)0x0200)
#define USART_FLAG_LBD                       ((uint16_t)0x0100)
#define USART_FLAG_TXE                       ((uint16_t)0x0080)
#define USART_FLAG_TC                        ((uint16_t)0x0040)
#define USART_FLAG_RXNE                      ((uint16_t)0x0020)
#define USART_FLAG_IDLE                      ((uint16_t)0x0010)
#define USART_FLAG_ORE                       ((uint16_t)0x0008)
#define USART_FLAG_NE                        ((uint16_t)0x0004)
#define USART_FLAG_FE                        ((uint16_t)0x0002)
#define USART_FLAG_PE                        ((uint16_t)0x0001)

#define CR1_UART_ENABLE                 0x2000UL  /*!< USART Enable Mask */
#else
#include <QDebug>
#endif // !PC_SOFTWARE

void SmHwBt::init(void)
{
    static_assert(sizeof(SmHwBtPacket) == SM_HW_BT_PACKET_SIZE, "Size of SmHwBtPacket must be equal to SM_HW_BT_PACKET_SIZE!");
    // Clear received data
    memset(&mRxPacket,0,sizeof(SmHwBtPacket));
    mHeader = nullptr;
    mText = nullptr;
    mHeaderSize = 0;
    mTextSize = 0;
#ifndef PC_SOFTWARE
    mPowerPin = new SmHalGpio<BT_EN_PORT, BT_EN_PIN>();
    mPowerPin->setModeSpeed(SM_HAL_GPIO_MODE_OUT_PP, SM_HAL_GPIO_SPEED_2M);

    mRxPin = new SmHalGpio<BT_RX_PORT, BT_RX_PIN>();
    mTxPin = new SmHalGpio<BT_TX_PORT, BT_TX_PIN>();

    mRxPin->setModeSpeed(SM_HAL_GPIO_MODE_IN_FLOAT, SM_HAL_GPIO_SPEED_2M);
    mTxPin->setModeSpeed(SM_HAL_GPIO_MODE_AF_PP, SM_HAL_GPIO_SPEED_2M);

    mStPin = new SmHalGpio<BT_ST_PORT, BT_ST_PIN>();
    mStPin->setModeSpeed(SM_HAL_GPIO_MODE_IN_FLOAT, SM_HAL_GPIO_SPEED_2M);
#endif
    // Initialize default BT state
    enable();
#ifndef PC_SOFTWARE
    // Enable clocking
    SmHalRcc::clockEnable(RCC_PERIPH_USART1);

    // Enable DMA1 (will be used for transmission)
    SmHalRcc::clockEnable(RCC_PERIPH_DMA1);

    // Set baud rate
    setBaud(USART_BAUD);
#endif // !PC_SOFTWARE
}

void SmHwBt::preConf(void)
{
    if (SmHalRtc::getInstance()->isHm10Preconf() == true)
        return;

#ifndef PC_SOFTWARE
    // Configuration is not done yet
    setBaud(USART_BAUD_PREINIT);
#endif
    disable();

    // Wait a bit before ADC calibration
    uint32_t timestamp = SmHalSysTimer::getTimeStamp();
    timestamp += 100;    // 100ms
    // Wait a bit before proceed
    while (SmHalSysTimer::getTimeStamp() <= timestamp);
}

void SmHwBt::enable(void)
{
#ifndef PC_SOFTWARE
    mTxPin->setModeSpeed(SM_HAL_GPIO_MODE_AF_PP, SM_HAL_GPIO_SPEED_2M);
    mPowerPin->resetPin();
#endif
}

void SmHwBt::disable(void)
{
#ifndef PC_SOFTWARE
    mTxPin->setModeSpeed(SM_HAL_GPIO_MODE_IN_FLOAT, SM_HAL_GPIO_SPEED_2M);
    mPowerPin->setPin();
#endif
}

/// @todo implement properly
void SmHwBt::send(void)
{
#ifdef PC_SOFTWARE
    EmulatorWindow::getInstance()->sendPacket((char *)&mTxPacket, sizeof(mTxPacket));
#else
    static uint32_t tmp = 2017;
    sprintf((char *)&mTxPacket, "BLABLABLA %u", tmp++);
    uint32_t tmpreg = 0;
    // USART1 DMA TX disable
    DMA1_Channel4->CCR &=~DMA_CCR1_EN;

    /*--------------------------- DMAy Channelx CNDTR Configuration ---------------*/
    /* Write to DMAy Channelx CNDTR */
    DMA1_Channel4->CNDTR = sizeof(mTxPacket);

    // USART1 DMA TX enable
    DMA1_Channel4->CCR |= DMA_CCR1_EN;
#endif
}

bool SmHwBt::isConnected(void)
{
#ifndef PC_SOFTWARE
    return mStPin->readPin();
#else
    return true;
#endif
}

#ifdef PC_SOFTWARE
void SmHwBt::injectPacket(char *data, uint8_t size)
{
    if (size > sizeof(SmHwBtPacket))
        size = sizeof(SmHwBtPacket);
    memcpy(&mRxPacket,data,size);
    mRxDone = true;
}
#endif

void SmHwBt::update(void)
{
    if (mRxDone)
    {
        mRxDone = false;

        // Check received packet crc32
        uint32_t receivedCRC = SmCrc::calc32(0xFFFFFFFFUL, mRxPacket.content.raw, sizeof(mRxPacket.content.raw));
        if (mRxPacket.header.crc32 != receivedCRC)
        {
            // Wrong CRC32, nothing to do, just return
            // qDebug("Wrong CRC32\n");
            return;
        }

        //qDebug("");

        /// @todo Decryption can be done here, packet type and packet content will be decrypted

        // Check received packet type
        if (mRxPacket.header.type >= SM_HW_BT_PACKET_TYPE_MAX)
        {
            // Wrong type, nothing to do, just return
            //return;
        }

        // Everything seems to be fine, proceed to the content parsing

        // Set default response values
        mTxPacket.header.type = SM_HW_BT_PACKET_ACK;
        mTxPacket.content.ack.seqNumber = 0;
        mTxPacket.content.ack.type = mRxPacket.header.type;

        SmHalRtc::SmHalRtcTime datetime;

        switch (mRxPacket.header.type)
        {
            case SM_HW_BT_PACKET_DATETIME:
                // Get required fields from received packed
                datetime.year   = mRxPacket.content.datetime.year;
                datetime.month  = mRxPacket.content.datetime.month;
                datetime.day    = mRxPacket.content.datetime.day;
                datetime.hour   = mRxPacket.content.datetime.hour;
                datetime.minute = mRxPacket.content.datetime.minute;
                datetime.second = mRxPacket.content.datetime.second;

                //char buff[100];
                //sprintf(buff,"DD/MM/YYYY HH:MM:SS = %02u/%02u/%04u %02u:%02u:%02u\n",
                //        datetime.day,  datetime.month,  datetime.year,
                //        datetime.hour, datetime.minute, datetime.second);
                //qDebug() << buff;
                // Set local date/time
                SmHalRtc::getInstance()->setDateTime(datetime);
                break;
            case SM_HW_BT_PACKET_VERSION:
                strcpy(mTxPacket.content.version, SmDesktop::getInstance()->getVersion());
                break;
            case SM_HW_BT_PACKET_NOTIFICATION_HEADER:
                /// TODO: implement correct numbers
                mHeaderSize = 10;
                break;
            default:
                return;
        }

        mTxPacket.header.crc32 = SmCrc::calc32(0xFFFFFFFFUL, mTxPacket.content.raw, sizeof(mTxPacket.content.raw));

        send();
    }
}

void SmHwBt::setBaud(uint32_t baud)
{
#ifdef PC_SOFTWARE
    Q_UNUSED(baud)
#else
    // Init USART 115200_8n1
    uint32_t wordLength = USART_WORDLENGTH_8B;
    uint32_t stopBits = USART_STOPBITS_1;
    uint32_t parity = USART_PARITY_NO;
    uint32_t flowControl = USART_FLOWCONTROL_NONE;
    uint32_t mode = USART_MODE_RX | USART_MODE_TX;

    uint32_t tmpreg = 0x00, apbclock = 0x00;
    uint32_t integerdivider = 0x00;
    uint32_t fractionaldivider = 0x00;
    uint32_t usartxbase = 0;

/*---------------------------- USART CR2 Configuration -----------------------*/
    tmpreg = ((USART_TypeDef * )USART_BASE)->CR2;
    /* Clear STOP[13:12] bits */
#define CR2_STOP_CLEAR_Mask       ((uint32_t)0xCFFFUL)  /*!< USART CR2 STOP Bits Mask */
    tmpreg &= CR2_STOP_CLEAR_Mask;
    tmpreg |= stopBits;
    ((USART_TypeDef * )USART_BASE)->CR2 = (uint16_t)tmpreg;

/*---------------------------- USART CR1 Configuration -----------------------*/
    tmpreg = ((USART_TypeDef * )USART_BASE)->CR1;
    /* Clear M, PCE, PS, TE and RE bits */
#define CR1_CLEAR_Mask            ((uint16_t)0xE9F3)  /*!< USART CR1 Mask */
    tmpreg &= CR1_CLEAR_Mask;
    tmpreg |= wordLength | parity | mode;
    ((USART_TypeDef * )USART_BASE)->CR1 = (uint16_t)tmpreg;

/*---------------------------- USART CR3 Configuration -----------------------*/
    tmpreg = ((USART_TypeDef * )USART_BASE)->CR3;
    /* Clear CTSE and RTSE bits */
#define CR3_CLEAR_Mask            ((uint16_t)0xFCFF)  /*!< USART CR3 Mask */
    tmpreg &= CR3_CLEAR_Mask;
    tmpreg |= flowControl;
    ((USART_TypeDef * )USART_BASE)->CR3 = (uint16_t)tmpreg;

/*---------------------------- USART BRR Configuration -----------------------*/
    /* Configure the USART Baud Rate -------------------------------------------*/
    if (USART_BASE == USART1_BASE)
    {
        apbclock = SmHalRcc::getClocks()->PCLK2_Frequency;
    }
    else
    {
        apbclock = SmHalRcc::getClocks()->PCLK1_Frequency;
    }
    /* Determine the integer part */
#define CR1_OVER8_Set             ((u16)0x8000)  /* USART OVER8 mode Enable Mask */
    if ((((USART_TypeDef * )USART_BASE)->CR1 & CR1_OVER8_Set) != 0)
    {
        /* Integer part computing in case Oversampling mode is 8 Samples */
        integerdivider = ((25 * apbclock) / (2 * (baud)));
    }
    else /* if ((USARTx->CR1 & CR1_OVER8_Set) == 0) */
    {
        /* Integer part computing in case Oversampling mode is 16 Samples */
        integerdivider = ((25 * apbclock) / (4 * (baud)));
    }
    tmpreg = (integerdivider / 100) << 4;

    /* Determine the fractional part */
    fractionaldivider = integerdivider - (100 * (tmpreg >> 4));

    /* Implement the fractional part in the register */
    if ((((USART_TypeDef * )USART_BASE)->CR1 & CR1_OVER8_Set) != 0)
    {
        tmpreg |= ((((fractionaldivider * 8) + 50) / 100)) & ((uint8_t)0x07);
    }
    else /* if ((USARTx->CR1 & CR1_OVER8_Set) == 0) */
    {
        tmpreg |= ((((fractionaldivider * 16) + 50) / 100)) & ((uint8_t)0x0F);
    }

    /* Write to USART BRR */
    ((USART_TypeDef * )USART_BASE)->BRR = (uint16_t)tmpreg;

    /* Enable the selected USART by setting the UE bit in the CR1 register */
    ((USART_TypeDef * )USART_BASE)->CR1 |= CR1_UART_ENABLE;

#define USART_DMAReq_Tx                      ((uint16_t)0x0080)
    ((USART_TypeDef * )USART_BASE)->CR3 |= USART_DMAReq_Tx;

    /*--------------------------- DMAy Channelx CCR Configuration -----------------*/
    /* Get the DMAy_Channelx CCR value */
    tmpreg = DMA1_Channel4->CCR;
    /* Clear MEM2MEM, PL, MSIZE, PSIZE, MINC, PINC, CIRC and DIR bits */
#define CCR_CLEAR_Mask           ((uint32_t)0xFFFF800F)
    tmpreg &= CCR_CLEAR_Mask;
    /* Configure DMAy Channelx: data transfer, data size, priority level and mode */
    /* Set DIR bit according to DMA_DIR value */
    /* Set CIRC bit according to DMA_Mode value */
    /* Set PINC bit according to DMA_PeripheralInc value */
    /* Set MINC bit according to DMA_MemoryInc value */
    /* Set PSIZE bits according to DMA_PeripheralDataSize value */
    /* Set MSIZE bits according to DMA_MemoryDataSize value */
    /* Set PL bits according to DMA_Priority value */
    /* Set the MEM2MEM bit according to DMA_M2M value */
#define DMA_DIR_PeripheralDST              ((uint32_t)0x00000010)
#define DMA_Mode_Normal                    ((uint32_t)0x00000000)
#define DMA_PeripheralInc_Disable          ((uint32_t)0x00000000)
#define DMA_MemoryInc_Enable               ((uint32_t)0x00000080)
#define DMA_PeripheralDataSize_Byte        ((uint32_t)0x00000000)
#define DMA_MemoryDataSize_Byte            ((uint32_t)0x00000000)
#define DMA_Priority_Low                   ((uint32_t)0x00000000)
#define DMA_M2M_Disable                    ((uint32_t)0x00000000)
    tmpreg |= DMA_DIR_PeripheralDST | DMA_Mode_Normal |
            DMA_PeripheralInc_Disable | DMA_MemoryInc_Enable |
            DMA_PeripheralDataSize_Byte | DMA_MemoryDataSize_Byte |
            DMA_Priority_Low | DMA_M2M_Disable;

    /* Write to DMAy Channelx CCR */
    DMA1_Channel4->CCR = tmpreg;

    /*--------------------------- DMAy Channelx CNDTR Configuration ---------------*/
    /* Write to DMAy Channelx CNDTR */
    DMA1_Channel4->CNDTR = sizeof(mTxPacket);

/*--------------------------- DMAy Channelx CPAR Configuration ----------------*/
    /* Write to DMAy Channelx CPAR */
    DMA1_Channel4->CPAR = (uint32_t)&(USART1->DR);

/*--------------------------- DMAy Channelx CMAR Configuration ----------------*/
    /* Write to DMAy Channelx CMAR */
    DMA1_Channel4->CMAR = (uint32_t)&mTxPacket;
#endif
}

bool SmHwBt::isNotification(void)
{
    return (mHeaderSize != 0);
}

void SmHwBt::clearNotification(void)
{
    if (mHeader != nullptr)
        delete mHeader;
    if (mText != nullptr)
        delete mText;

    mText = nullptr;
    mHeader = nullptr;
    mHeaderSize = 0;
}

void SmHwBt::getHeader(uint16_t** ppHeader, uint16_t* pHeaderSize)
{
    *ppHeader = mHeader;
    *pHeaderSize = mHeaderSize;
}

void SmHwBt::getText(uint16_t** ppText, uint16_t* pTextSize )
{
    *ppText = mText;
    *pTextSize = mTextSize;
}
