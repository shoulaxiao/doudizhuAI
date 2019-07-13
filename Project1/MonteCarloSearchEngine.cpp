#include "MonteCarloSearchEngine.h" 
#include <random> 
#include <iostream>
void printt(int*temp);
int iTest = 0;//测试

/**
郭忠浩 修改：2018.7.29用户方位信息更新错误，AI稳赢（整除错误）
**/
MonteCarloSearchEngine::MonteCarloSearchEngine()
{
	//初始化随机种子：测试
	srand((unsigned)time(NULL));  

	pRecomCards = new RecommendCards(); 
	pMyDdzBoard = new fDdz(); 		// 推荐出牌生成器  我方
	pUpDdzBoard = new fDdz();		//上家	
	pDownDdzBoard = new fDdz();		//下家
	pCinfo = new CardsInfo();    //牌的信息
	//初始化测试数据
	AIWin = 0;
	UpWin = 0;
	DownWin = 0;
	nodeSeed = 0;
}
MonteCarloSearchEngine::~MonteCarloSearchEngine()
{
	delete(pRecomCards);
	delete(pMyDdzBoard);
	delete(pUpDdzBoard);
	delete(pDownDdzBoard);
	delete(pCinfo);
	//清除节点
	delete(nodesList);
}
//初始化搜索引擎
void MonteCarloSearchEngine::Init()
{
	pMyDdzBoard->haveHelpFriend = pRealDdz->haveHelpFriend;
	pUpDdzBoard->haveHelpFriend = pRealDdz->haveHelpFriend;
	pDownDdzBoard->haveHelpFriend = pRealDdz->haveHelpFriend;

	//初始化固定基本信息
	GetOtherPlayerCards(pUpDdzBoard->iOnHand,pDownDdzBoard->iOnHand);  
	pMyDdzBoard->cDir = pRealDdz->cDir;
	pMyDdzBoard->cLandlord = pRealDdz->cLandlord;
	pMyDdzBoard->cLastPlay = pRealDdz->cLastPlay;

	//初始化方位信息  ///确定三家的方位
	switch (pMyDdzBoard->cDir)
	{
	case 'A':
		pUpDdzBoard->cDir = 'C';
		pDownDdzBoard->cDir = 'B';
		break; 
	case 'B':
		pUpDdzBoard->cDir = 'A';
		pDownDdzBoard->cDir = 'C';
		break;
	case 'C':
		pUpDdzBoard->cDir = 'B';
		pDownDdzBoard->cDir = 'A';
		break;
	}



	pUpDdzBoard->cLastPlay = pDownDdzBoard->cDir;	//上家的上家等于我们的下家的方位
	pUpDdzBoard->cLandlord = pRealDdz->cLandlord;	//上家的地主
	pDownDdzBoard->cLastPlay = pMyDdzBoard->cDir;   //下家的上家等于我的方位
	pDownDdzBoard->cLandlord = pRealDdz->cLandlord;

	//初始化模拟牌局信息
	pMyDdzBoard->iLastPassCount = pRealDdz->iLastPassCount;  //当前桌面连续PASS数
	pMyDdzBoard->iLastTypeCount = pRealDdz->iLastTypeCount;	 //当前桌面牌型张数
	pMyDdzBoard->iLastMainPoint = pRealDdz->iLastMainPoint;	//当前桌面主牌点数
	pMyDdzBoard->iUpPlayerCards = pRealDdz->iUpPlayerCards;	 //上家的手牌数
	pMyDdzBoard->iDownPlayerCards = pRealDdz->iDownPlayerCards;	//下家的手牌数

	
	//记录AI最后一次有效桌面信息
	memcpy(pMyDdzBoard->iOnHand, pRealDdz->iOnHand, sizeof(pRealDdz->iOnHand));
	//将手牌复制进pMyDdzBoard

	int i;
	for (i = 0; pRealDdz->iOnTable[i][0] != -2; i++);   
														
	if (pRealDdz->iLastPassCount == 1)
	{	
		//记录最近一次Pass出牌数组，以-1结尾，Pass记为-1  
		pMyDdzBoard->iLastOnTable = pRealDdz->iOnTable[i - 2];
	}													

	else
	{
		//如果第一次出牌
		if (i==0)
			pMyDdzBoard->iLastOnTable = pRealDdz->iOnTable[0];

		else
			pMyDdzBoard->iLastOnTable = pRealDdz->iOnTable[i - 1];
	}


	//读取出牌数组
	for (i = 0; pMyDdzBoard->iLastOnTable[i] > 0; i++)
	{
		iLastValidCards[i] = pMyDdzBoard->iLastOnTable[i];
	}

	//最近一次Pass出牌数组，以-1结尾，Pass记为-1
	iLastValidCards[i] = -1;


	//计算AI手牌数
	int myCardsCount = pCinfo->CountCards(pRealDdz->iOnHand);


	//初始化上家  
	pUpDdzBoard->iLastPassCount = pRealDdz->iLastPassCount;  //当前桌面连续PASS数
	pUpDdzBoard->iLastTypeCount = pRealDdz->iLastTypeCount;	 //当前桌面牌型张数
	pUpDdzBoard->iUpPlayerCards = pRealDdz->iDownPlayerCards; //上家手牌数
	pUpDdzBoard->iLastMainPoint = pRealDdz->iLastMainPoint;		//当前桌面主牌点数
	pUpDdzBoard->iDownPlayerCards = myCardsCount;				//下家手牌数
	pUpDdzBoard->iLastOnTable = iLastValidCards;				//最近一次非Pass出牌数组

	//初始化下家
	pDownDdzBoard->iLastPassCount = pRealDdz->iLastPassCount;
	pDownDdzBoard->iLastTypeCount = pRealDdz->iLastTypeCount;
	pDownDdzBoard->iUpPlayerCards = myCardsCount;
	pDownDdzBoard->iLastMainPoint = pRealDdz->iLastMainPoint;
	pDownDdzBoard->iDownPlayerCards = pRealDdz->iUpPlayerCards;
	pDownDdzBoard->iLastOnTable = iLastValidCards;


}

void printt(int*temp) {

	if (temp[0] == -2)
	{
		cout << "PASS ";
		return;
	}
	int j;
	int a = 0;
	for (int i = 0; temp[i] != -1; i++)
	{
		a++;
	}
	cout << "  ";
	//cout <<"还有"<< a<<"张牌 ";
	for (int pass = a; pass >0; pass--)
	{
		for (int i = 0; i < pass - 1; i++)
		{
			if (temp[i] > temp[i + 1]) {
				j = temp[i];
				temp[i] = temp[i + 1];
				temp[i + 1] = j;
			}
		}
	}

	for (int i = 0; i<a; i++)
	{
		if (temp[i] / 4 == 0)
			cout << 3 << " ";    //3
		else if (temp[i] / 4 == 1)
			cout << 4 << " ";   //4
		else if (temp[i] / 4 == 2)
			cout << 5 << " ";   //5
		else if (temp[i] / 4 == 3)
			cout << 6 << " ";    //6
		else if (temp[i] / 4 == 4)
			cout << 7 << " ";    //7
		else if (temp[i] / 4 == 5)
			cout << 8 << " ";    //8
		else if (temp[i] / 4 == 6)
			cout << 9 << " ";   //9
		else if (temp[i] / 4 == 7)
			cout << 10 << " ";    //10
		else if (temp[i] / 4 == 8)
			cout << " J ";    //J
		else if (temp[i] / 4 == 9)
			cout << " Q ";    //Q
		else if (temp[i] / 4 == 10)
			cout << " K ";   //K
		else if (temp[i] / 4 == 11)
		{
			cout << " A ";    //A
		}
		else if (temp[i] / 4 == 12)
		{
			cout << 2 << " ";   //2
		}
		else if (temp[i] == 52)
		{
			cout << " 小王 ";    //小王
		}
		else if (temp[i] == 53)
		{
			cout << " 大王 ";   //大王
		}
	}
	cout << ",";
}
//开始搜索,配合队友
void MonteCarloSearchEngine::SearchAGoodMove(Ddz *pDdz)
{
	iTest = 0;
	pRealDdz = pDdz;
	//初始化各种值

	Init();

	bool model = pRecomCards->GetModel(pRealDdz);

	//如果(pRealDdz->iUpPlayerCards != 上家手牌|| pRealDdz->iDownPlayerCards != 下家手牌)
	if (!model)
	{
		GetOtherPlayerCards(pRecomCards->iUpCards, pRecomCards->iDownCards);
	}

	//帮助AI根据当前牌局生成当前推荐出牌  iNodesCount 可以生成可行的出牌方案的节点的数量
	iNodesCount = pRecomCards->HelpPla(pMyDdzBoard, 0);

	int iPass[3] = { -2, -1, -1 };

	//防止出牌器出错
	if (iNodesCount == 0)
	{
		int iTmp[4] = { -2,-1,-1,-1 };
		if (pDdz->iLastMainPoint <= -1)
		{
			if (pDdz->iOnHand[0] / 4 == pDdz->iOnHand[1] / 4)
			{
				iTmp[0] = pDdz->iOnHand[0];
				iTmp[1] = pDdz->iOnHand[1];
				iTmp[2] = -2;
				memcpy(ibestMove, iTmp, sizeof(ibestMove));
				return;
			}
			else
			{
				iTmp[0] = pDdz->iOnHand[0];
				iTmp[1] = -2;
				memcpy(ibestMove, iTmp, sizeof(ibestMove));
				return;
			}
		}
		else
		{
			memcpy(ibestMove, iPass, sizeof(ibestMove));
			return;
		}
	}

	//只有一种出牌方案的时候
	if (iNodesCount == 1)
	{
		if (iFShowInConsole)
		{
			if (iTest == 0)
			{
				for (int i = 0; pMyDdzBoard->iPlaArr[i][0] != -1; i++)
				{
					cout << "只有一个可行解 ";
					printt(pMyDdzBoard->iPlaArr[i]);
				}
				iTest++;
			}
		}


		//int iCountsPlaArr = Cinfo->CountCards(pMyDdzBoard->iPlaArr[0]);
		//int iCountsInHand= Cinfo->CountCards(pMyDdzBoard->iOnHand);

		//if (iCountsPlaArr == iCountsInHand || iCountsInHand - iCountsPlaArr <= 2)//可以赢或者出了这手牌数剩下小于等于2，直接出
		if (pRecomCards->MinHandNum(pMyDdzBoard->iOnHand) <= 2)//可以赢或者出了这手数剩下小于等于2，直接出
		{
			memcpy(ibestMove, pMyDdzBoard->iPlaArr[0], sizeof(ibestMove));
			return;
		}

		if (pMyDdzBoard->cDir == pMyDdzBoard->cLandlord) //如果AI是地主，直接出
		{
			memcpy(ibestMove, pMyDdzBoard->iPlaArr[0], sizeof(ibestMove));
			return;
		}
		else//AI不是地主
		{
			//跟牌
			if (pMyDdzBoard->iLastMainPoint != -1)
			{
				//如果上家是地主
				if (pUpDdzBoard->cDir == pMyDdzBoard->cLandlord)
				{
					if (pMyDdzBoard->iLastPassCount == 1)//如果跟队友的牌
					{
						if (300 < pMyDdzBoard->iLastTypeCount&&pMyDdzBoard->iLastTypeCount < 403)//出的是单牌和对子
						{
							if (pMyDdzBoard->iPlaArr[0][0] < 44)//对子和单牌不出大于K的
							{
								memcpy(ibestMove, pMyDdzBoard->iPlaArr[0], sizeof(ibestMove));
								return;
							}
							else
							{
								memcpy(ibestMove, iPass, sizeof(ibestMove));
								return;
							}
						}
						else if (500 < pMyDdzBoard->iLastTypeCount&& pMyDdzBoard->iLastTypeCount < 606)//三条，三带一或一对的牌型
						{
							if (Cinfo->AnalyzeMainMaxPoint(pMyDdzBoard->iPlaArr[0]) < 10)//不出最大点数大于10的
							{
								memcpy(ibestMove, pMyDdzBoard->iPlaArr[0], sizeof(ibestMove));
								return;
							}
							else
							{
								memcpy(ibestMove, iPass, sizeof(ibestMove));
								return;
							}

						}
						else
						{
							memcpy(ibestMove, iPass, sizeof(ibestMove));
							return;
						}
					}
					else//跟地主的牌
					{
						memcpy(ibestMove, pMyDdzBoard->iPlaArr[0], sizeof(ibestMove));
						return;
					}
				}


				//如果下家是地主
				if (pDownDdzBoard->cDir == pMyDdzBoard->cLandlord)
				{
					//跟队友的牌
					if (pMyDdzBoard->iLastPassCount == 0)
					{
						if (300 < pMyDdzBoard->iLastTypeCount&&pMyDdzBoard->iLastTypeCount < 403)//出的是单牌和对子
						{

							if (pMyDdzBoard->iDownPlayerCards >= 10)//如果地主手牌大于等于10张
							{
								if (pMyDdzBoard->iPlaArr[0][0] < 44)//对子和单牌不出大于K的
								{
									memcpy(ibestMove, pMyDdzBoard->iPlaArr[0], sizeof(ibestMove));
									return;
								}
								else {
									memcpy(ibestMove, iPass, sizeof(ibestMove));
									return;
								}
							}
							else
							{
								memcpy(ibestMove, pMyDdzBoard->iPlaArr[0], sizeof(ibestMove));
								return;
							}
						}

						else if (500 < pMyDdzBoard->iLastTypeCount&& pMyDdzBoard->iLastTypeCount < 606)//如果是三条，三带一或一对
						{
							if (Cinfo->AnalyzeMainMaxPoint(pMyDdzBoard->iPlaArr[0]) < 10)//不出最大点数大于10的
							{
								memcpy(ibestMove, pMyDdzBoard->iPlaArr[0], sizeof(ibestMove));
								return;
							}
							else {
								memcpy(ibestMove, iPass, sizeof(ibestMove));
								return;
							}

						}
						else
						{
							memcpy(ibestMove, iPass, sizeof(ibestMove));
							return;
						}
					}
					else//跟地主的牌
					{
						memcpy(ibestMove, pMyDdzBoard->iPlaArr[0], sizeof(ibestMove));
						return;
					}
				}
			}
			/*else//自己出牌，直接出
			{
				memcpy(ibestMove, pMyDdzBoard->iPlaArr[0], sizeof(ibestMove));
				return;
			}*/
		}

		memcpy(ibestMove, iPass, sizeof(ibestMove));
		return;
	}

	if (iFShowInConsole)
	{
		//Test
		if (iTest == 0) {
			cout << " 可行解为";
			for (int i = 0; pMyDdzBoard->iPlaArr[i][0] != -1; i++)
			{

				printt(pMyDdzBoard->iPlaArr[i]);
			}
			iTest++;
		}
	}

	if (iNodesCount > 1) {//有多个可行解


		//如果AI不是地主
		if (pMyDdzBoard->cDir != pMyDdzBoard->cLandlord)
		{
			//跟牌
			if (pMyDdzBoard->iLastMainPoint != -1)
			{
				//如果上家是地主
				if (pUpDdzBoard->cDir == pMyDdzBoard->cLandlord)
				{
					if (pMyDdzBoard->iLastPassCount == 0)// 如果跟地主的牌，那么出最小的
					{
						memcpy(ibestMove, pMyDdzBoard->iPlaArr[0], sizeof(ibestMove));
						return;
					}
					else if (pMyDdzBoard->iLastPassCount == 1)//如果跟队友的牌
					{
						if (300 < pMyDdzBoard->iLastTypeCount&&pMyDdzBoard->iLastTypeCount < 403)//出的是单牌和对子
						{
							if (pMyDdzBoard->iPlaArr[0][0] >= 44)//对子和单牌不出大于K的
							{
								memcpy(ibestMove, iPass, sizeof(ibestMove));
								return;
							}
							else //   <44
							{
								memcpy(ibestMove, pMyDdzBoard->iPlaArr[0], sizeof(ibestMove));
								return;
							}
						}
						else if (500 < pMyDdzBoard->iLastTypeCount&& pMyDdzBoard->iLastTypeCount < 606)////如果是三条，三带一或一对
						{
							if (Cinfo->AnalyzeMainMaxPoint(pMyDdzBoard->iPlaArr[0]) < 8)//不出最大点数大于10的
							{
								memcpy(ibestMove, pMyDdzBoard->iPlaArr[0], sizeof(ibestMove));
								return;
							}
							else
							{
								memcpy(ibestMove, iPass, sizeof(ibestMove));
								return;
							}

						}
						else {
							memcpy(ibestMove, iPass, sizeof(ibestMove));
							return;
						}
					}
				}

				//如果下家是地主
				if (pDownDdzBoard->cDir == pMyDdzBoard->cLandlord)
				{
					//跟队友的牌
					if (pMyDdzBoard->iLastPassCount == 0)
					{
						if (300 < pMyDdzBoard->iLastTypeCount&&pMyDdzBoard->iLastTypeCount < 403)//出的是单牌和对子
						{
							if (pMyDdzBoard->iDownPlayerCards >= 10)//如果地主手牌大于等于10张
							{
								if (pMyDdzBoard->iPlaArr[0][0] < 44)//对子和单牌不出大于K的，
								{

									memcpy(ibestMove, pMyDdzBoard->iPlaArr[0], sizeof(ibestMove));
									return;
								}
								else
								{
									memcpy(ibestMove, iPass, sizeof(ibestMove));
									return;
								}
							}
							else {

								memcpy(ibestMove, pMyDdzBoard->iPlaArr[0], sizeof(ibestMove));
								return;
							}

						}
						else if (500 < pMyDdzBoard->iLastTypeCount&&pMyDdzBoard->iLastTypeCount < 606)////如果是三条，三带一或一对
						{
							if (Cinfo->AnalyzeMainMaxPoint(pMyDdzBoard->iPlaArr[0]) < 8)//不出最大点数大于10的
							{
								memcpy(ibestMove, pMyDdzBoard->iPlaArr[0], sizeof(ibestMove));
								return;
							}
							else
							{
								memcpy(ibestMove, iPass, sizeof(ibestMove));
								return;
							}

						}
						else {
							memcpy(ibestMove, iPass, sizeof(ibestMove));
							return;
						}
					}
					else {//跟地主的牌
						memcpy(ibestMove, pMyDdzBoard->iPlaArr[0], sizeof(ibestMove));
						return;
					}
				}
			}

		}
		if (pMyDdzBoard->cDir == pMyDdzBoard->cLandlord&&pMyDdzBoard->iDownPlayerCards == 17 && pMyDdzBoard->iUpPlayerCards == 17) {
			memcpy(ibestMove, pMyDdzBoard->iPlaArr[0], sizeof(ibestMove));
			return;
		}
	}

		nodesList = new MTCNode[iNodesCount]; //节点数组

											  //初始化节点
		for (int i = 0; pMyDdzBoard->iPlaArr[i][0] != -1; i++)
			//多种出牌可行解集（各出牌解由牌编号升序组成-1间隔,-2收尾）//PASS首项为-3；//pass为-2-1;
		{
			//把每一种可行解存进节点中
			memcpy(nodesList[i].iCards, pMyDdzBoard->iPlaArr[i], sizeof(pMyDdzBoard->iPlaArr[i]));

			nodesList[i].node = i;
			nodesList[i].iPlay = 0;
			nodesList[i].fUCB1 = 0;
			nodesList[i].fAverage = 0;
			nodesList[i].iWin = 0;
		}

		//时间PASS
		DWORD k = GetTickCount(); //获取毫秒级数目  ///GetTickCount() window 的函数

		int iStartTime = k / 1000; // 开始时间以秒为单位

								   //cout << iStartTime << endl;
		int iRunTime = 0;

		//int BestUCb1 = 0;
		int bestNode = -1;

		iTotalPlay = 0;

		unsigned seed;
		seed = (unsigned)time(NULL);

		if (iFShowInConsole)
		{
			cout << endl << "MCTS Running Time(s) ";
		}

		int printRunTime = 0;

		while (iRunTime <= MaxRunTime)
		{
			if (iFShowInConsole)
			{
				//Test
				if (iRunTime != printRunTime)
				{
					cout << iRunTime << "  ";
					printRunTime = iRunTime;
				}
			}


			seed += 1;
			int i = 0;
			//判断是否每个节点都被访问过
			for (i = 0; i < iNodesCount; i++)
			{
				if (nodesList[i].iPlay == 0)
				{
					//TODEBUG
					bestNode = i;
					break;
				}
			}

			//取消：如果所有节点都被访问过，选择UCB1最高的节点
			//随机选择节点
			if (i == iNodesCount)
			{
				iRunTime = GetTickCount() / 1000 - iStartTime;
				if (iRunTime <= MaxRunTime / 3)

					bestNode = GetRandomNumFromRange(0, iNodesCount, 0); //随机选取节点

				else {
					//已经运行时间大于最大运行时间的三分之一时，不再随机选取节点，而是选取fUCB1值最大的节点
					bestNode = 0;
					for (i = 0; i < iNodesCount; i++)
					{
						if (nodesList[i].fUCB1 >= nodesList[bestNode].fUCB1)
						{
							bestNode = i;
						}
					}
				}
			}

			int winner = RunMonteCarloSimulation(bestNode);

			iTotalPlay++;

			UpdateUCB1(nodesList, bestNode, winner, iTotalPlay);

			iRunTime = GetTickCount() / 1000 - iStartTime;

			if (iRunTime == 12) { break; }
		}

		if (iFShowInConsole)
		{
			cout << endl;
			cout << "模拟出牌已运行" << iRunTime << "秒" << endl;
			cout << "共模拟" << iTotalPlay << "次" << endl;
			cout << "AIWIN:" << AIWin << " ,UPWIN:" << UpWin << " DownWIN:" << DownWin << endl;
		}

		bestNode = 0;
		for (int j = 0; j < iNodesCount; j++)
		{
			if (iFShowInConsole) 
			{
				cout << "node " << j << "average=" << nodesList[j].fAverage <<
					"= " << nodesList[j].iWin << "/" << nodesList[j].iPlay <<
					" UCB1=" << nodesList[j].fUCB1 << endl;
			}


			//找出胜率最大的节点
			if (nodesList[j].fAverage >= nodesList[bestNode].fAverage)
				bestNode = j;
			if (iFShowInConsole)
			{
				cout << " I choose the:" << bestNode << endl;
			}
		}
		memcpy(ibestMove, nodesList[bestNode].iCards, sizeof(ibestMove));

		delete(nodesList);

	
	
	
	
	//ofstream SaveFile("data.txt", ios::app);
	//int i, j;
	//j = 0;
	//SaveFile << "本次手牌： " ;
	//for (j = 0; pDdz->iOnHand[j] != -1; j++)
	//	SaveFile << pDdz->iOnHand[j] << ',';
	//SaveFile << endl;

	//SaveFile << "本次出牌节点：" << endl;
	//for (i = 0; i<iNodesCount; i++)
	//{
	//	for (j = 0; nodesList[i].iCards[j] != -1; j++)
	//		SaveFile << nodesList[i].iCards[j] << ',';
	//	SaveFile << endl;
	//}
	//SaveFile << "本次出牌：" ;
	//for (j = 0; ibestMove[j] != -1; j++)
	//	SaveFile << ibestMove[j] << ',';
	//SaveFile << endl;
	//SaveFile << "模拟出牌已运行" << iRunTime << "秒" << endl;
	//SaveFile << "共模拟" << iTotalPlay << "次" << endl;
	//SaveFile << "AIWIN:" << AIWin << " ,UPWIN:" << UpWin << " DownWIN:" << DownWin << endl;
	//SaveFile << endl;
	//SaveFile.close();
	
	//ibestMove = nodesList[bestNode].iCards;

}




//针对AI节点进行蒙特卡洛模拟
//AI赢返回1，否则返回0
int MonteCarloSearchEngine::RunMonteCarloSimulation(int node)
{
	if (iTotalPlay>0)
		//初始化模拟界面
		Init();
	//cout << " node:" << node << "in " << iNodesCount << endl;

	Update(node, 0);

	int iDepth = 1;

	//if (iTotalPlay>=1)
	//GetOtherPlayerCards(pUpDdzBoard->iOnHand, pDownDdzBoard->iOnHand);
	//初始化上家下家玩家信息
	//pUpDdzBoard->iLastOnTable = nodesList[node].iCards;
	//pDownDdzBoard->iLastOnTable = nodesList[node].iCards;

	int iIsGameOver = -1;

	unsigned seed;
	seed = (unsigned)time(NULL);

	while (iIsGameOver==-1)
	{	
		seed += 10000;//改变seed

		//产生节点
		int thisNode;
		int nodesCount = MakeMove(iDepth);

		//选择随机节点出牌
		if (nodesCount > 1)
		{
			thisNode = GetRandomNumFromRange(0, nodesCount, iDepth);
		}
		else
		{
			thisNode = 0;
		}//更新局面信息
		Update(thisNode, iDepth);

		iDepth = iDepth + 1; //深度增加1

		iIsGameOver = IsGameOver();  //AI赢return 1  输return 0,没结束return -1;
	}
	return iIsGameOver;
}

// 计算并更新本节点局面模拟值
void MonteCarloSearchEngine::Update(int node, int iDepth)
{


	int iCardsCount;

	fDdz *thisBoard = pMyDdzBoard;

	switch (iDepth % 3)
	{
	case 1:thisBoard = pDownDdzBoard;
		break;
	case 2:thisBoard = pUpDdzBoard;
		break;
	}
	//ofstream SaveFile("SimData.txt", ios::app);
	//int i, j;
	//j = 0;

	/*SaveFile << "第" << iTotalPlay << "次模拟"  ;
	SaveFile << "层:" << iDepth << " ";
	SaveFile << "手牌:";
	for (int i = 0; thisBoard->iOnHand[i] != -1; i++)
	{
		SaveFile << thisBoard->iOnHand[i] << ",";
	}
	SaveFile << endl;
	SaveFile << "节点:";*/
	//测试用//

	if (iDepth == 0 && iTotalPlay>0)
	{
		memcpy(thisBoard->iPlaArr[node], nodesList[node].iCards, sizeof(nodesList[node].iCards));
	}

	//for (int i = 0; thisBoard->iPlaArr[node][i] != -1; i++)
	//{
	//	SaveFile << thisBoard->iPlaArr[node][i] << ",";
	//}
	//SaveFile << endl;
	//SaveFile.close();

	//如果是pass
	
	if (thisBoard->iPlaArr[node][0] <= -1)
	{
		thisBoard->iLastPassCount++;
		iCardsCount = 0;
		if (thisBoard->iLastPassCount >= 2)	//连续两家PASS
		{
			thisBoard->iLastPassCount = 0;
			thisBoard->iLastTypeCount = 0;
			thisBoard->iLastMainPoint = -1;

			memset(iLastValidCards, -1, sizeof(iLastValidCards));

			thisBoard->iLastOnTable = iLastValidCards;
		}
		//else什么也不改变
	}
	else
	{
		//计算牌张数
		//for (iCardsCount = 0; thisBoard->iPlaArr[node][iCardsCount] != -1; iCardsCount++);
		//更新thisBoard数据
		//假设iplaArr是有序的
		//TODO：测试代码
		auto my_lambda_func = [](int *iOnHand, int elem)
		{
			int i = 0, j = 0;
			for (j = 0; j<21; j++)
			{

				if (iOnHand[j] == elem)
					i++;
				else
				{
					iOnHand[j - i] = iOnHand[j];
				}
			}
		};

		for (int n = 0; thisBoard->iPlaArr[node][n] != -1; n++)
		{
			my_lambda_func(thisBoard->iOnHand, thisBoard->iPlaArr[node][n]);
		}
		thisBoard->iLastPassCount = 0;
		thisBoard->iLastMainPoint = pCinfo->AnalyzeMainPoint(thisBoard->iPlaArr[node]);
		thisBoard->iLastTypeCount = pCinfo->AnalyzeTypeCount(thisBoard->iPlaArr[node]);
		memcpy(iLastValidCards, thisBoard->iPlaArr[node], sizeof(thisBoard->iPlaArr[node]));
		thisBoard->iLastOnTable = iLastValidCards;
	}
	if (thisBoard == pMyDdzBoard)
	{
		pUpDdzBoard->iLastPassCount = thisBoard->iLastPassCount;
		pUpDdzBoard->iLastOnTable = iLastValidCards;
		pUpDdzBoard->iLastTypeCount = thisBoard->iLastTypeCount;
		pUpDdzBoard->iLastMainPoint = thisBoard->iLastMainPoint;
		pDownDdzBoard->iLastPassCount = thisBoard->iLastPassCount;
		pDownDdzBoard->iLastOnTable = iLastValidCards;
		pDownDdzBoard->iLastTypeCount = thisBoard->iLastTypeCount;
		pDownDdzBoard->iLastMainPoint = thisBoard->iLastMainPoint;
	}
	if (thisBoard == pUpDdzBoard)
	{
		pMyDdzBoard->iLastPassCount = thisBoard->iLastPassCount;
		pMyDdzBoard->iLastOnTable = iLastValidCards;
		pMyDdzBoard->iLastTypeCount = thisBoard->iLastTypeCount;
		pMyDdzBoard->iLastMainPoint = thisBoard->iLastMainPoint;
		pDownDdzBoard->iLastPassCount = thisBoard->iLastPassCount;
		pDownDdzBoard->iLastOnTable = iLastValidCards;
		pDownDdzBoard->iLastTypeCount = thisBoard->iLastTypeCount;
		pDownDdzBoard->iLastMainPoint = thisBoard->iLastMainPoint;
	}

	if (thisBoard == pDownDdzBoard)
	{
		pUpDdzBoard->iLastPassCount = thisBoard->iLastPassCount;
		pUpDdzBoard->iLastOnTable = iLastValidCards;
		pUpDdzBoard->iLastTypeCount = thisBoard->iLastTypeCount;
		pUpDdzBoard->iLastMainPoint = thisBoard->iLastMainPoint;
		pMyDdzBoard->iLastPassCount = thisBoard->iLastPassCount;
		pMyDdzBoard->iLastOnTable = iLastValidCards;
		pMyDdzBoard->iLastTypeCount = thisBoard->iLastTypeCount;
		pMyDdzBoard->iLastMainPoint = thisBoard->iLastMainPoint;
	}
	iCardsCount = pCinfo->CountCards(thisBoard->iOnHand);
	if (thisBoard == pMyDdzBoard)
	{
		pUpDdzBoard->iDownPlayerCards = iCardsCount;
		pDownDdzBoard->iUpPlayerCards = iCardsCount;
	}
	if (thisBoard == pUpDdzBoard)
	{
		pDownDdzBoard->iDownPlayerCards = iCardsCount;
		pMyDdzBoard->iUpPlayerCards = iCardsCount;
	}
	if (thisBoard == pDownDdzBoard)
	{
		pUpDdzBoard->iUpPlayerCards = iCardsCount;
		pMyDdzBoard->iDownPlayerCards = iCardsCount;
	}

}




void MonteCarloSearchEngine::UpdateUCB1(MTCNode nodes[], int node, int Winner, int iTotalPlay)
{

	nodes[node].iPlay++;
	if (Winner == 1)
		nodes[node].iWin++;
	if (iTotalPlay == 0)
	{
		nodes[node].fUCB1 = 0;
		return;
	}

	nodes[node].fAverage = (float)nodes[node].iWin / (float)nodes[node].iPlay;

	//更新所有节点的FUCB1
	for (int i = 0; i < iNodesCount; i++)
	{
		if (nodes[i].iPlay == 0)
			break;
		float fUCB1 = 2 * log(iTotalPlay);
		fUCB1 = fUCB1 / nodes[i].iPlay;
		fUCB1 = sqrtf(fUCB1);
		nodes[i].fUCB1 = nodes[i].fAverage + fUCB1;
		///w/n+   根号 (2* log(iTotalPlay))/iplay
	}
}



int MonteCarloSearchEngine::MakeMove(int iDepth)
{
	int iCount = 0;
	fDdz *thisBoard = pMyDdzBoard;
	switch (iDepth % 3)
	{
	case 1:thisBoard = pDownDdzBoard;
		break;
	case 2:thisBoard = pUpDdzBoard;
		break;
	}
	
	//pRecomCards->InitPlaGenerate(thisBoard);

	iCount = pRecomCards->HelpPla(thisBoard,1);

	//iDepthPos[iDepth + 1] = iDepthPos[iDepth] + iCount;
	//AddDepthNodesInList(iDepth);
	return iCount;

}



//UNDONE
void MonteCarloSearchEngine::GetOtherPlayerCards(int* iUpPlayerCards, int* iDownPlayerCards)
{
	//int iOnUpHand[21] = { 1, 2, 3, 9, 11, 12, 14, 16, 19, 20, 24, 25, 32, 41, 44, 52, 53, -1 };
	//memcpy(pUpDdzBoard->iOnHand, iOnUpHand, sizeof(iOnUpHand));
	//int iOnDownHand[21] = { 6, 7, 10, 15, 21, 27, 31, 33, 34, 35, 37, 38, 47, 48, 49, 50, 51, -1 };
	//memcpy(pDownDdzBoard->iOnHand, iOnDownHand, sizeof(iOnDownHand));
	
	for (int i = 0; i < 21; i++)
	{
		iUpPlayerCards[i] = -1;
		iDownPlayerCards[i] = -1;
	}

	int count = pRealDdz->iUpPlayerCards + pRealDdz->iDownPlayerCards;

	std::vector<int> vi(count); // 定义一个大小为N的vector
	//将其他玩家手中牌放入一个数组

	for (int i = 0; i<count; ++i)
	{
		vi[i] = pRealDdz->iOnOtherHand[i];
	}

	//随机种子
	//srand((unsigned)time(NULL));
	//将整个数组随机排序
	std::random_shuffle(vi.begin(), vi.end());  ////STL中的函数random_shuffle()用来对一个元素序列进行重新随机排序

	int i = 0;

	//生成己方上一手玩家的随机牌
	for (vector<int>::iterator it = vi.begin(); it != vi.begin() + pRealDdz->iUpPlayerCards; it++)
	{

		iUpPlayerCards[i] = *it;
		i++;
	}

	iUpPlayerCards[i] = -1;

	i = 0;
	pCinfo->SortById(iUpPlayerCards);

	//生成另一个非己方玩家的随机牌
	i = 0;
	for (vector<int>::iterator it = vi.begin() + pRealDdz->iUpPlayerCards; it != vi.end(); it++)
	{
		iDownPlayerCards[i] = *it;
		i++;
	}

	iDownPlayerCards[i] = -1;
	pCinfo->SortById(iDownPlayerCards);
}

//判断牌局是否结束,并返回极值
int MonteCarloSearchEngine::IsGameOver()
{
	//我方牌出尽
	if (pMyDdzBoard->iOnHand[0] == -1)
	{
		AIWin++;
		return 1;
	}

	else if (pUpDdzBoard->iOnHand[0] == -1)
	{
		UpWin++;
		//如果是地主，地主赢（AI农民输）
		if (pUpDdzBoard->cDir == pUpDdzBoard->cLandlord)
			return 0;
		//如果不是地主，且AI是地主（AI地主输）
		else if (pMyDdzBoard->cDir == pMyDdzBoard->cLandlord)
			return 0;
		//如果不是地主,且AI不是地主（AI农民赢）
		else if (pMyDdzBoard->cDir != pMyDdzBoard->cLandlord)
			return 1;
	}
	else if (pDownDdzBoard->iOnHand[0] == -1)
	{
		DownWin ++;
		//如果是地主，地主赢
		if (pDownDdzBoard->cDir == pDownDdzBoard->cLandlord)
			return 0;
		//如果不是地主，且AI是地主
		else if (pMyDdzBoard->cDir == pMyDdzBoard->cLandlord)
			return 0;
		//如果不是地主,且AI不是地主
		else if (pMyDdzBoard->cDir != pMyDdzBoard->cLandlord)
			return 1;
	}

	return -1;
}



void MonteCarloSearchEngine::InitBoard()
{
}


// 左开右闭
int MonteCarloSearchEngine::GetRandomNumFromRange(int iStart, int iEnd,int iDepth)
{
	std::vector<int> nodes(iEnd); // 定义一个大小为N的vector
	for (int i = 0; i < iEnd; i++)
	{
		nodes[i] = iStart;
		iStart++;
	}

	std::random_shuffle(nodes.begin(), nodes.end());//迭代器

	int i;
	if (iDepth==0)
	{
		nodeSeed++;
		if (nodeSeed >= iEnd)
			nodeSeed = 0;
			i = nodeSeed;
	}
	else
	{
		 i = nodes[iEnd - 1];
	}

	int thisNode = nodes[i];
	return thisNode;
}
