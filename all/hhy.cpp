#include "sysinclude.h"
#pragma pack(1)
extern void bgp_FsmTryToConnectPeer();
extern void bgp_FsmSendTcpData(char *pBuf,DWORD dwLen);

#define BGP_VERSION 4

/*notification错误类型*/
#define BGP_MESSAGE_HEAD_ERROR 1
#define BGP_OPEN_MESSAGE_ERROR 2
#define BGP_UPDATE_MESSAGE_ERROR 3
#define BGP_HOLD_TIMEOUT_ERROR 4
#define BGP_FSM_ERROR 5
#define BGP_FSM_EXIT 6

/*TCP异常类型*/
#define BGP_TCP_CLOSE 1 /*tcp close*/
#define BGP_TCP_FATAL_ERROR 2 /*BGP Transport fatal error*/
#define BGP_TCP_RETRANSMISSION_TIMEOUT 3 /*retransmission timeout*/

/*结构体的大小*/
#define BGPHEAD_SIZE 19
#define OPENHEAD_SIZE 10
#define NOTIFYHEAD_SIZE 2


/*定义几个常用的结构体*/
/*BGP消息头*/
struct BGPHead{
	BYTE marker[16];
	WORD length;
	BYTE type;
};

/*OPEN消息头*/
struct OPENHead{
	BYTE version;
	WORD mas;
	WORD holdTime;
	DWORD id;
	BYTE oplen;
};

/*notification消息头*/
struct NOTIFYHead{
	BYTE errorCode;
	BYTE errSubCode;
};

/*三个发送消息函数，过程为：构造->发送*/
/*keepalive消息发送函数*/
void sendKeepAliveMessage() {
	//构造BGP头
	BGPHead* bgpHead = new BGPHead;
	for (int i = 0; i < 16 * sizeof(BYTE); ++i)
		bgpHead->marker[i] = 0xff;
	bgpHead->length = htons(BGPHEAD_SIZE);
	bgpHead->type = BGP_KEEPALIVE;
	
	//发送
	bgp_FsmSendTcpData((char*)bgpHead, BGPHEAD_SIZE);
}

void sendNotificationMessage(BYTE type) {
	//构造BGP头
	int size = BGPHEAD_SIZE + NOTIFYHEAD_SIZE;
	BYTE* message = new BYTE[size];//maker鉴权信息
	for (int i = 0; i < 16 * sizeof(BYTE); ++i)
		message[i] = 0xff;
	BGPHead* bgpHead = (BGPHead*)message;
	bgpHead->length = htons(size);
	bgpHead->type = BGP_NOTIFY;
	
	//构造notification消息头
	NOTIFYHead* notifyHead = (NOTIFYHead*)(message + BGPHEAD_SIZE);
	notifyHead->errorCode = type;
	notifyHead->errSubCode = 0;//辅助码，此时任意
	
	//发送
	bgp_FsmSendTcpData((char*)message, size);
}

void sendOpenMessage(BgpPeer* pPeer) {
	//构造BGP头
	int size = BGPHEAD_SIZE + OPENHEAD_SIZE;
	BYTE* message = new BYTE[size];
	for (int i = 0; i < 16; ++i)
		message[i] = 0xff;
	BGPHead* bgpHead = (BGPHead*)message;
	bgpHead->length = htons(size);
	bgpHead->type = BGP_OPEN;
	
	//构造OPEN头
	OPENHead* openHead = (OPENHead*)(message + BGPHEAD_SIZE);
	openHead->version = 4;
	openHead->mas = htons(pPeer->bgp_wMyAS);
	openHead->holdTime = htons(pPeer->bgp_dwCfgHoldtime);
	openHead->oplen = 0;
	openHead->id = htonl(pPeer->bgp_dwMyRouterID);
	
	//发送
	bgp_FsmSendTcpData((char*)message, size);
}

//IE 10
BYTE stud_bgp_FsmEventOpen(BgpPeer *pPeer,BYTE *pBuf,unsigned int len) 
{
	//首先检查消息格式是否合法
	OPENHead* openHead = (OPENHead*)(pBuf + BGPHEAD_SIZE);
	BGPHead* bgpHead = (BGPHead*)pBuf;
	
	//如果BGP消息头长度不符，或者open消息的版本号不是ipv4号
	if((ntohs(bgpHead->length) < 29) || (openHead->version != 4)){
		pPeer->bgp_byState = BGP_STATE_IDLE;                          //转回idle状态
		sendNotificationMessage(BGP_OPEN_MESSAGE_ERROR);              //发送notification消息
		return BYTE(-1);
	}
	
	//open消息格式正确
	//如果是opensent状态，发送KeepAlive并转到openconfirm，否则转回idle状态
	if (pPeer->bgp_byState == BGP_STATE_OPENSENT) {
		pPeer->bgp_byState = BGP_STATE_OPENCONFIRM;
		sendKeepAliveMessage();
	}else
		pPeer->bgp_byState = BGP_STATE_IDLE;
	return 0;
}

//IE 11
BYTE stud_bgp_FsmEventKeepAlive(BgpPeer *pPeer,BYTE *pBuf,unsigned int len)
{
	//首先检查消息格式是否合法
	BGPHead* bgpHead = (BGPHead*)pBuf;
	
	//如果BGP消息头长度不符
	if(ntohs(bgpHead->length) < BGPHEAD_SIZE){
		pPeer->bgp_byState = BGP_STATE_IDLE;                              //转回idle状态
		sendNotificationMessage(BGP_MESSAGE_HEAD_ERROR);                      //发送notification消息
		return BYTE(-1);
	}
	
	//合法的KeepAlive消息
	if (pPeer->bgp_byState == BGP_STATE_OPENCONFIRM || pPeer->bgp_byState == BGP_STATE_ESTABLISHED)
		pPeer->bgp_byState = BGP_STATE_ESTABLISHED;
	else
		pPeer->bgp_byState = BGP_STATE_IDLE;
	return 0;
}

//IE 13
BYTE stud_bgp_FsmEventNotification(BgpPeer *pPeer,BYTE *pBuf,unsigned int len)
{
	//首先检查消息格式是否合法
	NOTIFYHead* notifyHead = (NOTIFYHead*)(pBuf + NOTIFYHEAD_SIZE);
	BGPHead* bgpHead = (BGPHead*)pBuf;
	
	//如果BGP消息头长度不符
	if(ntohs(bgpHead->length) < BGPHEAD_SIZE + NOTIFYHEAD_SIZE){
		pPeer->bgp_byState = BGP_STATE_IDLE;                            //转回idle状态
		sendNotificationMessage(BGP_MESSAGE_HEAD_ERROR);                    //发送notification消息
		return BYTE(-1);
	}
	
	//不合法的消息和合法的消息都会转入IDLE状态，但是返回值不同
	//合法的消息
	pPeer->bgp_byState = BGP_STATE_IDLE;
	return 0;
}

//IE 12
BYTE stud_bgp_FsmEventUpdate(BgpPeer *pPeer,BYTE *pBuf,unsigned int len)
{
	//首先检查消息格式是否合法
	//如果BGP消息头长度不符
	BGPHead* bgpHead = (BGPHead*)pBuf;
	if (htons(bgpHead->length) != 23) {
		pPeer->bgp_byState = BGP_STATE_IDLE;                             //转回idle状态
		sendNotificationMessage(BGP_UPDATE_MESSAGE_ERROR);               //发送notification消息
		return BYTE(-1);
	}
	
	//在非established状态下接收到update消息，转回IDLE状态
	if (pPeer->bgp_byState != BGP_STATE_ESTABLISHED)
		pPeer->bgp_byState = BGP_STATE_IDLE;
	
	return 0;
}

//IE 4-6        
BYTE stud_bgp_FsmEventTcpException(BgpPeer *pPeer, BYTE msgType)           
{
	switch(msgType) {
	case BGP_TCP_CLOSE:						//IE 4
		if (pPeer->bgp_byState == BGP_STATE_OPENSENT)
			pPeer->bgp_byState = BGP_STATE_ACTIVE;
		else
			pPeer->bgp_byState = BGP_STATE_IDLE;
		break;
	case BGP_TCP_FATAL_ERROR:				//IE 6
		pPeer->bgp_byState = BGP_STATE_IDLE;
		break;
	case BGP_TCP_RETRANSMISSION_TIMEOUT:	//IE 5
		if (pPeer->bgp_byState == BGP_STATE_CONNECT || pPeer->bgp_byState == BGP_STATE_ACTIVE)
			pPeer->bgp_byState = BGP_STATE_ACTIVE;
		else
			pPeer->bgp_byState = BGP_STATE_IDLE;
		break;
	default:
		break;
	}
	return 0;
}

//IE 7-9
BYTE stud_bgp_FsmEventTimerProcess(BgpPeer *pPeer,BYTE msgType)
{
	switch(msgType) {
	case BGP_CONNECTRETRY_TIMEOUT:	//IE7
		if (pPeer->bgp_byState == BGP_STATE_ACTIVE) {
			pPeer->bgp_byState = BGP_STATE_CONNECT;
			bgp_FsmTryToConnectPeer();
		} else if(pPeer->bgp_byState != BGP_STATE_CONNECT)
			pPeer->bgp_byState = BGP_STATE_IDLE;
		break;
	case BGP_HOLD_TIMEOUT:			//IE8
		pPeer->bgp_byState = BGP_STATE_IDLE;
		if (pPeer->bgp_byState == BGP_STATE_OPENCONFIRM)
			sendNotificationMessage(BGP_HOLD_TIMEOUT_ERROR);
		break;
	case BGP_KEEPALIVE_TIMEOUT:		//IE9
		if (pPeer->bgp_byState == BGP_STATE_OPENCONFIRM || pPeer->bgp_byState == BGP_STATE_ESTABLISHED)
			sendKeepAliveMessage();
		else if(pPeer->bgp_byState == BGP_STATE_CONNECT)
			sendNotificationMessage(BGP_FSM_ERROR);
		else
			pPeer->bgp_byState = BGP_STATE_IDLE;
		break;
	default:
		break;
	}
	return 0;
}

//IE 1     
BYTE stud_bgp_FsmEventStart(BgpPeer *pPeer)      
{
	if (pPeer->bgp_byState != BGP_STATE_IDLE)
		return 0;
	pPeer->bgp_byState = BGP_STATE_CONNECT;
	bgp_FsmTryToConnectPeer();
	return 0;
}

//IE 2     
BYTE stud_bgp_FsmEventStop(BgpPeer *pPeer)       
{
	pPeer->bgp_byState = BGP_STATE_IDLE;
	return 0;
}

//IE 3
BYTE stud_bgp_FsmEventConnect(BgpPeer *pPeer)   
{
	if(pPeer->bgp_byState == BGP_STATE_ACTIVE || pPeer->bgp_byState == BGP_STATE_CONNECT){
		sendOpenMessage(pPeer);
		pPeer->bgp_byState = BGP_STATE_OPENSENT;
	}
	else
		pPeer->bgp_byState = BGP_STATE_IDLE;
	return 0;
}
