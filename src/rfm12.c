#include "stm32f10x.h"
#include "delay.h"

#define SS_LO() GPIOB->BSRR = GPIO_BSRR_BR12
#define SS_HI() GPIOB->BSRR = GPIO_BSRR_BS12
#define SCK_LO() GPIOB->BSRR = GPIO_BSRR_BR13
#define SCK_HI() GPIOB->BSRR = GPIO_BSRR_BS13
#define MOSI_LO() GPIOB->BSRR = GPIO_BSRR_BR15
#define MOSI_HI() GPIOB->BSRR = GPIO_BSRR_BS15

typedef enum {
	RGITFFIT=0x8000,  //TX ready for next byte or FIFO received data Status
	POR=0x4000, // Power on Reset Status
	RGURFFOV=0x2000,	// TX Register underun or RX FIFO Overflow Status
	WKUP=0x1000,      // Wakeup Timer overflow Status
	INTEXT=0x0800,      // Interrup on external source Status
	LBD=0x0400,      // Low battery detect Status
	FFEM=0x0200,     // FIFO Empty Status
	ATS=0x0100,      // Antenna Tuning Signal Detect Status
	RSSI=0x0080,      // Received Signal Strength Indicator Status
	DQD=0x0040,      // Data Quality Dedector Status
	CRL=0x0020,      // Clock Recovery Locked status
	ATGL=0x0010,      // Toggling in each AFC Cycle
	OFFS_SIGN=0x0008,      // Measured Offset Frequency Sign Value 1='+', 0='-'
	OFFS=0x0004,      // Measured offset Frequency value (3 bits)
	OFFS_MASK=0x0003      // Measured offset mask
} RFM12Status;

void GPIO_config() {
	GPIO_InitTypeDef gpioInit;

	RCC_APB2PeriphClockCmd(RCC_APB2ENR_IOPBEN, ENABLE);

	gpioInit.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_13; //MOSI, SCK
	gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpioInit);

	gpioInit.GPIO_Pin = GPIO_Pin_14; //MISO
	gpioInit.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOB, &gpioInit);

	gpioInit.GPIO_Pin = GPIO_Pin_12; //SS
	gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &gpioInit);

	gpioInit.GPIO_Pin = GPIO_Pin_5; //NIRQ
	gpioInit.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &gpioInit);

	gpioInit.GPIO_Pin = GPIO_Pin_6; //NRES
	gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &gpioInit);

	SS_HI();
	SCK_LO();
	MOSI_LO();
}

static void __fixedDelay() {
	__NOP();__NOP();__NOP();__NOP();__NOP();
}

void RFM12_writeCMD(uint16_t cmd) {
	SS_LO();
	for(int i = 16; i --> 0; ) {
		if((cmd >> i) & 1) {
			MOSI_HI();
		} else {
			MOSI_LO();
		}
		SCK_LO();
		__fixedDelay();
		SCK_HI();
		__fixedDelay();
	}
	SCK_LO();
	SS_HI();
}

int RFM12_readStatus() {
	SS_LO();
	uint16_t status = 0;
	for(int i = 16; i --> 0; ) {
		MOSI_LO();
		SCK_LO();
		__fixedDelay();
		SCK_HI();
		__fixedDelay();
		if(GPIOB->IDR & GPIO_IDR_IDR14) {
			status |= (1 << i);
		}
	}
	SCK_LO();
	SS_HI();
	return status;
}

void RFM12_send(uint8_t data) {
	while(GPIOB->IDR & GPIO_IDR_IDR5);
	RFM12_writeCMD(0xB800 | data);
}

void RFM12_init() {
	RFM12_writeCMD(0x80D7); //"Configuration Settings": 433MHz, 12pF, TXreg, RXFIFOBuf
	RFM12_writeCMD(0x8208); //"Power Management": osc. on
	RFM12_writeCMD(0xA620); //"Frequency Setting": 433.9200MHz
	RFM12_writeCMD(0xC623); //"Data Rate": 9.579kbps
	RFM12_writeCMD(0x94A0); //"Receiver Control": PINVDI, VDIFAST, RXBW=134kHz, LNAG=0dBm, DRSSI=-103dBm
	RFM12_writeCMD(0xC2AC); //"Data Filter & Clock Recovery": RCEOVERMODE=AUTO, RCEOVERSPEED=SLOW, QATH=4, FILTTYPE=DIGITAL
	RFM12_writeCMD(0xCA83); //"FIFO and Reset Mode": FIFOINTLevel=8, FIFOFillStart=Sync, RSTSENS=Low, SYNC=2bytes, FIFO Fill
	RFM12_writeCMD(0xCED4); //"Synchronization Pattern":SYNC=2DD4；
	RFM12_writeCMD(0xC483); //"Automatic Frequency Control": AFC=EN, OFFSREG=EN, NO RSTRIC
	RFM12_writeCMD(0x9820); //"TX Control": 45kHz, MAX OUT
	RFM12_writeCMD(0xCC57); //"PLL Settings": DITHER=DISABLE, CLKRISE=SLOW, PLLB=256kbps
	RFM12_writeCMD(0xE000); //"Wake-Up Timer":NOT USE
	RFM12_writeCMD(0xC800); //"Low Duty-Cycle":NOT USE
	RFM12_writeCMD(0xC080); //"Low Battery Detect and µC Clock":2.5MHz,2.2V
}

void RFM12_reset() {
	GPIOB->BSRR = GPIO_BSRR_BR6;
	delayMs(100);
	GPIOB->BSRR = GPIO_BSRR_BS6;
}

void RFM12_setBand(int band) {
	RFM12_writeCMD(0x80C7 | ((band & 0x3) << 4));
}

void RFM12_setFreq(int band, uint32_t freq) {
	if(band == 0) {
		RFM12_writeCMD(0xA000 | ((10 * (freq - 310000) / 25) & 0xFFF));
	} else if(band == 1) {
		RFM12_writeCMD(0xA000 | ((10 * (freq - 430000) / 25) & 0xFFF));
	} else if(band == 2) {
		RFM12_writeCMD(0xA000 | ((10 * (freq - 860000) / 50) & 0xFFF));
	} else if(band == 3) {
		RFM12_writeCMD(0xA000 | ((10 * (freq - 900000) / 75) & 0xFFF));
	}
}

//static uint32_t getRand() {
//    static uint32_t lfsr = 1;
//    return (lfsr = (lfsr >> 1) ^ ((uint32_t)(0 - (uint32_t)(lfsr & 1)) & 0xC0109200));
//}

int main() {
	delayInit();
	GPIO_config();
	RFM12_reset();
	delayMs(200);
	RFM12_init();

//	RFM12_writeCMD(0x0);
//	RFM12_writeCMD(0xB8AA);
//	RFM12_writeCMD(0xB8AA);
//	RFM12_writeCMD(0x8238); //TXON
//	delayMs(1);


	while (1) {
	RFM12_writeCMD(0x0);
	RFM12_writeCMD(0xB8AA);
	RFM12_writeCMD(0xB8AA);
	RFM12_writeCMD(0x8238); //TXON
	delayMs(1);
	RFM12_writeCMD(0x8208); //TXOFF
	delayMs(1);

//		uint32_t r = getRand();
//		RFM12_send((r >> 0) & 0xFF);
//		RFM12_send((r >> 8) & 0xFF);
//		RFM12_send((r >> 16) & 0xFF);
//		RFM12_send((r >> 24) & 0xFF);
	}
}
