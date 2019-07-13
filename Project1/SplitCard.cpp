#include "SplitCard.h"

/*
*  
* 最后修订者：杨丽君&&林小惠 2017.7.13
*/
SplitCard::SplitCard()
{
	iCards = NULL;//外部编码

	//返回的结果
	iResult[0] = iRocket;//火箭	
	iResult[1] = iBomb;	//炸弹
	iResult[2] = iThree;//三条
	iResult[3] = iLinkThree;//三顺
	iResult[4] = iLinkSingle;//单顺
	iResult[5] = iLinkDouble;//双顺
	iResult[6] = iDouble;//对子
	iResult[7] = iSingle;//单牌
}

SplitCard::~SplitCard()
{
}

SplitCard* SplitCard::Instance()
{
	static SplitCard instance;

	return &instance;
}

//初始化数据
void SplitCard::initData()
{
	for (int k = 0; k < 4; k++)
	{
		iRocket[k] = 0;//火箭
	}
	for (int j = 0; j < 38; j++)
	{
		iMyCards[j] = 0;//内部编码
		iWaitOrder[j] = 0;//待连牌的牌
		iOrder[j] = 0;//已连牌的牌
		iMinLinkIndex[j] = 0;//最小连数的连牌

		iSingleInRepeatIndex[j] = -1;//重牌里的单牌的索引
		iSingleInLinkIndex[j] = -1;//顺子里的单牌的索引
		iLinkIndex[j] = -1;//顺子的索引

		iBomb[j] = 0;//炸弹
		iSingle[j] = 0;//单牌
		iDouble[j] = 0;//对牌
		iThree[j] = 0;//三条
		iLinkSingle[j] = 0;//单顺
		iLinkDouble[j] = 0;//双顺
		iLinkThree[j] = 0;//三顺
	}
	for (int i = 0; i < 26; i++)
	{
		iRepeat[i] = i % 2;//重牌  0-25  0 1 0 1 0 1...
	}
}

//整理牌型，标准编码与内部编码的索引相同（ok）
void SplitCard::transcode()
{
	for (int i = 0; i < 38; i++)
	{
		if (iCards[i] == -1)
		{
			iMyCards[i] = -1;
			break;
		}
		if (iCards[i] / 4 == 0)
			iMyCards[i] = 3;    //3
		if (iCards[i] / 4 == 1)
			iMyCards[i] = 4;    //4
		if (iCards[i] / 4 == 2)
			iMyCards[i] = 5;    //5
		if (iCards[i] / 4 == 3)
			iMyCards[i] = 6;    //6
		if (iCards[i] / 4 == 4)
			iMyCards[i] = 7;    //7
		if (iCards[i] / 4 == 5)
			iMyCards[i] = 8;    //8
		if (iCards[i] / 4 == 6)
			iMyCards[i] = 9;    //9
		if (iCards[i] / 4 == 7)
			iMyCards[i] = 10;    //10
		if (iCards[i] / 4 == 8)
			iMyCards[i] = 11;    //J
		if (iCards[i] / 4 == 9)
			iMyCards[i] = 12;    //Q
		if (iCards[i] / 4 == 10)
			iMyCards[i] = 13;    //K
		if (iCards[i] / 4 == 11)
			iMyCards[i] = 14;    //A
		if (iCards[i] / 4 == 12)
			iMyCards[i] = 22;    //2
		if (iCards[i] == 52)
			iMyCards[i] = 66;    //小王
		if (iCards[i] == 53)
			iMyCards[i] = 99;    //大王
	}
}


//判断重牌（ok）
void SplitCard::judgeRepeat()
{
	for (int i = 0, j = 0, k = 0; iMyCards[i] != -1; i++)
	{
		if (iMyCards[i] == iMyCards[i + 1])
		{
			iRepeat[j] = iMyCards[i + 1];
			iRepeat[j + 1]++;
		}
		else
		{
			j += 2;
			//搜索无重复的单牌
			if (iMyCards[i] != iMyCards[i - 1] && iMyCards[i] != iMyCards[i + 1])
			{
				iSingleInRepeatIndex[k] = i;
				k++;
			}
		}
	}
}

//判断连牌（ok）
void SplitCard::judgeOrder()
{
	int iLinkEnd = iWaitOrder[0];//连牌中的最后一个牌值
	int iLinkEndIndex = 0;//一组连牌的开始索引
	int iEndIndex = -1;//结束符“-1”的索引
	bool bStartLock = true;//是否是一组连牌的标志

	//开始搜索连牌
	for (int i = 1, j = 1; iWaitOrder[i] != -1; i++, j++)
	{
		//一组连牌的开始
		if (bStartLock)
		{
			//开始的两位是否相连
			if (iLinkEnd + 1 == iWaitOrder[i])
			{
				iOrder[iLinkEndIndex] = iLinkEnd;
				iOrder[j] = iWaitOrder[i];
				iLinkEnd = iOrder[j];
			}
			//开始的两位是否相等
			else if (iLinkEnd == iWaitOrder[i])
			{
				iOrder[iLinkEndIndex] = 0;
				iOrder[j] = iWaitOrder[i];
			}
			//开始的两位是否相隔
			else if (iLinkEnd + 1 < iWaitOrder[i])
			{
				iOrder[iLinkEndIndex] = 0;
				iOrder[j] = iWaitOrder[i];
				iLinkEnd = iOrder[j];
			}
			//开始的两位的第二位为0
			else
			{
				iOrder[j] = 0;
			}
			//开锁
			if (iWaitOrder[i] != 0)
			{
				bStartLock = false;
			}
		}
		else
		{
			//与后一位相连
			if (iLinkEnd + 1 == iWaitOrder[i])
			{
				iOrder[j] = iWaitOrder[i];
				iLinkEnd = iOrder[j];
			}
			//与后一位相隔，一组连牌结束
			else if (iLinkEnd + 1 < iWaitOrder[i])
			{
				iLinkEndIndex = i;
				iLinkEnd = iWaitOrder[i];
				bStartLock = true;
			}
			//与后一位相等，或后一位为0
			else
			{
				iOrder[j] = 0;
			}
		}
		iEndIndex = j + 1;//结束符的计算
	}
	if (iEndIndex == -1)
	{
		iEndIndex = 0;
	}
	iOrder[iEndIndex] = -2;
	iEndIndex++;
	iOrder[iEndIndex] = -1;
}

//选取最小连数的牌（ok）
void SplitCard::judgeMinLink(int iMinLink)
{
	int iLinkEnd = 0;//连牌中的最后一个牌值
	int iFiveIndex[] = { -1, -1, -1, -1, -1 };//五连牌的索引
	int iIndex = 0;//最小五连牌的保存索引
	int iLinkNum = 0;//连牌的数量
	int iIsCircle = -1;//是否结束最小五连牌搜索的标志

	//循环搜索最小五连牌
	while (iIsCircle != iIndex)
	{
		//首先在WaitOrder中搜索连牌
		judgeOrder();
		//将上次最小五连的结束索引保留
		iIsCircle = iIndex;
		//Order起始位置特殊处理
		if (iOrder[0] != 0)
		{
			iLinkEnd = iOrder[0] - 1;
		}
		//其次在Order中搜索最小五连牌
		for (int i = 0; iOrder[i] != -1; i++)
		{
			//找到满足条件的五连牌
			if (iLinkNum == iMinLink)
			{
				for (int k = 0; k < iMinLink; k++)
				{
					iMinLinkIndex[iIndex] = iFiveIndex[k];
					iWaitOrder[iFiveIndex[k]] = 0;
					iFiveIndex[k] = -1;
					iIndex++;
				}
				iMinLinkIndex[iIndex] = -2;
				iIndex++;
				iLinkNum = 0;
				iLinkEnd = 0;
				break;
			}
			//正在搜索满足条件的五连牌
			else
			{
				//相连
				if (iOrder[i] != 0 && iLinkEnd + 1 == iOrder[i])
				{
					iFiveIndex[iLinkNum] = i;
					iLinkEnd = iOrder[i];
					iLinkNum++;
				}

				//一组连牌结束，但小于五连
				else if (iLinkEnd + 1 < iOrder[i])
				{
					for (int k = iLinkNum; k >= 0; k--)
					{
						iFiveIndex[k] = -1;//连牌不成立，五连牌的索引重新全令为-1
					}
					iLinkNum = 0;
					iLinkEnd = iOrder[i];
					iFiveIndex[iLinkNum] = i;
					iLinkNum++;
				}
				//第一位为0，或第一二位相等
				else
				{
					//第一位为0，但第二位不为0
					if (iLinkEnd == 0)
					{
						iLinkEnd = iOrder[i + 1] - 1;
					}
				}
			}
		}
	}
	//结束符
	iMinLinkIndex[iIndex] = -1;
}

//判断顺子（ok）
void SplitCard::judgeLink(int iMinLink)
{
	int iLinkExtendIndex[12] = { 0 };//扩展的连牌，以每组五连的结束索引为起始符，-2为结束符
	int iWriteIndex = -1;
	int iReadIndex = 0;
	bool bIsExtendLink = false;
	int iLinkSingleIndex = 0;

	judgeMinLink(iMinLink);

	//搜索剩余牌值
	for (int i = 0; iWaitOrder[i] != -1; i++)
	{
		if (iWaitOrder[i] != 0)
		{
			//搜索是否可以扩展
			//扩展是否结束，如果结束就在MinLinkIndex中搜索，否则在LinkExtendIndex中继续扩展
			if (!bIsExtendLink)
			{
				for (int j = iMinLink - 1; iMinLinkIndex[j] != -1; j = j + iMinLink + 1)
				{
					if (iMinLinkIndex[j] != 0 && iMyCards[iMinLinkIndex[j]] + 1 == iWaitOrder[i])
					{
						iWriteIndex++;
						iLinkExtendIndex[iWriteIndex] = iMinLinkIndex[j];
						iWriteIndex++;
						iLinkExtendIndex[iWriteIndex] = i;
						iWaitOrder[i] = 0;
						bIsExtendLink = true;
					}
				}
			}
			else
			{
				for (int j = i; iWaitOrder[j] != -1; j++)
				{
					if (iMyCards[iLinkExtendIndex[iWriteIndex]] + 1 == iWaitOrder[j])
					{
						iWriteIndex++;
						iLinkExtendIndex[iWriteIndex] = j;
						iWaitOrder[j] = 0;
						i = j;
					}
					else if (iMyCards[iLinkExtendIndex[iWriteIndex]] + 1 < iWaitOrder[j] || iWaitOrder[j + 1] == -1)
					{
						iWriteIndex++;
						iLinkExtendIndex[iWriteIndex] = -2;
						i = j - 1;
						bIsExtendLink = false;
						break;
					}
					else
					{
						continue;
					}
				}
			}
		}
		if (iWaitOrder[i + 1] == -1)
		{
			iWriteIndex++;
			iLinkExtendIndex[iWriteIndex] = -2;
		}
	}

	//合并连牌
	for (int j = 0; iMinLinkIndex[j] != -1; iLinkSingleIndex++, j++)
	{
		//读取五连牌
		if (iMinLinkIndex[j] != -2)
		{
			iLinkIndex[iLinkSingleIndex] = iMinLinkIndex[j];
		}
		//读取扩展牌
		else
		{
			//可扩展
			if (iLinkExtendIndex[iReadIndex] != 0 && iLinkExtendIndex[iReadIndex] == iMinLinkIndex[j - 1])
			{
				iReadIndex++;
				iLinkIndex[iLinkSingleIndex] = iLinkExtendIndex[iReadIndex];
				iReadIndex++;
				iLinkSingleIndex++;
				while (iLinkExtendIndex[iReadIndex] != -2)
				{
					iLinkIndex[iLinkSingleIndex] = iLinkExtendIndex[iReadIndex];
					iReadIndex++;
					iLinkSingleIndex++;
				}
				iLinkIndex[iLinkSingleIndex] = -2;
				iReadIndex++;
			}
			//不可扩展
			else
			{
				iLinkIndex[iLinkSingleIndex] = -2;
			}
		}
	}
	//结束符
	iLinkIndex[iLinkSingleIndex] = -1;
	//判断最后的顺子是否可以再合并
	for (int k = 0; iLinkIndex[k] != -1; k++)
	{
		if (iLinkIndex[k] == -2)
		{
			if (iMyCards[iLinkIndex[k - 1]] + 1 == iMyCards[iLinkIndex[k + 1]])
			{
				for (int l = k; iLinkIndex[l] != -1; l++)
				{
					iLinkIndex[l] = iLinkIndex[l + 1];
				}
			}
		}
	}
}

//判断单顺（ok）
void SplitCard::judgeLinkSingle()
{
	int iEndIndex = -1;//结束符“-1”的索引

	//初始化WaitOrder
	for (int l = 0; l < 38; l++)
	{
		iWaitOrder[l] = iMyCards[l];
	}

	judgeLink(5);//在顺子中进行筛选

	//将顺子筛选后的单牌进行存储
	for (int i = 0, j = 0; iWaitOrder[i] != -1; i++)
	{
		if (iWaitOrder[i] > 0)
		{
			iSingleInLinkIndex[j] = i;
			j++;
			iWaitOrder[i] = 0;
		}
	}

	//存储单顺外部编码
	for (int k = 0; iLinkIndex[k] != -1; k++)
	{
		if (iLinkIndex[k] != -2)
		{
			iLinkSingle[k] = iCards[iLinkIndex[k]];
		}
		else
		{
			iLinkSingle[k] = iLinkIndex[k];
		}
		iEndIndex = k + 1;
	}
	//防止无单顺时数组越界
	if (iEndIndex == -1)
	{
		iEndIndex++;
	}
	iLinkSingle[iEndIndex] = -1;
}

//修改单顺的BUG（ok）
void SplitCard::OptimizeLinkSingle() 
{

	judgeLinkSingle();

	int iLinkIndex[10];
	int j = 1, i = 0;
	iLinkIndex[0] = 0;
	//				0 1 2  3  4  5  6  7  8 9 10  11  12...
	//记录单顺首尾下标 1 2 3  4  5  -2  6  7  8 9 10  -2  -1
	//	iLinkIndex	0 4 -2 6  10 -2  -1
	for (i; iLinkSingle[i] != -1; i++)
	{
		if (iLinkSingle[i] == -2)
		{
			iLinkIndex[j] = i - 1;
			iLinkIndex[j + 1] = -2;
			if (iLinkSingle[i + 1] != -1)
			{
				iLinkIndex[j + 2] = i + 1;
				j = j + 3;
			}
			else
			{
				j = j + 2;
				iLinkIndex[j] = -1;
			}

		}
	}

	//	iLinkIndex[j] = -1;
	judgeThree();
	//				0 1 2  3  4  5  6  7  8 9 10  11  12...
	//记录单顺首尾下标 1 2 3  4  5  6  -2 7  8 9 10  11  12 -2  -1
	//	iLinkIndex	0 5 -2 7  12 -2  -1
	if (iLinkSingle[0] != -1)//如果有一个顺子及以上
	{
		for (int i = 0; iLinkIndex[i] != -1; i = i + 3)
		{
			for (int k = 0; iThree[k] != -1; k = k + 4)
			{
				if (iLinkSingle[iLinkIndex[i + 1]] / 4 - iLinkSingle[iLinkIndex[i]] / 4 >= 5)//首
				{
					if ( iLinkSingle[iLinkIndex[i]] / 4 == iThree[k] / 4)//如果六顺及以上的首位包含了三条，删掉
					{
						for (int j = iLinkIndex[i]; iLinkSingle[j] != -1; j++)
						{
							iLinkSingle[j] = iLinkSingle[j + 1];//去掉下标为 iLinkIndex[i]的牌编码
						}
						iLinkIndex[i + 1]--;
					}
					else if ( iLinkSingle[iLinkIndex[i + 1]] / 4 == iThree[k] / 4)//尾
					{
						
						for (int j = iLinkIndex[i + 1]; iLinkSingle[j] != -1; j++)
						{
							iLinkSingle[j] = iLinkSingle[j + 1];//去掉下标为 iLinkIndex[i+1]的牌编码
						}
						iLinkIndex[i + 1]--;
					}
				}
				
			}
		}
	}

	if (iLinkSingle[0] == -1)return;

	//以下查找是否存在单牌没加进顺子

	/*int iCopyCard[21];
	memset(iCopyCard, -1, sizeof(iCopyCard));
	for (i = 0; iCards[i]!=-1; i++)
	{
		if(iCards[i]<48)
			iCopyCard[i] = iCards[i];
	}

	iCopyCard[i] = -1;

	int iLinkCount = 0;
	int iOneLinkNum = 0;

	judgeBomb();
	for (int j = 0; iBomb[j] != -1; j++)
	{
		if (iBomb[j] != -2)
		{
			for (int i = 0; iCopyCard[i] != -1; i++)
			{
				if (iBomb[j] == iCopyCard[i])//删掉炸弹
				{
					for (int k = i; iCopyCard[k] != -1; k++)
					{
						iCopyCard[k] = iCopyCard[k + 1];
					}
					break;
				}
			}
		}
	}
	judgeLinkDouble();
	for (int j = 0; iLinkDouble[j] != -1; j++)
	{
		if (iLinkDouble[j] != -2)
		{
			for (int i = 0; iCopyCard[i] != -1; i++)
			{
				if (iLinkDouble[j] == iCopyCard[i])//删掉双顺
				{
					for (int k = i; iCopyCard[k] != -1; k++)
					{
						iCopyCard[k] = iCopyCard[k + 1];
					}
					break;
				}
			}
		}
	}
	 for (int j = 0; iLinkSingle[j]!=-1; j++)
		{
			if (iLinkSingle[j] != -2) 
			{
				for (int i = 0; iCopyCard[i] != -1; i++)
				{
					if (iLinkSingle[j] == iCopyCard[i])//删掉顺子
					{
						for (int k = i; iCopyCard[k] != -1; k++)
						{
							iCopyCard[k] = iCopyCard[k + 1];
						}
						break;
					}
				}
			}
		}



		int iDelShunziSingle[21] = {-1};
		int iCount = 0;
		for (i = 0; iCopyCard[i] != -1; i++)
		{
			if (iCopyCard[i] != iCopyCard[i - 1] && iCopyCard[i] != iCopyCard[i + 1])
			{
				iDelShunziSingle[iCount++] = iCopyCard[i];
			}
		}


		for (i = 0; iLinkSingle[i] != -1; i++);
			i=i-2;
		for (int k = 0; iCopyCard[k]!=-1; k++)
		{
			if (iCopyCard[k]/4 == iLinkSingle[i]/4+1)
			{
				i++;
				iLinkSingle[i] = iCopyCard[k];
			}
		}
		i++;
		iLinkSingle[i] = -2;
		i++;
		iLinkSingle[i] = -1;*/
}

//判断火箭(ok)
void SplitCard::judgeRocket()
{
	int j = 0;
	for (int i = 0; iMyCards[i] != -1; i++)
	{
		if (iMyCards[i] == 66 || iMyCards[i] == 99)
		{
			iRocket[j] = iCards[i];
			j++;
		}
	}
	iRocket[j] = -2;
	j++;
	iRocket[j] = -1;
	//只有一张王
	if (iRocket[1] == -2)
	{
		iRocket[0] = -1;
		iRocket[1] = 0;
	}
}

//判断炸弹（ok）
void SplitCard::judgeBomb()
{
	int iBombValue = 0;
	int iCount = 0;
	int iIndex = 0;

	for (int i = 1; i < 26; i += 2)
	{
		if (iRepeat[i] == 4)
		{
			iBombValue = iRepeat[i - 1];
			for (int j = 0; iMyCards[j] != -1; j++)
			{
				if (iBombValue == iMyCards[j])
				{
					iBomb[iIndex] = iCards[j];
					iCount++;
					iIndex++;
				}
				if (iCount == 4)
				{
					iBomb[iIndex] = -2;
					iIndex++;
					iCount = 0;
					break;
				}
			}
		}
	}
	iBomb[iIndex] = -1;
}

//拆除其他牌型后的对子
//判断对牌（ok）
void SplitCard::judgeDouble()
{
	int iDoubleValue = 0;
	int iCount = 0;
	int iIndex = 0;


	for (int i = 1; i < 26; i += 2)
	{
		if (iRepeat[i] == 2)
		{
			iDoubleValue = iRepeat[i - 1];
			for (int j = 0; iMyCards[j] != -1; j++)
			{
				if (iDoubleValue == iMyCards[j])
				{
					iDouble[iIndex] = iCards[j];
					iCount++;
					iIndex++;
				}
				if (iCount == 2)
				{
					iDouble[iIndex] = -2;
					iIndex++;
					iCount = 0;
					break;
				}
			}
		}
	}
	iDouble[iIndex] = -1;
}

//判断双顺（ok）
void SplitCard::judgeLinkDouble()
{
	int iIndex = 0;
	int iDoubleValue = 0;
	int iLastWaitOrderIndex = -1;

	//初始化WaitOrder
	for (int k = 1; k < 26; k += 2)
	{
		if (iRepeat[k] >= 2)
		{
			iDoubleValue = iRepeat[k - 1];
			for (int l = 0; iMyCards[l] != -1; l++)
			{
				if (iDoubleValue == iMyCards[l])
				{
					iWaitOrder[l] = iMyCards[l];
					iLastWaitOrderIndex = l + 1;
					break;
				}
			}
		}
	}
	if (iLastWaitOrderIndex == -1)
	{
		iLastWaitOrderIndex = 1;
	}
	iWaitOrder[iLastWaitOrderIndex] = -1;

	judgeLink(3);

	for (int i = 0; iLinkIndex[i] != -1; i++)
	{
		if (iLinkIndex[i] != -2)
		{
			for (int j = 0; iMyCards[j] != -1; j++)
			{
				if (iMyCards[iLinkIndex[i]] == iMyCards[j])
				{
					iLinkDouble[iIndex] = iCards[j];
					iIndex++;
					iLinkDouble[iIndex] = iCards[j + 1];
					iIndex++;
					break;
				}
			}
		}
		else
		{
			iLinkDouble[iIndex] = -2;
			iIndex++;
		}
	}
	iLinkDouble[iIndex] = -1;
}

//判断三条（ok）
void SplitCard::judgeThree()
{
	int iThreeValue = 0;
	int iCount = 0;
	int iIndex = 0;

	for (int i = 1; i < 26; i += 2)
	{
		if (iRepeat[i] == 3)
		{
			iThreeValue = iRepeat[i - 1];
			for (int j = 0; iMyCards[j] != -1; j++)
			{
				if (iThreeValue == iMyCards[j])
				{
					iThree[iIndex] = iCards[j];
					iCount++;
					iIndex++;
					if (iCount == 1)
					{
						iWaitOrder[j] = iMyCards[j];
					}
				}
				if (iCount == 3)
				{
					iThree[iIndex] = -2;
					iIndex++;
					iCount = 0;
					break;
				}
			}
		}
	}
	iThree[iIndex] = -1;
}

//判断三顺（ok）
void SplitCard::judgeLinkThree()
{
	judgeThree();//先将三条放进iThree里
	int iIndex = 0;//iLinkThree的下标
	int iLinkIndex[8];//记录相领的三条在iLinkThree数组的下标；
	int iLinkCount= 0;

	if (iThree[4] == -1||iThree[4]==0)//如果只有一个三条或者没有三条，返回；
	{
		iLinkThree[0] = -1;
		return;
	}
	else 
	{
		for (int i = 0; i < 8; i++)//初始化数组
		{
			iLinkIndex[i] = -1;
		}
		//012,3,456,7,8910,11,121314,15
		for (int i = 4; iThree[i]!=-1 ; i=i+4)//每个三条在iThree中开始的下标为0，4，8，12...
		{
			if (iThree[i - 4] / 4 + 1 == iThree[i]/4&& iThree[i] / 4!=12)//如果两个三条开始的下标相差1，即是三顺
			{
				if (iLinkCount == 0)			//如果还没有记录过三顺的下标
				{
					iLinkIndex[iLinkCount++] = i - 4;
					iLinkIndex[iLinkCount++] = i;
				}
				else
				{
					if (iLinkIndex[iLinkCount - 1] == i - 4)//如果是连续的三顺，例如3334444  444555联合起来变成333444555
					{
						iLinkIndex[iLinkCount++] = i;
					}
					else
					{
						iLinkIndex[iLinkCount++] = -2;
						iLinkIndex[iLinkCount++] = i - 4;
						iLinkIndex[iLinkCount++] = i;
					}
				}
					
			}
		}
	}
	
	for (int i = 0; iLinkIndex[i]!=-1; i++) //用记录的下标读取iThree中牌的编码存进iLinkThree中
	{
		if (iLinkIndex[i] != -2)
		{
			iLinkThree[iIndex++] = iThree[iLinkIndex[i]];
			iLinkThree[iIndex++] = iThree[iLinkIndex[i]+1];
			iLinkThree[iIndex++] = iThree[iLinkIndex[i]+2];
		}
		else if (iLinkIndex[i] == -2)
		{
			iLinkThree[iIndex++] = -2;
		}
	}
	iLinkThree[iIndex++] = -2;
	iLinkThree[iIndex++] = -1;
}

//将顺子判断后的单牌
//判断单牌（ok）
void SplitCard::judgeSingle()
{

int iIndex = 0;

	OptimizeLinkSingle();

	int iCopyCards[21];//复制iCard;

	int i = 0;
	
	for (i; iCards[i]!=-1; i++)
	{
		iCopyCards[i] = iCards[i];
	}
	iCopyCards[i] = -1;

	for (int j = 0; iResult[4][j] != -1; j++)//iResult[4]为顺子
	{
		for (int i = 0; iCopyCards[i]!=-1; i++)
		{
			//如果和顺子中的牌相等，删除
			if (iResult[4][j] == iCopyCards[i])
			{
				//iCopyCards[i] = -1;
				for (int k = i; iCopyCards[k]!=-1; k++)
				{
					iCopyCards[k] = iCopyCards[k+1];
				}
				break;
			}
		}
	}

	if (iCopyCards[1] == -1&& iCopyCards[0]!=-1) //如果除去顺子后只剩下一张牌
	{
		iSingle[iIndex++] = iCopyCards[0];
	}
	else 
	{
		for (int i = 0; iCopyCards[i] != -1; i++)
		{
			if (iCopyCards[i] / 4 != iCopyCards[i + 1] / 4 && iCopyCards[i] / 4 != iCopyCards[i - 1] / 4)
			{
				iSingle[iIndex++] = iCopyCards[i];
				iSingle[iIndex++] = -2;
			}
		}
	}
	iSingle[iIndex] = -1;



	//以下计算除去连对后剩下的单牌（比较时除去连对还是单顺剩下的单牌少）
		judgeLinkDouble();//result[5]
		int delLinkDoubleLeftSingle[38] = { 0 };
		int delIndex = 0;
		for (i=0; iCards[i] != -1; i++)
		{
			iCopyCards[i] = iCards[i];
		}
		iCopyCards[i] = -1;

		for (int j = 0; iResult[5][j] != -1; j++)//iResult[5]为双顺
		{
			for (int i = 0; iCopyCards[i] != -1; i++)
			{
				//如果和双顺中的牌相等，删除
				if (iResult[5][j] == iCopyCards[i])
				{
					//iCopyCards[i] = -1;
					for (int k = i; iCopyCards[k] != -1; k++)
					{
						iCopyCards[k] = iCopyCards[k + 1];
					}
					break;
				}
			}
		}

		if (iCopyCards[1] == -1 && iCopyCards[0] != -1) //如果除去双顺后只剩下一张牌
		{
			delLinkDoubleLeftSingle[delIndex++]= iCopyCards[0];
		}
		else
		{
			for (int i = 0; iCopyCards[i] != -1; i++)
			{
				if (iCopyCards[i] / 4 != iCopyCards[i + 1] / 4 && iCopyCards[i] / 4 != iCopyCards[i - 1] / 4)
				{
					delLinkDoubleLeftSingle[delIndex++] = iCopyCards[i];
					delLinkDoubleLeftSingle[delIndex++] = -2;
				}
			}
		}
		delLinkDoubleLeftSingle[delIndex]= -1;

		int doubleAverage = 0;
		double linkSingleAverage = 0;
		int j;
		int k = 0;//计算去掉双顺后的单牌数；
		for (j=0; delLinkDoubleLeftSingle[j] != -1; j++)
		{
			//iSingle[j] = delLinkDoubleLeftSingle[j];
			if (delLinkDoubleLeftSingle[j] != -2)
			{
				doubleAverage += delLinkDoubleLeftSingle[j];
				k++;
			}
		}

		int f = 0;
		for (j = 0; iSingle[j] != -1; j++)
		{
		
			if (iSingle[j] != -2)
			{
				linkSingleAverage += iSingle[j];
				f++;
			}
		}

		if (f > k)//去掉双顺的单牌少
		{
			for (j = 0; delLinkDoubleLeftSingle[j] != -1; j++)
			{
				iSingle[j] = delLinkDoubleLeftSingle[j];
			}
		}
iSingle[j] = -1;
}


//对外接口（ok）
int** SplitCard::startSplitCard(int iInitCards[])
{
	iCards = iInitCards;
	//初始化数据
	initData();
	//防止iInitCards中无值
	if (iCards[0] == -1)
	{
		iBomb[0] = -1;
		iSingle[0] = -1;
		iDouble[0] = -1;
		iThree[0] = -1;
		iLinkSingle[0] = -1;
		iLinkDouble[0] = -1;
		iLinkThree[0] = -1;

		return iResult;
	}

	transcode();//转换编码

	judgeRepeat();//搜索重牌
	judgeRocket();//判断火箭
	judgeBomb();//判断炸弹
	//judgeLinkSingle();//判断单顺
	OptimizeLinkSingle();//修改了单顺的BUG
	judgeDouble();//判断对牌
	judgeLinkDouble();//判断双顺
	judgeThree();//判断三条
	judgeLinkThree();//判断三顺
	judgeSingle();//判断单牌

	
	return iResult;
}

//对外接口（ok）
int* SplitCard::startSplitCard(int iInitCards[], int type)
{
	iCards = iInitCards;
	//初始化数据
	initData();
	//防止iInitCards中无值
	if (iCards[0] == -1)
	{
		return iCards;
	}

	transcode();//转换编码
	judgeRepeat();//搜索重牌

	switch (type)
	{
	case 0:
		judgeRocket();//判断火箭
		return iRocket; 

	case 1:
		judgeBomb();//判断炸弹
		return iBomb;

	case 2:
		judgeThree();//判断三条
		return iThree;

	case 3:
		judgeLinkThree();//判断三顺
		return iLinkThree;

	case 4:
		//judgeLinkSingle();//判断单顺
		OptimizeLinkSingle();
		return iLinkSingle;

	case 5:
		judgeLinkDouble();//判断双顺
		return iLinkDouble;

	case 6:
		judgeDouble();//判断对牌
		return iDouble;

	case 7:
		judgeSingle();//判断单牌
		return iSingle;

	default:
		return NULL;
	}
}
