/**
  ******************************************************************************
  * @file:      g_i2c.c
  * @author:    anhgiau (nguyenanhgiau1008@gmail.com)
  * @version:   1.0.1
  * @date:      5/30/2018
  * @brief:     Implementation
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, WE SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
  * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT 
  * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
  * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  ******************************************************************************
 */
 
/* Includes ------------------------------------------------------------------*/
 #include "g_i2c.h"
 
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TIMEOUT(x, y)           timeout = y; \
                                while (x) { if (timeout-- == 0) return -1; }

#define I2C_SPEED               (400000)
#define I2C_SLAVE_ADDRESS7      (0xA0)
/* I2C SPE mask */
#define CR1_PE_Set              ((uint16_t)0x0001)
#define CR1_PE_Reset            ((uint16_t)0xFFFE)

/* I2C START mask */
#define CR1_START_Set           ((uint16_t)0x0100)
#define CR1_START_Reset         ((uint16_t)0xFEFF)

#define CR1_POS_Set           ((uint16_t)0x0800)
#define CR1_POS_Reset         ((uint16_t)0xF7FF)

/* I2C STOP mask */
#define CR1_STOP_Set            ((uint16_t)0x0200)
#define CR1_STOP_Reset          ((uint16_t)0xFDFF)

/* I2C ACK mask */
#define CR1_ACK_Set             ((uint16_t)0x0400)
#define CR1_ACK_Reset           ((uint16_t)0xFBFF)

/* I2C ENARP mask */
#define CR1_ENARP_Set           ((uint16_t)0x0010)
#define CR1_ENARP_Reset         ((uint16_t)0xFFEF)

/* I2C NOSTRETCH mask */
#define CR1_NOSTRETCH_Set       ((uint16_t)0x0080)
#define CR1_NOSTRETCH_Reset     ((uint16_t)0xFF7F)

/* I2C registers Masks */
#define CR1_CLEAR_Mask          ((uint16_t)0xFBF5)

/* I2C DMAEN mask */
#define CR2_DMAEN_Set           ((uint16_t)0x0800)
#define CR2_DMAEN_Reset         ((uint16_t)0xF7FF)

/* I2C LAST mask */
#define CR2_LAST_Set            ((uint16_t)0x1000)
#define CR2_LAST_Reset          ((uint16_t)0xEFFF)

/* I2C FREQ mask */
#define CR2_FREQ_Reset          ((uint16_t)0xFFC0)

/* I2C ADD0 mask */
#define OAR1_ADD0_Set           ((uint16_t)0x0001)
#define OAR1_ADD0_Reset         ((uint16_t)0xFFFE)

/* I2C ENDUAL mask */
#define OAR2_ENDUAL_Set         ((uint16_t)0x0001)
#define OAR2_ENDUAL_Reset       ((uint16_t)0xFFFE)

/* I2C ADD2 mask */
#define OAR2_ADD2_Reset         ((uint16_t)0xFF01)

/* I2C F/S mask */
#define CCR_FS_Set              ((uint16_t)0x8000)

/* I2C CCR mask */
#define CCR_CCR_Set             ((uint16_t)0x0FFF)

/* I2C FLAG mask */
#define FLAG_Mask               ((uint32_t)0x00FFFFFF)

/* I2C Interrupt Enable mask */
#define ITEN_Mask               ((uint32_t)0x07000000)


#define I2C_IT_BUF                      ((uint16_t)0x0400)
#define I2C_IT_EVT                      ((uint16_t)0x0200)
#define I2C_IT_ERR                      ((uint16_t)0x0100)


#define  ClockSpeed            400000

#define I2C_DIRECTION_TX 0
#define I2C_DIRECTION_RX 1

#define OwnAddress1 0x28
#define OwnAddress2 0x30


#define I2C1_DMA_CHANNEL_TX           DMA1_Channel6
#define I2C1_DMA_CHANNEL_RX           DMA1_Channel7

#define I2C2_DMA_CHANNEL_TX           DMA1_Channel4
#define I2C2_DMA_CHANNEL_RX           DMA1_Channel5

#define I2C1_DR_Address              0x40005410
#define I2C2_DR_Address              0x40005810
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Private function ----------------------------------------------------------*/

int 
g_i2c_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  I2C_InitTypeDef  I2C_InitStructure;
  
  // Enable GPIOB clocks
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  
  // Configure I2C clock and GPIO
  GPIO_StructInit(&GPIO_InitStructure);
  
  /* I2C1 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  
  /* I2C1 SDA and SCL configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  /* Configure I2Cx                */
  I2C_StructInit(&I2C_InitStructure);
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = OwnAddress1;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = ClockSpeed;
  
  I2C_Init(I2C1, &I2C_InitStructure);
  I2C_Cmd(I2C1, ENABLE); 
  return 0;
}

/**
*
*/
int 
g_i2c_device_ready(uint8_t slaveAddr)
{
  int result = -1;
  __IO uint32_t Timeout = 0;
  Timeout = 0xFFFF;
  while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)){
    if (Timeout-- == 0)
      return I2C_ERR_BUSY;
  }
  
  I2C_GenerateSTART(I2C1, ENABLE);
  Timeout = 0xFFFF;
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)){
    if (Timeout-- == 0)
      return I2C_ERR_WRITE;
  }
  
  I2C_SendData(I2C1, slaveAddr<<1);
  Timeout = 0xFFFF;
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if (Timeout-- == 0)
      return I2C_ERR_WRITE;
  }
  I2C_GenerateSTOP(I2C1, ENABLE);
  return result;
}

/**
*
*/
int 
g_i2c_write_bytes(uint8_t address, uint8_t *buf, uint8_t len)
{
  __IO uint32_t temp = 0;
  __IO uint32_t Timeout = 0;
  /* Enable Error IT (used in all modes: DMA, Polling and Interrupts */
  I2C1->CR2 |= I2C_IT_ERR;
  /* Send START condition */
  I2C1->CR1 |= CR1_START_Set;
  /* Wait until SB flag is set: EV5 */
  while ((I2C1->SR1&0x0001) != 0x0001){
    if (Timeout-- == 0)
      return I2C_ERR_WRITE;
  }
  
  /* Send slave address */
  address &= OAR1_ADD0_Reset;/* Reset the address bit0 for write*/
  /* Send the slave address */
  I2C1->DR = address;
  Timeout = 0xFFFF;
  /* Wait until ADDR is set: EV6 */
  while ((I2C1->SR1 &0x0002) != 0x0002){
    if (Timeout-- == 0)
      return I2C_ERR_WRITE;
  }
  
  /* Clear ADDR flag by reading SR2 register */
  temp = I2C1->SR2;
  /* Write the first data in DR register (EV8_1) */
  I2C1->DR = *buf;
  /* Increment */
  buf++;
  /* Decrement the number of bytes to be written */
  len--;
  /* While there is data to be written */
  while (len--){
    /* Poll on BTF to receive data because in polling mode we can not guarantee the
    EV8 software sequence is managed before the current byte transfer completes */
    while ((I2C1->SR1 & 0x00004) != 0x000004);
    /* Send the current byte */
    I2C1->DR = *buf;
    /* Point to the next byte to be written */
    buf++;
  }
  /* EV8_2: Wait until BTF is set before programming the STOP */
  while ((I2C1->SR1 & 0x00004) != 0x000004);
  /* Send STOP condition */
  I2C1->CR1 |= CR1_STOP_Set;
  /* Make sure that the STOP bit is cleared by Hardware */
  while ((I2C1->CR1&0x200) == 0x200);
  
  return I2C_ERR_NONE;
}

/**
*
*/
int 
g_i2c_write_reg(uint8_t address, uint8_t reg, uint8_t *buf, uint8_t len)
{
  __IO uint32_t temp = 0;
  __IO uint32_t Timeout = 0;
  /* Enable Error IT (used in all modes: DMA, Polling and Interrupts */
  I2C1->CR2 |= I2C_IT_ERR;
  /* Send START condition */
  I2C1->CR1 |= CR1_START_Set;
  /* Wait until SB flag is set: EV5 */
  while ((I2C1->SR1&0x0001) != 0x0001){
    if (Timeout-- == 0)
      return I2C_ERR_WRITE;
  }
  
  /* Send slave address */
  address &= OAR1_ADD0_Reset;/* Reset the address bit0 for write*/
  /* Send the slave address */
  I2C1->DR = address;
  Timeout = 0xFFFF;
  /* Wait until ADDR is set: EV6 */
  while ((I2C1->SR1 &0x0002) != 0x0002){
    if (Timeout-- == 0)
      return I2C_ERR_WRITE;
  }
  /* Clear ADDR flag by reading SR2 register */
  temp = I2C1->SR2;
  
  /* Send register address */
  I2C1->DR = reg;
  /* Poll on BTF to receive data because in polling mode we can not guarantee the
  EV8 software sequence is managed before the current byte transfer completes */
  while ((I2C1->SR1 & 0x00004) != 0x000004);
  
  /* Send START condition */
  I2C1->CR1 |= CR1_START_Set;
  /* Wait until SB flag is set: EV5 */
  while ((I2C1->SR1&0x0001) != 0x0001){
    if (Timeout-- == 0)
      return I2C_ERR_WRITE;
  }
  
  /* Send the slave address that will be write */
  I2C1->DR = address;
  Timeout = 0xFFFF;
  /* Wait until ADDR is set: EV6 */
  while ((I2C1->SR1 &0x0002) != 0x0002){
    if (Timeout-- == 0)
      return I2C_ERR_WRITE;
  }
  /* Clear ADDR flag by reading SR2 register */
  temp = I2C1->SR2;
  
  /* Write the first data in DR register (EV8_1) */
  I2C1->DR = *buf;
  /* Increment */
  buf++;
  /* Decrement the number of bytes to be written */
  len--;
  /* While there is data to be written */
  while (len--){
    /* Poll on BTF to receive data because in polling mode we can not guarantee the
    EV8 software sequence is managed before the current byte transfer completes */
    while ((I2C1->SR1 & 0x00004) != 0x000004);
    /* Send the current byte */
    I2C1->DR = *buf;
    /* Point to the next byte to be written */
    buf++;
  }
  /* EV8_2: Wait until BTF is set before programming the STOP */
  while ((I2C1->SR1 & 0x00004) != 0x000004);
  /* Send STOP condition */
  I2C1->CR1 |= CR1_STOP_Set;
  /* Make sure that the STOP bit is cleared by Hardware */
  while ((I2C1->CR1&0x200) == 0x200);
  
  return I2C_ERR_NONE;
}

/**
*
*/
int 
g_i2c_read_bytes(uint8_t address, uint8_t *buf, uint8_t len)
{
  __IO uint32_t temp = 0;
  __IO uint32_t Timeout = 0;
  /* Enable Error IT (used in all modes: DMA, Polling and Interrupts */
  I2C1->CR2 |= I2C_IT_ERR;
  
  if (len == 1){
    Timeout = 0xFFFF;
    /* Send START condition */
    I2C1->CR1 |= CR1_START_Set;
    /* Wait until SB flag is set: EV5  */
    while ((I2C1->SR1&0x0001) != 0x0001){
      if (Timeout-- == 0)
        return I2C_ERR_READ;
    }
    
    /* Send slave address */
    address &= OAR1_ADD0_Set;/* Set the address bit0 for read */
    /* Send the slave address */
    I2C1->DR = address;
    /* Wait until ADDR is set: EV6_3, then program ACK = 0, clear ADDR
    and program the STOP just after ADDR is cleared. The EV6_3 
    software sequence must complete before the current byte end of transfer.*/
    /* Wait until ADDR is set */
    Timeout = 0xFFFF;
    while ((I2C1->SR1&0x0002) != 0x0002){
      if (Timeout-- == 0)
        return I2C_ERR_READ;
    }
    
    /* Clear ACK bit */
    I2C1->CR1 &= CR1_ACK_Reset;
    /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
    software sequence must complete before the current byte end of transfer */
    __disable_irq();
    /* Clear ADDR flag */
    temp = I2C1->SR2;
    /* Program the STOP */
    I2C1->CR1 |= CR1_STOP_Set;
    /* Re-enable IRQs */
    __enable_irq();
    /* Wait until a data is received in DR register (RXNE = 1) EV7 */
    while ((I2C1->SR1 & 0x00040) != 0x000040);
    /* Read the data */
    *buf = I2C1->DR;
    /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
    while ((I2C1->CR1&0x200) == 0x200);
    /* Enable Acknowledgement to be ready for another reception */
    I2C1->CR1 |= CR1_ACK_Set;
  }
  else if (len == 2){
    /* Set POS bit */
    I2C1->CR1 |= CR1_POS_Set;
    Timeout = 0xFFFF;
    /* Send START condition */
    I2C1->CR1 |= CR1_START_Set;
    /* Wait until SB flag is set: EV5 */
    while ((I2C1->SR1&0x0001) != 0x0001){
      if (Timeout-- == 0)
        return I2C_ERR_READ;
    }
    Timeout = 0xFFFF;
    /* Send slave address */
    address |= OAR1_ADD0_Set;/* Set the address bit0 for read */
    /* Send the slave address */
    I2C1->DR = address;
    /* Wait until ADDR is set: EV6 */
    while ((I2C1->SR1&0x0002) != 0x0002){
      if (Timeout-- == 0)
        return I2C_ERR_READ;
    }
    /* EV6_1: The acknowledge disable should be done just after EV6,
    that is after ADDR is cleared, so disable all active IRQs around ADDR clearing and 
    ACK clearing */
    __disable_irq();
    /* Clear ADDR by reading SR2 register  */
    temp = I2C1->SR2;
    /* Clear ACK */
    I2C1->CR1 &= CR1_ACK_Reset;
    /*Re-enable IRQs */
    __enable_irq();
    /* Wait until BTF is set */
    while ((I2C1->SR1 & 0x00004) != 0x000004);
    /* Disable IRQs around STOP programming and data reading because of the limitation ?*/
    __disable_irq();
    /* Program the STOP */
    I2C1->CR1 |= CR1_STOP_Set;
    /* Read first data */
    *buf = I2C1->DR;
    /* Re-enable IRQs */
    __enable_irq();
    /* Increment pointer */
    buf++;
    /* Read second data */
    *buf = I2C1->DR;
    /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
    while ((I2C1->CR1&0x200) == 0x200);
    /* Enable Acknowledgement to be ready for another reception */
    I2C1->CR1  |= CR1_ACK_Set;
    /* Clear POS bit */
    I2C1->CR1  &= CR1_POS_Reset;
  }
  else {
    Timeout = 0xFFFF;
    /* Send START condition */
    I2C1->CR1 |= CR1_START_Set;
    /* Wait until SB flag is set: EV5 */
    while ((I2C1->SR1&0x0001) != 0x0001){
      if (Timeout-- == 0)
        return I2C_ERR_READ;
    }
    Timeout = 0xFFFF;
    /* Send slave address */
    address |= OAR1_ADD0_Set;/* Reset the address bit0 for write */
    /* Send the slave address */
    I2C1->DR = address;
    /* Wait until ADDR is set: EV6 */
    while ((I2C1->SR1&0x0002) != 0x0002){
      if (Timeout-- == 0)
        return I2C_ERR_READ;
    }
    /* Clear ADDR by reading SR2 status register */
    temp = I2C1->SR2;
    /* While there is data to be read */
    while (len){
      /* Receive bytes from first byte until byte N-3 */
      if (len != 3){
        /* Poll on BTF to receive data because in polling mode we can not guarantee the
        EV7 software sequence is managed before the current byte transfer completes */
        while ((I2C1->SR1 & 0x00004) != 0x000004);
        /* Read data */
        *buf = I2C1->DR;
        /*Increment*/
        buf++;
        /* Decrement the read bytes counter */
        len--;		
      }
      
      /* it remains to read three data: data N-2, data N-1, Data N */
      if (len == 3){
        /* Wait until BTF is set: Data N-2 in DR and data N -1 in shift register */
        while ((I2C1->SR1 & 0x00004) != 0x000004);
        /* Clear ACK */
        I2C1->CR1 &= CR1_ACK_Reset;
        
        /* Disable IRQs around data reading and STOP programming because of the
        limitation ? */
        __disable_irq();
        /* Read Data N-2 */
        *buf = I2C1->DR;
        /* Increment */
        buf++;
        /* Program the STOP */
        I2C1->CR1 |= CR1_STOP_Set;
        /* Read DataN-1 */
        *buf = I2C1->DR;
        /* Re-enable IRQs */
        __enable_irq();
        /* Increment */
        buf++;
        /* Wait until RXNE is set (DR contains the last data) */
        while ((I2C1->SR1 & 0x00040) != 0x000040);
        /* Read DataN */
        *buf = I2C1->DR;
        /* Reset the number of bytes to be read by master */
        len = 0;
      }
    }
  }
  
  /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
  while ((I2C1->CR1&0x200) == 0x200);
  /* Enable Acknowledgement to be ready for another reception */
  I2C1->CR1 |= CR1_ACK_Set;
  return I2C_ERR_NONE;
}

/**
*
*/
int 
g_i2c_read_reg(uint8_t address, uint8_t reg, uint8_t *buf, uint8_t len)
{
  __IO uint32_t temp = 0;
  __IO uint32_t Timeout = 0;
  /* Enable Error IT (used in all modes: DMA, Polling and Interrupts */
  I2C1->CR2 |= I2C_IT_ERR;
  
  if (len == 1){
    Timeout = 0xFFFF;
    /* Send START condition */
    I2C1->CR1 |= CR1_START_Set;
    /* Wait until SB flag is set: EV5 */
    while ((I2C1->SR1&0x0001) != 0x0001){
      if (Timeout-- == 0)
        return I2C_ERR_WRITE;
    }
    
    /* Send slave address */
    address &= OAR1_ADD0_Reset;/* Reset the address bit0 for write*/
    /* Send the slave address */
    I2C1->DR = address;
    Timeout = 0xFFFF;
    /* Wait until ADDR is set: EV6 */
    while ((I2C1->SR1 &0x0002) != 0x0002){
      if (Timeout-- == 0)
        return I2C_ERR_WRITE;
    }
    /* Clear ADDR flag by reading SR2 register */
    temp = I2C1->SR2;
    
    /* Send register address */
    I2C1->DR = reg;
    /* Poll on BTF to receive data because in polling mode we can not guarantee the
    EV8 software sequence is managed before the current byte transfer completes */
    while ((I2C1->SR1 & 0x00004) != 0x000004);
    
    /* Send START condition */
    I2C1->CR1 |= CR1_START_Set;
    /* Wait until SB flag is set: EV5  */
    while ((I2C1->SR1&0x0001) != 0x0001){
      if (Timeout-- == 0)
        return I2C_ERR_READ;
    }
    
    /* Send slave address */
    address &= OAR1_ADD0_Set;/* Set the address bit0 for read */
    /* Send the slave address */
    I2C1->DR = address;
    /* Wait until ADDR is set: EV6_3, then program ACK = 0, clear ADDR
    and program the STOP just after ADDR is cleared. The EV6_3 
    software sequence must complete before the current byte end of transfer.*/
    /* Wait until ADDR is set */
    Timeout = 0xFFFF;
    while ((I2C1->SR1&0x0002) != 0x0002){
      if (Timeout-- == 0)
        return I2C_ERR_READ;
    }
    
    /* Clear ACK bit */
    I2C1->CR1 &= CR1_ACK_Reset;
    /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
    software sequence must complete before the current byte end of transfer */
    __disable_irq();
    /* Clear ADDR flag */
    temp = I2C1->SR2;
    /* Program the STOP */
    I2C1->CR1 |= CR1_STOP_Set;
    /* Re-enable IRQs */
    __enable_irq();
    /* Wait until a data is received in DR register (RXNE = 1) EV7 */
    while ((I2C1->SR1 & 0x00040) != 0x000040);
    /* Read the data */
    *buf = I2C1->DR;
    /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
    while ((I2C1->CR1&0x200) == 0x200);
    /* Enable Acknowledgement to be ready for another reception */
    I2C1->CR1 |= CR1_ACK_Set;
  }
  else if (len == 2){
    /* Set POS bit */
    I2C1->CR1 |= CR1_POS_Set;
    Timeout = 0xFFFF;
    /* Send START condition */
    I2C1->CR1 |= CR1_START_Set;
    /* Wait until SB flag is set: EV5 */
    while ((I2C1->SR1&0x0001) != 0x0001){
      if (Timeout-- == 0)
        return I2C_ERR_READ;
    }
    Timeout = 0xFFFF;
    /* Send slave address */
    address |= OAR1_ADD0_Set;/* Set the address bit0 for read */
    /* Send the slave address */
    I2C1->DR = address;
    /* Wait until ADDR is set: EV6 */
    while ((I2C1->SR1&0x0002) != 0x0002){
      if (Timeout-- == 0)
        return I2C_ERR_READ;
    }
    /* EV6_1: The acknowledge disable should be done just after EV6,
    that is after ADDR is cleared, so disable all active IRQs around ADDR clearing and 
    ACK clearing */
    __disable_irq();
    /* Clear ADDR by reading SR2 register  */
    temp = I2C1->SR2;
    /* Clear ACK */
    I2C1->CR1 &= CR1_ACK_Reset;
    /*Re-enable IRQs */
    __enable_irq();
    /* Wait until BTF is set */
    while ((I2C1->SR1 & 0x00004) != 0x000004);
    /* Disable IRQs around STOP programming and data reading because of the limitation ?*/
    __disable_irq();
    /* Program the STOP */
    I2C1->CR1 |= CR1_STOP_Set;
    /* Read first data */
    *buf = I2C1->DR;
    /* Re-enable IRQs */
    __enable_irq();
    /* Increment pointer */
    buf++;
    /* Read second data */
    *buf = I2C1->DR;
    /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
    while ((I2C1->CR1&0x200) == 0x200);
    /* Enable Acknowledgement to be ready for another reception */
    I2C1->CR1  |= CR1_ACK_Set;
    /* Clear POS bit */
    I2C1->CR1  &= CR1_POS_Reset;
  }
  else {
    Timeout = 0xFFFF;
    /* Send START condition */
    I2C1->CR1 |= CR1_START_Set;
    /* Wait until SB flag is set: EV5 */
    while ((I2C1->SR1&0x0001) != 0x0001){
      if (Timeout-- == 0)
        return I2C_ERR_READ;
    }
    Timeout = 0xFFFF;
    /* Send slave address */
    address |= OAR1_ADD0_Set;/* Reset the address bit0 for write */
    /* Send the slave address */
    I2C1->DR = address;
    /* Wait until ADDR is set: EV6 */
    while ((I2C1->SR1&0x0002) != 0x0002){
      if (Timeout-- == 0)
        return I2C_ERR_READ;
    }
    /* Clear ADDR by reading SR2 status register */
    temp = I2C1->SR2;
    /* While there is data to be read */
    while (len){
      /* Receive bytes from first byte until byte N-3 */
      if (len != 3){
        /* Poll on BTF to receive data because in polling mode we can not guarantee the
        EV7 software sequence is managed before the current byte transfer completes */
        while ((I2C1->SR1 & 0x00004) != 0x000004);
        /* Read data */
        *buf = I2C1->DR;
        /*Increment*/
        buf++;
        /* Decrement the read bytes counter */
        len--;		
      }
      
      /* it remains to read three data: data N-2, data N-1, Data N */
      if (len == 3){
        /* Wait until BTF is set: Data N-2 in DR and data N -1 in shift register */
        while ((I2C1->SR1 & 0x00004) != 0x000004);
        /* Clear ACK */
        I2C1->CR1 &= CR1_ACK_Reset;
        
        /* Disable IRQs around data reading and STOP programming because of the
        limitation ? */
        __disable_irq();
        /* Read Data N-2 */
        *buf = I2C1->DR;
        /* Increment */
        buf++;
        /* Program the STOP */
        I2C1->CR1 |= CR1_STOP_Set;
        /* Read DataN-1 */
        *buf = I2C1->DR;
        /* Re-enable IRQs */
        __enable_irq();
        /* Increment */
        buf++;
        /* Wait until RXNE is set (DR contains the last data) */
        while ((I2C1->SR1 & 0x00040) != 0x000040);
        /* Read DataN */
        *buf = I2C1->DR;
        /* Reset the number of bytes to be read by master */
        len = 0;
      }
    }
  }
  
  /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
  while ((I2C1->CR1&0x200) == 0x200);
  /* Enable Acknowledgement to be ready for another reception */
  I2C1->CR1 |= CR1_ACK_Set;
  return I2C_ERR_NONE;
}