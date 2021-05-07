#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include "controlcan.h"
#include <iostream>
#include "ilm8513_elmo.h"
#include "can_global.h"

#include <ctime>
#include <cstdlib>
#include "unistd.h"

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


int i;
int ilm8513_elmo_count;

EXTERN_C DWORD Q_START_CANOPEN		(DWORD DeviceInd,	DWORD motor_ID		,DWORD Reserved		)
{
	//需要发送的帧，结构体设置
	VCI_CAN_OBJ send[1];		//VCI_CAN_OBJ结构体是CAN帧结构体,即1个结构体表示一个帧的数据结构。在发送函数VCI_Transmit和接收函数VCI_Receive中,被用来传送CAN信息帧。
	send[0].ID=0x00;			//帧ID。32位变量,数据格式为靠右对齐。详情请参照:《8. 附件 1 : ID 对齐方式 .pdf》说明文档。
	send[0].SendType=0;			//发送帧类型。
								//=0时为正常发送(发送失败会自动重发,重发时间为4秒,4秒内没有发出则取消);
								//=1时为单次发送(只发送一次,发送失败不会自动重发,总线只产生一帧数据);
								//其它值无效。
								//(二次开发,建议SendType=1,提高发送的响应速度)
	send[0].RemoteFlag=0;		//是否是远程帧。=0时为为数据帧,=1时为远程帧(数据段空)。
	send[0].ExternFlag=0;		//是否是扩展帧。=0时为标准帧(11位ID),=1时为扩展帧(29位ID)。
	send[0].DataLen=2;			//数据长度 DLC (<=8),即CAN帧Data有几个字节。约束了后面Data[8]中的有效字节。

	send[0].Data[0] = 0x01;		//Data[8]
	send[0].Data[1] = motor_ID;		//CAN帧的数据。由于CAN规定了最大是8个字节,所以这里预留了8个字节的空间,受
									//DataLen约束。如DataLen定义为3,即Data[0]、Data[1]、Data[2]是有效的。
	if(VCI_Transmit(VCI_USBCAN2, 0, DeviceInd, send, 1) == 1)		//VCI_Transmit发送函数。返回值为实际发送成功的帧数。
							//参数定义
							//DevType 设备类型。对应不同的产品型号 详见:适配器设备类型定义。
							//DevIndex 设备索引,比如当只有一个USB-CAN适配器时,索引号为0,这时再插入一个USB-CAN适配器那么后面插入的这个设备索引号就是1,以此类推。
							//CANIndex CAN通道索引。第几路 CAN。即对应卡的CAN通道号,CAN1为0,CAN2为1。
							//pSend 要发送的帧结构体 VCI_CAN_OBJ数组的首指针。
							//返回值: 返回实际发送的帧数,=-1表示USB-CAN设备不存在或USB掉线。
	{
		printf("Index:%04d  ",ilm8513_elmo_count);ilm8513_elmo_count++;
		printf("CAN1 TX ID:0x%08X",send[0].ID);
		if(send[0].ExternFlag==0) printf(" Standard ");
		if(send[0].ExternFlag==1) printf(" Extend   ");
		if(send[0].RemoteFlag==0) printf(" Data   ");
		if(send[0].RemoteFlag==1) printf(" Remote ");
		printf("DLC:0x%02X",send[0].DataLen);
		printf(" data:0x");
		for(i=0;i<send[0].DataLen;i++)
		{
			printf(" %02X",send[0].Data[i]);
		}

		printf("\n");
		send[0].ID+=1;
		*can_control_word = 0;
		std::cout << *can_control_word << std::endl;
	}									//用can0发送一遍数据
}	

EXTERN_C DWORD Q_SETMODE_CANOPEN	(DWORD DeviceInd,	DWORD motor_ID		,DWORD Modename		,DWORD Reserved)
{
	//需要发送的帧，结构体设置
	
	VCI_CAN_OBJ send[1];		//VCI_CAN_OBJ结构体是CAN帧结构体,即1个结构体表示一个帧的数据结构。在发送函数VCI_Transmit和接收函数VCI_Receive中,被用来传送CAN信息帧。
	send[0].ID=0x600|motor_ID;	//帧ID。32位变量,数据格式为靠右对齐。详情请参照:《8. 附件 1 : ID 对齐方式 .pdf》说明文档。
	send[0].SendType=0;			//发送帧类型。
								//=0时为正常发送(发送失败会自动重发,重发时间为4秒,4秒内没有发出则取消);
								//=1时为单次发送(只发送一次,发送失败不会自动重发,总线只产生一帧数据);
								//其它值无效。
								//(二次开发,建议SendType=1,提高发送的响应速度)
	send[0].RemoteFlag=0;		//是否是远程帧。=0时为为数据帧,=1时为远程帧(数据段空)。
	send[0].ExternFlag=0;		//是否是扩展帧。=0时为标准帧(11位ID),=1时为扩展帧(29位ID)。
	send[0].DataLen=8;			//数据长度 DLC (<=8),即CAN帧Data有几个字节。约束了后面Data[8]中的有效字节。

	send[0].Data[0] = 0x22;		//数据类型0x22
	send[0].Data[1] = 0x60;		
	send[0].Data[2] = 0x60;		//对象字典0x6060
	send[0].Data[3] = 0x00;		//子对象字典0x00
	send[0].Data[4] = Modename;		
	send[0].Data[5] = 0x00;		
	send[0].Data[6] = 0x00;
	send[0].Data[7] = 0x00;		// 0 No mode
								// 1 Profile position mode
								// 2 Velocity (not supported)
								// 3 Profiled velocity mode
								// 4 Torque profiled mode
								// 5 Reserved, not supported
								// 6 Homing mode
								// 7 Interpolated position mode
								// 8 Cyclic synchronous position

	if(VCI_Transmit(VCI_USBCAN2, 0, DeviceInd, send, 1) == 1)		//VCI_Transmit发送函数。返回值为实际发送成功的帧数。
							//参数定义
							//DevType 设备类型。对应不同的产品型号 详见:适配器设备类型定义。
							//DevIndex 设备索引,比如当只有一个USB-CAN适配器时,索引号为0,这时再插入一个USB-CAN适配器那么后面插入的这个设备索引号就是1,以此类推。
							//CANIndex CAN通道索引。第几路 CAN。即对应卡的CAN通道号,CAN1为0,CAN2为1。
							//pSend 要发送的帧结构体 VCI_CAN_OBJ数组的首指针。
							//返回值: 返回实际发送的帧数,=-1表示USB-CAN设备不存在或USB掉线。
	{
		printf("Index:%04d  ",ilm8513_elmo_count);ilm8513_elmo_count++;
		printf("CAN1 TX ID:0x%08X",send[0].ID);
		if(send[0].ExternFlag==0) printf(" Standard ");
		if(send[0].ExternFlag==1) printf(" Extend   ");
		if(send[0].RemoteFlag==0) printf(" Data   ");
		if(send[0].RemoteFlag==1) printf(" Remote ");
		printf("DLC:0x%02X",send[0].DataLen);
		printf(" data:0x");
		for(i=0;i<send[0].DataLen;i++)
		{
			printf(" %02X",send[0].Data[i]);
		}

		printf("\n");
		send[0].ID+=1;
		*can_control_word = 0;
		std::cout << *can_control_word << std::endl;
	}									//用can0发送一遍数据
}

EXTERN_C DWORD Q_SETACC_CANOPEN		(DWORD DeviceInd,	DWORD motor_ID		,DWORD Acceleration		,DWORD Reserved)
{
	// while (n >= 16)
  	// {
  	// 	m = n % 16;          //m保存余数
	// 	n = n / 16;
	// 	arr[i] = exchange_int_char(m);
 	// 	i++;
 	// }
	//需要发送的帧，结构体设置
	VCI_CAN_OBJ send[1];		//VCI_CAN_OBJ结构体是CAN帧结构体,即1个结构体表示一个帧的数据结构。在发送函数VCI_Transmit和接收函数VCI_Receive中,被用来传送CAN信息帧。
	send[0].ID=0x600|motor_ID;	//帧ID。32位变量,数据格式为靠右对齐。详情请参照:《8. 附件 1 : ID 对齐方式 .pdf》说明文档。
	send[0].SendType=0;			//发送帧类型。
								//=0时为正常发送(发送失败会自动重发,重发时间为4秒,4秒内没有发出则取消);
								//=1时为单次发送(只发送一次,发送失败不会自动重发,总线只产生一帧数据);
								//其它值无效。
								//(二次开发,建议SendType=1,提高发送的响应速度)
	send[0].RemoteFlag=0;		//是否是远程帧。=0时为为数据帧,=1时为远程帧(数据段空)。
	send[0].ExternFlag=0;		//是否是扩展帧。=0时为标准帧(11位ID),=1时为扩展帧(29位ID)。
	send[0].DataLen=8;			//数据长度 DLC (<=8),即CAN帧Data有几个字节。约束了后面Data[8]中的有效字节。

	send[0].Data[0] = 0x22;		//数据类型0x22
	send[0].Data[1] = 0x83;		
	send[0].Data[2] = 0x60;		//对象字典0x6083
	send[0].Data[3] = 0x00;		//子对象字典0x00
	send[0].Data[4] = 0x40;	
	send[0].Data[5] = 0x42;
	send[0].Data[6] = 0x0f;
	send[0].Data[7] = 0x00;		//加速度1*10^6,0x000f4240=1e6

	if(VCI_Transmit(VCI_USBCAN2, 0, DeviceInd, send, 1) == 1)		//VCI_Transmit发送函数。返回值为实际发送成功的帧数。
							//参数定义
							//DevType 设备类型。对应不同的产品型号 详见:适配器设备类型定义。
							//DevIndex 设备索引,比如当只有一个USB-CAN适配器时,索引号为0,这时再插入一个USB-CAN适配器那么后面插入的这个设备索引号就是1,以此类推。
							//CANIndex CAN通道索引。第几路 CAN。即对应卡的CAN通道号,CAN1为0,CAN2为1。
							//pSend 要发送的帧结构体 VCI_CAN_OBJ数组的首指针。
							//返回值: 返回实际发送的帧数,=-1表示USB-CAN设备不存在或USB掉线。
	{
		printf("Index:%04d  ",ilm8513_elmo_count);ilm8513_elmo_count++;
		printf("CAN1 TX ID:0x%08X",send[0].ID);
		if(send[0].ExternFlag==0) printf(" Standard ");
		if(send[0].ExternFlag==1) printf(" Extend   ");
		if(send[0].RemoteFlag==0) printf(" Data   ");
		if(send[0].RemoteFlag==1) printf(" Remote ");
		printf("DLC:0x%02X",send[0].DataLen);
		printf(" data:0x");
		for(i=0;i<send[0].DataLen;i++)
		{
			printf(" %02X",send[0].Data[i]);
		}

		printf("\n");
		send[0].ID+=1;
		*can_control_word = 0;
		std::cout << *can_control_word << std::endl;
	}									//用can0发送一遍数据
}

EXTERN_C DWORD Q_SETDEC_CANOPEN		(DWORD DeviceInd,	DWORD motor_ID		,DWORD Acceleration		,DWORD Reserved)
{
	// while (n >= 16)
  	// {
  	// 	m = n % 16;          //m保存余数
	// 	n = n / 16;
	// 	arr[i] = exchange_int_char(m);
 	// 	i++;
 	// }
	//需要发送的帧，结构体设置
	VCI_CAN_OBJ send[1];		//VCI_CAN_OBJ结构体是CAN帧结构体,即1个结构体表示一个帧的数据结构。在发送函数VCI_Transmit和接收函数VCI_Receive中,被用来传送CAN信息帧。
	send[0].ID=0x600|motor_ID;	//帧ID。32位变量,数据格式为靠右对齐。详情请参照:《8. 附件 1 : ID 对齐方式 .pdf》说明文档。
	send[0].SendType=0;			//发送帧类型。
								//=0时为正常发送(发送失败会自动重发,重发时间为4秒,4秒内没有发出则取消);
								//=1时为单次发送(只发送一次,发送失败不会自动重发,总线只产生一帧数据);
								//其它值无效。
								//(二次开发,建议SendType=1,提高发送的响应速度)
	send[0].RemoteFlag=0;		//是否是远程帧。=0时为为数据帧,=1时为远程帧(数据段空)。
	send[0].ExternFlag=0;		//是否是扩展帧。=0时为标准帧(11位ID),=1时为扩展帧(29位ID)。
	send[0].DataLen=8;			//数据长度 DLC (<=8),即CAN帧Data有几个字节。约束了后面Data[8]中的有效字节。

	send[0].Data[0] = 0x22;		//数据类型0x22
	send[0].Data[1] = 0x84;		
	send[0].Data[2] = 0x60;		//对象字典0x6084
	send[0].Data[3] = 0x00;		//子对象字典0x00
	send[0].Data[4] = 0x40;	
	send[0].Data[5] = 0x41;
	send[0].Data[6] = 0x0f;
	send[0].Data[7] = 0x00;		//加速度1*10^6,0x000f4240=1e6,但1e6无应答，1e6-1有应答，推测是减速度有上限。

	if(VCI_Transmit(VCI_USBCAN2, 0, DeviceInd, send, 1) == 1)		//VCI_Transmit发送函数。返回值为实际发送成功的帧数。
							//参数定义
							//DevType 设备类型。对应不同的产品型号 详见:适配器设备类型定义。
							//DevIndex 设备索引,比如当只有一个USB-CAN适配器时,索引号为0,这时再插入一个USB-CAN适配器那么后面插入的这个设备索引号就是1,以此类推。
							//CANIndex CAN通道索引。第几路 CAN。即对应卡的CAN通道号,CAN1为0,CAN2为1。
							//pSend 要发送的帧结构体 VCI_CAN_OBJ数组的首指针。
							//返回值: 返回实际发送的帧数,=-1表示USB-CAN设备不存在或USB掉线。
	{
		printf("Index:%04d  ",ilm8513_elmo_count);ilm8513_elmo_count++;
		printf("CAN1 TX ID:0x%08X",send[0].ID);
		if(send[0].ExternFlag==0) printf(" Standard ");
		if(send[0].ExternFlag==1) printf(" Extend   ");
		if(send[0].RemoteFlag==0) printf(" Data   ");
		if(send[0].RemoteFlag==1) printf(" Remote ");
		printf("DLC:0x%02X",send[0].DataLen);
		printf(" data:0x");
		for(i=0;i<send[0].DataLen;i++)
		{
			printf(" %02X",send[0].Data[i]);
		}

		printf("\n");
		send[0].ID+=1;
		*can_control_word = 0;
		std::cout << *can_control_word << std::endl;
	}									//用can0发送一遍数据
}

EXTERN_C DWORD Q_READYON_CANOPEN	(DWORD DeviceInd,	DWORD motor_ID		,DWORD Reserved		)
{
	//需要发送的帧，结构体设置
	
	VCI_CAN_OBJ send[1];		//VCI_CAN_OBJ结构体是CAN帧结构体,即1个结构体表示一个帧的数据结构。在发送函数VCI_Transmit和接收函数VCI_Receive中,被用来传送CAN信息帧。
	send[0].ID=0x600|motor_ID;	//帧ID。32位变量,数据格式为靠右对齐。详情请参照:《8. 附件 1 : ID 对齐方式 .pdf》说明文档。
	send[0].SendType=0;			//发送帧类型。
								//=0时为正常发送(发送失败会自动重发,重发时间为4秒,4秒内没有发出则取消);
								//=1时为单次发送(只发送一次,发送失败不会自动重发,总线只产生一帧数据);
								//其它值无效。
								//(二次开发,建议SendType=1,提高发送的响应速度)
	send[0].RemoteFlag=0;		//是否是远程帧。=0时为为数据帧,=1时为远程帧(数据段空)。
	send[0].ExternFlag=0;		//是否是扩展帧。=0时为标准帧(11位ID),=1时为扩展帧(29位ID)。
	send[0].DataLen=8;			//数据长度 DLC (<=8),即CAN帧Data有几个字节。约束了后面Data[8]中的有效字节。

	send[0].Data[0] = 0x22;		//数据类型0x22
	send[0].Data[1] = 0x40;		
	send[0].Data[2] = 0x60;		//对象字典0x6040
	send[0].Data[3] = 0x00;		//子对象字典0x00
	send[0].Data[4] = 0x06;		
	send[0].Data[5] = 0x00;		
	send[0].Data[6] = 0x00;
	send[0].Data[7] = 0X00;

	if(VCI_Transmit(VCI_USBCAN2, 0, DeviceInd, send, 1) == 1)		//VCI_Transmit发送函数。返回值为实际发送成功的帧数。
							//参数定义
							//DevType 设备类型。对应不同的产品型号 详见:适配器设备类型定义。
							//DevIndex 设备索引,比如当只有一个USB-CAN适配器时,索引号为0,这时再插入一个USB-CAN适配器那么后面插入的这个设备索引号就是1,以此类推。
							//CANIndex CAN通道索引。第几路 CAN。即对应卡的CAN通道号,CAN1为0,CAN2为1。
							//pSend 要发送的帧结构体 VCI_CAN_OBJ数组的首指针。
							//返回值: 返回实际发送的帧数,=-1表示USB-CAN设备不存在或USB掉线。
	{
		printf("Index:%04d  ",ilm8513_elmo_count);ilm8513_elmo_count++;
		printf("CAN1 TX ID:0x%08X",send[0].ID);
		if(send[0].ExternFlag==0) printf(" Standard ");
		if(send[0].ExternFlag==1) printf(" Extend   ");
		if(send[0].RemoteFlag==0) printf(" Data   ");
		if(send[0].RemoteFlag==1) printf(" Remote ");
		printf("DLC:0x%02X",send[0].DataLen);
		printf(" data:0x");
		for(i=0;i<send[0].DataLen;i++)
		{
			printf(" %02X",send[0].Data[i]);
		}

		printf("\n");
		send[0].ID+=1;
		*can_control_word = 0;
		std::cout << *can_control_word << std::endl;
	}									//用can0发送一遍数据
}

EXTERN_C DWORD Q_SWITCHON_CANOPEN	(DWORD DeviceInd,	DWORD motor_ID		,DWORD Reserved		)
{
	//需要发送的帧，结构体设置
	
	VCI_CAN_OBJ send[1];		//VCI_CAN_OBJ结构体是CAN帧结构体,即1个结构体表示一个帧的数据结构。在发送函数VCI_Transmit和接收函数VCI_Receive中,被用来传送CAN信息帧。
	send[0].ID=0x600|motor_ID;	//帧ID。32位变量,数据格式为靠右对齐。详情请参照:《8. 附件 1 : ID 对齐方式 .pdf》说明文档。
	send[0].SendType=0;			//发送帧类型。
								//=0时为正常发送(发送失败会自动重发,重发时间为4秒,4秒内没有发出则取消);
								//=1时为单次发送(只发送一次,发送失败不会自动重发,总线只产生一帧数据);
								//其它值无效。
								//(二次开发,建议SendType=1,提高发送的响应速度)
	send[0].RemoteFlag=0;		//是否是远程帧。=0时为为数据帧,=1时为远程帧(数据段空)。
	send[0].ExternFlag=0;		//是否是扩展帧。=0时为标准帧(11位ID),=1时为扩展帧(29位ID)。
	send[0].DataLen=8;			//数据长度 DLC (<=8),即CAN帧Data有几个字节。约束了后面Data[8]中的有效字节。

	send[0].Data[0] = 0x22;		//数据类型0x22
	send[0].Data[1] = 0x40;		
	send[0].Data[2] = 0x60;		//对象字典0x6040
	send[0].Data[3] = 0x00;		//子对象字典0x00
	send[0].Data[4] = 0x07;		
	send[0].Data[5] = 0x00;		
	send[0].Data[6] = 0x00;
	send[0].Data[7] = 0X00;

	if(VCI_Transmit(VCI_USBCAN2, 0, DeviceInd, send, 1) == 1)		//VCI_Transmit发送函数。返回值为实际发送成功的帧数。
							//参数定义
							//DevType 设备类型。对应不同的产品型号 详见:适配器设备类型定义。
							//DevIndex 设备索引,比如当只有一个USB-CAN适配器时,索引号为0,这时再插入一个USB-CAN适配器那么后面插入的这个设备索引号就是1,以此类推。
							//CANIndex CAN通道索引。第几路 CAN。即对应卡的CAN通道号,CAN1为0,CAN2为1。
							//pSend 要发送的帧结构体 VCI_CAN_OBJ数组的首指针。
							//返回值: 返回实际发送的帧数,=-1表示USB-CAN设备不存在或USB掉线。
	{
		printf("Index:%04d  ",ilm8513_elmo_count);ilm8513_elmo_count++;
		printf("CAN1 TX ID:0x%08X",send[0].ID);
		if(send[0].ExternFlag==0) printf(" Standard ");
		if(send[0].ExternFlag==1) printf(" Extend   ");
		if(send[0].RemoteFlag==0) printf(" Data   ");
		if(send[0].RemoteFlag==1) printf(" Remote ");
		printf("DLC:0x%02X",send[0].DataLen);
		printf(" data:0x");
		for(i=0;i<send[0].DataLen;i++)
		{
			printf(" %02X",send[0].Data[i]);
		}

		printf("\n");
		send[0].ID+=1;
		*can_control_word = 0;
		std::cout << *can_control_word << std::endl;
	}									//用can0发送一遍数据
} 

EXTERN_C DWORD Q_SETPX_CANOPEN		(DWORD DeviceInd,	DWORD motor_ID		,DWORD PX			,DWORD Reserved)
{
	//需要发送的帧，结构体设置
	
	VCI_CAN_OBJ send[1];		//VCI_CAN_OBJ结构体是CAN帧结构体,即1个结构体表示一个帧的数据结构。在发送函数VCI_Transmit和接收函数VCI_Receive中,被用来传送CAN信息帧。
	send[0].ID=0x300|motor_ID;	//帧ID。32位变量,数据格式为靠右对齐。详情请参照:《8. 附件 1 : ID 对齐方式 .pdf》说明文档。
	send[0].SendType=0;			//发送帧类型。
								//=0时为正常发送(发送失败会自动重发,重发时间为4秒,4秒内没有发出则取消);
								//=1时为单次发送(只发送一次,发送失败不会自动重发,总线只产生一帧数据);
								//其它值无效。
								//(二次开发,建议SendType=1,提高发送的响应速度)
	send[0].RemoteFlag=0;		//是否是远程帧。=0时为为数据帧,=1时为远程帧(数据段空)。
	send[0].ExternFlag=0;		//是否是扩展帧。=0时为标准帧(11位ID),=1时为扩展帧(29位ID)。
	send[0].DataLen=8;			//数据长度 DLC (<=8),即CAN帧Data有几个字节。约束了后面Data[8]中的有效字节。

	send[0].Data[0] = 0x50;		//数据类型0x50
	send[0].Data[1] = 0x78;		
	send[0].Data[2] = 0x00;		
	send[0].Data[3] = 0x00;		
	send[0].Data[4] = 0x00;		
	send[0].Data[5] = 0x00;		
	send[0].Data[6] = 0x00;
	send[0].Data[7] = 0X00;

	if(VCI_Transmit(VCI_USBCAN2, 0, DeviceInd, send, 1) == 1)		//VCI_Transmit发送函数。返回值为实际发送成功的帧数。
							//参数定义
							//DevType 设备类型。对应不同的产品型号 详见:适配器设备类型定义。
							//DevIndex 设备索引,比如当只有一个USB-CAN适配器时,索引号为0,这时再插入一个USB-CAN适配器那么后面插入的这个设备索引号就是1,以此类推。
							//CANIndex CAN通道索引。第几路 CAN。即对应卡的CAN通道号,CAN1为0,CAN2为1。
							//pSend 要发送的帧结构体 VCI_CAN_OBJ数组的首指针。
							//返回值: 返回实际发送的帧数,=-1表示USB-CAN设备不存在或USB掉线。
	{
		printf("Index:%04d  ",ilm8513_elmo_count);ilm8513_elmo_count++;
		printf("CAN1 TX ID:0x%08X",send[0].ID);
		if(send[0].ExternFlag==0) printf(" Standard ");
		if(send[0].ExternFlag==1) printf(" Extend   ");
		if(send[0].RemoteFlag==0) printf(" Data   ");
		if(send[0].RemoteFlag==1) printf(" Remote ");
		printf("DLC:0x%02X",send[0].DataLen);
		printf(" data:0x");
		for(i=0;i<send[0].DataLen;i++)
		{
			printf(" %02X",send[0].Data[i]);
		}

		printf("\n");
		send[0].ID+=1;
		*can_control_word = 0;
		std::cout << *can_control_word << std::endl;
	}									//用can0发送一遍数据
}

EXTERN_C DWORD Q_SETSPEED_CANOPEN	(DWORD DeviceInd,	DWORD motor_ID		,DWORD Speed		,DWORD Reserved)

{
	//需要发送的帧，结构体设置
	
	VCI_CAN_OBJ send[1];		//VCI_CAN_OBJ结构体是CAN帧结构体,即1个结构体表示一个帧的数据结构。在发送函数VCI_Transmit和接收函数VCI_Receive中,被用来传送CAN信息帧。
	send[0].ID=0x600|motor_ID;	//帧ID。32位变量,数据格式为靠右对齐。详情请参照:《8. 附件 1 : ID 对齐方式 .pdf》说明文档。
	send[0].SendType=0;			//发送帧类型。
								//=0时为正常发送(发送失败会自动重发,重发时间为4秒,4秒内没有发出则取消);
								//=1时为单次发送(只发送一次,发送失败不会自动重发,总线只产生一帧数据);
								//其它值无效。
								//(二次开发,建议SendType=1,提高发送的响应速度)
	send[0].RemoteFlag=0;		//是否是远程帧。=0时为为数据帧,=1时为远程帧(数据段空)。
	send[0].ExternFlag=0;		//是否是扩展帧。=0时为标准帧(11位ID),=1时为扩展帧(29位ID)。
	send[0].DataLen=8;			//数据长度 DLC (<=8),即CAN帧Data有几个字节。约束了后面Data[8]中的有效字节。

	send[0].Data[0] = 0x22;		//数据类型0x50
	send[0].Data[1] = 0x81;		
	send[0].Data[2] = 0x60;		//对象字典6081
	send[0].Data[3] = 0x00;		//子对象字典0x00
	send[0].Data[4] = 0x10;		
	send[0].Data[5] = 0x27;		
	send[0].Data[6] = 0x00;
	send[0].Data[7] = 0X00;		//速度2710=10000,速度c34f=49999,当50000时无应答，推测速度上限为50000

	if(VCI_Transmit(VCI_USBCAN2, 0, DeviceInd, send, 1) == 1)		//VCI_Transmit发送函数。返回值为实际发送成功的帧数。
							//参数定义
							//DevType 设备类型。对应不同的产品型号 详见:适配器设备类型定义。
							//DevIndex 设备索引,比如当只有一个USB-CAN适配器时,索引号为0,这时再插入一个USB-CAN适配器那么后面插入的这个设备索引号就是1,以此类推。
							//CANIndex CAN通道索引。第几路 CAN。即对应卡的CAN通道号,CAN1为0,CAN2为1。
							//pSend 要发送的帧结构体 VCI_CAN_OBJ数组的首指针。
							//返回值: 返回实际发送的帧数,=-1表示USB-CAN设备不存在或USB掉线。
	{
		printf("Index:%04d  ",ilm8513_elmo_count);ilm8513_elmo_count++;
		printf("CAN1 TX ID:0x%08X",send[0].ID);
		if(send[0].ExternFlag==0) printf(" Standard ");
		if(send[0].ExternFlag==1) printf(" Extend   ");
		if(send[0].RemoteFlag==0) printf(" Data   ");
		if(send[0].RemoteFlag==1) printf(" Remote ");
		printf("DLC:0x%02X",send[0].DataLen);
		printf(" data:0x");
		for(i=0;i<send[0].DataLen;i++)
		{
			printf(" %02X",send[0].Data[i]);
		}

		printf("\n");
		send[0].ID+=1;
		*can_control_word = 0;
		std::cout << *can_control_word << std::endl;
	}									//用can0发送一遍数据
}

EXTERN_C DWORD Q_SETMO_CANOPEN		(DWORD DeviceInd,	DWORD motor_ID		,DWORD Mo_Mode		,DWORD Reserved)
{
	//需要发送的帧，结构体设置
	
	VCI_CAN_OBJ send[1];		//VCI_CAN_OBJ结构体是CAN帧结构体,即1个结构体表示一个帧的数据结构。在发送函数VCI_Transmit和接收函数VCI_Receive中,被用来传送CAN信息帧。
	send[0].ID=0x600|motor_ID;	//帧ID。32位变量,数据格式为靠右对齐。详情请参照:《8. 附件 1 : ID 对齐方式 .pdf》说明文档。
	send[0].SendType=0;			//发送帧类型。
								//=0时为正常发送(发送失败会自动重发,重发时间为4秒,4秒内没有发出则取消);
								//=1时为单次发送(只发送一次,发送失败不会自动重发,总线只产生一帧数据);
								//其它值无效。
								//(二次开发,建议SendType=1,提高发送的响应速度)
	send[0].RemoteFlag=0;		//是否是远程帧。=0时为为数据帧,=1时为远程帧(数据段空)。
	send[0].ExternFlag=0;		//是否是扩展帧。=0时为标准帧(11位ID),=1时为扩展帧(29位ID)。
	send[0].DataLen=8;			//数据长度 DLC (<=8),即CAN帧Data有几个字节。约束了后面Data[8]中的有效字节。

	send[0].Data[0] = 0x22;		//数据类型0x50
	send[0].Data[1] = 0x40;		
	send[0].Data[2] = 0x60;		//对象字典6040
	send[0].Data[3] = 0x00;		//子对象字典0x00
	send[0].Data[4] = 0x0f;		
	send[0].Data[5] = 0x00;		
	send[0].Data[6] = 0x00;
	send[0].Data[7] = 0X00;		//mo=1

	if(VCI_Transmit(VCI_USBCAN2, 0, DeviceInd, send, 1) == 1)		//VCI_Transmit发送函数。返回值为实际发送成功的帧数。
							//参数定义
							//DevType 设备类型。对应不同的产品型号 详见:适配器设备类型定义。
							//DevIndex 设备索引,比如当只有一个USB-CAN适配器时,索引号为0,这时再插入一个USB-CAN适配器那么后面插入的这个设备索引号就是1,以此类推。
							//CANIndex CAN通道索引。第几路 CAN。即对应卡的CAN通道号,CAN1为0,CAN2为1。
							//pSend 要发送的帧结构体 VCI_CAN_OBJ数组的首指针。
							//返回值: 返回实际发送的帧数,=-1表示USB-CAN设备不存在或USB掉线。
	{
		printf("Index:%04d  ",ilm8513_elmo_count);ilm8513_elmo_count++;
		printf("CAN1 TX ID:0x%08X",send[0].ID);
		if(send[0].ExternFlag==0) printf(" Standard ");
		if(send[0].ExternFlag==1) printf(" Extend   ");
		if(send[0].RemoteFlag==0) printf(" Data   ");
		if(send[0].RemoteFlag==1) printf(" Remote ");
		printf("DLC:0x%02X",send[0].DataLen);
		printf(" data:0x");
		for(i=0;i<send[0].DataLen;i++)
		{
			printf(" %02X",send[0].Data[i]);
		}

		printf("\n");
		send[0].ID+=1;
		*can_control_word = 0;
		std::cout << *can_control_word << std::endl;
	}									//用can0发送一遍数据
}

EXTERN_C DWORD Q_SETPOS_CANOPEN		(DWORD DeviceInd,	DWORD motor_ID		,DWORD Position		,DWORD Reserved)
{
	//需要发送的帧，结构体设置
	
	VCI_CAN_OBJ send[1];		//VCI_CAN_OBJ结构体是CAN帧结构体,即1个结构体表示一个帧的数据结构。在发送函数VCI_Transmit和接收函数VCI_Receive中,被用来传送CAN信息帧。
	send[0].ID=0x600|motor_ID;	//帧ID。32位变量,数据格式为靠右对齐。详情请参照:《8. 附件 1 : ID 对齐方式 .pdf》说明文档。
	send[0].SendType=0;			//发送帧类型。
								//=0时为正常发送(发送失败会自动重发,重发时间为4秒,4秒内没有发出则取消);
								//=1时为单次发送(只发送一次,发送失败不会自动重发,总线只产生一帧数据);
								//其它值无效。
								//(二次开发,建议SendType=1,提高发送的响应速度)
	send[0].RemoteFlag=0;		//是否是远程帧。=0时为为数据帧,=1时为远程帧(数据段空)。
	send[0].ExternFlag=0;		//是否是扩展帧。=0时为标准帧(11位ID),=1时为扩展帧(29位ID)。
	send[0].DataLen=8;			//数据长度 DLC (<=8),即CAN帧Data有几个字节。约束了后面Data[8]中的有效字节。

	send[0].Data[0] = 0x22;		//数据类型0x50
	send[0].Data[1] = 0x7a;		
	send[0].Data[2] = 0x60;		//对象字典6040
	send[0].Data[3] = 0x00;		//子对象字典0x00
	send[0].Data[4] = 0x10;		
	send[0].Data[5] = 0x27;		
	send[0].Data[6] = 0x00;
	send[0].Data[7] = 0X00;		//0x2710=10000,0x7530 = 30000

	if(VCI_Transmit(VCI_USBCAN2, 0, DeviceInd, send, 1) == 1)		//VCI_Transmit发送函数。返回值为实际发送成功的帧数。
							//参数定义
							//DevType 设备类型。对应不同的产品型号 详见:适配器设备类型定义。
							//DevIndex 设备索引,比如当只有一个USB-CAN适配器时,索引号为0,这时再插入一个USB-CAN适配器那么后面插入的这个设备索引号就是1,以此类推。
							//CANIndex CAN通道索引。第几路 CAN。即对应卡的CAN通道号,CAN1为0,CAN2为1。
							//pSend 要发送的帧结构体 VCI_CAN_OBJ数组的首指针。
							//返回值: 返回实际发送的帧数,=-1表示USB-CAN设备不存在或USB掉线。
	{
		printf("Index:%04d  ",ilm8513_elmo_count);ilm8513_elmo_count++;
		printf("CAN1 TX ID:0x%08X",send[0].ID);
		if(send[0].ExternFlag==0) printf(" Standard ");
		if(send[0].ExternFlag==1) printf(" Extend   ");
		if(send[0].RemoteFlag==0) printf(" Data   ");
		if(send[0].RemoteFlag==1) printf(" Remote ");
		printf("DLC:0x%02X",send[0].DataLen);
		printf(" data:0x");
		for(i=0;i<send[0].DataLen;i++)
		{
			printf(" %02X",send[0].Data[i]);
		}

		printf("\n");
		send[0].ID+=1;
		*can_control_word = 0;
		std::cout << *can_control_word << std::endl;
	}									//用can0发送一遍数据
}

EXTERN_C DWORD Q_OPENOPER_CANOPEN	(DWORD DeviceInd,	DWORD motor_ID		,DWORD Reserved		)
{
	//需要发送的帧，结构体设置
	
	VCI_CAN_OBJ send[1];		//VCI_CAN_OBJ结构体是CAN帧结构体,即1个结构体表示一个帧的数据结构。在发送函数VCI_Transmit和接收函数VCI_Receive中,被用来传送CAN信息帧。
	send[0].ID=0x600|motor_ID;	//帧ID。32位变量,数据格式为靠右对齐。详情请参照:《8. 附件 1 : ID 对齐方式 .pdf》说明文档。
	send[0].SendType=0;			//发送帧类型。
								//=0时为正常发送(发送失败会自动重发,重发时间为4秒,4秒内没有发出则取消);
								//=1时为单次发送(只发送一次,发送失败不会自动重发,总线只产生一帧数据);
								//其它值无效。
								//(二次开发,建议SendType=1,提高发送的响应速度)
	send[0].RemoteFlag=0;		//是否是远程帧。=0时为为数据帧,=1时为远程帧(数据段空)。
	send[0].ExternFlag=0;		//是否是扩展帧。=0时为标准帧(11位ID),=1时为扩展帧(29位ID)。
	send[0].DataLen=8;			//数据长度 DLC (<=8),即CAN帧Data有几个字节。约束了后面Data[8]中的有效字节。

	send[0].Data[0] = 0x22;		//数据类型0x50
	send[0].Data[1] = 0x40;		
	send[0].Data[2] = 0x60;		//对象字典6040
	send[0].Data[3] = 0x00;		//子对象字典0x00
	send[0].Data[4] = 0x3f;		
	send[0].Data[5] = 0x00;		
	send[0].Data[6] = 0x00;
	send[0].Data[7] = 0X00;		//SET POINT

	if(VCI_Transmit(VCI_USBCAN2, 0, DeviceInd, send, 1) == 1)		//VCI_Transmit发送函数。返回值为实际发送成功的帧数。
							//参数定义
							//DevType 设备类型。对应不同的产品型号 详见:适配器设备类型定义。
							//DevIndex 设备索引,比如当只有一个USB-CAN适配器时,索引号为0,这时再插入一个USB-CAN适配器那么后面插入的这个设备索引号就是1,以此类推。
							//CANIndex CAN通道索引。第几路 CAN。即对应卡的CAN通道号,CAN1为0,CAN2为1。
							//pSend 要发送的帧结构体 VCI_CAN_OBJ数组的首指针。
							//返回值: 返回实际发送的帧数,=-1表示USB-CAN设备不存在或USB掉线。
	{
		printf("Index:%04d  ",ilm8513_elmo_count);ilm8513_elmo_count++;
		printf("CAN1 TX ID:0x%08X",send[0].ID);
		if(send[0].ExternFlag==0) printf(" Standard ");
		if(send[0].ExternFlag==1) printf(" Extend   ");
		if(send[0].RemoteFlag==0) printf(" Data   ");
		if(send[0].RemoteFlag==1) printf(" Remote ");
		printf("DLC:0x%02X",send[0].DataLen);
		printf(" data:0x");
		for(i=0;i<send[0].DataLen;i++)
		{
			printf(" %02X",send[0].Data[i]);
		}

		printf("\n");
		send[0].ID+=1;
		*can_control_word = 0;
		std::cout << *can_control_word << std::endl;
	}									//用can0发送一遍数据
}

EXTERN_C DWORD Q_CLEARSP_CANOPEN	(DWORD DeviceInd,	DWORD motor_ID		,DWORD Reserved		)
{
	//需要发送的帧，结构体设置
	
	VCI_CAN_OBJ send[1];		//VCI_CAN_OBJ结构体是CAN帧结构体,即1个结构体表示一个帧的数据结构。在发送函数VCI_Transmit和接收函数VCI_Receive中,被用来传送CAN信息帧。
	send[0].ID=0x600|motor_ID;	//帧ID。32位变量,数据格式为靠右对齐。详情请参照:《8. 附件 1 : ID 对齐方式 .pdf》说明文档。
	send[0].SendType=0;			//发送帧类型。
								//=0时为正常发送(发送失败会自动重发,重发时间为4秒,4秒内没有发出则取消);
								//=1时为单次发送(只发送一次,发送失败不会自动重发,总线只产生一帧数据);
								//其它值无效。
								//(二次开发,建议SendType=1,提高发送的响应速度)
	send[0].RemoteFlag=0;		//是否是远程帧。=0时为为数据帧,=1时为远程帧(数据段空)。
	send[0].ExternFlag=0;		//是否是扩展帧。=0时为标准帧(11位ID),=1时为扩展帧(29位ID)。
	send[0].DataLen=8;			//数据长度 DLC (<=8),即CAN帧Data有几个字节。约束了后面Data[8]中的有效字节。

	send[0].Data[0] = 0x22;		//数据类型0x50
	send[0].Data[1] = 0x40;		
	send[0].Data[2] = 0x60;		//对象字典6040
	send[0].Data[3] = 0x00;		//子对象字典0x00
	send[0].Data[4] = 0x0f;		
	send[0].Data[5] = 0x00;		
	send[0].Data[6] = 0x00;
	send[0].Data[7] = 0X00;		//SET POINT

	if(VCI_Transmit(VCI_USBCAN2, 0, DeviceInd, send, 1) == 1)		//VCI_Transmit发送函数。返回值为实际发送成功的帧数。
							//参数定义
							//DevType 设备类型。对应不同的产品型号 详见:适配器设备类型定义。
							//DevIndex 设备索引,比如当只有一个USB-CAN适配器时,索引号为0,这时再插入一个USB-CAN适配器那么后面插入的这个设备索引号就是1,以此类推。
							//CANIndex CAN通道索引。第几路 CAN。即对应卡的CAN通道号,CAN1为0,CAN2为1。
							//pSend 要发送的帧结构体 VCI_CAN_OBJ数组的首指针。
							//返回值: 返回实际发送的帧数,=-1表示USB-CAN设备不存在或USB掉线。
	{
		printf("Index:%04d  ",ilm8513_elmo_count);ilm8513_elmo_count++;
		printf("CAN1 TX ID:0x%08X",send[0].ID);
		if(send[0].ExternFlag==0) printf(" Standard ");
		if(send[0].ExternFlag==1) printf(" Extend   ");
		if(send[0].RemoteFlag==0) printf(" Data   ");
		if(send[0].RemoteFlag==1) printf(" Remote ");
		printf("DLC:0x%02X",send[0].DataLen);
		printf(" data:0x");
		for(i=0;i<send[0].DataLen;i++)
		{
			printf(" %02X",send[0].Data[i]);
		}

		printf("\n");
		send[0].ID+=1;
		*can_control_word = 0;
		std::cout << *can_control_word << std::endl;
	}									//用can0发送一遍数据
}
