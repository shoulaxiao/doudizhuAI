//修改人：李讯航
//对所有主要变量信息的声明
//日期：2018-7-19
//增加对iStatus的初始赋值为1；增加变量isRandom进行随机或固定生成牌；修改kPlayerName值

#include <stdio.h>
#include <string.h>
#include <iostream>
#define kPlayerName "理来玩地主"
#define kPlaMax 1000
#pragma once

#define iFShowInConsole true
#define iFShowOtherCard true
#define isRandom true

using namespace std;

struct Ddz
{
	int  iStatus;			//引擎状态-1错误,0结束,1开始
	char sCommandIn[80];	//通信输入内容
	char sCommandOut[80];	//通信输出内容
	int iOnHand[21];		//手中牌（所有值初始化为-1）
	int iOnTable[162][21];	//已出牌数组（所有值初始化为-2）每行是一手牌，以-1结尾，Pass记为-1；-2不代表没有出牌，-1代表没有出牌或者分割，-2的作用仅仅在于赋无关初值
	int iUpPlayerCards;		//上家手中牌数
	int iDownPlayerCards;	//下家手中牌数
	int iToTable[21];		//要出的牌
	int iOnOtherHand[55];   //在其他玩家手中的牌
	char sVer[80];			//协议版本号
	char sName[80];			//参赛选手称呼
	char cDir;				//玩家方位编号
	char cLandlord;			//地主玩家方位编号
	char cLastPlay;         //非模拟出牌时上位出牌玩家方位编号
	char cWinner;			//胜利者方位编号
	int iBid[3];			//叫牌方，A为iBid[0],B为[1],C为[2]
	int iBmax;				//当前叫牌数，值域{0,1,2,3}，（只做了初始化）
	int iOTmax;				//当前出牌手数
	int iRoundNow;			//当前局次
	int iRoundTotal;		//和总局数
	int iTurnNow;			//当前轮次
	int iTurnTotal;			//总轮数
	int iLevelUp;			//晋级选手数
	int iScoreMax;			//转换得分界限
	int iVoid;				//闲暇并行计算参数（没有用到）
	int iLef[3];			//本局底牌
	int iLastPassCount;		//当前桌面连续PASS数（值域[0,2],初值2，正常出牌取0，一家PASS取1，两家PASS取2）
	int iLastTypeCount;		//当前桌面牌型张数（值域[0,1108],初值0，iLastPassCount=0时更新值，=1时保留原值，=2时值为0）//最新出牌//十位以后代表的是牌的类型,个位代表牌的张数                  
	int iLastMainPoint;		//当前桌面主牌主点数（值域[0,14],初值-1，iLastPassCount=0时更新值，，=1时保留原值，=2时值为-1）
	bool haveHelpFriend;	//记录是否帮助过队友（当队友只有一张牌时，拿到出牌权，出最小单牌（判断队友是否打的起自己的最小单牌）） 初始化为false

	//Test
	double dWinCount;    //AI赢的局数
};

