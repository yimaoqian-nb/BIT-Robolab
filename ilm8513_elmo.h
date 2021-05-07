#ifndef ILM8513_ELMO_H_
#define ILM8513_ELMO_H_

////文件版本：2020/12/22

#define motor_1_ID 0x7e
#define motor_2_ID 0x7f


#ifdef __cplusplus
#define EXTERN_C  extern "C"
#else
#define EXTERN_C
#endif

EXTERN_C DWORD Q_START_CANOPEN		(DWORD DeviceInd,	DWORD motor_ID		,DWORD Reserved		);					//DeviceInd为0或1，表示用哪个can口发送
																													//motor_ID表示发送的节点名称，应使用.h中的宏定义表示
EXTERN_C DWORD Q_SETMODE_CANOPEN	(DWORD DeviceInd,	DWORD motor_ID		,DWORD Modename		,DWORD Reserved);	
EXTERN_C DWORD Q_SETACC_CANOPEN		(DWORD DeviceInd,	DWORD motor_ID		,DWORD Acceleration	,DWORD Reserved);
EXTERN_C DWORD Q_SETDEC_CANOPEN		(DWORD DeviceInd,	DWORD motor_ID		,DWORD Deceleration	,DWORD Reserved);
EXTERN_C DWORD Q_READYON_CANOPEN	(DWORD DeviceInd,	DWORD motor_ID		,DWORD Reserved		);
EXTERN_C DWORD Q_SWITCHON_CANOPEN	(DWORD DeviceInd,	DWORD motor_ID		,DWORD Reserved		);
EXTERN_C DWORD Q_SETPX_CANOPEN		(DWORD DeviceInd,	DWORD motor_ID		,DWORD PX			,DWORD Reserved);
EXTERN_C DWORD Q_SETSPEED_CANOPEN	(DWORD DeviceInd,	DWORD motor_ID		,DWORD Speed		,DWORD Reserved);
EXTERN_C DWORD Q_SETMO_CANOPEN		(DWORD DeviceInd,	DWORD motor_ID		,DWORD Mo_Mode		,DWORD Reserved);
EXTERN_C DWORD Q_SETPOS_CANOPEN		(DWORD DeviceInd,	DWORD motor_ID		,DWORD Position		,DWORD Reserved);
EXTERN_C DWORD Q_OPENOPER_CANOPEN	(DWORD DeviceInd,	DWORD motor_ID		,DWORD Reserved		);
EXTERN_C DWORD Q_CLEARSP_CANOPEN	(DWORD DeviceInd,	DWORD motor_ID		,DWORD Reserved		);

#endif   //ILM8513_ELMO_H_