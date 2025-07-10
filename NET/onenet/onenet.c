#include "debug.h"
#include "esp8266.h"
#include "onenet.h"
#include "mqttkit.h"
#include "base64.h"
#include "hmac_sha1.h"
#include "usart.h"
#include "usart3.h"
#include "cJSON.h"
#include "dht11.h"
#include "CountSensor.h"
#include "Kalman.h"
#include "math.h"
#include <string.h>
#include <stdio.h>


#define PROID			"6N5F59AL4m"

#define ACCESS_KEY		"P8VHaZP5qUsjXSnVB4HXxScdK9NBUTVjQzVdK9vZcyc="

#define DEVICE_NAME		"test"


char devid[16];

char key[48];

cJSON *raw_json,*params_json,*led_json;
extern int SETTEMP;					
extern unsigned char esp8266_buf[512];
extern float weight;
extern int he;
extern uint16_t co2;
extern u16 light;
extern float TEMP;	
extern int ang;

static unsigned char OTA_UrlEncode(char *sign)
{

	char sign_t[40];
	unsigned char i = 0, j = 0;
	unsigned char sign_len = strlen(sign);
	
	if(sign == (void *)0 || sign_len < 28)
		return 1;
	
	for(; i < sign_len; i++)
	{
		sign_t[i] = sign[i];
		sign[i] = 0;
	}
	sign_t[i] = 0;
	
	for(i = 0, j = 0; i < sign_len; i++)
	{
		switch(sign_t[i])
		{
			case '+':
				strcat(sign + j, "%2B");j += 3;
			break;
			
			case ' ':
				strcat(sign + j, "%20");j += 3;
			break;
			
			case '/':
				strcat(sign + j, "%2F");j += 3;
			break;
			
			case '?':
				strcat(sign + j, "%3F");j += 3;
			break;
			
			case '%':
				strcat(sign + j, "%25");j += 3;
			break;
			
			case '#':
				strcat(sign + j, "%23");j += 3;
			break;
			
			case '&':
				strcat(sign + j, "%26");j += 3;
			break;
			
			case '=':
				strcat(sign + j, "%3D");j += 3;
			break;
			
			default:
				sign[j] = sign_t[i];j++;
			break;
		}
	}
	
	sign[j] = 0;
	
	return 0;

}

#define METHOD		"sha1"
static unsigned char OneNET_Authorization(char *ver, char *res, unsigned int et, char *access_key, char *dev_name,
											char *authorization_buf, unsigned short authorization_buf_len, _Bool flag)
{
	
	size_t olen = 0;
	
	char sign_buf[64];								//����ǩ����Base64������ �� URL������
	char hmac_sha1_buf[64];							//����ǩ��
	char access_key_base64[64];						//����access_key��Base64������
	char string_for_signature[72];					//����string_for_signature������Ǽ��ܵ�key

//----------------------------------------------------�����Ϸ���--------------------------------------------------------------------
	if(ver == (void *)0 || res == (void *)0 || et < 1564562581 || access_key == (void *)0
		|| authorization_buf == (void *)0 || authorization_buf_len < 120)
		return 1;
	
//----------------------------------------------------��access_key����Base64����----------------------------------------------------
	memset(access_key_base64, 0, sizeof(access_key_base64));
	BASE64_Decode((unsigned char *)access_key_base64, sizeof(access_key_base64), &olen, (unsigned char *)access_key, strlen(access_key));
	//UsartPrintf(USART_DEBUG, "access_key_base64: %s\r\n", access_key_base64);
	
//----------------------------------------------------����string_for_signature-----------------------------------------------------
	memset(string_for_signature, 0, sizeof(string_for_signature));
	if(flag)
		snprintf(string_for_signature, sizeof(string_for_signature), "%d\n%s\nproducts/%s\n%s", et, METHOD, res, ver);
	else
		snprintf(string_for_signature, sizeof(string_for_signature), "%d\n%s\nproducts/%s/devices/%s\n%s", et, METHOD, res, dev_name, ver);
	//UsartPrintf(USART_DEBUG, "string_for_signature: %s\r\n", string_for_signature);
	
//----------------------------------------------------����-------------------------------------------------------------------------
	memset(hmac_sha1_buf, 0, sizeof(hmac_sha1_buf));
	
	hmac_sha1((unsigned char *)access_key_base64, strlen(access_key_base64),
				(unsigned char *)string_for_signature, strlen(string_for_signature),
				(unsigned char *)hmac_sha1_buf);
	
	//UsartPrintf(USART_DEBUG, "hmac_sha1_buf: %s\r\n", hmac_sha1_buf);
	
//----------------------------------------------------�����ܽ������Base64����------------------------------------------------------
	olen = 0;
	memset(sign_buf, 0, sizeof(sign_buf));
	BASE64_Encode((unsigned char *)sign_buf, sizeof(sign_buf), &olen, (unsigned char *)hmac_sha1_buf, strlen(hmac_sha1_buf));

//----------------------------------------------------��Base64����������URL����---------------------------------------------------
	OTA_UrlEncode(sign_buf);
	//UsartPrintf(USART_DEBUG, "sign_buf: %s\r\n", sign_buf);
	
//----------------------------------------------------����Token--------------------------------------------------------------------
	if(flag)
		snprintf(authorization_buf, authorization_buf_len, "version=%s&res=products%%2F%s&et=%d&method=%s&sign=%s", ver, res, et, METHOD, sign_buf);
	else
		snprintf(authorization_buf, authorization_buf_len, "version=%s&res=products%%2F%s%%2Fdevices%%2F%s&et=%d&method=%s&sign=%s", ver, res, dev_name, et, METHOD, sign_buf);
	//UsartPrintf(USART_DEBUG, "Token: %s\r\n", authorization_buf);
	
	return 0;

}

//	�������ƣ�	OneNet_DevLink
//	�������ܣ�	��onenet��������
_Bool OneNet_DevLink(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//Э���

	unsigned char *dataPtr;
	
	char authorization_buf[160];
	
	_Bool status = 1;
	
	OneNET_Authorization("2018-10-31", PROID, 1956499200, ACCESS_KEY, DEVICE_NAME,
								authorization_buf, sizeof(authorization_buf), 0);
	
	// UsartPrintf(USART_DEBUG, "OneNET_DevLink\r\n"
	// 						"NAME: %s,	PROID: %s,	KEY:%s\r\n"
    //                     , DEVICE_NAME, PROID, authorization_buf);
	
	if(MQTT_PacketConnect(PROID, authorization_buf, DEVICE_NAME, 256, 1, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//�ϴ�ƽ̨
		
		dataPtr = ESP8266_GetIPD(250);	
								//�ȴ�ƽ̨��Ӧ
		if(dataPtr != NULL)
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch(MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0:status = 0;break;
					
					case 1:break;
					case 2:break;
					case 3:break;
					case 4:break;
					case 5:break;
					
					default:break;
				}
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//ɾ��
	}
	// else
	// 	UsartPrintf(USART_DEBUG, "WARN:	MQTT_PacketConnect Failed\r\n");
	
return status;

}

unsigned char OneNet_FillBuf(char *buf)
{
	
	char text[100];
	
	memset(text, 0, sizeof(text));
	
	strcpy(buf, "{\"id\":\"123\",\"params\":{");

	memset(text, 0, sizeof(text));
	sprintf(text, "\"Temperature\":{\"value\":%d},",(int)TEMP);       
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"Humidity\":{\"value\":%d},",(int)DHT11.humidity);
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "\"Count\":{\"value\":%d},",CountSensor_Get1()-CountSensor_Get2());
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "\"Weight\":{\"value\":%d},",(int)weight);
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "\"HE\":{\"value\":%d},",he);
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "\"CO2\":{\"value\":%d},",co2);
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "\"LIGHT\":{\"value\":%d},",light);
	strcat(buf, text);
	
if(ang==0){
	memset(text, 0, sizeof(text));
	sprintf(text, "\"WARN\":{\"value\":%s},","true");
	strcat(buf, text);
}
else{
	memset(text, 0, sizeof(text));
	sprintf(text, "\"WARN\":{\"value\":%s},","false");
	strcat(buf, text);
}

if(weight>800){
	memset(text, 0, sizeof(text));
	sprintf(text, "\"notice\":{\"value\":%s},","true");
	strcat(buf, text);}
else{
	memset(text, 0, sizeof(text));
	sprintf(text, "\"notice\":{\"value\":%s},","false");
	strcat(buf, text);
}
	memset(text, 0, sizeof(text));
	sprintf(text, "\"SETTEMP\":{\"value\":%d}",SETTEMP);
	strcat(buf, text);

	strcat(buf, "}}");
	
	return strlen(buf);

}

//	�������ƣ�	OneNet_SendData
//	�������ܣ�	�ϴ����ݵ�ƽ̨
void OneNet_SendData(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//Э���
	
	char buf[1024];
	
	short body_len = 0, i = 0;
	
	// UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-MQTT\r\n");
	
	memset(buf, 0, sizeof(buf));
	
	body_len = OneNet_FillBuf(buf);						
	
	// UsartPrintf(USART_DEBUG, "buf===%s\r\n",buf);											//��ȡ��ǰ��Ҫ���͵����������ܳ���
	
	if(body_len)
	{
		if(MQTT_PacketSaveData(PROID, DEVICE_NAME, body_len, NULL, &mqttPacket) == 0)				//���
		{
			for(; i < body_len; i++)
				mqttPacket._data[mqttPacket._len++] = buf[i];
			
			ESP8266_SendData(mqttPacket._data, mqttPacket._len);									//�ϴ����ݵ�ƽ̨
			// UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", mqttPacket._len);
			
			MQTT_DeleteBuffer(&mqttPacket);															//ɾ��
		}
		// else
			// UsartPrintf(USART_DEBUG, "WARN:	EDP_NewBuffer Failed\r\n");
	}
	
}

//	�������ƣ�	OneNET_Publish
//	�������ܣ�	������Ϣ
void OneNET_Publish(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};						//Э���
	
	// UsartPrintf(USART_DEBUG, "Publish Topic: %s, Msg: %s\r\n", topic, msg);
	
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqtt_packet) == 0)
	{
		ESP8266_SendData(mqtt_packet._data, mqtt_packet._len);					//��ƽ̨���Ͷ�������
		
		MQTT_DeleteBuffer(&mqtt_packet);										//ɾ��
	}

}

//	�������ƣ�	OneNET_Subscribe
//	�������ܣ�	����
void OneNET_Subscribe(void)
{
	
	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};						//Э���
	
	char topic_buf[56];
	const char *topic = topic_buf;
	
	snprintf(topic_buf, sizeof(topic_buf), "$sys/%s/%s/thing/property/set", PROID, DEVICE_NAME);
	
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, &topic, 1, &mqtt_packet) == 0)
	{
		ESP8266_SendData(mqtt_packet._data, mqtt_packet._len);					//��ƽ̨���Ͷ�������
		MQTT_DeleteBuffer(&mqtt_packet);										//ɾ��
	}

}

//	�������ƣ�	OneNet_RevPro
//	�������ܣ�	ƽ̨�������ݼ��
void OneNet_RevPro(unsigned char *cmd)
{
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short topic_len = 0;
	unsigned short req_len = 0;
	
	unsigned char qos = 0;
	static unsigned short pkt_id = 0;
	
	unsigned char type = 0;
	
	short result = 0;

	// char *dataPtr = NULL;
	// char numBuf[10];
	// int num = 0;
	type = MQTT_UnPacketRecv(cmd);
	switch(type)
	{
		case MQTT_PKT_PUBLISH:																//���յ�Publish��Ϣ
		
			result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
			if(result == 0)
			{
				// char *data_ptr = NULL;
				
				// UsartPrintf(USART_DEBUG, "topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",
				// 	cmdid_topic, topic_len, req_payload, req_len);
					cJSON *raw_json = cJSON_Parse(req_payload);
	                cJSON *params_json = cJSON_GetObjectItem(raw_json,"params");			
	                cJSON *settemp_json = cJSON_GetObjectItem(params_json,"SETTEMP");	
					cJSON *he_json = cJSON_GetObjectItem(params_json,"HE");	

					if(settemp_json != NULL)
					{
                    SETTEMP =  settemp_json->valueint ;
					}
					
					if(he_json != NULL)
					{
                    he =  he_json->valueint ;
					}
                    cJSON_Delete(raw_json);
			}
			
		case MQTT_PKT_PUBACK:														//����Publish��Ϣ��ƽ̨�ظ���Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				// UsartPrintf(USART_DEBUG, "Tips:	MQTT Publish Send OK\r\n");
			
		break;
		
		case MQTT_PKT_SUBACK:																//����Subscribe��Ϣ��Ack
		
			if(MQTT_UnPacketSubscribe(cmd) == 0)
				// UsartPrintf(USART_DEBUG, "Tips:	MQTT Subscribe OK\r\n");
			// else
				// UsartPrintf(USART_DEBUG, "Tips:	MQTT Subscribe Err\r\n");
		
		break;
		
		default:
			result = -1;
		break;
	}
	
	ESP8266_Clear();									//��ջ���
	
	if(result == -1)
		return;
}
