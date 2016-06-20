#include "sm_hal_i2c.h"
#include "sm_hal_gpio.h"
#include "sm_hal_rcc.h"

#define I2C_SCL_PORT    GPIOB_BASE
#define I2C_SCL_PIN     10
#define I2C_SDA_PORT    GPIOB_BASE
#define I2C_SDA_PIN     11

#define I2C_DutyCycle_2                 ((uint16_t)0xBFFF) /*!< I2C fast mode Tlow/Thigh = 2 */
#define I2C_DutyCycle_16_9              ((uint16_t)0x4000) /*!< I2C fast mode Tlow/Thigh = 16/9 */
#define I2C_Mode_I2C                    ((uint16_t)0x0000)
#define I2C_Ack_Enable                  ((uint16_t)0x0400)
#define I2C_AcknowledgedAddress_7bit    ((uint16_t)0x4000)

#define I2C_NACKPosition_Next           ((uint16_t)0x0800)
#define I2C_NACKPosition_Current        ((uint16_t)0xF7FF)

#define I2C_SPEED       100000UL
#define I2C_DUTYCYCLE   I2C_DutyCycle_2
#define I2C_MODE        I2C_Mode_I2C
#define I2C_ACK         I2C_Ack_Enable
#define I2C_ACKADDR     I2C_AcknowledgedAddress_7bit
#define I2C_OWNADDR     1
#define I2C_TIMEOUT     100000

#define  I2C_EVENT_MASTER_BYTE_RECEIVED                    ((uint32_t)0x00030040)  /* BUSY, MSL and RXNE flags */
#define  I2C_EVENT_MASTER_MODE_SELECT                      ((uint32_t)0x00030001)  /* BUSY, MSL and SB flag */

void SmHalI2c::init(void)
{
    static bool isDone = false;

    if (isDone) return;

    // Configure I2C2 pins: PB10->SCL and PB11->SDA
    SmHalGpio<I2C_SCL_PORT, I2C_SCL_PIN> scl;
    SmHalGpio<I2C_SDA_PORT, I2C_SDA_PIN> sda;

    scl.setModeSpeed(SM_HAL_GPIO_MODE_AF_OD, SM_HAL_GPIO_SPEED_2M);
    sda.setModeSpeed(SM_HAL_GPIO_MODE_AF_OD, SM_HAL_GPIO_SPEED_2M);

    SmHalRcc::reset(RCC_PERIPH_I2C2);

    // Enable I2C2
#define CR1_PE_Set              ((uint16_t)0x0001)
    I2C2->CR1 |= CR1_PE_Set;

    uint32_t tmpreg = 0, freqrange = 0;
    uint32_t result = 0x04;
    uint32_t pclk1 = 8000000;

    /*---------------------------- I2Cx CR2 Configuration ------------------------*/
    /* Get the I2Cx CR2 value */
    tmpreg = I2C2->CR2;
    /* Clear frequency FREQ[5:0] bits */
#define CR2_FREQ_Reset          ((uint16_t)0xFFC0)
    tmpreg &= CR2_FREQ_Reset;
    pclk1 = SmHalRcc::getClocks()->PCLK1_Frequency;
    /* Set frequency bits depending on pclk1 value */
    freqrange = (uint16_t)(pclk1 / 1000000);
    tmpreg |= freqrange;
    /* Write to I2Cx CR2 */
    I2C2->CR2 = tmpreg;

    /*---------------------------- I2Cx CCR Configuration ------------------------*/
    /* Disable the selected I2C peripheral to configure TRISE */
#define CR1_PE_Reset            ((uint16_t)0xFFFE)
    I2C2->CR1 &= CR1_PE_Reset;
    /* Reset tmpreg value */
    /* Clear F/S, DUTY and CCR[11:0] bits */
    tmpreg = 0;

    /* Configure speed in standard mode */
    if (I2C_SPEED <= 100000)
    {
        /* Standard mode speed calculate */
        result = (uint16_t)(pclk1 / (I2C_SPEED << 1));
        /* Test if CCR value is under 0x4*/
        if (result < 0x04)
        {
            /* Set minimum allowed value */
            result = 0x04;
        }
        /* Set speed value for standard mode */
        tmpreg |= result;
        /* Set Maximum Rise Time for standard mode */
        I2C2->TRISE = freqrange + 1;
    }
    /* Configure speed in fast mode */
    else /*(I2C_InitStruct->I2C_ClockSpeed <= 400000)*/
    {
        if (I2C_DUTYCYCLE == I2C_DutyCycle_2)
        {
            /* Fast mode speed calculate: Tlow/Thigh = 2 */
            result = (uint16_t)(pclk1 / (I2C_SPEED * 3));
        }
        else /*I2C_InitStruct->I2C_DutyCycle == I2C_DutyCycle_16_9*/
        {
            /* Fast mode speed calculate: Tlow/Thigh = 16/9 */
            result = (uint16_t)(pclk1 / (I2C_SPEED * 25));
            /* Set DUTY bit */
            result |= I2C_DutyCycle_16_9;
        }

/* I2C CCR mask */
#define CCR_CCR_Set             ((uint16_t)0x0FFF)
        /* Test if CCR value is under 0x1*/
        if ((result & CCR_CCR_Set) == 0)
        {
            /* Set minimum allowed value */
            result |= (uint16_t)0x0001;
        }
/* I2C F/S mask */
#define CCR_FS_Set              ((uint16_t)0x8000)
        /* Set speed value and set F/S bit for fast mode */
        tmpreg |= (uint16_t)(result | CCR_FS_Set);
        /* Set Maximum Rise Time for fast mode */
        I2C2->TRISE = (uint16_t)(((freqrange * (uint16_t)300) / (uint16_t)1000) + (uint16_t)1);
    }

    /* Write to I2Cx CCR */
    I2C2->CCR = tmpreg;
    /* Enable the selected I2C peripheral */
    I2C2->CR1 |= CR1_PE_Set;

    /*---------------------------- I2Cx CR1 Configuration ------------------------*/
    /* Get the I2Cx CR1 value */
    tmpreg = I2C2->CR1;
/* I2C registers Masks */
#define CR1_CLEAR_Mask          ((uint16_t)0xFBF5)
    /* Clear ACK, SMBTYPE and  SMBUS bits */
    tmpreg &= CR1_CLEAR_Mask;
    /* Configure I2Cx: mode and acknowledgement */
    /* Set SMBTYPE and SMBUS bits according to I2C_Mode value */
    /* Set ACK bit according to I2C_Ack value */
    tmpreg |= (uint16_t)((uint32_t)I2C_MODE | I2C_ACK);
    /* Write to I2Cx CR1 */
    I2C2->CR1 = tmpreg;

    /*---------------------------- I2Cx OAR1 Configuration -----------------------*/
    /* Set I2Cx Own Address1 and acknowledged address */
    I2C2->OAR1 = (I2C_ACKADDR | I2C_OWNADDR);

    /* Enable the acknowledgement */
#define CR1_ACK_Set             ((uint16_t)0x0400)
    I2C2->CR1 |= CR1_ACK_Set;
}

void SmHalI2c::reset(void)
{
    SmHalGpio<I2C_SCL_PORT, I2C_SCL_PIN> scl;
    SmHalGpio<I2C_SDA_PORT, I2C_SDA_PIN> sda;

    scl.setModeSpeed(SM_HAL_GPIO_MODE_OUT_OD, SM_HAL_GPIO_SPEED_50M);
    sda.setModeSpeed(SM_HAL_GPIO_MODE_OUT_OD, SM_HAL_GPIO_SPEED_50M);

    sda.setPin();
    scl.setPin();

    // Clock out 9 SCL pulses
    for (int t = 0; t < I2C_TIMEOUT; t++);
    for (uint8_t i = 0; i < 9; i++)
    {
        scl.resetPin();
        for (int t = 0; t < I2C_TIMEOUT; t++);
        scl.setPin();
        for (int t = 0; t < I2C_TIMEOUT; t++);
    }

    // Initialize I2C hardware
    init();
}

bool SmHalI2c::start(void)
{
#define CR1_START_Set           ((uint16_t)0x0100)
    /* Generate a START condition */
    I2C2->CR1 |= CR1_START_Set;
    int t = 0;
    while(t++ <= I2C_TIMEOUT)
    {
        if (checkEvent(I2C_EVENT_MASTER_MODE_SELECT) != 0)
        {
            return true;
        }
    }
    // Failed, send STOP condition
    stop();
    return false;
}

bool SmHalI2c::stop(void)
{
#define CR1_STOP_Set            ((uint16_t)0x0200)
    /* Generate a STOP condition */
    I2C2->CR1 |= CR1_STOP_Set;
    int t = 0;
    while(t++ <= I2C_TIMEOUT)
    {
#define I2C_FLAG_BUSY                   ((uint32_t)0x00020000)
        if (getFlagStatus(I2C_FLAG_BUSY) == RESET)
        {
            return 1;
        }
    }
    // Failed, send STOP condition
    return false;
}

bool SmHalI2c::getFlagStatus(uint32_t I2C_FLAG)
{
    __IO uint32_t i2creg = 0, i2cxbase = 0;

    /* Get the I2Cx peripheral base address */
    i2cxbase = (uint32_t)I2C2;

    /* Read flag register index */
    i2creg = I2C_FLAG >> 28;

/* I2C FLAG mask */
#define FLAG_Mask               ((uint32_t)0x00FFFFFF)
    /* Get bit[23:0] of the flag */
    I2C_FLAG &= FLAG_Mask;

    if(i2creg != 0)
    {
        /* Get the I2Cx SR1 register address */
        i2cxbase += 0x14;
    }
    else
    {
        /* Flag in I2Cx SR2 Register */
        I2C_FLAG = (uint32_t)(I2C_FLAG >> 16);
        /* Get the I2Cx SR2 register address */
        i2cxbase += 0x18;
    }

    if(((*(__IO uint32_t *)i2cxbase) & I2C_FLAG) != (uint32_t)RESET)
    {
        /* I2C_FLAG is set */
        return true;
    }
    /* I2C_FLAG is reset */
    return false;
}


bool SmHalI2c::checkEvent(uint32_t event)
{
    uint32_t lastevent = 0;
    uint32_t flag1 = 0, flag2 = 0;

    /* Read the I2Cx status register */
    flag1 = I2C2->SR1;
    flag2 = I2C2->SR2;
    flag2 = flag2 << 16;

/* I2C FLAG mask */
#define FLAG_Mask               ((uint32_t)0x00FFFFFF)
    /* Get the last event value from I2C status register */
    lastevent = (flag1 | flag2) & FLAG_Mask;

    /* Check whether the last event contains the I2C_EVENT */
    if ((lastevent & event) == event)
    {
        return true;
    }
    return false;
}

bool SmHalI2c::transfer(uint8_t addr, uint8_t * wr, uint8_t wr_size, uint8_t * rd, uint8_t rd_size)
{
    uint8_t rx_done = 0;

    if (wr_size > 0)
    {
        if (start() == 0)
            return false;
        if (sendAddrWr(addr) == 0)
            return false;
        if (writeData(wr,wr_size) == 0)
            return false;
    }
    if (rd_size > 0)
    {
        if (start() == 0)
            return false;

        // Enable the acknowledgement
        I2C2->CR1 |= CR1_ACK_Set;

        if (sendAddrRd(addr) == 0)
            return false;

        for (uint8_t i = 0; i < rd_size; i++)
        {
            if (i == rd_size - 1)
            {
                // Disable the acknowledgement
                I2C2->CR1 &=~CR1_ACK_Set;
                // Generate STOP
                I2C2->CR1 |= CR1_STOP_Set;
            }
            int t = 0;
            rx_done = 0;
            while(t++ <= I2C_TIMEOUT)
            {
                if (checkEvent(I2C_EVENT_MASTER_BYTE_RECEIVED) != 0)
                {
                    rd[i] = I2C2->DR;
                    rx_done = 1;
                    break;
                }
            }
            if (rx_done == 0)
            {
                // Disable the acknowledgement
                I2C2->CR1 &=~CR1_ACK_Set;
                // Generate STOP
                I2C2->CR1 |= CR1_STOP_Set;
                return false;
            }
        }

    }
    if ((rd_size != 0) || (wr_size != 0))
    {
        // Enable the acknowledgement
        I2C2->CR1 &=~CR1_ACK_Set;
        stop();
    }
    return true;
//    1. Если txsize>0
//1.1 Передаём start.
//1.2 Передаём addr + W
//1.3 Передаём txsize байт начиная с адреса txdata
//2. Если rxsize>0
//2.1 Передаём start.
//2.2 Передаём addr + R
//1.3 Принимаем rxsize байт начиная с адреса rxdata
//3. Если (rxsize != 0) или (txsize != 0) передаём стоп, иначе возвращаем 0.
//В случае ошибки на любом этапе передаётся стоп и возвращается 0.
//В случае корректной работы возвращается 1.
}

bool SmHalI2c::sendAddrRd(uint16_t addr)
{
    addr <<= 1;

#define OAR1_ADD0_Set           ((uint16_t)0x0001)
    /* Set the address bit0 for read */
    addr |= OAR1_ADD0_Set;

    /* Send the address */
    I2C2->DR = addr;

    int t = 0;
    while(t++ <= I2C_TIMEOUT)
    {
#define  I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED           ((uint32_t)0x00030002)  /* BUSY, MSL and ADDR flags */
        if (checkEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) != 0)
        {
            return true;
        }
    }
    // Failed, send STOP condition
    stop();
    return false;
}

bool SmHalI2c::sendAddrWr(uint16_t addr)
{
    addr <<= 1;

    /* Reset the address bit0 for write */
    addr &=~OAR1_ADD0_Set;

    /* Send the address */
    I2C2->DR = addr;

    int t = 0;
    while(t++ <= I2C_TIMEOUT)
    {
#define  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED        ((uint32_t)0x00070082)  /* BUSY, MSL, ADDR, TXE and TRA flags */
        if (checkEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != 0)
        {
            return 1;
        }
    }
    // Failed, send STOP condition
    stop();
    return 0;
}

bool SmHalI2c::writeData(uint8_t * data, uint32_t datasize)
{
    uint8_t tx_done;

    for (uint8_t i = 0; i < datasize; i++)
    {
        // Write byte
        I2C2->DR = data[i];
        tx_done = 0;
        int t = 0;
        while(t++ <= I2C_TIMEOUT)
        {
#define  I2C_EVENT_MASTER_BYTE_TRANSMITTED                 ((uint32_t)0x00070084)  /* TRA, BUSY, MSL, TXE and BTF flags */
            if (checkEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED) != 0)
            {
                tx_done = 1;
                break;
            }
        }

        if (tx_done != 0)
            continue;

        /* Send STOP condition */
        stop();
        return 0;
    }
    return datasize;
}
