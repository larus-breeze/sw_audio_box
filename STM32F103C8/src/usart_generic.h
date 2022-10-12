/**
 * @file    usart_generic.h
 * @brief   implementation of generic usart functions
 * @author  Dr. Klaus Schaefer klaus.schaefer@h-da.de
 */
void USART_x_Init(unsigned baudrate)
{
  GPIO_InitTypeDef  GPIO_InitStruct={0};

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  USARTx_TX_GPIO_CLK_ENABLE();
  USARTx_RX_GPIO_CLK_ENABLE();
  /* Enable USARTx clock */
  USARTx_CLK_ENABLE();
#if USART_DMA
  DMAx_CLK_ENABLE();
#endif

  /*##-2- Configure peripheral GPIO ##########################################*/
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = USARTx_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;

  HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin 	    = USARTx_RX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;

  HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);

  USART_x_handle.Instance        = USARTx;

  USART_x_handle.Init.BaudRate   = baudrate;
  USART_x_handle.Init.WordLength = UART_WORDLENGTH_8B;
  USART_x_handle.Init.StopBits   = UART_STOPBITS_1;
  USART_x_handle.Init.Parity     = UART_PARITY_NONE;
  USART_x_handle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  USART_x_handle.Init.Mode       = UART_MODE_TX_RX;

  if(HAL_UART_DeInit(&USART_x_handle) != HAL_OK)
    Error_Handler();

  if(HAL_UART_Init(&USART_x_handle) != HAL_OK)
    Error_Handler();

  /* NVIC for USART */
  HAL_NVIC_SetPriority(USARTx_IRQn, 15, 0);
  HAL_NVIC_EnableIRQ(USARTx_IRQn);
}

extern "C" void USARTx_IRQHandler(void)
{
  HAL_UART_IRQHandler(&USART_x_handle);
}

#if USART_DMA

void UART_x_init_DMA(void)
{
  /*##-3- Configure the DMA ##################################################*/
    /* Configure the DMA handler for Transmission process */
    hdma_tx.Instance                 = USARTx_TX_DMA_CHANNEL;
    hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_tx.Init.Mode                = DMA_NORMAL;
    hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;

    HAL_DMA_Init(&hdma_tx);

    /* Associate the initialized DMA handle to the UART handle */
    __HAL_LINKDMA( &USART_x_handle, hdmatx, hdma_tx);

    /* Configure the DMA handler for reception process */
    hdma_rx.Instance                 = USARTx_RX_DMA_CHANNEL;
    hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_rx.Init.Mode                = DMA_NORMAL;
    hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;

    HAL_DMA_Init(&hdma_rx);

    /* Associate the initialized DMA handle to the the UART handle */
    __HAL_LINKDMA( &USART_x_handle, hdmarx, hdma_rx);

    /* NVIC configuration for DMA transfer complete interrupt (USART1_TX) */
    HAL_NVIC_SetPriority(USARTx_DMA_TX_IRQn, 15, 0);
    HAL_NVIC_EnableIRQ(USARTx_DMA_TX_IRQn);

    /* NVIC configuration for DMA transfer complete interrupt (USART1_RX) */
    HAL_NVIC_SetPriority(USARTx_DMA_RX_IRQn, 15, 0);
    HAL_NVIC_EnableIRQ(USARTx_DMA_RX_IRQn);
}

extern "C" void USARTx_DMA_TX_IRQHandler( void)
{
  HAL_DMA_IRQHandler(&hdma_tx);
}

extern "C" void USARTx_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_rx);
}
#endif
