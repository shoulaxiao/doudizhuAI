#pragma once
//////////////////////////////////
//////牌局信息控制台///////////////
//功能：用于初始化轮局，更新牌局////
//最后修订者：李讯航 修订时间：2018.7.22
//修订内容及目的：
//1、去掉GetInf()中对InitRound()的重复调用；
//2、增加在控制台内多局游戏以及调试的功能，调整了排版
//3、修改边界条件，增加了异常处理机制
//4、增加详细具体的注释,提高代码可读性
////////////////////////////////


#include "Ddz.h"
#include "CardsInfo.h"
#include "MonteCarloSearchEngine.h"
#include "CallNum.h"
#include<list>


class BoardMessageControl
{
public:	

	char cUpPlayer;    //上家编号

	char cDownPlayer;  //下家编号

	int  aiBid;       //AI叫分数

	int  who = 0;     //0表示轮到上家出牌，1表示AI出牌，2表示下家出牌

	int  who2 = 0;    //0表示地主为上家，1表示AI，2表示下家
	
	CardsInfo* CInfo; //在InitTurn中被初始化
	
	MonteCarloSearchEngine *engine;//在InitTurn中被初始化
	
	PokerModelAnalysis*Pma; //在BoardMessageControl中被初始化
	
	BoardMessageControl(); //初始化Pma
	
	~BoardMessageControl();  //释放engine
	
	void InitTurn(struct Ddz * pDdz);				//P01 初始化本轮数据

	void InitRound(struct Ddz * pDdz);				//M04 初始化本局数据
	
	void InputMsg(struct Ddz * pDdz);				//P02 输入信息

	void printCard(int*);                             //在控制台中对牌进行输出

	void AnalyzeMsg(struct Ddz * pDdz);				//P03 分析处理信息

	void GetDou(struct Ddz * pDdz);					//P0301 获取并处理版本信息DOU

	void GetInf(struct Ddz * pDdz);					//P0302 获取并处理轮局信息INF

	void GetDea(struct Ddz * pDdz);					//P0303 获取并处理牌套信息DEA

	void GetBid(struct Ddz * pDdz);					//P0304 获取并处理叫牌信息BID
 
	void GetLef(struct Ddz * pDdz);					//P0305 获取并处理底牌信息LEF

	void GetPla(struct Ddz * pDdz);					//P0306 获取并处理出牌信息PLA

	void CalPla(struct Ddz * pDdz);					//P030601 计算己方出牌策略

	void UpdateMyPla(struct Ddz * pDdz);			//P030602 根据己方出牌更新数据

	void UpdateHisPla(struct Ddz * pDdz);			//P030603 根据他人出牌更新数据

	void GetGam(struct Ddz * pDdz);					//P0307 获取并处理胜负信息GAM

	void OutputMsg(struct Ddz * pDdz);				//P04 输出信息

	void CalOthers(struct Ddz * pDdz);				//P05 计算其它数据

	int CalBid(struct Ddz * pDdz);					//I02 计算己方叫牌策略

	void OtherPlayerCards(Ddz *pDdz, int *iCards);  //计算其他两人手中剩牌
	
	

};

