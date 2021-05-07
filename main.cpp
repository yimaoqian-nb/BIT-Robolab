//样例只是提供一个简单的调用so库的方法供参考，程序接收，与发送函数设置在两个线程中，并且线程没有同步。
//现实中客户编程中，发送与接收函数不能同时调用（不支持多线程），如果在多线程中，一定需要互锁。需要客户自行完善代码。




#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include "controlcan.h"

#include <ctime>
#include <cstdlib>
#include "unistd.h"

VCI_BOARD_INFO pInfo;//用来获取设备信息。
int count=0;//数据列表中，用来存储列表序号。
VCI_BOARD_INFO pInfo1 [50];
int num=0;
void *receive_func(void* param)  //接收线程。
{
	int reclen=0;
	VCI_CAN_OBJ rec[3000];//接收缓存，设为3000为佳。
	int i,j;
	
	int *run=(int*)param;//线程启动，退出控制。
    int ind=0;
	
	while((*run)&0x0f)
	{
		if((reclen=VCI_Receive(VCI_USBCAN2,0,ind,rec,3000,100))>0)//调用接收函数，如果有数据，进行数据处理显示。
		{
			for(j=0;j<reclen;j++)
			{
				printf("Index:%04d  ",count);count++;//序号递增
				printf("CAN%d RX ID:0x%08X", ind+1, rec[j].ID);//ID
				if(rec[j].ExternFlag==0) printf(" Standard ");//帧格式：标准帧
				if(rec[j].ExternFlag==1) printf(" Extend   ");//帧格式：扩展帧
				if(rec[j].RemoteFlag==0) printf(" Data   ");//帧类型：数据帧
				if(rec[j].RemoteFlag==1) printf(" Remote ");//帧类型：远程帧
				printf("DLC:0x%02X",rec[j].DataLen);//帧长度
				printf(" data:0x");	//数据
				for(i = 0; i < rec[j].DataLen; i++)
				{
					printf(" %02X", rec[j].Data[i]);
				}
				printf(" TimeStamp:0x%08X",rec[j].TimeStamp);//时间标识。
				printf("\n");
			}
		}
		ind=!ind;//变换通道号，以便下次读取另一通道，交替读取。		
	}
	printf("run thread exit\n");//退出接收线程	
	pthread_exit(0);
}

main()  //主函数从这里开始
{
	printf(">>this is hello !\r\n");//指示程序已运行

	num=VCI_FindUsbDevice2(pInfo1);//VCI_FindUsbDevice2，当同一台PC上使用多个USB-CAN的时候,可用此函数查找当前的设备,并获取所有设备的序列号

	printf(">>USBCAN DEVICE NUM:");printf("%d", num);printf(" PCS");printf("\n");

		for(int i=0;i<num;i++)//从VCI_FindUsbDevice2搜索到的第一个设备开始遍历
		{
		printf("Device:");printf("%d", i);printf("\n");//第n个设备
                printf(">>Get VCI_ReadBoardInfo success!\n");
		
		printf(">>Serial_Num:%c", pInfo1[i].str_Serial_Num[0]);
		printf("%c", pInfo1[i].str_Serial_Num[1]);
		printf("%c", pInfo1[i].str_Serial_Num[2]);
		printf("%c", pInfo1[i].str_Serial_Num[3]);
		printf("%c", pInfo1[i].str_Serial_Num[4]);
		printf("%c", pInfo1[i].str_Serial_Num[5]);
		printf("%c", pInfo1[i].str_Serial_Num[6]);
		printf("%c", pInfo1[i].str_Serial_Num[7]);
		printf("%c", pInfo1[i].str_Serial_Num[8]);
		printf("%c", pInfo1[i].str_Serial_Num[9]);
		printf("%c", pInfo1[i].str_Serial_Num[10]);
		printf("%c", pInfo1[i].str_Serial_Num[11]);
		printf("%c", pInfo1[i].str_Serial_Num[12]);
		printf("%c", pInfo1[i].str_Serial_Num[13]);
		printf("%c", pInfo1[i].str_Serial_Num[14]);
		printf("%c", pInfo1[i].str_Serial_Num[15]);
		printf("%c", pInfo1[i].str_Serial_Num[16]);
		printf("%c", pInfo1[i].str_Serial_Num[17]);
		printf("%c", pInfo1[i].str_Serial_Num[18]);
		printf("%c", pInfo1[i].str_Serial_Num[19]);printf("\n");

		printf(">>hw_Type:%c", pInfo1[i].str_hw_Type[0]);
		printf("%c", pInfo1[i].str_hw_Type[1]);
		printf("%c", pInfo1[i].str_hw_Type[2]);
		printf("%c", pInfo1[i].str_hw_Type[3]);
		printf("%c", pInfo1[i].str_hw_Type[4]);
		printf("%c", pInfo1[i].str_hw_Type[5]);
		printf("%c", pInfo1[i].str_hw_Type[6]);
		printf("%c", pInfo1[i].str_hw_Type[7]);
		printf("%c", pInfo1[i].str_hw_Type[8]);
		printf("%c", pInfo1[i].str_hw_Type[9]);printf("\n");	

		printf(">>Firmware Version:V");
		printf("%x", (pInfo1[i].fw_Version&0xF00)>>8);
		printf(".");
		printf("%x", (pInfo1[i].fw_Version&0xF0)>>4);
		printf("%x", pInfo1[i].fw_Version&0xF);
		printf("\n");
	}		//第i个设备信息读取完成
	printf(">>\n");
	printf(">>\n");
	printf(">>\n");
	if(VCI_OpenDevice(VCI_USBCAN2,0,0)==1)//打开设备VCI_OpenDevice，此函数用以打开设备。注意一个设备只能打开一次。
	{
		printf(">>open deivce success!\n");//打开设备成功
	}else
	{
		printf(">>open deivce error!\n");
		exit(1);
	}
	if(VCI_ReadBoardInfo(VCI_USBCAN2,0,&pInfo)==1)//读取设备序列号、版本等信息。VCI_ReadBoardInfo此函数用以获取设备信息
	{
                printf(">>Get VCI_ReadBoardInfo success!\n");
		
		//printf(" %08X", pInfo.hw_Version);printf("\n");
		//printf(" %08X", pInfo.fw_Version);printf("\n");
		//printf(" %08X", pInfo.dr_Version);printf("\n");
		//printf(" %08X", pInfo.in_Version);printf("\n");
		//printf(" %08X", pInfo.irq_Num);printf("\n");
		//printf(" %08X", pInfo.can_Num);printf("\n");
		printf(">>Serial_Num:%c", pInfo.str_Serial_Num[0]);
		printf("%c", pInfo.str_Serial_Num[1]);
		printf("%c", pInfo.str_Serial_Num[2]);
		printf("%c", pInfo.str_Serial_Num[3]);
		printf("%c", pInfo.str_Serial_Num[4]);
		printf("%c", pInfo.str_Serial_Num[5]);
		printf("%c", pInfo.str_Serial_Num[6]);
		printf("%c", pInfo.str_Serial_Num[7]);
		printf("%c", pInfo.str_Serial_Num[8]);
		printf("%c", pInfo.str_Serial_Num[9]);
		printf("%c", pInfo.str_Serial_Num[10]);
		printf("%c", pInfo.str_Serial_Num[11]);
		printf("%c", pInfo.str_Serial_Num[12]);
		printf("%c", pInfo.str_Serial_Num[13]);
		printf("%c", pInfo.str_Serial_Num[14]);
		printf("%c", pInfo.str_Serial_Num[15]);
		printf("%c", pInfo.str_Serial_Num[16]);
		printf("%c", pInfo.str_Serial_Num[17]);
		printf("%c", pInfo.str_Serial_Num[18]);
		printf("%c", pInfo.str_Serial_Num[19]);printf("\n");

		printf(">>hw_Type:%c", pInfo.str_hw_Type[0]);
		printf("%c", pInfo.str_hw_Type[1]);
		printf("%c", pInfo.str_hw_Type[2]);
		printf("%c", pInfo.str_hw_Type[3]);
		printf("%c", pInfo.str_hw_Type[4]);
		printf("%c", pInfo.str_hw_Type[5]);
		printf("%c", pInfo.str_hw_Type[6]);
		printf("%c", pInfo.str_hw_Type[7]);
		printf("%c", pInfo.str_hw_Type[8]);
		printf("%c", pInfo.str_hw_Type[9]);printf("\n");

		printf(">>Firmware Version:V");
		printf("%x", (pInfo.fw_Version&0xF00)>>8);
		printf(".");
		printf("%x", (pInfo.fw_Version&0xF0)>>4);
		printf("%x", pInfo.fw_Version&0xF);
		printf("\n");	
	}else
	{
		printf(">>Get VCI_ReadBoardInfo error!\n");
		exit(1);
	}

	//初始化参数，严格参数二次开发函数库说明书。
	VCI_INIT_CONFIG config;
	config.AccCode=0;	//验收码。SJA1000的帧过滤验收码。对经过屏蔽码过滤为“有关位”进行匹配,全部匹配成功后,此帧可以被接收。否则不接收
	config.AccMask=0xFFFFFFFF;	//屏蔽码。SJA1000的帧过滤屏蔽码。对接收的CAN帧ID进行过滤,对应位为0的是“有关位”,对应位为1的是“无关位”。屏蔽码推荐设置为0xFFFFFFFF,即全部接收。
	config.Filter=1;//接收所有帧。滤波方式,允许设置为0-3,详细请参照2.2.3节的滤波模式对照表。
	config.Timing0=0x03;/*波特率125 Kbps  0x03  0x1C 波特率定时器 0(BTR0)。设置值见下表。*/
	config.Timing1=0x1C;/*波特率定时器 1(BTR1)。设置值见下表。*/
	config.Mode=0;//正常模式 模式。 =0表示正常模式(相当于正常节点), =1表示只听模式(只接收,不影响总线)，=2表示自发自收模式(环回模式)。		
	
	if(VCI_InitCAN(VCI_USBCAN2,0,0,&config)!=1)//VCI_InitCan此函数用以初始化指定的CAN通道。有多个CAN通道时,需要多次调用。
	{
		printf(">>Init CAN1 error\n");	//假如VCI_InitCAN返回值不是1，则发生错误，需要关闭can通道
		VCI_CloseDevice(VCI_USBCAN2,0);	//VCI_CloseDevice，此函数用以关闭设备。
	}

	if(VCI_StartCAN(VCI_USBCAN2,0,0)!=1)	//VCI_StartCAN此函数用以启动CAN卡的某一个CAN通道。有多个CAN通道时,需要多次调用。
	{
		printf(">>Start CAN1 error\n");
		VCI_CloseDevice(VCI_USBCAN2,0);

	}

	if(VCI_InitCAN(VCI_USBCAN2,0,1,&config)!=1)//初始化can通道2
	{
		printf(">>Init can2 error\n");
		VCI_CloseDevice(VCI_USBCAN2,0);

	}
	if(VCI_StartCAN(VCI_USBCAN2,0,1)!=1)//初始化can通道2
	{
		printf(">>Start can2 error\n");
		VCI_CloseDevice(VCI_USBCAN2,0);

	}
	//需要发送的帧，结构体设置
	VCI_CAN_OBJ send[1];		//VCI_CAN_OBJ结构体是CAN帧结构体,即1个结构体表示一个帧的数据结构。在发送函数VCI_Transmit和接收函数VCI_Receive中,被用来传送CAN信息帧。
	send[0].ID=0;				//帧ID。32位变量,数据格式为靠右对齐。详情请参照:《8. 附件 1 : ID 对齐方式 .pdf》说明文档。
	send[0].SendType=0;			//发送帧类型。
								//=0时为正常发送(发送失败会自动重发,重发时间为4秒,4秒内没有发出则取消);
								//=1时为单次发送(只发送一次,发送失败不会自动重发,总线只产生一帧数据);
								//其它值无效。
								//(二次开发,建议SendType=1,提高发送的响应速度)
	send[0].RemoteFlag=0;		//是否是远程帧。=0时为为数据帧,=1时为远程帧(数据段空)。
	send[0].ExternFlag=1;		//是否是扩展帧。=0时为标准帧(11位ID),=1时为扩展帧(29位ID)。
	send[0].DataLen=8;			//数据长度 DLC (<=8),即CAN帧Data有几个字节。约束了后面Data[8]中的有效字节。
	
	int i=0;
	for(i = 0; i < send[0].DataLen; i++)
	{
		send[0].Data[i] = i;	//Data[8]
									//CAN帧的数据。由于CAN规定了最大是8个字节,所以这里预留了8个字节的空间,受
									//DataLen约束。如DataLen定义为3,即Data[0]、Data[1]、Data[2]是有效的。
	}

	int m_run0=1;
	pthread_t threadid;
	int ret;
	ret=pthread_create(&threadid,NULL,receive_func,&m_run0);	//创建一个进程用于接受can信息（没弄懂）

	int times = 5;
	while(times--)
	{
		if(VCI_Transmit(VCI_USBCAN2, 0, 0, send, 1) == 1)		//VCI_Transmit发送函数。返回值为实际发送成功的帧数。
							//参数定义
							//DevType 设备类型。对应不同的产品型号 详见:适配器设备类型定义。
							//DevIndex 设备索引,比如当只有一个USB-CAN适配器时,索引号为0,这时再插入一个USB-CAN适配器那么后面插入的这个设备索引号就是1,以此类推。
							//CANIndex CAN通道索引。第几路 CAN。即对应卡的CAN通道号,CAN1为0,CAN2为1。
							//pSend 要发送的帧结构体 VCI_CAN_OBJ数组的首指针。
							//返回值: 返回实际发送的帧数,=-1表示USB-CAN设备不存在或USB掉线。
		{
			printf("Index:%04d  ",count);count++;
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
		}
		else
		{
			break;
		}										//用can0发送一遍数据
		
		if(VCI_Transmit(VCI_USBCAN2, 0, 1, send, 1) == 1)
		{
			printf("Index:%04d  ",count);count++;
			printf("CAN2 TX ID:0x%08X", send[0].ID);
			if(send[0].ExternFlag==0) printf(" Standard ");
			if(send[0].ExternFlag==1) printf(" Extend   ");
			if(send[0].RemoteFlag==0) printf(" Data   ");
			if(send[0].RemoteFlag==1) printf(" Remote ");
			printf("DLC:0x%02X",send[0].DataLen);
			printf(" data:0x");			
			for(i = 0; i < send[0].DataLen; i++)
			{
				printf(" %02X", send[0].Data[i]);
			}
			printf("\n");
			send[0].ID+=1;
		}
		else	break;
	}											//用can1发送一遍数据
	usleep(10000000);//延时单位us，这里设置 10 000 000=10s    10s后关闭接收线程，并退出主程序。
	m_run0=0;//线程关闭指令。
	pthread_join(threadid,NULL);//等待线程关闭。
	usleep(100000);//延时100ms。
	VCI_ResetCAN(VCI_USBCAN2, 0, 0);//复位CAN1通道。
	usleep(100000);//延时100ms。
	VCI_ResetCAN(VCI_USBCAN2, 0, 1);//复位CAN2通道。
	usleep(100000);//延时100ms。
	VCI_CloseDevice(VCI_USBCAN2,0);//关闭设备。
	//除收发函数外，其它的函数调用前后，最好加个毫秒级的延时，即不影响程序的运行，又可以让USBCAN设备有充分的时间处理指令。
	//goto ext;
}
