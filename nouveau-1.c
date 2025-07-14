void init_CAN1()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_CAN1_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	//Configuration des pins TX et RX

	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	CAN1->MCR = 0xFFFFFFFD; //clear sleep bit to wakeup
	while(CAN1->MSR & 0x2); //wait for CAN to wakeup
	CAN1->MCR |= 0x0000001; //switch to config mode
	while(!(CAN1->MSR & 0x1)); //wait for config mode

	CAN1->MCR &= 0x00000001; //set config
	CAN1->BTR = 0x00250042; //set bit timing
	CAN1->RF0R |= CAN_RF0R_RFOM0; //release MailBox

	CAN1->IER |= 0x2; //set interrupt RX FIFO0 (FMPIE0)

	HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 1);//Activate interrupt on NVIC
	HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);

	CAN1->MCR &= 0xFFFFFFFE; //normal mode
	while(CAN1->MSR & 0x1); //wait for normal mode
	CAN1->FA1R &= 0xFFFFFFFE; //deactivate filter 0
	CAN1->FMR |= 0x1; //initialize mode

	CAN1->FS1R |= 0x0000001; //32 bits configuration
	CAN1->FM1R |= 0x00000001; //Filter 0
	CAN1->FFA1R &= 0xFFFFFFFE; //FIFO0
	CAN1->sFilterRegister[0].FR1 = (0x00C << 21) | (0x1<<1); //ID with RTR=1
	CAN1->sFilterRegister[0].FR2 = (0x00C<<21) | (0x0<<1); //ID with RTR=0

	CAN1->FA1R |= 0x00000001; //active filtre
	CAN1->FMR &= 0x0; //leave filter init
	
	//Version 1.2
