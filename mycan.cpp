//author:Qian yimao
//company:Beijing Institute of Technology
//date:2020/12/21
//comment: 用于实现对于电机can总线控制验证，作用为，当运行程序时，刹车打开，电机开始转动，位置为，速度为，加速度为，减加速度为

//应用方法：在本文件夹下打开终端
//			使用 rm hello_cpp 清除hello_cpp文件
//			使用 make clean&make 编译文件
//													   
////////////////////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include "controlcan.h"
#include <iostream>
#include "ilm8513_elmo.h"
#include "can_global.h"
#include "mycan.h"

#include <ctime>
#include <cstdlib>
#include "unistd.h"

VCI_BOARD_INFO pInfo;//用来获取设备信息。
int mycan_count=0;//数据列表中，用来存储列表序号。
VCI_BOARD_INFO pInfo1 [50];
int num=0;
pthread_mutex_t mutex;

void *receive_func(void* param)  //接收线程。
{
	int reclen=0;
	VCI_CAN_OBJ rec[3000];//接收缓存，设为3000为佳。
	int i,j;
	
	int *run=(int*)param;//线程启动，退出控制。
    int ind=0;
	    

	while((*run)&0x0f)
	{
		pthread_mutex_lock(&mutex);			//上互斥锁
		if((reclen=VCI_Receive(VCI_USBCAN2,0,ind,rec,3000,100))>0)//调用接收函数，如果有数据，进行数据处理显示。
		{
			for(j=0;j<reclen;j++)
			{
				printf("Index:%04d  ",mycan_count);mycan_count++;//序号递增
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
				*can_control_word = 1;
				
				std::cout << *can_control_word << std::endl;
			}
		}
		pthread_mutex_unlock(&mutex);		//解互斥锁
		usleep(100);						//为节省时间可以优化掉

		// ind=!ind;//变换通道号，以便下次读取另一通道，交替读取。		
	}
	printf("run thread exit\n");//退出接收线程	
	pthread_exit(0);
}

main()  //主函数从这里开始
{
	printf(">>this is hello !\r\n");//指示程序已运行

	num=VCI_FindUsbDevice2(pInfo1);//VCI_FindUsbDevice2，当同一台PC上使用多个USB-CAN的时候,可用此函数查找当前的设备,并获取所有设备的序列号

	printf(">>\n");
	printf(">>\n");
	printf(">>\n");

	int threadlockstatue = pthread_mutex_init(&mutex, NULL);	//初始化互斥锁
/* ************************************************************************************************************************************************************************ */
//开启设备并读取设备信息
/* ************************************************************************************************************************************************************************ */

	if(VCI_OpenDevice(VCI_USBCAN2,0,0)==1)//打开设备VCI_OpenDevice，此函数用以打开设备。注意一个设备只能打开一次。
	{
		printf(">>001 open deivce success!\n");//打开设备成功
	}else
	{
		printf(">>open deivce error!\n");
		exit(1);
	}
	if(VCI_ReadBoardInfo(VCI_USBCAN2,0,&pInfo)==1)//读取设备序列号、版本等信息。VCI_ReadBoardInfo此函数用以获取设备信息
	{
                printf(">>002 Get VCI_ReadBoardInfo success!\n");
		
		//printf(" %08X", pInfo.hw_Version);printf("\n");
		//printf(" %08X", pInfo.fw_Version);printf("\n");
		//printf(" %08X", pInfo.dr_Version);printf("\n");
		//printf(" %08X", pInfo.in_Version);printf("\n");
		//printf(" %08X", pInfo.irq_Num);printf("\n");
		//printf(" %08X", pInfo.can_Num);printf("\n");
		// printf(">>Serial_Num:%c", pInfo.str_Serial_Num[0]);
		// printf("%c", pInfo.str_Serial_Num[1]);
		// printf("%c", pInfo.str_Serial_Num[2]);
		// printf("%c", pInfo.str_Serial_Num[3]);
		// printf("%c", pInfo.str_Serial_Num[4]);
		// printf("%c", pInfo.str_Serial_Num[5]);
		// printf("%c", pInfo.str_Serial_Num[6]);
		// printf("%c", pInfo.str_Serial_Num[7]);
		// printf("%c", pInfo.str_Serial_Num[8]);
		// printf("%c", pInfo.str_Serial_Num[9]);
		// printf("%c", pInfo.str_Serial_Num[10]);
		// printf("%c", pInfo.str_Serial_Num[11]);
		// printf("%c", pInfo.str_Serial_Num[12]);
		// printf("%c", pInfo.str_Serial_Num[13]);
		// printf("%c", pInfo.str_Serial_Num[14]);
		// printf("%c", pInfo.str_Serial_Num[15]);
		// printf("%c", pInfo.str_Serial_Num[16]);
		// printf("%c", pInfo.str_Serial_Num[17]);
		// printf("%c", pInfo.str_Serial_Num[18]);
		// printf("%c", pInfo.str_Serial_Num[19]);printf("\n");

		// printf(">>003 hw_Type:%c", pInfo.str_hw_Type[0]);
		// printf("%c", pInfo.str_hw_Type[1]);
		// printf("%c", pInfo.str_hw_Type[2]);
		// printf("%c", pInfo.str_hw_Type[3]);
		// printf("%c", pInfo.str_hw_Type[4]);
		// printf("%c", pInfo.str_hw_Type[5]);
		// printf("%c", pInfo.str_hw_Type[6]);
		// printf("%c", pInfo.str_hw_Type[7]);
		// printf("%c", pInfo.str_hw_Type[8]);
		// printf("%c", pInfo.str_hw_Type[9]);printf("\n");

		// printf(">>004 Firmware Version:V");
		// printf("%x", (pInfo.fw_Version&0xF00)>>8);
		// printf(".");
		// printf("%x", (pInfo.fw_Version&0xF0)>>4);
		// printf("%x", pInfo.fw_Version&0xF);
		// printf("\n");	
	}else
	{
		printf(">>Get VCI_ReadBoardInfo error!\n");
		exit(1);
	}
/* ************************************************************************************************************************************************************************ */
//初始化can口参数，初始波特率为500kbps，正常收发模式
/* ************************************************************************************************************************************************************************ */

	//初始化参数，严格参数二次开发函数库说明书。
	VCI_INIT_CONFIG config;
	config.AccCode=0;	//验收码。SJA1000的帧过滤验收码。对经过屏蔽码过滤为“有关位”进行匹配,全部匹配成功后,此帧可以被接收。否则不接收
	config.AccMask=0xFFFFFFFF;	//屏蔽码。SJA1000的帧过滤屏蔽码。对接收的CAN帧ID进行过滤,对应位为0的是“有关位”,对应位为1的是“无关位”。屏蔽码推荐设置为0xFFFFFFFF,即全部接收。
	config.Filter=1;//接收所有帧。滤波方式,允许设置为0-3,详细请参照2.2.3节的滤波模式对照表。
	config.Timing0=0x00;/*波特率500 Kbps  0x00  0x1C 波特率定时器 0(BTR0)。设置值见下表。*/
	config.Timing1=0x1C;/*波特率定时器 1(BTR1)。设置值见下表。*/
	config.Mode=0;//正常模式 模式。 =0表示正常模式(相当于正常节点), =1表示只听模式(只接收,不影响总线)，=2表示自发自收模式(环回模式)。		
	
/* ************************************************************************************************************************************************************************ */
//开启can0和can1两个通道
/* ************************************************************************************************************************************************************************ */

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
/* ************************************************************************************************************************************************************************ */
//初始化发送帧内容，包括
/* ************************************************************************************************************************************************************************ */
	int m_run0=1;
	pthread_t threadid;
	int ret;
	ret=pthread_create(&threadid,NULL,receive_func,&m_run0);	//创建一个进程用于接受can信息

	std::cout << "设配重启\n\n\n" << std::endl;
	// std::cout << *can_control_word << std::endl;
	while (!*can_control_word);
	// std::cout << *can_control_word << std::endl;
	usleep(100000);//延时10ms。
	pthread_mutex_lock(&mutex);
	std::cout << "\n01启动控制器" << std::endl;
	Q_START_CANOPEN	(can_0,motor_1_ID,reserve);
	pthread_mutex_unlock(&mutex);

	usleep(10000);//延时10ms。

	std::cout << "开始通信\n\n\n" << std::endl;
	pthread_mutex_lock(&mutex);
	Q_SETMODE_CANOPEN(can_0,motor_1_ID,1,reserve);
	pthread_mutex_unlock(&mutex);
	// while (!*can_control_word);	
	usleep(10000);//延时10ms。

	std::cout << "设置pp模式\n\n\n" << std::endl;
	pthread_mutex_lock(&mutex);
	Q_SETACC_CANOPEN(can_0,motor_1_ID,1000000,reserve);
	pthread_mutex_unlock(&mutex);
	// while (!*can_control_word);	
	usleep(10000);//延时10ms。

	std::cout << "设置加速度\n\n\n" << std::endl;
	pthread_mutex_lock(&mutex);
	Q_SETDEC_CANOPEN(can_0,motor_1_ID,1000000,reserve);
	pthread_mutex_unlock(&mutex);
	// while (!*can_control_word);	
	usleep(10000);//延时10ms。

	std::cout << "设置减速度\n\n\n" << std::endl;
	pthread_mutex_lock(&mutex);
	Q_READYON_CANOPEN(can_0,motor_1_ID,reserve);
	pthread_mutex_unlock(&mutex);
	// while (!*can_control_word);	
	usleep(10000);//延时10ms。

	std::cout << "switch on\n\n\n" << std::endl;
	pthread_mutex_lock(&mutex);
	Q_SWITCHON_CANOPEN(can_0,motor_1_ID,reserve);
	pthread_mutex_unlock(&mutex);
	// while (!*can_control_word);	
	usleep(10000);//延时10ms。

	std::cout << "准备\n\n\n" << std::endl;
	pthread_mutex_lock(&mutex);
	Q_SETPX_CANOPEN(can_0,motor_1_ID,0,reserve);
	pthread_mutex_unlock(&mutex);
	// while (!*can_control_word);	
	usleep(10000);//延时10ms。

	std::cout << "设置PX\n\n\n" << std::endl;
	pthread_mutex_lock(&mutex);
	Q_SETSPEED_CANOPEN(can_0,motor_1_ID,0,reserve);
	pthread_mutex_unlock(&mutex);
	// while (!*can_control_word);	
	usleep(10000);//延时10ms。

	std::cout << "设置速度\n\n\n" << std::endl;
	pthread_mutex_lock(&mutex);
	Q_SETMO_CANOPEN(can_0,motor_1_ID,1,reserve);
	pthread_mutex_unlock(&mutex);
	// while (!*can_control_word);
	usleep(10000);//延时10ms。


	std::cout << "打开MO\n\n\n" << std::endl;
	pthread_mutex_lock(&mutex);
	Q_SETPOS_CANOPEN(can_0,motor_1_ID,1,reserve);
	pthread_mutex_unlock(&mutex);
	// while (!*can_control_word);
	usleep(10000);//延时10ms。

	std::cout << "设置位置\n\n\n" << std::endl;
	pthread_mutex_lock(&mutex);
	Q_OPENOPER_CANOPEN(can_0,motor_1_ID,reserve);
	pthread_mutex_unlock(&mutex);
	// while (!*can_control_word);
	usleep(10000);//延时10ms。

	std::cout << "SET POINT\n\n\n" << std::endl;
	// pthread_mutex_lock(&mutex);
	// Q_CLEARSP_CANOPEN(can_0,motor_1_ID,reserve);
	// pthread_mutex_unlock(&mutex);
	// // while (!*can_control_word);
	// std::cout << "CLEAR SP\n\n\n" << std::endl;
	

	usleep(15000000);//延时单位us，这里设置 10 000 000=15s    10s后关闭接收线程，并退出主程序。
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
