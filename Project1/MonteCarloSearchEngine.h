/*********************************
*  简要描述:使用蒙特卡洛搜索树算法搜牌
*  最后修订者：郭忠浩 2018-7-23
*  作者:范晓梦 2016-6
*  说明:koudingkouding@gmail.com 有疑问请邮箱
***********************************/

#pragma once
#include <windows.h>
#include <list>
#include <map>
#include "RecommendCards.h"
#include<math.h>
#define MaxRunTime 12//允许执行蒙特卡洛算法的最大时间

class MonteCarloSearchEngine
{
public:
	MonteCarloSearchEngine();
	~MonteCarloSearchEngine();
	void Init();
	void SearchAGoodMove(Ddz *pDdz);
	int RunMonteCarloSimulation(int node);
	Ddz *pRealDdz;
	//int iAllPlayTimes;总共执行蒙特卡洛模拟的次数
	int iMaxDepth;
	fDdz *pMyDdzBoard;
	fDdz *pUpDdzBoard;
	fDdz *pDownDdzBoard;
	int iTotalPlay;//总共执行蒙特卡洛模拟的次数
	int iMyUpPlayerCards[21];
	int iMyDownPlayerCards[21];
	int ibestMove[21];
	RecommendCards *pRecomCards;
	//蒙特卡洛树节点
	struct MTCNode
	{
		int node;//节点下标
		int iWin;//本阵营模拟赢次数
		int iPlay;//本节点被访问次数
		float fUCB1;//UCB1值，初始值为-1
		float fAverage;
		int iCards[21];//该节点出牌
	};
	int iNodesCount;
	int iLastValidCards[21];//最后一次有效出牌
	MTCNode *nodesList;
	//int iPlaArr[20][21];
	CardsInfo *pCinfo;
	int iDepthPos[4];//每层节点在iPLArr起始位置下标
	//
	// 计算并更新本节点UCB1值
	void UpdateUCB1(MTCNode nodes[], int node, int Winner, int iTotalPlay);
	void Update(int node, int iDepth);
	// 产生节点，并返回节点数
	int MakeMove(int iDepth);
	// 将该层生成的节点添加到列表中
	//int AddDepthNodesInList(int iDepth);
	int IsGameOver();
	void GetOtherPlayerCards(int* iUpPlayerCards, int* iDownPlayerCards);
	void InitBoard();
	// 左开右闭
	int GetRandomNumFromRange(int iStart, int iEnd,int iDepth);
	void Run();//开始执行蒙特卡洛树
	int nodeSeed;
	/////测试用的数据
	int AIWin;
	int UpWin;
	int DownWin;

	CardsInfo*Cinfo;

};

