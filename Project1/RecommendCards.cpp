//江文金 2018.7.27
#include "RecommendCards.h"

//Test
void printCards(int*temp);

//出牌生成器初始化
//优化了单牌牌型判断，增加了出炸弹的情况
//添加专为模拟出牌使用的出牌函数
int RecommendCards::InitPlaGenerate(fDdz* pDdz)
{
	int i, j;
	for (i = 0; i<PlaMax; i++)		//清空推荐出牌数组
		for (j = 0; j<21; j++)
			pDdz->iPlaArr[i][j] = -1;

	iPlaCount = 0;
	isSim = false;

	iHandNum = MinHandNum(pDdz->iOnHand);
	iUpHandNum = MinHandNum(iUpCards);     
	iDownHandNum = MinHandNum(iDownCards);
	iLastMaxPoint = CInfo->AnalyzeMainMaxPoint(pDdz->iLastOnTable);//-1是为两家PASS 
	playerType = GetPlayerType(pDdz);

	return 0;
}


//帮助AI根据当前牌局生成当前推荐出牌
//最后修订者:江文金,最后修订时间:2018-07-26
int RecommendCards::HelpPla(struct fDdz * pDdz, bool IsSim)
{
	int iPass[3] = { -2, -1, -1 };
	InitPlaGenerate(pDdz);
	isSim = IsSim;             
	//iFShowInConsole=false
	if (iFShowInConsole)               
	{
		if (!IsSim) 
		{
			//Test
			//cout << "split card result" << endl;

			int **result = pSplitCards->startSplitCard(pDdz->iOnHand);
			//cout << " Rocket ";
			for (int j = 0; j < 8; j++)
			{
				int flag = 0;
				if (result[j][0] != -1)
				{
					if (j == 0)cout << " Rocket ";
					if (j == 1)cout << " Bomb ";
					if (j == 2)cout << " Three ";
					if (j == 3)cout << " LinkThree ";
					if (j == 4)cout << " LinkSingle ";
					if (j == 5)cout << " LinkDouble ";
					if (j == 6)cout << " Double ";
					if (j == 7)cout << " Single ";
				}
				
				for (int i = 0; result[j][i] != -1; i++)
				{
					if (result[j][i] != -2)
					{
						cout << result[j][i] / 4 + 3 << " ";
					}
					else
					{
						cout << " , ";
					}
					flag++;
				}
				if(flag)
				cout << endl;
			}
			cout << endl;
		}

		if (!IsSim)
		{
			cout << endl << "当前AI剩下手数 " << iHandNum << endl;
		}

		//Test
		if (!isSim&&judgeFriend(pDdz)) {
			cout << "It is my friend's cards" << endl;
		}
	}


	//按牌编码升序排列
	CInfo->SortById(pDdz->iOnHand);

	//计算AI手牌数
	int iCardcount = CInfo->CountCards(pDdz->iOnHand);
	
	//找到其他玩家的最大牌点
	int iMaxInother = 0;
	for (int i = 0; pRealDdz->iOnOtherHand[i] != -1; i++)
	{
		if (pRealDdz->iOnOtherHand[i]/4 > iMaxInother)
		{
			iMaxInother = pRealDdz->iOnOtherHand[i]/4;
		}
	}

	int iMayOnOtherBomb = 0;  //其他玩家可能的炸弹和火箭数
	int iMayOnOtherTwo = 0;   //其他玩家2的数量，未引用
	int iRepeat[15] = { 0 };  //各牌重复数
	iRepeat[14] = -1;

	//计算出在其他玩家手牌各类牌的点数和张数
	for (int i = 0; pRealDdz->iOnOtherHand[i] != -1; i++)
	{
		iRepeat[pRealDdz->iOnOtherHand[i] / 4]++;
	}

	//其他玩家可能存在炸弹数
	for (int i = 0; i < 14; i++)
	{
		if (iRepeat[i] == 4)
			iMayOnOtherBomb++;
	}
	//其他玩家可能存在火箭
	if (iRepeat[13] == 2)
	{
		iMayOnOtherBomb++;
	}

	int iTmp[20];                    //下一手，要出的牌
	memset(iTmp, -1, sizeof(iTmp));  //初始化
									 //之后是用来存储剩下的手牌的，-2收尾，之后为-1
									 //即一套分好牌型的牌
	int iKing[3] = { -1,-1,-1 };      //王
	int iKingIndex = 0;
	int iTwo[5] = { -1,-1,-1,-1,-1 };      //2
	int iTwoIndex = 0;

	//手中牌“王”和“2”的数量及其索引
	for (int i = 0; pDdz->iOnHand[i] != -1; i++)
	{
		if (pDdz->iOnHand[i] / 4 == 13)
		{
			iKing[iKingIndex++] = pDdz->iOnHand[i];
		}
		else if (pDdz->iOnHand[i] / 4 == 12)
		{
			iTwo[iTwoIndex++] = pDdz->iOnHand[i];
		}
	}

	//是否前面两家pass              //两家pass，返回true,否则返回false
	if (IfPassTwice(pDdz))
	{
		if (iHandNum == 1)          //手数，手数和手牌数有区别，比如剩下一对 手数是一手牌数是二
		{
			int i;
			for (i = 0; pDdz->iOnHand[i]!=-1; i++)
			{
				iTmp[i] = pDdz->iOnHand[i] != -1;   //不等于-1返回1，等于-1返回0
			}
			iTmp[i++] = -2;
			iTmp[i++] = -1;
			if(GetLegalCards(pDdz, iTmp))			//传入单个套牌信息并判断是否合法，如果合法，加入iplaarr
				return iPlaCount;
		}

		
		//只剩下一张手牌时,直接出
		if (iCardcount == 1)
		{
			iTmp[0] = pDdz->iOnHand[0];
			iTmp[1] = -2;
			iTmp[2] = -1;
			AddInPlaArr(pDdz, iTmp);	
			return iPlaCount;             
		}
		//只剩下两张手牌时
		else if (iCardcount == 2)
		{
			if (pDdz->iOnHand[0]/4 == pDdz->iOnHand[1]/4)//如果是对子（包括王炸）
			{
				iTmp[0] = pDdz->iOnHand[0];
				iTmp[1] = pDdz->iOnHand[1];
				iTmp[2] = -2;
				iTmp[3] = -1;
				AddInPlaArr(pDdz, iTmp);
				return iPlaCount;
			}
			if (pDdz->iOnHand[1] / 4 >= iMaxInother &&iMayOnOtherBomb == 0)//其他玩家无炸弹，且自己的牌点大于等于其他玩家
			{
				iTmp[0] = pDdz->iOnHand[1];
				iTmp[1] = -2;
				iTmp[2] = -1;
				AddInPlaArr(pDdz, iTmp);
				return iPlaCount;
			}

			if (playerType == 2 && pDdz->iDownPlayerCards == 1)//如果下家地主报单，出大的
			{
				iTmp[0] = pDdz->iOnHand[1];
				iTmp[1] = -2;
				iTmp[2] = -1;
				AddInPlaArr(pDdz, iTmp);
				return iPlaCount;
			}

			//否则出小的
			iTmp[0] = pDdz->iOnHand[0];
			iTmp[1] = -2;
			iTmp[2] = -1;
			AddInPlaArr(pDdz, iTmp);
			return iPlaCount;

		}
		//只剩下三张牌
		else if (iCardcount == 3)
		{
			if (pDdz->iOnHand[0] == pDdz->iOnHand[1] == pDdz->iOnHand[2])//有一个三条AAA型，直接出
			{
				iTmp[0] = pDdz->iOnHand[0];
				iTmp[1] = pDdz->iOnHand[1];
				iTmp[2] = pDdz->iOnHand[2];
				iTmp[3] = -2;
				iTmp[4] = -1;
				AddInPlaArr(pDdz, iTmp);
				return iPlaCount;
			}
			if (iKingIndex == 2)//有王炸，先出万炸
			{
				iTmp[0] = iKing[0];
				iTmp[1] = iKing[1];
				iTmp[2] = -2;
				iTmp[3] = -1;
				AddInPlaArr(pDdz, iTmp);
				return iPlaCount;
			}
			if (pDdz->iOnHand[0] == pDdz->iOnHand[1] && iMayOnOtherBomb == 0)//有一个对子AAB型，且其他人没有炸弹
			{
				if (pDdz->iOnHand[0]/4 >= iMaxInother)
				{
					iTmp[0] = pDdz->iOnHand[0];
					iTmp[1] = pDdz->iOnHand[1];
					iTmp[2] = -2;
					iTmp[3] = -1;
					AddInPlaArr(pDdz, iTmp);
					return iPlaCount;
				}
				if (pDdz->iOnHand[2]/4 >= iMaxInother)
				{
					iTmp[0] = pDdz->iOnHand[2];
					iTmp[1] = -2;
					iTmp[2] = -1;
					AddInPlaArr(pDdz, iTmp);
					return iPlaCount;
				}
			}
			if (pDdz->iOnHand[2] == pDdz->iOnHand[1] && iMayOnOtherBomb == 0)//有一个对子ABB型，且其他人没有炸弹
			{
				if (pDdz->iOnHand[1]/4 >= iMaxInother)//对子大
				{
					iTmp[0] = pDdz->iOnHand[0];
					iTmp[1] = pDdz->iOnHand[1];
					iTmp[2] = -2;
					iTmp[3] = -1;
					AddInPlaArr(pDdz, iTmp);
					return iPlaCount;
				}
				if (pDdz->iOnHand[0]/4 >= iMaxInother)//单牌大
				{
					iTmp[0] = pDdz->iOnHand[0];
					iTmp[1] = -2;
					iTmp[2] = -1;
					AddInPlaArr(pDdz, iTmp);
					return iPlaCount;
				}
			}
			//ABC型的牌，手数为3，应该不是在这里考虑的
		}
		//当手数等于2时
		if (iHandNum == 2)
		{
			//如果有火箭
			int *iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 0);//如果有王炸
			if (iCards[0] !=-1)
			{
				if (GetLegalCards(pDdz, iCards))
					return iPlaCount;
			}

			iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 6);//拆对子 0 1 -2 3 4 -2 5 6 
			if (iCards[0] != -1 && iCards[3] == -1)//只有一个对子
			{
				if (iCards[0]/4 >= iMaxInother && iMayOnOtherBomb==0)//AI的对子最大
				{
					if (GetLegalCards(pDdz, iCards))
						return iPlaCount;
				}
			} 

			iCards= pSplitCards->startSplitCard(pDdz->iOnHand, 2);//拆三条
			int iThreeCount = 0;         //三条总数
			int iSingleCount = 0;        //单牌总数

			for (int i = 0; iCards[i]!=-1; i++)
			{
				if (iCards[4] == -1)//0 1 2 -2 4 5 6 -2 8   //只有一个三条
				{
					if (iCards[0]/4 >= iMaxInother && iMayOnOtherBomb == 0)//AI的三条最大  
					{
						iTmp[0] = iCards[0];
						iTmp[1] = iCards[1];
						iTmp[2] = iCards[2];

						//找到那张单牌或者对子。。。
						iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 7);//拆单牌 0 -2 2 -2 4
						if (iCards[0] != -1)
						{
							iTmp[3] = iCards[0];
							iTmp[4] = -2;
							iTmp[5] = -1;
							if (GetLegalCards(pDdz, iTmp))				   //传入单个套牌信息并判断是否合法，如果合法，加入iplaarr
								return iPlaCount;
						}
						iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 6);//拆对子 0 1 -2 3 4 -2 6 7
						if (iCards[0] != -1&& iCards[3] ==-1 )
						{
							iTmp[3] = iCards[0];
							iTmp[4] = iCards[1];
							iTmp[5] = -2;
							iTmp[6] = -1;
							if (GetLegalCards(pDdz, iTmp))
								return iPlaCount;
						}

						if (GetLegalCards(pDdz, iTmp))		
							return iPlaCount;
					}
				}
				else if((iCards[4] != -1))	//不止一个三条
				{  
					//0 1 2 -2 4 5 6 -2 7 8 9 -2
					if (iCards[7] == -1)//两个的三顺
					{
						iTmp[0] = iCards[0];
						iTmp[1] = iCards[1];
						iTmp[2] = iCards[2];
						iTmp[3] = iCards[4];
						iTmp[4] = iCards[5];
						iTmp[5] = iCards[6];

						//找到那两张单牌或者对子。。。
						iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 7);//拆单牌

						if (iCards[2] != -1)//0 -2 2 -2 3 -2 -1
						{
							iTmp[6] = iCards[0];
							iTmp[7] = iCards[2];
							iTmp[8] = -2;
							iTmp[9] = -1;
							if (GetLegalCards(pDdz, iTmp))     
								return iPlaCount;
						}

						iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 6);//拆对子
						if (iCards[3] != -1)//0 1 -2 3 4 -2 4 5 -2
						{
							iTmp[6] = iCards[0];
							iTmp[7] = iCards[1];
							iTmp[8] = iCards[3];
							iTmp[9] = iCards[4];
							iTmp[10] = -2;
							iTmp[11] = -1;
							if (GetLegalCards(pDdz, iTmp))
								return iPlaCount;
						}

						if (GetLegalCards(pDdz, iTmp))
							return iPlaCount;
					}
				}
			
				if (iCards[i] == -2)iThreeCount++;
			}

			iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 7);//拆单牌  0 -2 2 -2 4
			for (int i = 0; iCards[i] != -1; i++)
			{
				if (iCards[i] == -2)iSingleCount++;
			}
			
			if (iThreeCount == 0)//没有三条，即单牌是单独的一手牌
			{
				if (iCards[0] != -1 && iCards[2] == -1 ) 
				{
					if (iCards[0]/4 >= iMaxInother && iMayOnOtherBomb == 0)//AI的单牌最大  
					{
						if (GetLegalCards(pDdz, iCards))
							return iPlaCount;
					}
				}
			}

			iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 4);//拆单顺
			for (int i = 0; iCards[i]!=-1; i++)
			{
				if (iCards[i]==-2)
				{
					if (iCards[i - 1] / 4 >= iMaxInother)
					{
						if (GetLegalCards(pDdz, iCards))
							return iPlaCount;
					}
				}
			}
		}
		
		//队友下家报单，出最小单牌,且之前没帮助过队友（防止队友打不起我们的最小单牌）
		if (playerType == 3 && pDdz->iDownPlayerCards == 1&& !pDdz->haveHelpFriend)
		{
			CInfo->SortById(pDdz->iOnHand);
			iTmp[0] = pDdz->iOnHand[0];				//找出最小的牌
			iTmp[1] = -2;
			iTmp[2] = -1;
			AddInPlaArr(pDdz, iTmp);
			pDdz->haveHelpFriend = true;		//要是之前帮了队友，队友手上都还有牌，说明队友打不起我们的最小单牌
			return iPlaCount;
		}
		else 
		{
			//不需要帮助队友
			Help0Pass(pDdz);
		}
	}
	else//跟牌
	{
		if (iHandNum == 2)//剩下最后两手牌时
		{
			//如果有火箭
			int *iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 0);
			if (iCards[0] != 1)
			{
				if (GetLegalCards(pDdz, iCards))
					return iPlaCount;
			}
		}
		else if (iHandNum == 3)
		{
			//如果有火箭
			int *iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 0);
			if (iCards[0] != 1)
			{
				int iTwo=0;
				for (int i = 0; pDdz->iOnHand[i]!=-1;i++)
				{
					if(pDdz->iOnHand[i]/4==12)
						iTwo++;
				}
				if (iTwo != 0)   //如果手上有“2”，打出“火箭”
				{
					if (GetLegalCards(pDdz, iCards))
						return iPlaCount;
				}
			}
		}
		int iTypeCount = pDdz->iLastTypeCount;
		/*if (301 == iTypeCount)//单张
		{
			if (IsSim||(playerType==2&&pDdz->iDownPlayerCards<=3)|| (playerType == 3 && pDdz->iUpPlayerCards <= 3)|| (playerType == 1 && (pDdz->iUpPlayerCards <= 3|| pDdz->iDownPlayerCards <= 3)))
				Help3SingleSim(pDdz);
			else
				Help3Single(pDdz);
		}
		else if (402 == iTypeCount)
		{
			if (IsSim || (playerType == 2 && pDdz->iDownPlayerCards <=3) || (playerType == 3 && pDdz->iUpPlayerCards <=3)|| (playerType == 1 && (pDdz->iUpPlayerCards <= 3 || pDdz->iDownPlayerCards <= 3)))
				Help4DoubleSim(pDdz);
			else
				Help4Double(pDdz);
		}
		else if (503 == iTypeCount)
			Help5Three(pDdz);
		else if (604 == iTypeCount)
		{
			if (isSim || (playerType == 2 && pDdz->iDownPlayerCards <= 3) || (playerType == 3 && pDdz->iUpPlayerCards <= 3) || (playerType == 1 && (pDdz->iUpPlayerCards <= 3 || pDdz->iDownPlayerCards <= 3)))
				Help6ThreeOneSim(pDdz);
			else
				Help6ThreeOne(pDdz);
		}
		else if (605 == iTypeCount)
		{
			if (isSim || (playerType == 2 && pDdz->iDownPlayerCards <= 3) || (playerType == 3 && pDdz->iUpPlayerCards <= 3) || (playerType == 1 && (pDdz->iUpPlayerCards <= 3 || pDdz->iDownPlayerCards <= 3)))
				Help6ThreeDoubleSim(pDdz);
			else
				Help6ThreeDouble(pDdz);
		}
		*/

		int iTemp = 0;
		//1**为火箭，2**为炸弹，3**为单牌，4**为对牌，5**为三条，6**为三带一单或一对
		//11**为四带一单或一对，7**、8**、9**分别为单顺、双顺、三顺，10**为飞机。
		//*为0-9的任意数，具体信息在CardsInfo.cpp的函数AnalyzeTypeCount那里
		if (301 == iTypeCount)//单张
		{
			if (IsSim)
				Help3SingleSim(pDdz);
			else
			{
				iTemp = iPlaCount;
				Help3Single(pDdz);
				if (iTemp == iPlaCount)
				{
					if ((playerType == 2 && pDdz->iDownPlayerCards <= 3) 
						|| (playerType == 3 && pDdz->iUpPlayerCards <= 3) 
						|| (playerType == 1 && (pDdz->iUpPlayerCards <= 3 || pDdz->iDownPlayerCards <= 3)))
					{
						Help3SingleSim(pDdz);
					}
				}
			}
				
		}
		else if (402 == iTypeCount)//对牌
		{
			if (IsSim)
				Help4DoubleSim(pDdz);
			else
			{
				iTemp = iPlaCount;
				Help4Double(pDdz);
				if (iTemp == iPlaCount)
				{
					if ((playerType == 2 && pDdz->iDownPlayerCards <= 3) || (playerType == 3 && pDdz->iUpPlayerCards <= 3) || (playerType == 1 && (pDdz->iUpPlayerCards <= 3 || pDdz->iDownPlayerCards <= 3)))
					{
						Help4DoubleSim(pDdz);
					}
				}
			}
			/*else
				Help4Double(pDdz);*/
		}
		else if (503 == iTypeCount)
			Help5Three(pDdz);
		else if (604 == iTypeCount)
		{
			if (isSim )
				Help6ThreeOneSim(pDdz);
			else
			{
				iTemp = iPlaCount;
				Help6ThreeOne(pDdz);
				if (iTemp == iPlaCount)
				{
					if ((playerType == 2 && pDdz->iDownPlayerCards <= 3) || (playerType == 3 && pDdz->iUpPlayerCards <= 3) || (playerType == 1 && (pDdz->iUpPlayerCards <= 3 || pDdz->iDownPlayerCards <= 3)))
					{
						Help6ThreeOneSim(pDdz);
					}
				}
			}
			/*else
				Help6ThreeOne(pDdz);*/
		}
		else if (605 == iTypeCount)
		{
			if (isSim)
				Help6ThreeDoubleSim(pDdz);
			else
			{
				iTemp = iPlaCount;
				Help6ThreeDouble(pDdz);
				if (iTemp == iPlaCount)
				{
					if ((playerType == 2 && pDdz->iDownPlayerCards <= 3) || (playerType == 3 && pDdz->iUpPlayerCards <= 3) || (playerType == 1 && (pDdz->iUpPlayerCards <= 3 || pDdz->iDownPlayerCards <= 3)))
					{
						Help6ThreeDoubleSim(pDdz);
					}
				}
			}
			/*else
				Help6ThreeDouble(pDdz);*/
		}
		else if (iTypeCount > 700 && iTypeCount < 800)
			Help7LinkSingle(pDdz);
		else if (iTypeCount > 800 && iTypeCount < 900)
			Help8LinkDouble(pDdz);
		else if (iTypeCount > 900 && iTypeCount < 1000)
			Help9LinkThree(pDdz);
		else if (iTypeCount > 1000 && iTypeCount < 1100)
		{
			if ((iTypeCount - 1000) % 4 == 0)
				Help10LinkThreeSingle(pDdz);
			else
				Help10LinkThreeDouble(pDdz);
		}
		else if (1106 == iTypeCount)
			Help11FourSingle(pDdz);
		else if (1108 == iTypeCount)
			Help11FourDouble(pDdz);


		//如果无牌可出，出炸弹
		if (iPlaCount == 0)
		{
			if (iHandNum <= 4)
				Help2Bomb(pDdz);
		}

		//如果无牌可出，出火箭
		if (iPlaCount == 0)
		{
			Help1Rocket(pDdz);
		}
			
		if (iPlaCount == 0)		
		{
			if (pDdz->iPlaArr[iPlaCount][0] != -2)
			{
				AddInPlaArr(pDdz, iPass);
				return iPlaCount;
			}
		}
	}
	return iPlaCount;
}

RecommendCards::RecommendCards() : CInfo(CardsInfo::Instance())
{
	pSplitCards = new SplitCard();
	pMoldel = new PokerModelAnalysis();
}

RecommendCards::~RecommendCards()
{
	delete(pSplitCards);
	delete(pMoldel);
}

RecommendCards* RecommendCards::Instance()
{
	static RecommendCards instance;

	return &instance;
}

//重新出牌，从iOnHand中查询牌型到iPlaArr
//最后修订者:成文杰,最后修订时间:2017-07-23
//playerType 1AI为地主 2下家为地主 3上家为地主
int RecommendCards::Help0Pass(fDdz * pDdz)
{
	////判断是否首出//   //首次出牌
	int cardsCount = CInfo->CountCards(pDdz->iOnHand);		//计算手牌数
	int iTmp;
	//如果首出
	if ((cardsCount == 17 && playerType != 1) || cardsCount == 20)
	{
		if (isSim)       //isSim = 1无规则应对
		{
			Help3SingleSim(pDdz);//查询所有不同点数单张牌型到iOnHand里面3 4 5 5 -1 -1|3 -2 4 -2 5 -2 -1
			Help4DoubleSim(pDdz);//无规则出对子3 3 4 4 4 -1 -1|3 3 -2 4 4 -2 -1
			Help6ThreeOneSim(pDdz);//无规则三带一单
			Help6ThreeDoubleSim(pDdz);//无规则三带二
			Help5Three(pDdz);//H05-START推荐出牌应对三条
		}
		else if (!isSim)	//isSim = 0
		{
			if (CInfo->CountCards(pSplitCards->startSplitCard(pDdz->iOnHand, 7)) > 4)//首出，大于四张单牌才出单
			{
				Help3Single(pDdz);
			}

			Help4Double(pDdz);

			iTmp= iPlaCount;
			Help6ThreeOne(pDdz);
			Help6ThreeDouble(pDdz);

			if (iTmp == iPlaCount)//没有三带一或者三带二，才出三张
			{
				Help5Three(pDdz);
			}
		}

		Help7LinkSingle(pDdz);
		Help8LinkDouble(pDdz);

		iTmp = iPlaCount;
		Help10LinkThreeSingle(pDdz);
		Help10LinkThreeDouble(pDdz);

		if (iTmp == iPlaCount)//没有三顺带单或者对
		{
			 Help9LinkThree(pDdz);
		}
	}
	else//非首出
	{
		if (isSim)
		{
			Help6ThreeOneSim(pDdz);
			Help6ThreeDoubleSim(pDdz);
			Help5Three(pDdz);
		}
		else if (!isSim)
		{
			iTmp = iPlaCount;
			Help6ThreeOne(pDdz);
			Help6ThreeDouble(pDdz);
			if (iTmp == iPlaCount)
			{
				Help5Three(pDdz);
			}
		}
		
		Help7LinkSingle(pDdz);
		Help8LinkDouble(pDdz);

		iTmp = iPlaCount;
		Help10LinkThreeSingle(pDdz);
		Help10LinkThreeDouble(pDdz);
		if (iTmp == iPlaCount)
		{
			Help9LinkThree(pDdz);
		}


		Help11FourSingle(pDdz);
		Help11FourDouble(pDdz);

		//AI是地主，没人报单时才出单牌，没人报双，才出对子
		if (playerType == 1)
		{
			if (pDdz->iUpPlayerCards >1&& pDdz->iDownPlayerCards>1)
			{
				if (isSim)
					Help3SingleSim(pDdz);
				else
					Help3Single(pDdz);
			}
			if (pDdz->iUpPlayerCards != 2&& pDdz->iDownPlayerCards!=2)
			{
				if (isSim)
					Help4DoubleSim(pDdz);
				else
					Help4Double(pDdz);
			}
		}
		else//AI不是地主     
		{
			if (playerType == 2)//下家是地主
			{
				if (pDdz->iDownPlayerCards > 1)
				{
					if (isSim)
						Help3SingleSim(pDdz);
					else
						Help3Single(pDdz);
				}
				if (pDdz->iDownPlayerCards != 2)
				{
					if (isSim)
						Help4DoubleSim(pDdz);
					else
						Help4Double(pDdz);
				}
			}
			else if (playerType == 3)//上家是地主
			{
				if (pDdz->iUpPlayerCards > 1)
				{
					if (isSim)
						Help3SingleSim(pDdz);
					else
						Help3Single(pDdz);
				}
				if (pDdz->iUpPlayerCards != 2)
				{
					if (isSim)
						Help4DoubleSim(pDdz);
					else
						Help4Double(pDdz);
				}
			}
		}

		if (iPlaCount == 0 && cardsCount==4)
			Help2Bomb(pDdz);
		if (iPlaCount == 0&&cardsCount==2)
			Help1Rocket(pDdz);
	}
	//如果还是没牌出？
	if (iPlaCount == 0)
	{
		AddInPlaArr(pDdz, pSplitCards->startSplitCard(pDdz->iOnHand, 6));//对子

		Help1Rocket(pDdz);//火箭

		AddInPlaArr(pDdz, pSplitCards->startSplitCard(pDdz->iOnHand,7));//单牌

		return iPlaCount;
	}
	if (iPlaCount == 0)//防止还是无牌可出
	{
		int iTmp[3] = { -2,-2,-1 };
		iTmp[0] = pDdz->iOnHand[0];
		GetLegalCards(pDdz, iTmp);
	}

	return iPlaCount;
}
//H00-END


//H01-START从iOnHand中查询组合火箭牌型到iPlaArr
//最后修订者:成文杰，最后修订时间:2017-07-19
//添加出火箭策略
int RecommendCards::Help1Rocket(struct fDdz * pDdz)
{
	int iCount = iPlaCount;//出牌解
	int *iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 0);
	if (iCards[0] > -1)
	{
		int Cards = CInfo->CountCards(pDdz->iOnHand);
		if (Cards == 2 || iHandNum <= 2)//自己能赢时，出。
			AddInPlaArr(pDdz, iCards);

		//如果自己不是地主，地主手数少的话。。并且不是队友出的牌的话，炸他
		else if (!judgeFriend(pDdz) && ((playerType == 2 && iDownHandNum <= 3) || (playerType == 3 && iUpHandNum <= 3)))
		{
			AddInPlaArr(pDdz, iCards);
		}
	}
	return iPlaCount-iCount;
}
//H01-END


//判断是否是队友出的牌  两种情况，上家是地主 且iLastPassCount==1，和 下家是地主 且iLastPassCount==0 ;
//这两种情况都是队友出的牌
//最后修订者:成文杰，最后修订时间:2017-07-19
//审核通过
bool RecommendCards::judgeFriend(fDdz* pDdz)
{
	//为了增加蒙特卡洛的随机性，这里不考虑在模拟下合作
	if (!isSim && ((playerType == 3 && pDdz->iLastPassCount == 1) || (playerType == 2 && pDdz->iLastPassCount == 0))) 
		return 1;
	else
		return 0;
}


//H02-START推荐出牌应对炸弹
//不炸队友
//最后修订者:成文杰，最后修订时间:2016-07-26
int RecommendCards::Help2Bomb(struct fDdz * pDdz)
{
	int *iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 1);

	int iCount = iPlaCount;//出牌解

	int Cards = CInfo->CountCards(pDdz->iOnHand);

	if (Cards == 4 && iCards[0] > -1)
		if (GetLegalCards(pDdz, iCards))
			return iPlaCount-iCount;   

	if (deleteTypeLeftSingleCount(pDdz->iOnHand, iCards) >= 4)   
		return 0; //拆了炸弹多了4张及以上的单牌，不考虑用这个炸弹。

	iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 1);

	if (judgeFriend(pDdz) && iHandNum > 2)return 0;	//如果是队友,且我们的牌大于两手，不炸

	//如果地主可能有火箭，且地主赢的几率很大时，不出炸弹
	int iKingCount = 0;
	for (int i = 0; pDdz->iOnHand[i] != -1; i++)
	{
		if (pDdz->iOnHand[i] == 52 || pDdz->iOnHand[i] == 53)iKingCount++;
	}

	if (iKingCount == 2)
	{
		//如果上家为地主且上手玩家手牌数小于4
		if (playerType == 3 && pDdz->iUpPlayerCards < 4)
			return 0;
		//如果下家为地主且下手玩家手牌数小于4
		if (playerType == 2 && pDdz->iDownPlayerCards <4)
			return 0;
	}

	if (!isSim)
	{
		if (pDdz->cDir == pDdz->cLandlord)//当自己是地主时
		{
			/*if(Cards>=17|| pDdz->iUpPlayerCards > 10 || pDdz->iDownPlayerCards > 10)
			return 0;	*/
			if (Cards >= 10 || (pDdz->iUpPlayerCards > 10 && pDdz->iDownPlayerCards > 10))
				return 0;
		}
		else if (playerType == 3 && pDdz->iUpPlayerCards > 10)//当其他玩家是地主时，要求地主的手牌小于等于10才出炸弹
			return 0;
		else if (playerType == 2 && pDdz->iDownPlayerCards > 10)
			return 0;
	}

	int iSave[21];
	memset(iSave, -1, sizeof(iSave));
	if (GetLegalCards(pDdz, iCards, iSave))
	{
		AddInPlaArr(pDdz, iSave);
		return iPlaCount-iCount;
	}

	//如果是跟牌,且不是队友的牌,无炸弹，检查是否有火箭
	if (!IfPassTwice(pDdz)&&!judgeFriend(pDdz))
	{
		Help1Rocket(pDdz);
	}
	return 0;
}
//H02-END

/*
//H03-START从iOnHand中查询单张牌型到iPlaArr
//最后修订者:成文杰,最后修订时间:17-07-20
int RecommendCards::Help3Single(struct fDdz * pDdz)
{
	int iCopyOnHand[21];
	int iCount = iPlaCount;//出牌解
	int iTmpCards[3];
	int iHandCards = CInfo->CountCards(pDdz->iOnHand);
	
	//如果只剩一张牌，且合法，直接出  
	if (pDdz->iOnHand[0] != -1 && pDdz->iOnHand[1] == -1)
	{
		iTmpCards[0] = pDdz->iOnHand[0];
		iTmpCards[1] = -2;
		iTmpCards[2] = -1;
		if (GetLegalCards(pDdz, iTmpCards))
			return iPlaCount - iCount;
	}

	memcpy(iCopyOnHand, pDdz->iOnHand, sizeof(iCopyOnHand));

	int*iCards= pSplitCards->startSplitCard(pDdz->iOnHand, 7);

	int iSingleCount = 0;
	int iThreeCount = 0;
	for (int i = 0; iCards[i]!=-1; i++)
	{
		if (iCards[i] == -2)iSingleCount++;
	}

	iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 2);
	for (int i = 0; iCards[i] != -1; i++)
	{
		if (iCards[i] == -2)iThreeCount++;
	}
	if (iLastMaxPoint <= -1 && iSingleCount <= iThreeCount) //自己出牌且三条数大于单张牌数
		return 0;

	iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 7);
	if ((playerType == 2 && pDdz->iDownPlayerCards <=2)|| (playerType == 3 && pDdz->iUpPlayerCards <= 2))
	{//如果下家或者上家地主报单或者报双，出最大的单牌
		int i;
		for (i = 0; iCards[i] != -1; i++);
		//0 1  2  3 4 5  6
		//1 -2 4 -2 5 -2 -1  
		int iTmp[3]= { -1,-2,-1 };
		if (i >= 2)
		{
			//if ((iCards[i - 2] == 52 || iCards[i - 2] == 53 )&& )i = i - 2;
			iTmp[0] = iCards[i - 2];
			if (GetLegalCards(pDdz, iTmp))
			{
				return iPlaCount - iCount;
			}
		}
	}
	
	if (playerType == 1)//AI是地主，跟牌或者出牌 都出最小单牌
	{
		int iTmp[3] = { -1,-2,-1 };
		if (iCards[0] >= 0)
		{
			iTmp[0] = iCards[0];
			if (GetLegalCards(pDdz, iTmp))
			{
				return iPlaCount - iCount;
			}
		}
	}

	int iTwo[5] = { -1,-1,-1,-1,-1 };//下标0 存放个数
	int iKing[3] = { -1,-1,-1 };//下标0 存放个数
	int iTwoIndex = 1;
	int iKingIndex = 1;
	for (int i = 0; iCopyOnHand[i] != -1; i++)
	{
		if (iCopyOnHand[i] / 4 == 12)
		{
			iTwo[iTwoIndex++] = iCopyOnHand[i];
		}
		else if (iCopyOnHand[i] == 52 || iCopyOnHand[i] == 53)
		{
			iKing[iKingIndex++] = iCopyOnHand[i];
		}
	}

	iTwo[0] = iTwoIndex - 1;
	iKing[0] = iKingIndex - 1;

	if (!judgeFriend(pDdz) && pDdz->cDir != pDdz->cLandlord)//跟地主的牌
	{
		if (iLastMaxPoint < 12)//如果地主出牌小于2，不考虑出王
		{
			for (int i = 0; iCopyOnHand[i] != -1; i++)
			{
				if (iCopyOnHand[i] == 52)
				{
					DeleteElementInCards(iCopyOnHand, 21, 52);
				}
				else if (iCopyOnHand[i] == 53)
				{
					DeleteElementInCards(iCopyOnHand, 21, 53);
				}
			}
		}
		else if (iLastMaxPoint == 12)//出的是2，如果地主手数多，不考虑用大王跟
		{
			if ((playerType == 2 && iDownHandNum > 5) || (playerType == 3 && iUpHandNum > 5))
			{
				for (int i = 0; iCopyOnHand[i] != -1; i++)
				{
					if (iCopyOnHand[i] == 53)
					{
						DeleteElementInCards(iCopyOnHand, 21, 53);
					}
				}
			}
		}
	}


	iCards = pSplitCards->startSplitCard(iCopyOnHand, 7);
	bool haveTwo = false;
	bool haveKing = false;
	for (int i = 0; iCards[i] != -1; i++)//判断里面是否有二和王
	{
		if (iCards[i] != -2)
		{
			if (iCards[i] / 4 == 12)
			{
				haveTwo = true;
				break;
			}
			else if (iCards[i] / 4 == 13)
			{
				haveKing = true;
				break;
			}
		}
	}

	if (iLastMaxPoint > -1)//跟牌
	{
		if (!haveTwo)
		{
			if (iHandNum < 6|| (playerType == 2 && pDdz->iDownPlayerCards<5))
			{//没有二， 手数小于6 或者下家是地主且下家手牌数小于5  查询是否有两个及以上的二，拆二
				if (iTwo[0] != 0)
				{
					int iTmp[2] = { -1,-1};
					iTmp[0] = iTwo[1];    
					AppendCards(iTmp, iCards);
					haveTwo = true;
				}
			}
		}
	}

	if (GetLegalCards(pDdz, iCards)) 
	{
		return iPlaCount - iCount;
	}

	if (iPlaCount == 0)
	{
		if (iLastMaxPoint < 0)//没有单牌时且自己出牌，不出单牌
			return 0;
		else//跟牌
		{
			int iTmp[3] = { -2,-2,-1 };

			if (judgeFriend(pDdz))return 0;//跟队友的牌

			//跟地主的牌
			else if (pDdz->cDir != pDdz->cLandlord)
			{
				int iSingle[3] = { -1,-2,-1 };
				//查找大于5张的单顺
				iCards = pSplitCards->startSplitCard(iCopyOnHand, 4);
				int iLinkSingleCount = 0;
				int i;
				int maxSingle = 0;

				//没有单牌时，取单顺牌数大于5的最大那一张单牌
				for (i = 0; iCards[i] != -1; i++)
				{
					if (iCards[i] == -2)// 0 1 2 3 4 5 6(-2)   7 8 9 10 11 12  13(-2)
					{
						if (iLinkSingleCount > 5)
						{
							if (iCards[i - 1] > maxSingle)
							{
								maxSingle = iCards[i - 1];
							}
						}
						iLinkSingleCount = 0;
					}
					else
						iLinkSingleCount++;
				}

				if (maxSingle != 0)
				{
					iSingle[0] = maxSingle;  
				}

				if (GetLegalCards(pDdz, iSingle))
				{
					return iPlaCount - iCount;
				}

				//如果单顺的单牌还是不行，或者没有
				
				//查找二
				if (haveTwo)return 0;
				else
				{
					if (iTwo[0] != 0)
					{
						iTmp[0] = iTwo[1];
						if (GetLegalCards(pDdz, iTmp))
						{
							return iPlaCount - iCount;
						}
						haveTwo = true;
					}
				}

				if ((playerType == 2 && iDownHandNum < 4) || (playerType == 3 && iUpHandNum < 4))
				{
					if (!haveTwo)
					{
						if (iTwo[0] != 0)
						{
							iTmp[0] = iTwo[1];
							if (GetLegalCards(pDdz, iTmp))
							{
								return iPlaCount - iCount;
							}
							haveTwo = true;
						}
					}
					else if (haveTwo && !haveKing&&iKing[0] != 0)
					{
						if (iKing[1] != -1 )
						{
							iTmp[0] = iKing[1];

							if (GetLegalCards(pDdz, iTmp))
							{
								return iPlaCount - iCount;
							}
							else
								return 0;
							haveKing = true;
						}
						else if (iKing[2] != -1)
						{
							iTmp[0] = iKing[2];
							if (GetLegalCards(pDdz, iTmp))
							{
								return iPlaCount - iCount;
							}
							else
								return 0;
						}
					}
				}
			}

			//AI是地主
			else if (pDdz->cDir == pDdz->cLandlord)
			{
				if (!haveTwo)
				{
					if (iTwo[0] != 0)
					{
						iTmp[0] = iTwo[1];
						if (GetLegalCards(pDdz, iTmp))
						{
							return iPlaCount - iCount;
						}
						haveTwo = true;
					}
				}
				if (!haveKing&&iKing[0] != -1)
				{
					if (iKing[1] != -1)
					{
						iTmp[0] = iKing[1];
						if (GetLegalCards(pDdz, iTmp))
						{
							return iPlaCount - iCount;
						}
						else
							return 0;
					}
					else if (iKing[2] != -1)
					{
						iTmp[0] = iKing[2];
						if (GetLegalCards(pDdz, iTmp))
						{
							return iPlaCount - iCount;
						}
						else
							return 0;
					}
				}
			}
		}
	}
	return 0;
}
//H03-END
*/

//H03-START从iOnHand中查询单张牌型到iPlaArr
//最后修订者:江文金,最后修订时间:2018-07-26
//修复了AI地主，上下家报单或双，AI跟最小单牌的问题；逻辑结构上的优化；
int RecommendCards::Help3Single(struct fDdz * pDdz)
{
	int iCopyOnHand[21];
	int iCount = iPlaCount;//出牌解
	int iTmpCards[3];
	int iHandCards = CInfo->CountCards(pDdz->iOnHand);

	//如果只剩一张牌，且合法，直接出  
	if (pDdz->iOnHand[0] != -1 && pDdz->iOnHand[1] == -1)
	{
		iTmpCards[0] = pDdz->iOnHand[0];
		iTmpCards[1] = -2;
		iTmpCards[2] = -1;
		if (GetLegalCards(pDdz, iTmpCards))
			return iPlaCount - iCount;
	}

	memcpy(iCopyOnHand, pDdz->iOnHand, sizeof(iCopyOnHand));

	int * iCards = pSplitCards->startSplitCard(iCopyOnHand, 7);
	bool haveTwo = false;
	bool haveKing = false;
	for (int i = 0; iCards[i] != -1; i++)//判断手牌是否有单个二和王
	{
		if (iCards[i] != -2)
		{
			if (iCards[i] / 4 == 12)
			{
				haveTwo = true;
				break;
			}
			else if (iCards[i] / 4 == 13)
			{
				haveKing = true;
				break;
			}
		}
	}

	int iTwo[5] = { -1,-1,-1,-1,-1 };//下标0 存放个数
	int iKing[3] = { -1,-1,-1 };//下标0 存放个数
	int iTwoIndex = 1;
	int iKingIndex = 1;
	for (int i = 0; iCopyOnHand[i] != -1; i++)
	{
		if (iCopyOnHand[i] / 4 == 12)
		{
			iTwo[iTwoIndex++] = iCopyOnHand[i];
		}
		else if (iCopyOnHand[i] == 52 || iCopyOnHand[i] == 53)
		{
			iKing[iKingIndex++] = iCopyOnHand[i];
		}
	}

	iTwo[0] = iTwoIndex - 1;
	iKing[0] = iKingIndex - 1;
		

	iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 7);

	//下家或者上家地主报单或者报双； AI是地主，下家或上家报单或报双
	if ((playerType == 2 && pDdz->iDownPlayerCards <= 2)
		|| (playerType == 3 && pDdz->iUpPlayerCards <= 2)
		|| (playerType == 1 && (pDdz->iDownPlayerCards <= 2 || pDdz->iUpPlayerCards <= 2)))
	{
		if (!isSim)
		{
			//出王或2
			if (iKing[0] != 0)
			{
				int iTmp[3] = { -1,-2,-1 };
				iTmp[0] = iKing[1];
				if (GetLegalCards(pDdz, iTmp))
					return iPlaCount - iCount;
			}
			if (iTwo[3] != 0)
			{
				int iTmp[3] = { -1,-2,-1 };
				iTmp[0] = iTwo[1];
				if (GetLegalCards(pDdz, iTmp))
					return iPlaCount - iCount;
			}

			//如果都不行，出最大的单牌
			int i;
			for (i = 0; iCards[i] != -1; i++);
			//0 1  2  3 4 5  6
			//1 -2 4 -2 5 -2 -1  
			int iTmp[3] = { -1,-2,-1 };
			if (i >= 2)
			{
				//if ((iCards[i - 2] == 52 || iCards[i - 2] == 53 )&& )i = i - 2;
				iTmp[0] = iCards[i - 2];
				if (GetLegalCards(pDdz, iTmp))
					return iPlaCount - iCount;
			}
		}
		else
		{
			//出最大的牌
			CInfo->SortById(iCopyOnHand); //iCopyOnHand[]从小到大排序

			int i;
			for (i = 0; iCopyOnHand[i] != -1; i++);

			int iTmp[3] = { -1,-2,-1 };
			iTmp[0] = iCopyOnHand[i - 1];

			if (GetLegalCards(pDdz, iTmp))
				return iPlaCount - iCount;
		}
	}

	//没有单二，手数小于6 或者下家是地主且下家手牌数小于5，有两个及以上的二，拆二
	if (iLastMaxPoint > -1)//跟牌
	{
		if (!haveTwo)
		{
			if (iHandNum < 6 || (playerType == 2 && pDdz->iDownPlayerCards<5))
			{
				if (iTwo[0] != 0)
				{
					int iTmp[2] = { -1,-1 };
					iTmp[0] = iTwo[1];
					AppendCards(iTmp, iCards);
					//haveTwo = true;
				}
			}
		}
	}

	//跟地主的牌
	if (!judgeFriend(pDdz) && pDdz->cDir != pDdz->cLandlord)
	{
		//如果地主出牌小于2，不考虑出王
		if (iLastMaxPoint < 12)
		{
			for (int i = 0; iCopyOnHand[i] != -1; i++)
			{
				if (iCopyOnHand[i] == 52)
				{
					DeleteElementInCards(iCopyOnHand, 21, 52);
				}
				else if (iCopyOnHand[i] == 53)
				{
					DeleteElementInCards(iCopyOnHand, 21, 53);
				}
			}
		}
		//出的是2，如果地主手数多，不考虑用大王跟
		else if (iLastMaxPoint == 12)
		{
			if ((playerType == 2 && iDownHandNum > 5) || (playerType == 3 && iUpHandNum > 5))
			{
				for (int i = 0; iCopyOnHand[i] != -1; i++)
				{
					if (iCopyOnHand[i] == 53)
					{
						DeleteElementInCards(iCopyOnHand, 21, 53);
					}
				}
			}
		}
		if (GetLegalCards(pDdz, iCards))
		{
			return iPlaCount - iCount;
		}
	}


	iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 7);

	int iSingleCount = 0;
	int iThreeCount = 0;
	for (int i = 0; iCards[i] != -1; i++)
	{
		if (iCards[i] == -2)iSingleCount++;
	}

	iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 2);
	for (int i = 0; iCards[i] != -1; i++)
	{
		if (iCards[i] == -2)iThreeCount++;
	}
	if (iLastMaxPoint <= -1 && iSingleCount <= iThreeCount) //自己出牌且三条数大于单张牌数
		return 0;

	/* //没用，可删
	int iTmp[3] = { -1,-2,-1 };
	if (iCards[0] >= 0)
	{
		for (int i = 0; iCards[i] != -1; i++)
		{
			if (iCards[i] != -2)
			{
				iTmp[0] = iCards[i];
				if (GetLegalCards(pDdz, iTmp))
				{
					return iPlaCount - iCount;
				}
			}
		}
	}*/

	iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 7);

	//一般情况：跟牌或者出牌 都出最小单牌
	if (GetLegalCards(pDdz, iCards))
	{
		return iPlaCount - iCount;
	}


	//本次模拟出牌生成器产生的多种出牌可行解数量为0
	if (iPlaCount == 0)
	{
		if (iLastMaxPoint < 0)//没有单牌时且自己出牌，不出单牌
			return 0;
		else//跟牌
		{
			int iTmp[3] = { -2,-2,-1 };

			if (judgeFriend(pDdz))return 0;//跟队友的牌

										   //跟地主的牌
			else if (pDdz->cDir != pDdz->cLandlord)
			{
				int iSingle[3] = { -1,-2,-1 };
				//查找大于5张的单顺
				iCards = pSplitCards->startSplitCard(iCopyOnHand, 4);
				int iLinkSingleCount = 0;
				int i;
				int maxSingle = 0;

				//没有单牌时，取单顺牌数大于5的最大那一张单牌
				for (i = 0; iCards[i] != -1; i++)
				{
					if (iCards[i] == -2)// 0 1 2 3 4 5 6(-2)   7 8 9 10 11 12  13(-2)
					{
						if (iLinkSingleCount > 5)
						{
							if (iCards[i - 1] > maxSingle)
							{
								maxSingle = iCards[i - 1];
							}
						}
						iLinkSingleCount = 0;
					}
					else
						iLinkSingleCount++;
				}

				if (maxSingle != 0)
				{
					iSingle[0] = maxSingle;
				}

				if (GetLegalCards(pDdz, iSingle))
				{
					return iPlaCount - iCount;
				}

				//如果单顺的单牌还是不行，或者没有

				//查找二
				if (haveTwo)return 0;
				else
				{
					if (iTwo[0] != 0)
					{
						iTmp[0] = iTwo[1];
						if (GetLegalCards(pDdz, iTmp))
						{
							return iPlaCount - iCount;
						}
						haveTwo = true;
					}
				}

				if ((playerType == 2 && iDownHandNum < 4) || (playerType == 3 && iUpHandNum < 4))
				{
					if (!haveTwo)
					{
						if (iTwo[0] != 0)
						{
							iTmp[0] = iTwo[1];
							if (GetLegalCards(pDdz, iTmp))
							{
								return iPlaCount - iCount;
							}
							haveTwo = true;
						}
					}
					else if (haveTwo && !haveKing&&iKing[0] != 0)
					{
						if (iKing[1] != -1)
						{
							iTmp[0] = iKing[1];

							if (GetLegalCards(pDdz, iTmp))
							{
								return iPlaCount - iCount;
							}
							else
								return 0;
							haveKing = true;
						}
						else if (iKing[2] != -1)
						{
							iTmp[0] = iKing[2];
							if (GetLegalCards(pDdz, iTmp))
							{
								return iPlaCount - iCount;
							}
							else
								return 0;
						}
					}
				}
			}

			//AI是地主
			else if (pDdz->cDir == pDdz->cLandlord)
			{
				if (!haveTwo)
				{
					if (iTwo[0] != 0)
					{
						iTmp[0] = iTwo[1];
						if (GetLegalCards(pDdz, iTmp))
						{
							return iPlaCount - iCount;
						}
						haveTwo = true;
					}
				}
				if (!haveKing&&iKing[0] != -1)
				{
					if (iKing[1] != -1)
					{
						iTmp[0] = iKing[1];
						if (GetLegalCards(pDdz, iTmp))
						{
							return iPlaCount - iCount;
						}
						else
							return 0;
					}
					else if (iKing[2] != -1)
					{
						iTmp[0] = iKing[2];
						if (GetLegalCards(pDdz, iTmp))
						{
							return iPlaCount - iCount;
						}
						else
							return 0;
					}
				}
			}
		}
	}
	return 0;
}
//H03-END


//查询所有不同点数单张牌型到iOnHand里面          //无规则  3 4 5 5 -1 -1 // 3 -2 4 -2 5 -2 -1
int RecommendCards::Help3SingleSim(fDdz* pDdz)
{
	int i;
	int iTmpCards[60] = { -1 };

	int count = iPlaCount;
	int point = -1;
	int t = 0;
	for (i = 0; pDdz->iOnHand[i] >= 0; i++)
	{
		if (t > 0)
		{
			if (pDdz->iOnHand[i] / 4 == point)
				continue;
		}
		iTmpCards[t] = pDdz->iOnHand[i];
		point = iTmpCards[t] / 4;
		t++;
		iTmpCards[t] = -2;
		t++;
	}                               
	iTmpCards[t] = -1;
	if (GetLegalCards(pDdz, iTmpCards))
	{
		return iPlaCount - count;
	}
	return 0;
}

///////PASS//////////
//H04-START推荐出牌应对对牌
//最后修订者:江文金，最后修订时间:2018-07-21
int RecommendCards::Help4Double(struct fDdz * pDdz)
{
	//拆不在双顺组合里的对子牌
	//双顺最大点用于检查对子是否是双顺内的组合
	int i,  t;
	int iCopyOnHand[21];
	int iCount = iPlaCount;
	int * iCards;

	//将手中的牌复制进iCopyOnHand
	memcpy(iCopyOnHand, pDdz->iOnHand, sizeof(pDdz->iOnHand));

	//是否可以直接出
	i = CInfo->CountCards(pDdz->iOnHand);
	if (i == 2)  
	{
		iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 6);
		if (iCards[0] >= 0)
		{
			if (GetLegalCards(pDdz, iCards))
				return iPlaCount - iCount;
		}
	}
	
	//排除2和王
	if (pDdz->iLastMainPoint<11)
	{
		for (i = 48; i <= 53; i++)
		{
			DeleteElementInCards(iCopyOnHand, 21, i);
		}
	}

	//判断炸弹,并排除
	iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 1);
	if (iCards[0] > -1)
	{
		for (i = 0; iCards[i] != -1; i++)
		{
			if (iCards[i] != -2)
				DeleteElementInCards(iCopyOnHand, 21, iCards[i]);
		}
	}

	//判断三条,并排除
	iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 2);
	if (iCards[0] > -1)
	{
		for (i = 0; iCards[i] != -1; i++)
		{
			if (iCards[i] != -2)
				DeleteElementInCards(iCopyOnHand, 21, iCards[i]);
		}
	} 

	//找双顺
	iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 5);
	//排除在双顺里面的对子
	if (iCards[0] > -1)
	{
		for (i = 0; iCards[i] != -1; i++)
		{
			if (iCards[i] != -2)
				DeleteElementInCards(iCopyOnHand, 21, iCards[i]);
		}
	}

	//排除在单顺里面的对子
	iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 4);
	if (iCards[0] > -1)
	{
		for (i = 0; iCards[i] != -1; i++)
		{
			if (iCards[i] != -2)
				DeleteElementInCards(iCopyOnHand, 21, iCards[i]);
		}
	}

	//分析对子
	iCards = pSplitCards->startSplitCard(iCopyOnHand, 6);
	if (GetLegalCards(pDdz, iCards))
	{
		return iPlaCount - iCount;
	}

	//如果出牌，退出
	if (pDdz->iLastMainPoint < 0)
		return 0;

	//如果还是没对子，从新赋值，再筛选
	memcpy(iCopyOnHand, pDdz->iOnHand, sizeof(pDdz->iOnHand));

	//如果桌面的牌小于A，排除二
	if (pDdz->iLastMainPoint<11&&iHandNum>2)
	{
		for (i = 48; i <= 53; i++)
		{
			DeleteElementInCards(iCopyOnHand, 21, i);
		}
	}

	//判断炸弹,并排除
	iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 1);
	if (iCards[0] > -1)
	{
		for (i = 0; iCards[i] != -1; i++)
		{
			if (iCards[i] != -2)
				DeleteElementInCards(iCopyOnHand, 21, iCards[i]);
		}
	}

	//判断三条,并排除
	iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 2);
	if (iCards[0] > -1)
	{
		for (i = 0; iCards[i] != -1; i++)
		{
			if (iCards[i] != -2)
				DeleteElementInCards(iCopyOnHand, 21, iCards[i]);
		}
	}

	//判断对子 ，这里应该考虑对子是否会拆掉一个顺子
	iCards = pSplitCards->startSplitCard(iCopyOnHand, 6);

	if (!isSim)
	{
		if (judgeTypeAndAddInArr(pDdz, iCards))
			return iPlaCount - iCount;
	}
	else
	{
		if (GetLegalCards(pDdz,iCards))
			return iPlaCount - iCount;
	}
	
	//判断是否是队友出牌，若是，则返回
	if (judgeFriend(pDdz))return 0;

	//如果依然没有牌，检查非炸弹2
	int dui2[4] = { -1, -1, -1, -1 };
	t = 0;
	for (i = 0; pDdz->iOnHand[i] != -1; i++)
	{
		if (pDdz->iOnHand[i] / 4 == 12)
		{
			if (t == 3)
			{
				dui2[0] = -1;
				break;
			}
			dui2[t] = pDdz->iOnHand[i];
			t++;
		}
	}

	i = CInfo->CountCards(pDdz->iOnHand);
	//如果有对2,不能在第一手出
	if (dui2[1] != -1)
	{
		if (pDdz->cDir == pDdz->cLandlord&&i == 20)	//pDdz->cDir != pDdz->cLandlord
			return 0;
		//否则
		dui2[2] = -2;
		dui2[3] = -1;
		if (GetLegalCards(pDdz, dui2))
		{
			return iPlaCount - iCount;
		}
	}

	return iPlaCount - iCount;
}


//无规则出对子        //3 3 4 4 4 -1 -1//3 3 -2 4 4 -2 -1
int RecommendCards::Help4DoubleSim(struct fDdz * pDdz)
{
	int iTmpCards[60] = { -1 };
	int t = 0;
	int i;
	int point = -1;
	int iCount = iPlaCount;

	//寻找对子，放进iTmpCards数组中
	for (i = 0; pDdz->iOnHand[i + 1] >= 0; i++)
	{
		if (pDdz->iOnHand[i] / 4 == pDdz->iOnHand[i + 1] / 4)
		{
			if (pDdz->iOnHand[i] / 4 != point)//防止三条及以上算两次对子
			{
				iTmpCards[t] = pDdz->iOnHand[i];
				point = pDdz->iOnHand[i] / 4;
				t++;
				iTmpCards[t] = pDdz->iOnHand[i + 1];
				t++;
				iTmpCards[t] = -2;
				t++;

			}
		}
	}
	iTmpCards[t] = -1;
	if (GetLegalCards(pDdz, iTmpCards))
	{
		return iPlaCount - iCount;
	}
	return iPlaCount - iCount;
}


//H05-START推荐出牌应对三条
//如果自己有出牌权，不拆三顺，如果没有，拆三顺（仅当地主牌小于等于3时考虑三条二 或者自己只有两手牌时)
//最后修订者:江文金，最后修订时间:2018-07-18
int RecommendCards::Help5Three(struct fDdz * pDdz)
{
	int iCopyOnHand[21];
	int i;
	memcpy(iCopyOnHand, pDdz->iOnHand, sizeof(pDdz->iOnHand));

	int iCount = iPlaCount;

	int *iCards;

	//是否可以直接出
	i = CInfo->CountCards(pDdz->iOnHand);
	if (i == 3)
	{
		iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 2);
		if (iCards[0] >= 0)
		{
			if (GetLegalCards(pDdz, iCards))
				return iPlaCount - iCount;
		}
	}

	//判断炸弹,并排除
	iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 1);
	if (iCards[0] > -1)
	{
		for (i = 0; iCards[i] != -1; i++)
		{
			if (iCards[i] != -2)
				DeleteElementInCards(iCopyOnHand,21, iCards[i]);
		}
	}

	//去掉二，当上次牌型最大点数不大于A，且我们手数大于等于3，或者地主的牌大于三张时，不考虑出三条二  
	if ((playerType == 2 && pDdz->iDownPlayerCards > 3) || (playerType == 3 && pDdz->iUpPlayerCards > 3)||playerType==1)
	{
		if (pDdz->iLastMainPoint < 11 && iHandNum >= 3)
		{
			for (i = 48; i < 52; i++)
			{
				DeleteElementInCards(iCopyOnHand, 21, i);
			}
		}
	}

	//去掉三顺 
	iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 3);
	if (iCards[0] > -1)
	{
		for (i = 0; iCards[i] != -1; i++)
		{
			if (iCards[i] != -2)
				DeleteElementInCards(iCopyOnHand, 21, iCards[i]);
		}
	}

	//判断三条,这里应该考虑三条是否会拆掉一个顺子
	iCards = pSplitCards->startSplitCard(iCopyOnHand, 2);

	if (iCards[0] <= -1)return 0;

	if (deleteTypeLeftSingleCount(iCopyOnHand, iCards) >= 2)return 0;
	iCards = pSplitCards->startSplitCard(iCopyOnHand, 2);

	/*if (!isSim)
		if (judgeTypeAndAddInArr(pDdz, iCards))
			return iPlaCount - iCount;
	else
		if (GetLegalCards(pDdz, iCards))
			return iPlaCount - iCount;
	*/
	
	//如果没有，且我方有出牌权。退出
	if (pDdz->iLastMainPoint < 0)
	{
		if(iCards[0]/4>10&& CInfo->CountCards(pDdz->iOnHand)>10)//有出牌权 如果三条大于等于A，且手牌数大于10，不出这个
			return 0;
		else
		{
			int iTmp[7] = {-1,-1,-1,-2,-1,-1,-1}; 
			iTmp[0] = iCards[0];
			iTmp[1] = iCards[1];
			iTmp[2] = iCards[2];
			iTmp[3] = -2;
			iTmp[4] = -1;
			iCards= pSplitCards->startSplitCard(iCopyOnHand, 7);
			if (iCards[0] < 48)
			{
				iTmp[3] = iCards[0];
				iTmp[4] = -2;
				iTmp[5] = -1;
			}

			if (judgeTypeAndAddInArr(pDdz, iTmp))   
				return iPlaCount - iCount;

			iCards = pSplitCards->startSplitCard(iCopyOnHand, 6);
			if (iTmp[3]==-2&&iCards[0] < 48)
			{
				iTmp[3] = iCards[0];
				iTmp[4] = iCards[1];
				iTmp[5] = -2;
				iTmp[6] = -1;
			}
			
			if (judgeTypeAndAddInArr(pDdz, iTmp))
				return iPlaCount - iCount;
		}
	}
	else//跟牌
	{
		memcpy(iCopyOnHand, pDdz->iOnHand, sizeof(pDdz->iOnHand));

		//判断炸弹,并排除
		int *iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 1);
		if (iCards[0] > -1)
		{
			for (i = 0; iCards[i] != -1; i++)
			{
				if (iCards[i] != -2)
					DeleteElementInCards(iCopyOnHand, 21, iCards[i]);
			}
		}

		//去掉2  
		if ((pDdz->iLastMainPoint < 11&&iHandNum>=3)||(playerType==2&&pDdz->iDownPlayerCards>3)|| (playerType == 3 && pDdz->iUpPlayerCards > 3))
		{
			for (i = 48; i < 52; i++)
			{
				DeleteElementInCards(iCopyOnHand, 21, i);
			}
		}

		iCards = pSplitCards->startSplitCard(iCopyOnHand, 2);
		
		if (!isSim)
		{
			if (judgeTypeAndAddInArr(pDdz, iCards))
				return iPlaCount - iCount;
		}
		else
		{
			if (GetLegalCards(pDdz, iCards))
				return iPlaCount - iCount;
		}
	}
	return 0;
}
//H05-END

//SplitCards拆牌要求：带的单牌不包括2和王
//H0601-START推荐出牌应对三带一单
//最后修订者:成文杰，最后修订时间:2017-07-16
int RecommendCards::Help6ThreeOne(struct fDdz * pDdz)
{
	int iTmpCards[60] = { -1 };
	int iCount = iPlaCount;

	//判断有无合法三条,并保存到iSave1里
	int t = 0;
	int i = 0;
	int *iCards;
	t = CInfo->CountCards(pDdz->iOnHand);
	int j;

	//如果可以直接出，则直接出（通过）
	if (t == 4)
	{
		j = 0;
		iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 2);
		if (iCards[0] > -1)
		{
			for (i = 0; iCards[i] != -1; i++)//把三条都赋值给iTempCards
			{
				if (iCards[i] != -2)
				{
					iTmpCards[j] = iCards[i];
					j++;
				}
			}

			for (i = 0; pDdz->iOnHand[i] != -1; i++)//把另外一张牌找出来
			{
				if (pDdz->iOnHand[i] / 4 != iTmpCards[0] / 4)
				{
					iTmpCards[j] = pDdz->iOnHand[i];
					j++;
					break;
				}
			}
			iTmpCards[j] = -2;
			j++;
			iTmpCards[j] = -1;
			if (GetLegalCards(pDdz, iTmpCards))
			{
				return iPlaCount - iCount;
			}
		}
	}

	int iCopyOnHand[21];
	memcpy(iCopyOnHand, pDdz->iOnHand, sizeof(pDdz->iOnHand));

	if (iLastMaxPoint < 0 && iHandNum > 4)//有出牌权，且手数大于4 去掉A。。
	{
		for (i = 44; i < 48; i++)
		{
			DeleteElementInCards(iCopyOnHand, 21, i);
		}
	}

	//判断炸弹,并排除
	iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 1);

	//排除炸弹
	for (i = 0; iCards[i] != -1; i++)
	{
		if (iCards[i] != -2)
		{
			DeleteElementInCards(iCopyOnHand, 21, iCards[i]);
		}
	}

	//去掉二，当上次牌型最大点数不大于A，且我们手数大于等于3，或者地主的牌大于三张时，不考虑出三条二  
	if ((playerType == 2 && pDdz->iDownPlayerCards > 3) || (playerType == 3 && pDdz->iUpPlayerCards > 3) || (playerType == 1&& pDdz->iDownPlayerCards > 3 && pDdz->iUpPlayerCards > 3))
	{
		if (pDdz->iLastMainPoint < 11 && iHandNum >= 3)
		{
			for (i = 48; i < 52; i++)
			{
				DeleteElementInCards(iCopyOnHand, 21, i);
			}
		}
	}
	
	iCards = pSplitCards->startSplitCard(iCopyOnHand, 2);

	//如果没有三条
	if (iCards[0] < 0)
	{
		return 0;
	}

	//如果有三条,保存三条到iTmpCards(通过)
	t = 0;
	int santiaoNum = 0;
	for (i = 0; iCards[i] != -1; i++)
	{
		if (iCards[0] == -2)
			break;
		if (iCards[i] == -2)
		{
			iTmpCards[t] = iCards[i - 3];
			t++;
			iTmpCards[t] = iCards[i - 2];
			t++;
			iTmpCards[t] = iCards[i - 1];
			t++;
			iTmpCards[t] = -2;
			t++;
			santiaoNum++;
		}
	}
	iTmpCards[t] = -1;

	if (santiaoNum)
	{
		int santiao[6];
		int danwang = -1;
		int er[4] = { -1, -1, -1, -1 };
		t = 0;
		j = 0;
		//找手上的单王和非炸弹2并保存起来(通过)
		int iHandCards = CInfo->CountCards(pDdz->iOnHand);
		for (i = iHandCards; i > iHandCards - 7; i--)
		{
			if (52 <= pDdz->iOnHand[i] && 53 >= pDdz->iOnHand[i])
			{
				j++;
				danwang = pDdz->iOnHand[i];

			}
			if (48 <= pDdz->iOnHand[i] && 51 >= pDdz->iOnHand[i])
			{
				er[t] = pDdz->iOnHand[i];
				t++;
			}
		}

		//如果2是炸弹,不保存
		if (er[3] >= 48)
			er[0] = -1;
		if (j > 1)//如果是王炸，不保存
			danwang = -1;


		//查找单牌

		//去掉王
		for (i = 52; i <=53; i++)
		{
			DeleteElementInCards(iCopyOnHand, 21,i);
		}

		iCards = pSplitCards->startSplitCard(iCopyOnHand, 7);

		int dan[21] = { -1, -1, -1, -1 };

		//如果有单
		if (iCards[0] > -1)
		{
			dan[0] = iCards[0];
			dan[1] = -1;
		}

		//如果没单且是出牌，离开
		if (iCards[0] <= -1 && pDdz->iLastMainPoint < 0)
		{
			return 0;
		}

		if (dan[0] <=-1)
		{
			//查找大于5张的单顺
			iCards = pSplitCards->startSplitCard(iCopyOnHand, 4);
			int iLinkSingleCount = 0;
			int i;
			for (i = 0; iCards[i] != -1; i++)
			{
				if (iCards[i] == -2)// 0 1 2 3 4 5(-2) 6 7 8 9 10 11 12(-2)
				{
					if (iLinkSingleCount > 5)
					{
						dan[0] = iCards[i - iLinkSingleCount];
						dan[1] = -1;
						break;
					}
					iLinkSingleCount = 0;
				}
				else
				{
					iLinkSingleCount++;
				}
			}
		}
	

		if (judgeFriend(pDdz))//如果是跟队友的牌，跳过
		{
			return 0;
		}

		//如果没单，是跟牌,且不是跟队友的牌，且三条数大于1，查找对子里的单牌
		if (dan[0] <= -1&& santiaoNum>1)
		{
			iCards = pSplitCards->startSplitCard(iCopyOnHand, 6);//查找对子
			if (iCards[0] > -1)
			{
				dan[0] = iCards[0];
				dan[1] = -1;
			}
		}

		
		//如果没有有效单牌
		if (dan[0] <= -1)
		{
			bool condition = AnalyzeCondition(pDdz);//不利局势返回false,非不利返回true
			if (!condition || iHandNum <= 2)
			{
				/*
				iCards = pSplitCards->startSplitCard(iCopyOnHand, 6);//查找对子
				if (iCards[0] > -1)
				{
					dan[0] = iCards[0];
					dan[1] = -1;
				}*/
				if (dan[0] <= -1)
				{
					if (er[0] != -1)
					{
						dan[0] = er[0];
						dan[1] = -1;
					}
				}
				/*if (danwang != -1)
				{
					dan[t] = danwang;
					t++;
				}*/
			}	
		}


		if (dan[0] == -1)//如果仍然没有单牌，返回
		{
			return 0;
		}

		//整理合并单牌和三条
		for (i = 0; iTmpCards[i] != -1; i++)
		{
			if (iTmpCards[i] == -2)
			{
				santiao[2] = iTmpCards[i - 1];
				santiao[1] = iTmpCards[i - 2];
				santiao[0] = iTmpCards[i - 3];
				santiao[3] = -1;
				int santiaopoint = CInfo->AnalyzeMainMaxPoint(santiao);
				for (int j = 0; dan[j] != -1; j++)
				{
					if (dan[j] / 4 != santiaopoint)
					{
						santiao[3] = dan[j];
						santiao[4] = -2;
						santiao[5] = -1;
						if (!isSim)
						{
							if(judgeTypeAndAddInArr(pDdz, santiao))
								return iPlaCount - iCount;
						}
						else
						{
							if (GetLegalCards(pDdz, santiao))
								return iPlaCount - iCount;
						}
					}
				}
			}
		}
	}
	return iPlaCount - iCount;
}
//H0601-END

//无规则三带一单
int RecommendCards::Help6ThreeOneSim(struct fDdz * pDdz)
{
	int iCount = iPlaCount;
	int iTmpCards[21] = { -1 };

	int i, iTemp, j;
	int point = -1;
	int danNum = 0;

	int santiaoPoint = 0;
	int t = 0;
	int *iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 7);
	//如果有单
	for (i = 0; pDdz->iOnHand[i + 2] >= 0; i++)
	{
		if (pDdz->iOnHand[i] / 4 == pDdz->iOnHand[i + 1] / 4
			&& pDdz->iOnHand[i] / 4 == pDdz->iOnHand[i + 2] / 4)
		{
			iTemp = i;

			for (j = 0; pDdz->iOnHand[j] >= 0; j++)
			{
				t = 0;
				//该单牌不能是三条中的数并且将炸弹排除
				if (j == iTemp || j == iTemp + 1 || j == iTemp + 2 || pDdz->iOnHand[j] / 4 == pDdz->iOnHand[i] / 4)
					continue;
				//该单牌也不能跟上一个同一个三条的单牌点数相同
				if (pDdz->iOnHand[j] / 4 == point)
					continue;
				iTmpCards[t] = pDdz->iOnHand[i];
				t++;
				iTmpCards[t] = pDdz->iOnHand[i + 1];
				t++;
				iTmpCards[t] = pDdz->iOnHand[i + 2];
				t++;
				iTmpCards[t] = pDdz->iOnHand[j];
				point = pDdz->iOnHand[j] / 4;
				t++;
				iTmpCards[t] = -1;
				santiaoPoint = CInfo->AnalyzeMainMaxPoint(iTmpCards);   // 计算主牌最大点数
				iTmpCards[t] = -2;
				t++;
				iTmpCards[t] = -1;

				if (iCards[0] != -1&& iCards[0]/4<12&& iCards[0]!= iTmpCards[0])	
				{
					iTmpCards[3] = iCards[0];
				}

				GetLegalCards(pDdz,iTmpCards);
			}

		}
	}

	return iPlaCount - iCount;
}


//H0602-START推荐出牌应对三带一对
//最后修订者:成文杰，最后修订时间:2017-07-17
int RecommendCards::Help6ThreeDouble(struct fDdz * pDdz)
{
	int iTmpCards[60] = { -1 };
	int iCount = iPlaCount;
	int t = 0;
	int i = 0;
	int *iCards;
	t = CInfo->CountCards(pDdz->iOnHand);
	//如果可以直接出，则直接出
	if (t == 5)
	{
		iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 2);
		if (iCards[0] > -1 && iCards[0] / 4 > iLastMaxPoint)
		{
			iTmpCards[0] = iCards[0];
			iTmpCards[1] = iCards[1];
			iTmpCards[2] = iCards[2];
			iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 6);
			if (iCards[0] > -1)
			{
				iTmpCards[3] = iCards[0];
				iTmpCards[4] = iCards[1];
				iTmpCards[5] = -2;
				iTmpCards[6] = -1;

				if (GetLegalCards(pDdz, iTmpCards))
				{
					return iPlaCount - iCount;
				}
			}
		}
	}

	//////////////////////////
	int iCopyOnHand[21];
	memcpy(iCopyOnHand, pDdz->iOnHand, sizeof(pDdz->iOnHand));

	//判断炸弹,并排除
	iCards = pSplitCards->startSplitCard(iCopyOnHand, 1);

	if (iCards[0] > -1)
	{
		for (i = 0; iCards[i] != -1; i++)
		{
			if (iCards[i] != -2)
				DeleteElementInCards(iCopyOnHand, 21, iCards[i]);
		}
	}


	if (iLastMaxPoint < 0 && iHandNum > 4)//有出牌权，且手数大于4 去掉A。。
	{
		for (i = 44; i < 48; i++)
		{
			DeleteElementInCards(iCopyOnHand, 21, i);
		}
	}

	i = CInfo->CountCards(pDdz->iOnHand);

	if (IfPassTwice(pDdz))//如果有出牌权
	{
		iCards = pSplitCards->startSplitCard(iCopyOnHand, 7);
		if (iCards[0] > -1 && iCards[0] < 28)//如果存在10以下的单牌，优先出三带一
			return 0;
	}

	/*//去掉2  
	if ((playerType == 2 && pDdz->iDownPlayerCards > 3) 
		|| (playerType == 3 && pDdz->iUpPlayerCards > 3)
		|| (playerType == 1 && pDdz->iDownPlayerCards > 3 && pDdz->iUpPlayerCards > 3))
	{
		if (pDdz->iLastMainPoint < 11 && iHandNum >= 3)
		{
			for (i = 48; i < 52; i++)
			{
				DeleteElementInCards(iCopyOnHand, 21, i);
			}
		}
	}*/
	
	//判断有无三条
	iCards = pSplitCards->startSplitCard(iCopyOnHand, 2);

	//如果没有三条
	if (iCards[0] < 0)
		return 0;

	//如果有三条,保存三条到iTmpCards
	t = 0;
	int santiaoNum = 0;
	for (i = 0; iCards[i] != -1; i++)
	{
		if (iCards[0] == -2)
			break;
		if (iCards[i] == -2)
		{
			iTmpCards[t] = iCards[i - 3];
			t++;
			iTmpCards[t] = iCards[i - 2];
			t++;
			iTmpCards[t] = iCards[i - 1];
			t++;
			iTmpCards[t] = -2;
			t++;
			santiaoNum++;
		}
	}
	iTmpCards[t] = -1;

	if (santiaoNum)
	{
		bool condition = AnalyzeCondition(pDdz);
		if (condition||iHandNum>2)//排除2和王
		{
			for (i = 48; i < 54; i++)
			{
				DeleteElementInCards(iCopyOnHand, 21, i);
			}
		}

		////包括双顺里面的对子////////
		int santiao[8];
		iCards = pSplitCards->startSplitCard(iCopyOnHand, 6);
		int duizi[60];
		memset(duizi, -1, sizeof(duizi));

		for ( i = 0; iCards[i]!=-1; i++)
		{
			duizi[i] = iCards[i];
		}
		duizi[i] = -1;

		if (duizi[0] <= -1&& santiaoNum>1)
		{
			//如果没对子且是出牌，离开
			if (pDdz->iLastMainPoint < 0)
				return 0;

			//如果是队友的牌，不跟
			if (judgeFriend(pDdz))
				return 0;

			//如果敌方手牌还比较多，不拆三条
			if ((playerType == 2 && pDdz->iDownPlayerCards > 10) || (playerType == 3 && pDdz->iUpPlayerCards > 10))
				return 0;

			//否则查找三条里面的对子,并保存在iCard里
			int  t = 0;
			for (int i = 0; iTmpCards[i] != -1; i++)
			{
				if (iTmpCards[i] == -2)
				{
					duizi[t] = iTmpCards[i - 2];
					++t;
					duizi[t] = iTmpCards[i - 1];
					++t;
					duizi[t] = -2;
					++t;
					break;//只拆一个三条
				}
			}
			duizi[t] = -1;
		}

		//如果仍然没对子
		if (duizi[0] == -1)
			return 0;

		for (i = 0; iTmpCards[i] != -1; i++)
		{
			int santiaoPoint;
			if (iTmpCards[i] == -2)
			{
				santiao[0] = iTmpCards[i - 3];
				santiao[1] = iTmpCards[i - 2];
				santiao[2] = iTmpCards[i - 1];
				santiao[3] = -1;
				santiaoPoint = CInfo->AnalyzeMainMaxPoint(santiao);
				if (duizi[0] / 4 != santiaoPoint)
				{
					santiao[3] = duizi[0];
					santiao[4] = duizi[1];
					santiao[5] = -2;
					santiao[6] = -1;

					if (!isSim)
					{
						if (judgeTypeAndAddInArr(pDdz, santiao))
						{
							return  iPlaCount - iCount;
						}
					}
					else
					{
						if (GetLegalCards(pDdz, santiao))
						{
							return  iPlaCount - iCount;
						}
					}
				}					
			}
		}
	}
	return iPlaCount - iCount;
}
//H0602-END

//无规则三带一对
int RecommendCards::Help6ThreeDoubleSim(struct fDdz * pDdz)
{
	int num = 0;
	int iCount = iPlaCount;
	int i, iTemp, j;
	int iTmpCards[500] = { -1 };
	int point = -1;
	int t = 0;
	int santiaoPoint = -1;
	for (i = 0; pDdz->iOnHand[i + 2] >= 0; i++)
	{
		if (t + 5 + 1 > 500)
			break;
		//找到一个合适的三条
		if (num + 1<kPlaMax
			&& pDdz->iOnHand[i] / 4 > iLastMaxPoint
			&& pDdz->iOnHand[i] / 4 == pDdz->iOnHand[i + 1] / 4
			&& pDdz->iOnHand[i] / 4 == pDdz->iOnHand[i + 2] / 4)
		{
			iTemp = i;
			for (j = 0; pDdz->iOnHand[j + 1] >= 0; j++)
			{
				t = 0;
				//将对子在三条中的数排除
				if (j >= iTemp - 1 && j <= iTemp + 2)
					continue;
				//该对子也不能跟上一个同一个三条的对子点数相同
				if (pDdz->iOnHand[j] / 4 == point)
					continue;
				if (pDdz->iOnHand[j]>=52)
					continue;
				if (pDdz->iOnHand[j] / 4 == pDdz->iOnHand[j + 1] / 4)
				{
					iTmpCards[t] = pDdz->iOnHand[i];
					t++;
					iTmpCards[t] = pDdz->iOnHand[i + 1];
					t++;
					iTmpCards[t] = pDdz->iOnHand[i + 2];
					t++;
					iTmpCards[t] = pDdz->iOnHand[j];
					point = pDdz->iOnHand[j] / 4;
					t++;
					iTmpCards[t] = pDdz->iOnHand[j + 1];
					t++;
					iTmpCards[t] = -1;
					santiaoPoint = CInfo->AnalyzeMainMaxPoint(iTmpCards);
					iTmpCards[t] = -2;
					t++;
					iTmpCards[t] = -1;

					if (GetLegalCards(pDdz, iTmpCards))
						num++;
				}
			}
		}
	}
	return iPlaCount - iCount;
}


//TODO
//H07-START推荐出牌应对单顺
//最后修订者:成文杰，最后修订时间:2017-07-17
int RecommendCards::Help7LinkSingle(struct fDdz * pDdz)
{
	int i, iLength,   t;
	int iCopyCards[21];
	int iCount = iPlaCount;
	CInfo->SortById(pDdz->iOnHand);//按牌编码升序排列(jwj)
	int shunziCount = 0;
	int *iCards;

	memcpy(iCopyCards, pDdz->iOnHand, sizeof(pDdz->iOnHand));
	t = CInfo->CountCards(pDdz->iOnHand);

	//如果炸弹不在顺子中间的话，排除炸弹
	if (deleteTypeLeftSingleCount(iCopyCards, pSplitCards->startSplitCard(iCopyCards, 1)) < 4)
	{
		iCards = pSplitCards->startSplitCard(iCopyCards, 1);

		for (i = 0; iCards[i] != -1; i++)
		{
			if (iCards[i] != -2)
			{
				DeleteElementInCards(iCopyCards, 21, iCards[i]);
			}
		}
	}
	

	//去掉王和2
	for (i = 48; i <= 53; i++)
	{
		DeleteElementInCards(iCopyCards, 21, i);	
	}

	int iTmpShunzi[21];
	memset(iTmpShunzi, -1, sizeof(iTmpShunzi));

	int NodesCount = 0;
	
	int iDanShunNum = 0;
	int iDanShunNoBombNum = 0;
	int MaxNumNoBomb = 0;


	//如果是出牌
	if (pDdz->iLastMainPoint < 0)
	{
		iCards = pSplitCards->startSplitCard(iCopyCards, 4);
		if (!isSim)
		{
			if (judgeTypeAndAddInArr(pDdz, iCards))
			{
				return iPlaCount - iCount;
			}
		}
		else
		{
			if (GetLegalCards(pDdz, iCards))
			{
				return iPlaCount - iCount;
			}
		}
	}

	//如果跟牌，尽最大可能出跟牌节点  //这里没有判断是否是队友打的
	else
	{
		iLength = pDdz->iLastTypeCount - 700;  //当前桌面顺子长度
		int point = pDdz->iLastMainPoint;
		while(point + iLength - 1 <= 11)   //单顺最大长度为12
		{
			int iTmp = 0;
			iTmp = iTmpShunzi[0];

			iDanShunNum = AnalyzeSingleLink(iLength, point, pDdz->iOnHand, iTmpShunzi);
			point=point+1;
			if (iDanShunNum == iLength && iTmp != iTmpShunzi[0])//防止出现一样的顺子结点
			{
				if (!isSim)
				{
					judgeTypeAndAddInArr(pDdz, iTmpShunzi);
				}
				else
				{
					GetLegalCards(pDdz, iTmpShunzi);
				}
			}
		}
	}
	return iPlaCount - iCount;
}
//H07-END

//H08-START推荐出牌应对双顺
//最后修订者:成文杰，最后修订时间:2017-07-16
//修订出牌时出双顺的情况
int RecommendCards::Help8LinkDouble(struct fDdz * pDdz)
{
	int i, j, k, iLength;
	int iCopyOnHand[21];
	int n = 1;
	int iFlag = 0;
	int iCardsFlag = 0;
	int iCard = 0;
	int iTmpShuangshun[21];
	
	int iCount = iPlaCount;

	memset(iTmpShuangshun, -1, sizeof(iTmpShuangshun));
	CInfo->SortById(pDdz->iOnHand);
	
	memcpy(iCopyOnHand, pDdz->iOnHand, sizeof(pDdz->iOnHand));//拷贝函数

	//排除炸弹
	int *iCards = pSplitCards->startSplitCard(iCopyOnHand, 1);
	for (i = 0; iCards[i] != -1; i++)
	{
		if (iCards[i] != -2)
		{
			DeleteElementInCards(iCopyOnHand, 21, iCards[i]);
		}
	}

	//排除2和小王
	for (int i = 0;iCopyOnHand[i]!=-1; i++)
	{
		if (iCopyOnHand[i] >= 48)
		{
			iCopyOnHand[i] = -1;
		}
	}

	//排除三顺的牌(jwj)
	iCards = pSplitCards->startSplitCard(iCopyOnHand, 3);
	{
		if (iCards[i] != -2)
		{
			DeleteElementInCards(iCopyOnHand, 21, iCards[i]);
		}
	}

	//如果拆了双顺，导致的单牌很多。不搞这个
	if (deleteTypeLeftSingleCount(iCopyOnHand, pSplitCards->startSplitCard(iCopyOnHand, 5)) >= 2)
		return 0;

	//这是哪位大佬写的函数，表示看了半天auto的用法也没看见带有 =[&] 的
	//应该是一个返回类型为int的函数,感觉这里就当一个普通的函数理解也可以
	auto GetShuangShun = [&](int *iCards, int iLength,int iLastMainPoint)->int
	{
		int shuangshunNum = 0;
		int iCopyCards[21];
		for (i = 0; iCards[i] != -1; i++)
		{
			iCopyCards[i] = iCards[i];
		}
		iCopyCards[i] = -1;

		//四连变三连 三连变对子 对子变单排 单排变没有
		for (i = 0; i < CInfo->CountCards(iCopyCards); i++)
		{
			if (iCopyCards[i] / 4 != iCopyCards[i + 1] / 4)
			{
				for (j = i; j < CInfo->CountCards(iCopyCards); j++)
					iCopyCards[j] = iCopyCards[j + 1];
				i--;
			}
		}

		//三连和对子变成单牌
		for (i = 0; i < CInfo->CountCards(iCopyCards); i++)
		{
			if (iCopyCards[i] / 4 == iCopyCards[i + 1] / 4)
			{
				for (j = i; j < CInfo->CountCards(iCopyCards); j++)
					iCopyCards[j] = iCopyCards[j + 1];
				i--;
			}
		}

		//以上将数组iCopyCards[21]处理成能组成对子的单排数组且最大为A

		for (i = 0; iCopyCards[i] >= 0; i++)
		{
			if (iCopyCards[i] / 4 == iCopyCards[i + 1] / 4 - 1)
				n++;    //n的初值为1
			else
			{
				n = 1;
				i = iFlag;    //没问题
				iFlag++;
			}
			if (n == iLength)
			{
				if (shuangshunNum + 1 < kPlaMax && iCopyCards[i + 2 - iLength] / 4 > iLastMainPoint)
				{
					for (j = i + 2 - iLength; j <= i + 1; j++)
					{
						for (k = 0; k < CInfo->CountCards(pDdz->iOnHand); k++)
						{
							if (iCopyCards[j] / 4 == pDdz->iOnHand[k] / 4 && iCardsFlag < 2)
							{
								iTmpShuangshun[iCard] = pDdz->iOnHand[k];
								iCard++;
								iCardsFlag++;
							}
						}
						iCardsFlag = 0;
					}
					iTmpShuangshun[iCard] = -2;
					iTmpShuangshun[iCard + 1] = -1;
					if (GetLegalCards(pDdz, iTmpShuangshun))
						shuangshunNum++;
					iCard = 0;
				}
				n = 1;
				i = iFlag;
				iFlag++;
			}
		}
		return shuangshunNum;
	};


	if (pDdz->iLastTypeCount > 0)//跟牌
	{
		iLength = (pDdz->iLastTypeCount - 800) / 2;
		GetShuangShun(iCopyOnHand,iLength,pDdz->iLastMainPoint);
	}
	else//如果不是跟牌
	{
		for (iLength = 3; iLength <=11; iLength++)
		{
			GetShuangShun(iCopyOnHand, iLength,pDdz->iLastMainPoint);
		}
	}
	return iPlaCount-iCount;
}
//H08-END


//最后修订者:江文金，最后修订时间:2018-07-21
//修正应对出牌出三顺的情况
//修复了储存三顺的错误
//已看
int RecommendCards::Help9LinkThree(struct fDdz * pDdz)
{
	int iTmpSanshun[21];
	memset(iTmpSanshun, -1, sizeof(iTmpSanshun));
	int sanshunNum = 0;
	int i, iTemp, j, k, iLength = pDdz->iLastTypeCount - 900;
	int iTempArr[12][5];

	if (CInfo->CountCards(pDdz->iOnHand) < 6)
		return 0;

	int iTypeCount = 0;
	int iCount = iPlaCount;
	if (pDdz->iLastMainPoint>= 0)//跟牌
	{
		// iTempArr[i][0]用来装点数为 i 有多少张牌，
		// iTempArr[i][1],iTempArr[i][2],iTempArr[i][3],iTempArr[i][4]用来装点数为 i 的牌的编码，
		memset(iTempArr, -1, 12 * 5 * 4);
		CInfo->SortById(pDdz->iOnHand);    //先将数组进行排序

		/*for (i = 0; pDdz->iOnHand[i] > -1; i++)
		{
			if (pDdz->iOnHand[i] < 48)
			{
				//出点数为 iCards[i]/4 的牌有多少张，用 j 来表示
				j = 1;
				while (-1 != iTempArr[pDdz->iOnHand[i] / 4][j])
					j++;

				//将编码放入iTempArr[iCards[i]/4][j]里面，因为该编码的点数为iCards[i]/4
				iTempArr[pDdz->iOnHand[i] / 4][j] = pDdz->iOnHand[i];

				//将点数为 iCards[i]/4 的牌的张数 改为 j;
				iTempArr[pDdz->iOnHand[i] / 4][0] = j;
			}
		}*/

		int new_point;
		j = 0;
		for (i = 0; pDdz->iOnHand[i] > -1; i++)
		{
			if (pDdz->iOnHand[i] < 48)
			{
				if (j == 0)
					new_point = pDdz->iOnHand[i] / 4;

				//出点数为 iOnHand[i]/4 的牌有多少张，用 j 来表示
				if (new_point = pDdz->iOnHand[i] / 4)
				{
					j++;
					//将编码放入iTempArr[iOnHand[i]/4][j]里面，因为该编码的点数为iOnHand[i]/4
					iTempArr[pDdz->iOnHand[i] / 4][j] = pDdz->iOnHand[i];
				}
				if (new_point != pDdz->iOnHand[i + 1] / 4)
				{
					//将点数为 iOnHand[i]/4 的牌的张数 改为 j;
					iTempArr[pDdz->iOnHand[i] / 4][0] = j;
					j = 0;
				}
			}
		}

		for (i = pDdz->iLastMainPoint + 1; i < 12; i++)
		{
			iTemp = iLength - 3;
			if (iTempArr[i][0] >= 3)
			{
				j = i;
				while (iTemp)
				{
					if (iTempArr[j][0] >= 3)
					{
						iTemp -= 3;
						j++;
					}
					else break;
				}
				if (sanshunNum + 1 < kPlaMax && iTemp == 0)
				{
					k = 0;
					while (k < iLength)
					{
						iTmpSanshun[k] = iTempArr[j][1];
						k++;
						iTmpSanshun[k] = iTempArr[j][2];
						k++;
						iTmpSanshun[k] = iTempArr[j][3];
						k++;
						j--;
					}
					iTmpSanshun[k] = -2;
					k++;
					iTmpSanshun[k] = -1;

					//GetLegalCards(pDdz,iTmpSanshun);
					if (GetLegalCards(pDdz, iTmpSanshun))//(jwj)
					{
						return iPlaCount - iCount;
					}
				}
			}
		}
	}

	//出牌时,直接出拆牌后三顺
	if (pDdz->iLastMainPoint < 0)//出牌
	{
		int iCopyCards[21];
		memcpy(iCopyCards, pDdz->iOnHand, sizeof(pDdz->iOnHand));

		//排除炸弹
		i = CInfo->CountCards(pDdz->iOnHand);
		int *iCards;
		if (!isSim)
		{
			iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 1);
			for (i = 0; iCards[i] != -1; i++)
			{
				if (iCards[i] != -2)
				{
					DeleteElementInCards(iCopyCards, 21, iCards[i]);
				}
			}
		}

		iCards = pSplitCards->startSplitCard(iCopyCards, 3);

		//如果有三顺，直接出
		if (iCards[0] >= 0)
		{
			j = 0;
			for (i = 0; iCards[i] != -1; i++)
			{
				if (iCards[i] == -2)
				{
					iTmpSanshun[j] = -2;
					j++;
					iTmpSanshun[j] = -1;
					GetLegalCards(pDdz, iTmpSanshun);
					j = 0;  
				}
				else{
					iTmpSanshun[j] = iCards[i];
					j++;
				}
			}
		}
	}
	return iPlaCount-iCount;
}
//H09-END


//H1001-START推荐出牌应对三顺带单
//最后修订者:江文金，最后修订时间:2018-07-21
int RecommendCards::Help10LinkThreeSingle(struct fDdz * pDdz)
{
	int iCount = iPlaCount;

	int iTmpSanshundan[40];
	int sanshundanNum = 0;
	int i, iTemp, j, k;
	int iTempArr[12][5];
	int nFlag = 0;
	int iProtect = 0;
	int iLength = (pDdz->iLastTypeCount - 1000);
	memset(iTempArr, -1, 12 * 5 * 4);
	CInfo->SortById(pDdz->iOnHand);    //先将数组进行排序
	int iCopyOnHand[21] = { -1 };

	int *iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 7); //单牌
	int iSingle = 0;
	for (int i = 0; iCards[i]!=-1; i++)
	{
		if (iCards[i] != -2)iSingle++;
	}
	if (iSingle < 2 && playerType == 1)return 0;//自己是地主且不够单牌

	int iCardCount = CInfo->CountCards(pDdz->iOnHand);

	if (iCardCount == 8)
	{
		iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 3);
		if (iCards[0] != -1)
		{
			int iTmp[10];
			memset(iTmp, -1, sizeof(iTmp));
			int i;
			for (i = 0; pDdz->iOnHand[i] != -1; i++)
			{
				iTmp[i] = pDdz->iOnHand[i];
			}
			iTmp[i++] = -2;
			iTmp[i] = -1;
			if (GetLegalCards(pDdz, iTmp))
			{
				return iPlaCount - iCount;
			}
		}	
	}

	memcpy(iCopyOnHand, pDdz->iOnHand, sizeof(pDdz->iOnHand));//拷贝函数

	//判断炸弹,并排除
	iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 1);

	//排除炸弹
	for (i = 0; iCards[i] != -1; i++)
	{
		if (iCards[i] != -2)
		{
			DeleteElementInCards(iCopyOnHand, 21, iCards[i]);
		}
	}
	int *iOnHand = iCopyOnHand;

	//自己出牌
	if (iLength < 0)
	{
		iLength = 8;
		i = CInfo->CountCards(pDdz->iOnHand);

		iCards = pSplitCards->startSplitCard(iOnHand, 3); //判断三顺
		if (iCards[0] >= 0)
		{
			int tmax = 0;//最大三顺数
			j = 0;
			for (i = 0; iCards[i] != -1; i++)
			{
				if (iCards[i] == -2)
				{
					if (j > tmax)
					{
						tmax = j;
					}
					j = 0;
				}
				else
					j++;//牌张数
			}
			iLength = tmax + tmax / 3;
		}
	}

	int new_point;
	j = 0;
	for (i = 0; pDdz->iOnHand[i] > -1; i++)
	{
		if (pDdz->iOnHand[i] < 48)//小于二
		{
			if (j == 0)
				new_point = pDdz->iOnHand[i] / 4;

			//出点数为 iOnHand[i]/4 的牌有多少张，用 j 来表示
			if (new_point = pDdz->iOnHand[i] / 4)
			{
				j++;
				//将编码放入iTempArr[iOnHand[i]/4][j]里面，因为该编码的点数为iOnHand[i]/4
				iTempArr[pDdz->iOnHand[i] / 4][j] = pDdz->iOnHand[i];
			}
			if (new_point != pDdz->iOnHand[i + 1] / 4)
			{
				//将点数为 iOnHand[i]/4 的牌的张数 改为 j;
				iTempArr[pDdz->iOnHand[i] / 4][0] = j;
				j = 0;
			}
		}
	}

	CInfo->SortById(iOnHand);    //先将数组进行排序
	for (i = pDdz->iLastMainPoint + 1; i < 12; i++)
	{
		iTemp = iLength;
		if (iTempArr[i][0] >= 3)
		{
			j = i;
			while (iTemp)
			{
				if (iTempArr[j][0] >= 3)
				{
					iTemp -= 4;
					j++;
				}
				else break;
			}
			if (iTemp == 0)
			{
				k = 0;
				j--;
				while (k < iLength / 4 * 3)
				{
					iTmpSanshundan[k] = iTempArr[j][1];
					k++;
					iTmpSanshundan[k] = iTempArr[j][2];
					k++;
					iTmpSanshundan[k] = iTempArr[j][3];
					k++;
					j--;
				}

			
				iCards = pSplitCards->startSplitCard(iOnHand, 7);
				for (int e = 0; iCards[e]!=-1 ; e++)
				{
					if (iCards[e] != -2&&iCards[e]<48)//小于二的单牌
					{
						iTmpSanshundan[k] = iCards[e];//6 7  7 8
						k++;
						if (k == iLength)
						{
							iTmpSanshundan[k] = -2;
							k++;
							iTmpSanshundan[k] = -1;
							if (GetLegalCards(pDdz, iTmpSanshundan))
							{
								sanshundanNum++;
							}
							k = 0;
							break;
						}
					}
				}
				//如果没有单牌，拆对子    
				iCards = pSplitCards->startSplitCard(iOnHand, 6);
				for (int e = 0; iCards[e] != -1; e++)
				{
					if (iCards[e] == -2)
					{
						iTmpSanshundan[k] = iCards[e-1];
						k++;
						iTmpSanshundan[k] = iCards[e-2];
						k++;
						if (k == iLength)
						{
							iTmpSanshundan[k] = -2;
							k++;
							iTmpSanshundan[k] = -1;
							if (GetLegalCards(pDdz, iTmpSanshundan))
							{
								sanshundanNum++;
							}
							k = 0;
							break;
						}
					}
				}
			}
		}
	}
	return iPlaCount-iCount;
}
//H1001-END

//TODO 最后一手牌的时候可以带二
//H1002-START推荐出牌应对三顺带双
//最后修订者:江文金，最后修订时间:2018-07-21
int RecommendCards::Help10LinkThreeDouble(struct fDdz * pDdz)
{
	int iCount = iPlaCount;
	int iCardsNum = CInfo->CountCards(pDdz->iOnHand);
	int *iCards;

	if (iCardsNum == 10)
	{
		iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 3);//判断三顺
		if (iCards[0] != -1)
		{
			iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 6);//判断对牌
			if (iCards[3] != -1)
			{
				int iTmp[12];
				memset(iTmp, -1, sizeof(iTmp));
				int i;
				for (i = 0; pDdz->iOnHand[i]!=-1; i++)
				{
					iTmp[i] = pDdz->iOnHand[i];
				}
				iTmp[i++] = -2;
				iTmp[i++] = -1;
				if (GetLegalCards(pDdz, iTmp))
				{
					return iPlaCount - iCount;
				}
			}
		}
	}


	iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 7);
	int iSingle = 0;

	for (int i = 0; iCards[i]!=-1; i++)
	{
		if (iCards[i] != -2&& iSingle<48)iSingle++;//计算小于二的单牌数
	}
	if (iSingle >= 2)return 0;//出三顺带单啊   


	//拆了对子后相比原来多了一张及以上单牌，不搞这个，防止3445567这种情况拆顺子
	if(deleteTypeLeftSingleCount(pDdz->iOnHand, pSplitCards->startSplitCard(pDdz->iOnHand, 6)) >= 1)
		return 0;

	int iTmpSanshunshuang[70];
	int sanshunshuangNum = 0;
	int i, iTemp, j, k, m, iLength = (pDdz->iLastTypeCount - 1000);
	int iTempArr[12][5];
	int nFlag = 0;
	int iCopyOnHand[21] = { -1 };
	int bombPoint[4] = { -1, -1, -1, -1 };//保存炸弹点数
	int t = 0;//炸弹数
	int point = -1;//添加

	iCards = pSplitCards->startSplitCard(pDdz->iOnHand, 1);
	j = 0;
	if (iCards[0] > -1)
	{
		for (i = 0; iCards[i] != -1; i++)
		{
			if (iCards[i] == -2)
			{
				bombPoint[j] = iCards[i - 1] / 4;
				j++;
			}
		}
	}
	//排除炸弹和王
	j = 0;
	for (i = 0; pDdz->iOnHand[i] != -1; i++)
	{
		if (pDdz->iOnHand[i] / 4 != bombPoint[0] 
			&& pDdz->iOnHand[i] / 4 != bombPoint[1] 
			&& pDdz->iOnHand[i] / 4 != bombPoint[2]
			&& pDdz->iOnHand[i] / 4 != bombPoint[3])
		{
			if (pDdz->iOnHand[i] < 52)
			{
				iCopyOnHand[j] = pDdz->iOnHand[i];
				j++;
			}
		}
	}

	iCopyOnHand[j] = -1;

	int *iOnHand = iCopyOnHand;

	auto GetSanShunDouble = [&](int Length)
	{
		for (i = pDdz->iLastMainPoint + 1; i <12; i++)
		{
			iTemp = iLength;
			if (iTempArr[i][0] >= 3)
			{
				j = i;
				while (iTemp)
				{
					if (iTempArr[j][0] >= 3)
					{

						iTemp -= 5;
						j++;
					}
					else break;
				}
				if (iTemp == 0)
				{
					k = 0;
					j--;
					while (k < iLength / 5 * 3)
					{
						iTmpSanshunshuang[k] = iTempArr[j][1];
						k++;
						iTmpSanshunshuang[k] = iTempArr[j][2];
						k++;
						iTmpSanshunshuang[k] = iTempArr[j][3];
						k++;
						j--;
					}

					for (j = 0; pDdz->iOnHand[j] >= 0; j++)
					{
						nFlag = 1;
						for (m = 0; m < k; m++)
						{
							if (iOnHand[j] / 4 == iTmpSanshunshuang[m] / 4)
							{
								nFlag = 0;
								break;
							}
							if (iOnHand[j] / 4 == 12 && iHandNum > 2)//手数大于4时，不考虑带对二
							{
								nFlag = 0;
								break;
							}
							if (iOnHand[j] / 4 >= 13)//不能带王
							{
								nFlag = 0;
								break;
							}
							if (iOnHand[j] / 4 == bombPoint[0] || iOnHand[j] / 4 == bombPoint[1])//不能带炸弹
							{
								nFlag = 0;
								break;
							}
						}
						if (sanshunshuangNum + 1<kPlaMax && nFlag)
						{
							if (iOnHand[j] / 4 == point)     //添加(jwj)
								continue;
							if (iOnHand[j] / 4 != iOnHand[j + 1] / 4)
								continue;
							point = iOnHand[j] / 4;
							iTmpSanshunshuang[k] = iOnHand[j];
							k++;
							iTmpSanshunshuang[k] = iOnHand[j + 1];
							k++;
							if (k == iLength)
							{
								iTmpSanshunshuang[k] = -2;
								k++;
								iTmpSanshunshuang[k] = -1;
								GetLegalCards(pDdz, iTmpSanshunshuang);
								//break;
							}
						}
					}
				}
			}
		}
	};
	
	memset(iTempArr, -1, 12 * 5 * 4);

	CInfo->SortById(iOnHand);    //先将数组进行排序

	int new_point;
	j = 0;
	for (i = 0; pDdz->iOnHand[i] > -1; i++)
	{
		if (pDdz->iOnHand[i] < 48)
		{
			if (j == 0)
				new_point = pDdz->iOnHand[i] / 4;

			//出点数为 iOnHand[i]/4 的牌有多少张，用 j 来表示
			if (new_point = pDdz->iOnHand[i] / 4)
			{
				j++;
				//将编码放入iTempArr[iOnHand[i]/4][j]里面，因为该编码的点数为iOnHand[i]/4
				iTempArr[pDdz->iOnHand[i] / 4][j] = pDdz->iOnHand[i];
			}
			if (new_point != pDdz->iOnHand[i + 1] / 4)
			{
				//将点数为 iOnHand[i]/4 的牌的张数 改为 j;
				iTempArr[pDdz->iOnHand[i] / 4][0] = j;
				j = 0;
			}
		}
	}

	if (pDdz->iLastMainPoint < 0)
	{
		iLength = 10;
		int i = CInfo->CountCards(pDdz->iOnHand);
		
		iCards = pSplitCards->startSplitCard(iOnHand, 3);
		//如果有三顺，直接出排除炸弹和王的最大三顺
		int maxSanshun = 6;//最大三顺数
		if (iCards[0] >= 0)
		{

			j = 0;
			for (i = 0; iCards[i] != -1; i++)
			{
				if (iCards[i] == -2)
				{
					if (j > maxSanshun)
					{
						maxSanshun = j;
					}
					j = 0;
				}
				else
					j++;//牌张数
			}

			iLength = maxSanshun + 2 * (maxSanshun / 3);
		}

		while (iLength >= 10) 
		{
			GetSanShunDouble(iLength);
			maxSanshun -= 3;
			if (maxSanshun>0)
			iLength = maxSanshun + 2  * (maxSanshun / 3);
		}
	}
	else
	{
		GetSanShunDouble(iLength);
	}

	//int lastMinPoint = CInfo->AnalyzeMainPoint(pDdz->iLastOnTable);

	return iPlaCount-iCount;
}
//H1002-END*/1

//H1101-START推荐出牌应对四带二单
//最后修订者:成文杰，最后修订时间:2017-07-17
int RecommendCards::Help11FourSingle(struct fDdz * pDdz)
	{
		int Cards = CInfo->CountCards(pDdz->iOnHand);

		if (Cards<6)
			return 0;

		if (pDdz->iLastMainPoint < 0)//自己出牌
		{
			//如果是出的第一手牌或者手数大于4或者手牌数大于等于17时,退出
			if (Cards >= 17 || iHandNum > 4)
				return 0;
		}
		else//跟牌
		{
			if (judgeFriend(pDdz))
				return 0;
		}

		int iCount = iPlaCount;
		int iTmpBomb[38];
		int iFourSingle[38];

		memset(iTmpBomb, -1, sizeof(iTmpBomb));
		memset(iFourSingle, -1, sizeof(iFourSingle));

		CInfo->SortById(pDdz->iOnHand);

		int iCopyCards[21];

		memcpy(iCopyCards, pDdz->iOnHand, sizeof(pDdz->iOnHand));

		int*iCards = pSplitCards->startSplitCard(iCopyCards, 1);

		if (iCards[0] == -1)//没有炸弹
			return 0;

		if (deleteTypeLeftSingleCount(iCopyCards, iCards) >= 4)//拆了炸弹后多了四张及以上的单牌，不出
			return 0;

		iCards = pSplitCards->startSplitCard(iCopyCards, 1);
		//存储炸弹 并且，删掉炸弹
		int i;
		for (i = 0; iCards[i] != -1; i++)
		{
			iTmpBomb[i] = iCards[i];
			if (iCards[i] != -2)
			{
				DeleteElementInCards(iCopyCards, 21, iCards[i]);
			}
		}
		iTmpBomb[i] = -1;

		if (pDdz->iLastMainPoint < 0)//自己出牌
		{
			if (iHandNum <= 4)
			{
				int count = 0;
				for (i = 0; iCopyCards[i] != -1; i++)
				{
					if (iCopyCards[i] >= 48)
						count++;
				}
				if (count >= 2)return 0;//如果剩下的牌有两张及以上牌点大时，不考虑出四带二；
			}
		}

		iCards = pSplitCards->startSplitCard(iCopyCards, 7);
		if (CInfo->CountCards(iCards)<2)//只有一个单牌或者没有
			return 0;

		int index = 0;
		for (int i = 0; iTmpBomb[i] != -1; i++)//只取最小的两个单牌
		{
			if (iTmpBomb[i] == -2)
			{
				iFourSingle[index++] = iTmpBomb[i - 4];
				iFourSingle[index++] = iTmpBomb[i - 3];
				iFourSingle[index++] = iTmpBomb[i - 2];
				iFourSingle[index++] = iTmpBomb[i - 1];
				iFourSingle[index++] = iCards[0];
				iFourSingle[index++] = iCards[2];
				iFourSingle[index++] = -2;
				iFourSingle[index++] = -1;
			}
		}

		if (GetLegalCards(pDdz, iFourSingle))
			return iPlaCount - iCount;

		return iPlaCount - iCount;
	}
//H1101-END

//H1102-START推荐出牌应对四带二对
//最后修订者:成文杰，最后修订时间:2017-07-17
int RecommendCards::Help11FourDouble(struct fDdz * pDdz)
{
	int Cards = CInfo->CountCards(pDdz->iOnHand);

	if (Cards < 8)
		return 0;

	if (pDdz->iLastMainPoint < 0)//自己出牌
	{
		if (Cards >= 17 || iHandNum > 4)//如果是出的第一手牌或者手牌不够6张,或者手数大于4时,退出
			return 0;
	}
	else//跟牌
	{
		if (judgeFriend(pDdz))return 0;
	}
	
	int iCount = iPlaCount;
	int iTmpBomb[38];
	int iTmpDouble[38];
	int iFourDouble[38];

	memset(iTmpBomb, -1, sizeof(iTmpBomb));
	memset(iTmpDouble, -1, sizeof(iTmpDouble));
	memset(iFourDouble, -1, sizeof(iFourDouble));

	CInfo->SortById(pDdz->iOnHand);

	int iCopyCards[21];

	memcpy(iCopyCards, pDdz->iOnHand, sizeof(pDdz->iOnHand));

	int*iCards = pSplitCards->startSplitCard(iCopyCards, 1);

	if (deleteTypeLeftSingleCount(iCopyCards, iCards)>=4)//拆了炸弹后多了四张及以上的单牌，不出
		return 0;

	iCards = pSplitCards->startSplitCard(iCopyCards, 1);
	if (iCards[0] == -1)//没有四条
		return 0;

	//存储炸弹并且，删掉炸弹
	int i;
	for (i = 0; iCards[i]!=-1; i++)
	{
		iTmpBomb[i] = iCards[i];
		if (iCards[i] != -2)
		{
			DeleteElementInCards(iCopyCards, 21, iCards[i]);
		}
	}
	iTmpBomb[i] = -1;

	if (pDdz->iLastMainPoint < 0)//自己出牌
	{
		if (iHandNum <= 4)
		{
			int count = 0;
			for (i = 0; iCopyCards[i] != -1; i++)//计算大于等于2的牌数
			{
				if (iCopyCards[i] >= 48)
					count++;
			}
			if (count >= 2)return 0;//如果剩下的牌有两张及以上牌点大时，不考虑出四带二；
		}
	}


	//接下来考虑拆了对子后单牌是否会增多，即是否是顺子中的对子
	iCards = pSplitCards->startSplitCard(iCopyCards, 6);

	if (CInfo->CountCards(iCards) < 4)//只有一个对子，或没有对子
		return 0;

	if (deleteTypeLeftSingleCount(iCopyCards, iCards) >= 4)//拆了对子后多了四张及以上的单牌，不出
		return 0;

	iCards = pSplitCards->startSplitCard(iCopyCards, 6);
	//把对子存进数组
	for ( i = 0; iCards[i] != -1; i++)
	{
		iTmpDouble[i] = iCards[i];
	}
	iTmpDouble[i] = -1;

	int index = 0;
	for (int i = 0; iTmpBomb[i]!=-1; i++)//只取最小的两个对子
	{
		if (iTmpBomb[i] == -2)
		{
			iFourDouble[index++] = iTmpBomb[i - 4];
			iFourDouble[index++] = iTmpBomb[i - 3];
			iFourDouble[index++] = iTmpBomb[i - 2];
			iFourDouble[index++] = iTmpBomb[i - 1];
			iFourDouble[index++] = iTmpDouble[0];
			iFourDouble[index++] = iTmpDouble[1];
			iFourDouble[index++] = iTmpDouble[3];
			iFourDouble[index++] = iTmpDouble[4];
			iFourDouble[index++] = -2;
			iFourDouble[index++] = -1;
		}
	}
	if (GetLegalCards(pDdz, iFourDouble))
		return iPlaCount - iCount;
	return iPlaCount - iCount;
}
//H1102-END


//计算手中的单牌数 在拆掉某一牌型前后的差值（拆牌后剩下的单牌 减 拆牌前的单牌）
//！！！如果type是通过int*type=startSplitCards(Ddz*pDdz,int type) 
//(这个函数会Initdata())得到的，调用完后type会被清空！！！
//最后修订者:成文杰，最后修订时间:2017-07-17
int RecommendCards::deleteTypeLeftSingleCount(int*iCopyCard, int*type)
{
	int iCopyCards[21] = {-1};
	int iCopyType[38];
	for (int i = 0; i <38; i++)
	{
		iCopyType[i] = type[i];
	}
	for (int i = 0; i <21; i++)
	{
		iCopyCards[i] = iCopyCard[i];
	}
	int iBeforeSingle = 0;//拆牌前的单牌数
	int iAfterSingle = 0;//拆牌后剩下的单牌数

	iBeforeSingle=CInfo->CountCards(pSplitCards->startSplitCard(iCopyCards, 7));

	for (int i = 0; iCopyType[i] != -1; i++)
	{
		if (iCopyType[i] != -2)
		{
			DeleteElementInCards(iCopyCards, 21, iCopyType[i]);
		}
	}
	iAfterSingle= CInfo->CountCards(pSplitCards->startSplitCard(iCopyCards, 7));

	return iAfterSingle - iBeforeSingle;
}


//判断拆出的牌中，剩下的牌是否会产生很多单牌，如果不会，判断是否合法，如果合法加入可行解数组
//该函数会调用deleteTypeLeftSingleCount（） 因此拆牌函数返回的数组会被清空
//最后修订者:成文杰，最后修订时间:2017-07-21
int RecommendCards::judgeTypeAndAddInArr(fDdz *pDdz, int*iCards)
{
	int iCount = iPlaCount;
	int iTmp[38];
	memset(iTmp, -1, sizeof(iTmp));
	int iTmpIndex = 0;
	int iCopyType[38];
	memset(iCopyType, -1, sizeof(iCopyType));

	int i;
	for (i = 0; iCards[i] != -1; i++)
	{
		iCopyType[i] = iCards[i];
	}
	iCopyType[i] = -1;

	for ( i = 0; iCopyType[i] != -1; i++)
	{
		if (iCopyType[i] == -2)
		{
			iTmp[iTmpIndex++] = -2;
			iTmp[iTmpIndex++] = -1;
			iTmpIndex = 0;
			if (deleteTypeLeftSingleCount(pDdz->iOnHand, iTmp)< 2)//筛选不会拆掉顺子的，并判断是否合法
			{
				GetLegalCards(pDdz, iTmp);
			}
			memset(iTmp, -1, sizeof(iTmp));
		}
		else
		{
			iTmp[iTmpIndex++] = iCopyType[i];
		}
	}
	return iPlaCount - iCount;
}

//获取玩家类型，1为AI是地主，2为下家是地主,3为上家是地主
//最后修订者:成文杰，最后修订时间:2017-7-13
//审核合格
int RecommendCards::GetPlayerType(fDdz *pDdz)
{
	if (pDdz->cDir == pDdz->cLandlord)
		return 1;
	else if (pDdz->cLastPlay == pDdz->cLandlord)
		return 3;
	else
		return 2;
}

//计算其他玩家手中剩牌数
//（未被调用）
int RecommendCards::OtherPlayersCards(fDdz *pDdz, Ddz *rDdz)
{
	int i;
	//如果是AI
	if (pDdz->cDir == rDdz->cDir)
	{
		pDdz->iUpPlayerCards = rDdz->iUpPlayerCards;//上手玩家手牌数
		pDdz->iDownPlayerCards = rDdz->iDownPlayerCards;//下手玩家手牌数
	}
	//如果是AI上手玩家
	else if (pDdz->cDir == rDdz->cLastPlay)
	{

		for (i = 0; rDdz->iOnHand; i++);
		pDdz->iUpPlayerCards = rDdz->iDownPlayerCards;//上手玩家手牌数
		pDdz->iDownPlayerCards = i;
	}
	//如果是AI下手玩家
	else
	{
		for (i = 0; rDdz->iOnHand; i++);
		pDdz->iUpPlayerCards = i;//上手玩家手牌数
		pDdz->iDownPlayerCards = rDdz->iUpPlayerCards;
	}
	return 0;
}

//判断前面两家是否PASS两次
bool RecommendCards::IfPassTwice(fDdz *pDdz)
{
	if (pDdz->iLastTypeCount <= 0)//等于0时为两家Pass
	{
		return true;
	}
	return false;
}


//DEBUG:所有这里面的判断都不能简单解决首次出牌时的随意出牌问题
//BUG
// 配合SpitCard的返回格式拆出牌型
// 从iCards里面分离出能打过上家出牌的解,保存到参数iSave里面,iSave必须初始化为-1
//最后修订者:范晓梦，最后修订时间:16-07-28 23:50
//修订内容：增加判断出牌时面对的情况
//2016/8/8:增加判断炸弹，火箭情况
//2016/8/10 :取消传入带牌主牌可以判断主牌的语句，避免混淆
int RecommendCards::GetLegalCards(fDdz *pDdz, int *iCards, int *iSave)
{
	//对牌、三条、单顺、双顺、三顺以最大牌张比较大小；三带一、三顺带牌、四带二仅按其中三条、三顺、四条的牌张比较大小，与带牌大小无关。
	//火箭大于炸弹，火箭及炸弹均大于其他牌型，炸弹之间按牌张大小进行比较。
	int iCardsTmp[50];
	int t = 0;//用于指定iCards下标
	int start;//每个牌型开始的下标
	int iNodesCount = 0;

	//把数组改成“xxx -2 xxxx -2 -1”结构
	int num;
	for (num = 0; iCards[num] != -1; num++);
	if (iCards[num - 1] != -2)
	{
		iCards[num] = -2;
		iCards[num + 1] = -1;
	}

	for (int i = 0; iCards[i] != -1; i++)
	{
		start = 0;
		if (iCards[i] == -2)
		{
			iCardsTmp[t] = -1;
			CInfo->SortById(iCardsTmp);
			int iTypeCount = CInfo->AnalyzeTypeCount(iCardsTmp);
			int iType = iTypeCount / 100;						//得到牌型	
			int iCardsCount = iTypeCount % 100;					//得到牌数
			int myPoint = 0;

			if (pDdz->iLastMainPoint == -1 || pDdz->iLastTypeCount / 100 == iType)//两家都pass或者桌面最新的牌型和我的相等
			{
				if (pDdz->iLastTypeCount % 100 == iCardsCount || pDdz->iLastMainPoint == -1)
				{
					iCardsTmp[iCardsCount] = -1;
					//如果合法,添加到iSave
					myPoint = CInfo->AnalyzeMainMaxPoint(iCardsTmp);		//分析iCard的最大点数
					if (iLastMaxPoint<myPoint)
					{
						iCardsTmp[t] = -1;
						AppendCards(iCardsTmp, iSave);
						iNodesCount++;
					}
				}
			}

			//如果出炸弹且上家不是炸弹也不是火箭
			if (iType == 2 && pDdz->iLastTypeCount / 100>2)
			{
				iCardsTmp[t] = -1;
				AppendCards(iCardsTmp, iSave);
				iNodesCount++;
			}

			//如果火箭
			if (iType == 1)
			{
				iCardsTmp[t] = -1;
				AppendCards(iCardsTmp, iSave);
				iNodesCount++;
			}
			t = 0;
		}
		else
		{
			iCardsTmp[t] = iCards[i];
			++t;
		}
	}
	return iNodesCount;
}

//传入单个套牌信息并判断是否合法，如果合法，加入iplaarr
int RecommendCards::GetLegalCards(fDdz *pDdz, int *iCards)
{
	//对牌、三条、单顺、双顺、三顺以最大牌张比较大小；三带一、三顺带牌、四带二仅按其中三条、三顺、四条的牌张比较大小，与带牌大小无关。
	//火箭大于炸弹，火箭及炸弹均大于其他牌型，炸弹之间按牌张大小进行比较。

	int iCount = iPlaCount;				//本次模拟出牌生成器产生的多种出牌可行解数量(jwj)
	int iCardsTmp[30];
	int t = 0;				//用于指定iCards下标
	int iTypeCount;
	int iType;
	int iCardsCount;
	int myPoint;
	for (int i = 0; iCards[i] != -1; i++)
	{
		/////////处理数据//////////////////
		if (iCards[i] == -2)// 等于-2时，
		{
			iCardsTmp[t] = -1;
			CInfo->SortById(iCardsTmp);						//按牌编码升序排列
			iTypeCount = CInfo->AnalyzeTypeCount(iCardsTmp);//牌型
				iType = iTypeCount / 100;
			iCardsCount = iTypeCount % 100;//牌数
			myPoint = CInfo->AnalyzeMainMaxPoint(iCardsTmp);//分析牌型的最大点数
			iCardsTmp[t] = -2;
			t++;
			iCardsTmp[t] = -1;
			t = 0;
			////////////判断////////////////////
			if (pDdz->iLastTypeCount / 100 == iType)
			{
				if (pDdz->iLastTypeCount % 100 == iCardsCount)//跟牌必须牌数相等
				{
					if (iLastMaxPoint < myPoint)
					{
						AddInPlaArr(pDdz, iCardsTmp);
					}
				}
			}
			if (pDdz->iLastMainPoint == -1)//如果两家PASS
			{
				if (iTypeCount > 0)
				{
					AddInPlaArr(pDdz, iCardsTmp);
				}
			}

			//如果出炸弹且上家不是炸弹也不是火箭
			if (iType == 2 && pDdz->iLastTypeCount / 100 > 2)
			{
				AddInPlaArr(pDdz, iCardsTmp);
			}
			//如果火箭
			if (iType == 1)
			{
				AddInPlaArr(pDdz, iCardsTmp);
			}
		}
		else
		{
			iCardsTmp[t] = iCards[i];
			++t;
		}
	}

	return iPlaCount - iCount;
}


//把一套分好牌型的牌解析出牌存储在IPLARR数组里面 
//TODO:修改局面信息
//最后修订者:成文杰，最后修订时间:17-07-16 
//修订内容：增加节点限制
int RecommendCards::AddInPlaArr(fDdz *pDdz, int *iCards)
{
	for (int i = 0; iCards[i] != -1; i++)   //判断牌是否在手中
	{
		if (iCards[i] != -2)
		{
			bool isIn = false;
			for (int j = 0; pDdz->iOnHand[j] != -1; j++)
			{
				if (iCards[i] == pDdz->iOnHand[j])//判断是否在手牌中 
				{
					isIn = true;
					break;
				}
			}
			if (!isIn)                      //如果牌没有在手牌中，返回结果
				return iPlaCount;
		}
	}

	int num = iPlaCount;
	int t = 0;//第二维数组下标
	if (iCards[0] != -2)                //如果有牌，返回true
	{
		for (int i = 0; iCards[i] != -1; i++)
		{
			if (iCards[i] != -2)
			{
				//如果节点数已经到达上限，直接返回    //方法达到上限
				if (num + 2 > PlaMax)
					return num;
				pDdz->iPlaArr[num][t] = iCards[i];
				t++;
			}
			else
			{
				pDdz->iPlaArr[num][t] = -1;
				t = 0;
				num++;         //num为方法索引
			}
		}
	}
	else//PASS
	{
		//如果节点数已经到达上限，直接返回
		if (num + 2 > PlaMax)
			return num;
		pDdz->iPlaArr[num][t] = -2;
		t++;
		pDdz->iPlaArr[num][t] = -1;
		num++;
	}
	iPlaCount = num;
	return num;
}

// 解析拆牌类产生的牌并存在iSave里面
//（未被调用）
int RecommendCards::AnalyzeSplitedCards(int* iCards, int *iSave)
{
	int t = 0;//用于指定iCards下标
	for (int i = 0; iCards[i] != -1; i++)
	{

		if (iCards[i] != -2)
		{
			iSave[t] = iCards[i];
			++t;
		}
	}
	iSave[t] = -1;
	return t;
}


//将from数组加到to数组后面，to数组分隔符为-2，结束符为-1
//从结束符开始写入数据，并以-2，-1结尾
void RecommendCards::AppendCards(int* from, int* to)
{
	int i;
	//读到to数组-1部分
	for (i = 0; to[i] != -1; i++);
	int j = i;

	for (i = 0; from[i] != -1; i++, j++)
	{
		to[j] = from[i];
	}

	//改变后缀
	if (to[j - 1] != -2)
	{
		to[j] = -2;
		to[j + 1] = -1;
	}
}

//计算手牌最小手数
int RecommendCards::MinHandNum(int* iOnHand)
{
	int iCopyOnHand[21];
	for (int i = 0; i < 21; i++)
	{
		iCopyOnHand[i] = iOnHand[i];
	}
	int num = 0;//手数
	int t = 0;
	int j = 0;
	int bestStart = 0;
	int i = 0;
	int iTmpCards[21];

	memset(iTmpCards, -1, sizeof(iTmpCards));

	//从拆牌数组中，找寻最大牌张数的牌型,并从icopyHand中排除,并增加手数
	auto findMaxNumCards = [&](int *iSplitedCards)
	{
		if (iSplitedCards[0] > -1)
		{
			t = 0;//最大牌张数
			j = 0;
			bestStart = 0;//最大牌张起始位置
			for (i = 0; iSplitedCards[i] != -1; i++)
			{
				if (iSplitedCards[i] == -2)
				{
					if (j > t)
					{
						t = j;
						bestStart = i - t;
					}
					j = 0;
				}
				else
					j++;//计算牌张数
			}
			t = 0;
			for (i = bestStart; iSplitedCards[i] != -2; i++)
			{
				iTmpCards[t] = iSplitedCards[i];
				t++;
			}
			iTmpCards[t] = -1;
			num++;

			//从iCopyOnHand中排除
			if (iTmpCards[0] > -1)
			{
				for (i = 0; iTmpCards[i] != -1; i++)
				{
					DeleteElementInCards(iCopyOnHand,21, iTmpCards[i]);
				}
			}
		}
	};

	//判断牌型数并排除,并增加手数  
	auto DeleteTypeCards = [&](int *iCards)
	{
		if (iCards[0] > -1)
		{
			t = 0;
			for (i = 0; iCards[i] != -1; i++)
			{
				if (iCards[i] == -2)
				{
					iTmpCards[t] = -1;
					for (j = 0; iTmpCards[j] != -1; j++)
					{
						DeleteElementInCards(iCopyOnHand, 21,iTmpCards[j]);//从后边排除顺序来看，不会排除多余的
					}
					num++;
					t = 0;
					iTmpCards[0] = -1;
				}
				else
				{
					iTmpCards[t] = iCards[i];
					t++;
				}
			}
		}
	};

	//首先判断火箭
	int *iCards = pSplitCards->startSplitCard(iOnHand, 0);
	if (iCards[0] > -1)
	{
		DeleteElementInCards(iCopyOnHand, 21, 52);
		DeleteElementInCards(iCopyOnHand, 21, 53);	
		num++;
	}

	int iSingleNum = 0;
	int iThreeNum = 0;
	int iDoubleNum = 0;
	
	//判断单牌并删除
	iCards = pSplitCards->startSplitCard(iCopyOnHand, 7);
	for (int i = 0;iCards[i]!=-1; i++)
	{
		if (iCards[i] != -2)
		{
			iSingleNum++;
		}
	}
	DeleteTypeCards(iCards);

	//判断单顺,找到所有单顺,并排除
	iCards = pSplitCards->startSplitCard(iCopyOnHand, 4);
	DeleteTypeCards(iCards);

	//判断炸弹
	iCards = pSplitCards->startSplitCard(iOnHand, 1);
	DeleteTypeCards(iCards);

	//记录三条的数量
	iCards = pSplitCards->startSplitCard(iCopyOnHand, 2);
	for (int i = 0; iCards[i] != -1; i++)
	{
		if (iCards[i] == -2)
		{
			iThreeNum++;
		}
	}

	//找到所有三顺,并排除
	iCards = pSplitCards->startSplitCard(iCopyOnHand, 3);
	DeleteTypeCards(iCards);
	//判断三条，并删除
	iCards = pSplitCards->startSplitCard(iCopyOnHand, 2);
	DeleteTypeCards(iCards);

	//判断对子
	iCards = pSplitCards->startSplitCard(iCopyOnHand, 6);
	for (int i = 0; iCards[i] != -1; i++)
	{
		if (iCards[i] != -2)
		{
			iDoubleNum++;
		}
	}

	//判断双顺,找到所有双顺,并排除
	iCards = pSplitCards->startSplitCard(iCopyOnHand, 5);
	DeleteTypeCards(iCards);

	iCards = pSplitCards->startSplitCard(iCopyOnHand, 6);
	DeleteTypeCards(iCards);

	int iTmpNum = 0;
	if (iThreeNum != 0)
	{
		if (iSingleNum>=iThreeNum)
		{
			num -= iThreeNum;
		}
		else if (iSingleNum < iThreeNum)
		{
			num -= iSingleNum;
			iTmpNum = iThreeNum - iSingleNum;
		}
		if (iDoubleNum >= iTmpNum)
		{
			num -= iTmpNum;
		}
		else if (iDoubleNum < iTmpNum)
		{
			num -= iDoubleNum;
		}
	}

	//判断王(剩余牌必为王或2，拆牌不清楚)
	for (i = 0; iCopyOnHand[i] != -1; i++)
	{
		if (iCopyOnHand[i] >= 48)
			num++;
	}
	return num;
}

//获得模型
bool RecommendCards::GetModel(Ddz * realDdz)
{
	pRealDdz = realDdz;
	pMoldel->GetOtherPokersCards(iUpCards, iDownCards, pRealDdz);

	if (iFShowInConsole) 
	{
		//上家的牌
		cout << "猜测上家的牌为 " << endl;
		printCards(iUpCards);
	
		//下家的牌
		cout << "猜测下家的牌为 " << endl;
		printCards(iDownCards);

		cout << "其他两家的牌为 ";
		printCards(pRealDdz->iOnOtherHand);
	}
	
	//分析手牌数
	int i = CInfo->CountCards(iUpCards);
	int j = CInfo->CountCards(iDownCards);
	if(pRealDdz->iUpPlayerCards != i || pRealDdz->iDownPlayerCards != j)
		return false;
	CInfo->SortById(iUpCards);
	CInfo->SortById(iDownCards);
	return true;
}

//将牌打印出来
void printCards(int*temp) {

	int j;
	int a = 0;
	for (int i = 0; temp[i] != -1; i++)
		a++;
	cout << "  ";
	//cout <<"还有"<< a<<"张牌 ";
	for (int pass = a; pass >0; pass--)    //从小到大排序
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
			cout << 4 << " ";    //4
		else if (temp[i] / 4 == 2)
			cout << 5 << " ";    //5
		else if (temp[i] / 4 == 3)
			cout << 6 << " ";    //6
		else if (temp[i] / 4 == 4)
			cout << 7 << " ";    //7
		else if (temp[i] / 4 == 5)
			cout << 8 << " ";    //8
		else if (temp[i] / 4 == 6)
			cout << 9 << " ";    //9
		else if (temp[i] / 4 == 7)
			cout << 10 << " ";   //10
		else if (temp[i] / 4 == 8)
			cout << " J ";    //J
		else if (temp[i] / 4 == 9)
			cout << " Q ";    //Q
		else if (temp[i] / 4 == 10)
			cout << " K ";    //K
		else if (temp[i] / 4 == 11)
			cout << " A ";    //A
		else if (temp[i] / 4 == 12)
			cout << 2 << " ";    //2
		else if (temp[i] == 52)
			cout << " 小王 ";    //小王
		else if (temp[i] == 53)
			cout << " 大王 ";    //大王
	}
	cout << endl << endl;
}


//不利局势返回false,非不利返回true
bool RecommendCards::AnalyzeCondition(fDdz* pDdz)
{
	int sort = 0;//因素
	//从手牌数入手
	if (pDdz->cLandlord == pDdz->cDir)
	{
		if (pDdz->iUpPlayerCards <= 6 || pDdz->iDownPlayerCards <= 6)
			sort++;
	}
	else//AI不是地主
	{
		//上家是地主且牌数小于等于6
		if (pDdz->cLastPlay == pDdz->cLandlord&&pDdz->iUpPlayerCards <= 6)
			sort++;
		//上家不是地主（即下家为地主），下家牌数小于等于6
		if (pDdz->cLastPlay != pDdz->cLandlord&&pDdz->iDownPlayerCards <= 6)
			sort++;
	}

	//从手数入手
	if (pDdz->cLandlord == pDdz->cDir)
	{
		if (iUpHandNum <= 3 || iDownHandNum <= 3)
			sort++;
		if (iUpHandNum < iHandNum&&iDownHandNum < iHandNum)
			sort++;
	}
	else//AI不是地主
	{
		//上家是地主，且上家的手数小于等于3
		if (pDdz->cLastPlay == pDdz->cLandlord&&iUpHandNum <= 3)
			sort++;
		if (pDdz->cLastPlay != pDdz->cLandlord&&iDownHandNum <= 3)
			sort++;
	}
	//从牌价值入手
	int MyValue = CardsValue(pDdz->iOnHand);
	int UpValue = CardsValue(iUpCards);
	int DownValue = CardsValue(iDownCards);
	if (pDdz->cLandlord == pDdz->cDir)
	{
		if (MyValue <UpValue&& MyValue <DownValue)
			sort++;
	}
	else
	{
		if (pDdz->cLastPlay == pDdz->cLandlord&&UpValue >MyValue&&UpValue>DownValue)
			sort++;
		if (pDdz->cLastPlay != pDdz->cLandlord&&DownValue >MyValue&&DownValue >UpValue)
			sort++;
	}
	if (sort >= 2)
		return false;
	else
		return true;
}

//从iCards[]里剔除数值为iElement的牌
void RecommendCards::DeleteElementInCards(int* iCards, int iCardsCount, int iElement)
{
	int i, j;
	i = j = 0;
	for (j = 0; j < iCardsCount; j++)
	{
		if (iCards[j] == iElement)
			i++;
		else
			iCards[j - i] = iCards[j];
	}
}

//得到最大的顺子长度，不考虑是否可以拆牌
//最后修订者:江文金，最后修订时间:2018-07-21
int RecommendCards::AnalyzeSingleLink(int Length, int iLastMainPoint, int *Cards, int* iTmpShunzi)
{
	if (Length < 5||Length>12) //不合法
		return 0;

	int i, j, k;
	int iCards[21];
	for (i = 0; i < 21; i++)
	{
		iCards[i] = Cards[i];
	}

	for (int i = 0; iCards[i] != -1; i++) //去掉2和小王   
	{
		if (iCards[i] >= 48)
		{
			iCards[i] = -1;
		}
	}

	//去除重牌并从小到大排序
	CInfo->GetUnReaptedCards(iCards);

	int iBiaoji, iFlag, num ; 
	num = 0;

	for (i = 0; iCards[i + Length - 1] >= 0; i++)
	{
		num = 0;//牌张数
		iBiaoji = 0;
		iFlag = 0;

		//如果大于于上一张牌的最小点数
		if (iCards[i] / 4 > iLastMainPoint)//必须要全部牌都大于上一个玩家的最小点数
		{
			for (j = i; j < Length - 1 + i; j++)
			{
				//如果与后面的牌点不连序，标记并退出循环
				if (iCards[j] / 4 != iCards[j + 1] / 4 - 1)
				{
					iBiaoji = 1;
					break;
				}
			}
		}
		else
			continue;

		if (iBiaoji == 0)
		{
			for (k = i; k < Length + i; k++)
			{
				if (iCards[k] < 0)
					break;

				iTmpShunzi[iFlag] = iCards[k];
				iFlag++;

				if (k == Length - 1 + i)
				{
					num = iFlag;
					iTmpShunzi[iFlag++] = -2;
					iTmpShunzi[iFlag++] = -1;
					return num;
				}
			}
		}
	}

	return num;
}


int RecommendCards::CardsValue(int* iCards)
{
	int i;
	int dSum = 100;			//估值
	for (i = 0; iCards[i] >= 0; i++)
	{
		dSum = dSum - 5;			//手牌越少越好每多一张牌优势减5分
		if (i >= 1 && iCards[i - 1] / 4 == iCards[i] / 4)
			dSum = dSum + 2;	//对子加2分
		if (i >= 4 && iCards[i - 4] / 4 <= 7
			&& iCards[i - 4] / 4 + 1 == iCards[i - 3] / 4
			&& iCards[i - 3] / 4 + 1 == iCards[i - 2] / 4
			&& iCards[i - 2] / 4 + 1 == iCards[i - 1] / 4
			&& iCards[i - 1] / 4 + 1 == iCards[i] / 4)
			dSum = dSum + 6;	//2以下相邻五牌单顺加6分
	}
	return dSum;
}
