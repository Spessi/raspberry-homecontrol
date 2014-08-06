#ifndef RF433__H
#define RF433__H

#define RF433_TX_PIN 0
#define RF433_SOCKET_NBR_MAX 3


/* home codes */
#define RF433_HOUSECODE_1	0x1FFF


/* remote codes */
#define RF433_RECEIVER1_1	0x1FF0
#define RF433_RECEIVER1_2	0xF1F0
#define RF433_RECEIVER1_3	0xFF10

/* status codes */
#define RF433_ON		0x0010
#define RF433_OFF		0x0001


/* prototypes */
void rf433_init(void);
int cmd_rf(char*, int);
void rf433_sendHigh(void);
void rf433_sendLow(void);
void rf433_sendSync(void);
void rf433_powerOn(unsigned char nbr);
void rf433_powerOff(unsigned char nbr);

#endif