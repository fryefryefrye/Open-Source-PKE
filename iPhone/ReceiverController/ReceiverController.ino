#include <SPI.h>
#include <RF24.h>
#include <RF24BLE.h>
#include <printf.h>

#include "irk.h"







#define MAC_LEN 6
#define RECV_PAYLOAD_SIZE 28

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7, 8);
RF24BLE BLE(radio);
/**********************************************************/

void BleDataCheckTask();
unsigned char input[32]={0};
//there are 3 channels at which BLE broadcasts occur
//hence channel can be 0,1,2 
byte channel =0; //using single channel to receive

void setup()
{
    Serial.begin(115200);
    Serial.println(F("RF24_BLE_address"));
    printf_begin();

	BLE.recvBegin(RECV_PAYLOAD_SIZE,channel);
}




void loop()
{
	BleDataCheckTask();
} // Loop





void BleDataCheckTask()
{
	byte status=BLE.recvPacket((uint8_t*)input,RECV_PAYLOAD_SIZE,channel);

	unsigned char AdMac[MAC_LEN];
	//0x40 = Advertising package with a random private address. 
	if(input[0]==0x40)
	{
		//Get the MAC address. Reverse order in BT payload.
		for (byte i = 0; i < MAC_LEN; i++)
		{
			AdMac[MAC_LEN-1-i] = input[i+2];
		}
		printf("Check = %02X %02X %02X %02X %02X %02X\r\n"
			,AdMac[0],AdMac[1],AdMac[2],AdMac[3],AdMac[4],AdMac[5]);

		for (byte i = 0; i < IRK_LIST_NUMBER; i++)
		{
			//Check with all IRK we got one by one.
			if(btm_ble_addr_resolvable(AdMac,irk[i]))
			{
				printf("MacAdd= %02X %02X %02X %02X %02X %02X Belongs to:%s\r\n"
					,AdMac[0],AdMac[1],AdMac[2],AdMac[3],AdMac[4],AdMac[5]
				,IrkListName[i]);
			}
		}
		return;
	}
}

