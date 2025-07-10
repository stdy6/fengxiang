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
	
	char sign_buf[64];								//保存签名的Base64编码结果 和 URL编码结果
	char hmac_sha1_buf[64];							//保存签名
	char access_key_base64[64];						//保存access_key的Base64编码结合
	char string_for_signature[72];					//保存string_for_signature，这个是加密的key

//----------------------------------------------------参数合法性--------------------------------------------------------------------
	if(ver == (void *)0 || res == (void *)0 || et < 1564562581 || access_key == (void *)0
		|| authorization_buf == (void *)0 || authorization_buf_len < 120)
		return 1;
	
//----------------------------------------------------将access_key进行Base64解码----------------------------------------------------
	memset(access_key_base64, 0, sizeof(access_key_base64));
	BASE64_Decode((unsigned char *)access_key_base64, sizeof(access_key_base64), &olen, (unsigned char *)access_key, strlen(access_key));
	//UsartPrintf(USART_DEBUG, "access_key_base64: %s\r\n", access_key_base64);
	
//----------------------------------------------------计算string_for_signature-----------------------------------------------------
	memset(string_for_signature, 0, sizeof(string_for_signature));
	if(flag)
		snprintf(string_for_signature, sizeof(string_for_signature), "%d\n%s\nproducts/%s\n%s", et, METHOD, res, ver);
	else
		snprintf(string_for_signature, sizeof(string_for_signature), "%d\n%s\nproducts/%s/devices/%s\n%s", et, METHOD, res, dev_name, ver);
	//UsartPrintf(USART_DEBUG, "string_for_signature: %s\r\n", string_for_signature);
	
//----------------------------------------------------加密-------------------------------------------------------------------------
	memset(hmac_sha1_buf, 0, sizeof(hmac_sha1_buf));
	
	hmac_sha1((unsigned char *)access_key_base64, strlen(access_key_base64),
				(unsigned char *)string_for_signature, strlen(string_for_signature),
				(unsigned char *)hmac_sha1_buf);
	
	//UsartPrintf(USART_DEBUG, "hmac_sha1_buf: %s\r\n", hmac_sha1_buf);
	
//----------------------------------------------------将加密结果进行Base64编码------------------------------------------------------
	olen = 0;
	memset(sign_buf, 0, sizeof(sign_buf));
	BASE64_Encode((unsigned char *)sign_buf, sizeof(sign_buf), &olen, (unsigned char *)hmac_sha1_buf, strlen(hmac_sha1_buf));

//----------------------------------------------------将Base64编码结果进行URL编码---------------------------------------------------
	OTA_UrlEncode(sign_buf);
	//UsartPrintf(USART_DEBUG, "sign_buf: %s\r\n", sign_buf);
	
//----------------------------------------------------计算Token--------------------------------------------------------------------
	if(flag)
		snprintf(authorization_buf, authorization_buf_len, "version=%s&res=products%%2F%s&et=%d&method=%s&sign=%s", ver, res, et, METHOD, sign_buf);
	else
		snprintf(authorization_buf, authorization_buf_len, "version=%s&res=products%%2F%s%%2Fdevices%%2F%s&et=%d&method=%s&sign=%s", ver, res, dev_name, et, METHOD, sign_buf);
	//UsartPrintf(USART_DEBUG, "Token: %s\r\n", authorization_buf);
	
	return 0;

}

//	函数名称：	OneNet_DevLink
//	函数功能：	与onenet创建连接
_Bool OneNet_DevLink(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//协议包

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
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//上传平台
		
		dataPtr = ESP8266_GetIPD(250);	
								//等待平台响应
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
		
		MQTT_DeleteBuffer(&mqttPacket);								//删包
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

//	函数名称：	OneNet_SendData
//	函数功能：	上传数据到平台
void OneNet_SendData(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//协议包
	
	char buf[1024];
	
	short body_len = 0, i = 0;
	
	// UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-MQTT\r\n");
	
	memset(buf, 0, sizeof(buf));
	
	body_len = OneNet_FillBuf(buf);						
	
	// UsartPrintf(USART_DEBUG, "buf===%s\r\n",buf);											//获取当前需要发送的数据流的总长度
	
	if(body_len)
	{
		if(MQTT_PacketSaveData(PROID, DEVICE_NAME, body_len, NULL, &mqttPacket) == 0)				//封包
		{
			for(; i < body_len; i++)
				mqttPacket._data[mqttPacket._len++] = buf[i];
			
			ESP8266_SendData(mqttPacket._data, mqttPacket._len);									//上传数据到平台
			// UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", mqttPacket._len);
			
			MQTT_DeleteBuffer(&mqttPacket);															//删包
		}
		// else
			// UsartPrintf(USART_DEBUG, "WARN:	EDP_NewBuffer Failed\r\n");
	}
	
}

//	函数名称：	OneNET_Publish
//	函数功能：	发布消息
void OneNET_Publish(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};						//协议包
	
	// UsartPrintf(USART_DEBUG, "Publish Topic: %s, Msg: %s\r\n", topic, msg);
	
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqtt_packet) == 0)
	{
		ESP8266_SendData(mqtt_packet._data, mqtt_packet._len);					//向平台发送订阅请求
		
		MQTT_DeleteBuffer(&mqtt_packet);										//删包
	}

}

//	函数名称：	OneNET_Subscribe
//	函数功能：	订阅
void OneNET_Subscribe(void)
{
	
	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};						//协议包
	
	char topic_buf[56];
	const char *topic = topic_buf;
	
	snprintf(topic_buf, sizeof(topic_buf), "$sys/%s/%s/thing/property/set", PROID, DEVICE_NAME);
	
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, &topic, 1, &mqtt_packet) == 0)
	{
		ESP8266_SendData(mqtt_packet._data, mqtt_packet._len);					//向平台发送订阅请求
		MQTT_DeleteBuffer(&mqtt_packet);										//删包
	}

}

//	函数名称：	OneNet_RevPro
//	函数功能：	平台返回数据检测
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
		case MQTT_PKT_PUBLISH:																//接收的Publish消息
		
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
			
		case MQTT_PKT_PUBACK:														//发送Publish消息，平台回复的Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				// UsartPrintf(USART_DEBUG, "Tips:	MQTT Publish Send OK\r\n");
			
		break;
		
		case MQTT_PKT_SUBACK:																//发送Subscribe消息的Ack
		
			if(MQTT_UnPacketSubscribe(cmd) == 0)
				// UsartPrintf(USART_DEBUG, "Tips:	MQTT Subscribe OK\r\n");
			// else
				// UsartPrintf(USART_DEBUG, "Tips:	MQTT Subscribe Err\r\n");
		
		break;
		
		default:
			result = -1;
		break;
	}
	
	ESP8266_Clear();									//清空缓存
	
	if(result == -1)
		return;
}
