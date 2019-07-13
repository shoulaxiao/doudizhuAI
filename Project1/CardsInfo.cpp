//江文金 2018.7.16
//
#include "CardsInfo.h"

CardsInfo::CardsInfo()
{
}
CardsInfo::~CardsInfo()
{
}

CardsInfo* CardsInfo::Instance()
{
	static CardsInfo instance;

	return &instance;
}

//B00-START判断出牌类型是否为弃权
//最后修订者:梅险,最后修订时间:15-02-10
int CardsInfo::IsType0Pass(int iCs[])
{
	int iCount;
	iCount = CountCards(iCs);
	if (iCount == 0)
		return 1;
	return 0;
}
//B00-END

//B01-START判断出牌类型是否为火箭
//最后修订者:夏侯有杰&梅险,最后修订时间:15-02-10
int CardsInfo::IsType1Rocket(int iCs[])
{
	if ((iCs[2] == -1) && (iCs[0] + iCs[1] == 52 + 53))
		return 1;
	return 0;
}
//B01-END

//B02-START判断出牌类型是否为炸弹
//最后修订者:夏侯有杰&梅险,最后修订时间:15-03-10
//修订内容及目的:防止牌是空的
int CardsInfo::IsType2Bomb(int iCs[])
{
	if (4 != CountCards(iCs))
		return 0;
	if ((iCs[4] == -1) && (iCs[0] / 4 != -1 && iCs[0] / 4 == iCs[1] / 4 && iCs[0] / 4 == iCs[2] / 4 && iCs[0] / 4 == iCs[3] / 4))
		return 1;
	return 0;
}
//B02-END

//B03-START判断出牌类型是否为单牌
//最后修订者:夏侯有杰,最后修订时间:15-03-10
//修订内容及目的:if判断条件错误
int CardsInfo::IsType3Single(int iCs[])
{
	if (iCs[0] != -1 && iCs[1] == -1)
		return 1;
	return 0;
}
//B03-END

//B04-START判断出牌类型是否为对牌
//最后修订者:夏侯有杰,最后修订时间:15-02-13
int	CardsInfo::IsType4Double(int iCs[])
{
	if (IsType1Rocket(iCs))
		return 0;
	if (iCs[0] / 4 == iCs[1] / 4 && iCs[0] != -1 && iCs[2] == -1)
		return 1;
	return 0;
}
//B04-END

//B05-判断出牌类型是否为三条
//最后修订者:夏侯有杰,最后修订时间:15-02-13
int	CardsInfo::IsType5Three(int iCs[])
{
	if (iCs[0] != -1 && iCs[0] / 4 == iCs[1] / 4 && iCs[0] / 4 == iCs[2] / 4 && iCs[3] == -1)
		return 1;
	return 0;
}
//B05-END

//B0601-START判断出牌类型是否为三带一单
//最后修订者:夏侯有杰,最后修订时间:15-02-12
int CardsInfo::IsType6ThreeOne(int iCs[])
{
	if (IsType2Bomb(iCs) || 4 != CountCards(iCs))
		return 0;
	SortByMuch(iCs);///根据牌型排序
	if (iCs[0] / 4 == iCs[1] / 4 && iCs[0] / 4 == iCs[2] / 4)
		return 1;
	return 0;
}
//B0601-END

//B0602-START判断出牌类型是否为三带一对
//最后修订者:夏侯有杰,最后修订时间:15-02-12
int CardsInfo::IsType6ThreeDouble(int iCs[])
{
	if (5 != CountCards(iCs))
		return 0;
	SortByMuch(iCs);
	if (iCs[0] / 4 == iCs[1] / 4 && iCs[0] / 4 == iCs[2] / 4)
		if (iCs[3] / 4 == iCs[4] / 4)
			return 1;
	return 0;
}
//B0602-END

//B07-START判断出牌类型是否为单顺
//最后修订者:谢文,最后修订时间:15-02-12
int CardsInfo::IsType7LinkSingle(int iCs[])
{
	int iCount;
	int i;
	iCount = CountCards(iCs);
	if (iCount >= 5)
	{
		SortById(iCs);
		for (i = 1; iCs[i] >= 0; i++)
			///>=48为2 和大小王
			if (iCs[i - 1] / 4 + 1 != iCs[i] / 4 || iCs[i] >= 48)
				return 0;
		return 1;
	}
	return 0;
}
//B07-END

//B08-START判断出牌类型是否为连对
//最后修订者:谢文&夏侯有杰,最后修订时间:15-03-10
int CardsInfo::IsType8LinkDouble(int iCs[])
{
	int iCount = CountCards(iCs);
	int i;
	if (iCount < 6 || iCount % 2 != 0)
		return 0;
	SortById(iCs);   //把iCs牌排序
	for (i = 1; i < iCount; i++)
	{
		//判断i是单数还是双数
		if (i % 2)
		{
			//如果i是单数，则第i张牌的编码编码会等于上一张牌编码+1
			if (iCs[i] / 4 != iCs[i - 1] / 4)
				return 0;
		}
		else
		{
			//如果i是双数，则第i张牌的编码编码会等于上一张牌编码
			if (iCs[i] / 4 != iCs[i - 1] / 4 + 1)
				return 0;
		}
	}
	return 1;
}
//B08-END

//B09-START判断出牌类型是否为三顺
//最后修订者:范晓梦,最后修订时间:16-08-12
//修订：iTempArr数组大小出错
int	CardsInfo::IsType9LinkThree(int iCs[])
{
	int iTempArr[12] = { 0 };   //初始化数组iTempArr用来记录 3 - A 每个点数的张数
	int iCount = CountCards(iCs);
	int i, iMinNum, iMaxNum;   //iMinNum 为 iCs 最小点数,iMaxNum 为 iCs 最大点数
	
	///AAABBBCCC...型最少为6张牌 
	if (iCount < 6 || iCount % 3 != 0)
		return 0;

	SortById(iCs);   //把iCs牌排序

	iMinNum = iCs[0] / 4;
	iMaxNum = iMinNum + iCount / 3 - 1; //如果牌为333444555 那么iCount/3=3 iMinNum与iMaxNum相差 iCount / 3 - 1

	for (i = 0; i < iCount; i++)
	{
		//判断iCs[i]是否在有效点数范围内
		if (iCs[i] > 47 || iCs[i] / 4 < iMinNum || iCs[i] / 4 > iMaxNum)
		{
			return 0;
		}

		///如果在 那牌点数对应的下标的值+1
		iTempArr[iCs[i] / 4]++;
	}

	for (i = iMinNum; i <= iMaxNum; i++)
	{
		//判断是否每个有效点数都为3张牌
		if (iTempArr[i] != 3)
			return 0;
	}
	return 1;

}
//B09-END

//B1001-START判断三顺带单，返回1是，0不是
//最后修订者:夏侯有杰,最后修订时间:15-03-10
//修订内容及目的:防止44455556的出现时排序后顺序乱
int	CardsInfo::IsType10LinkThreeSingle(int iCs[])
{
	int iCount = CountCards(iCs);
	int iTempArr[18];
	int i, n, m, j, iNum, iTemp; //iNum记录有多少个 3+1

	if (iCount < 8 || iCount % 4 != 0)
		return 0;

	memset(iTempArr, -1, 18 * 4);   //初始化iTempArr，值都为-1

	iNum = iCount / 4;

	SortByMuch(iCs);   //排序

	//判断是不是有炸弹
	n = 1, m = 0;

	while (n)
	{
		for (i = m; i < m + 4; i++)
			iTempArr[i] = iCs[i];

		///Q:当m=0时上面的for循环开始只取了iCs中的前四张牌

		//判断iTempArr是不是炸弹，不是则跳出
		if (0 == IsType2Bomb(iTempArr))
		{
			n = 0;
		}
		else
		{
			//如果是的话,那么这个炸弹应该是一个顺子加一个单牌，单牌就应该放在iCs的后面
			iTemp = iCs[m];

			///44445555经过下面的for循环后变成44455554  

			for (j = m + 1; j < iCount; j++)
			{
				iCs[j - 1] = iCs[j];
				iCs[j] = iTemp;
				iTemp = iCs[j];
			}

			m = m + 3;
		}
		
		memset(iTempArr, -1, 18 * 4); 
		///void *memset(void *s, int ch, size_t n);
		///函数解释：将s中当前位置后面的n个字节 
		///（typedef unsigned int size_t ）用 ch 替换并返回 s 。
	}

	//将三顺赋值给iTempArr
	for (i = 0; i < 3 * iNum; i++)
	{
		iTempArr[i] = iCs[i];
	}

	//判断iTempArr是不是三顺
	if (1 == IsType9LinkThree(iTempArr))
	{
		//将iTempArr传回iCs   防止55544465这样的牌型出现
		for (i = 0; i < 3 * iNum; i++)
		{
			iCs[i] = iTempArr[i];
		}
		return 1;
	}
	return 0;

}
//B1001-END


//B1002-START   判断三顺带对，返回1是，0不是
//最后修订者:夏侯有杰,最后修订时间:15-02-13
int	CardsInfo::IsType10LinkThreeDouble(int	iCs[])
{
	int iCount = CountCards(iCs);

	int iTempArr[18];

	int i, n, j, k, iNum, iTemp; //iNum记录有多少个 3+2

	if (iCount < 10 || iCount % 5 != 0)
		return 0;

	memset(iTempArr, -1, 18 * 4);   //初始化iTempArr，值都为-1

	iNum = iCount / 5;

	SortByMuch(iCs);   //排序

	//判断是不是有炸弹
	n = 1;
	while (n)
	{
		for (i = 0; i < 4; i++)
		{
			iTempArr[i] = iCs[i];
		}

		//判断iTempArr是不是炸弹，不是则跳出
		if (0 == IsType2Bomb(iTempArr))
		{
			n = 0;
		}
		else
		{
			///such as.3335544455 把炸弹拆成两个对子。。。。。
			//如果是的话,那么这个炸弹应该是属于2个对子，就应该放在iCs的后面
			for (k = 0; k < 4; k++)
			{
				iTemp = iCs[0];
				/*
				3355444553
				3554445533
				5544455333
				5444553335
				4445533355
				3335544455*/
				for (j = 1; j < iCount; j++)
				{	
					iCs[j - 1] = iCs[j];
					iCs[j] = iTemp;
					iTemp = iCs[j];
				}
			}
		}
		memset(iTempArr, -1, 18 * 4);
	}

	//将三顺赋值给iTempArr
	for (i = 0; i < 3 * iNum; i++)
	{
		iTempArr[i] = iCs[i];
	}

	//判断iTempArr是不是三顺
	if (0 == IsType9LinkThree(iTempArr))
		return 0;

	//重置iTempArr
	memset(iTempArr, -1, 18 * 4);

	j = 0;
	for (i = 3 * iNum; i < iCount; i++)
	{

		iTempArr[j] = iCs[i];
		if (j == 1)
		{
			//判断是不是对子
			if (0 == IsType4Double(iTempArr))
				return 0;
			memset(iTempArr, -1, 18 * 4);
			j = 0;
		}
		else
		{
			j++;
		}
	}

	return 1;
}
//B1002-END

//B1101-START判断出牌类型是否为四带二单
//最后修订者:梅险,最后修订时间:15-02-10
int CardsInfo::IsType11FourSingle(int iCs[])
{
	int iCount;
	iCount = CountCards(iCs);

	SortByMuch(iCs);  //同点数多牌的排在前面

	if (iCount == 6)  //6=4+1+1
		if (iCs[0] / 4 == iCs[1] / 4 && iCs[0] / 4 == iCs[2] / 4 && iCs[0] / 4 == iCs[3] / 4)  //有四
			return 1;
	return 0;
}
//B1101-END

//B1102-START判断出牌类型是否为四带二对
//最后修订者:梅险,最后修订时间:15-02-10
int CardsInfo::IsType11FourDouble(int iCs[])
{
	int iCount;
	iCount = CountCards(iCs);
	SortByMuch(iCs);  //同点数多牌的排在前面
	if (iCount == 8)
		if (iCs[0] / 4 == iCs[1] / 4 && iCs[0] / 4 == iCs[2] / 4 && iCs[0] / 4 == iCs[3] / 4)  //有四
			if (iCs[4] / 4 == iCs[5] / 4 && iCs[6] / 4 == iCs[7] / 4)  //8=4+2+2
				return 1;
	return 0;
}
//B1102-END

//I03-START分析牌型和张数
//1**为火箭，2**为炸弹，3**为单牌，4**为对牌，5**为三条，6**为三带一单或一对
//11**为四带一单或一对，7**、8**、9**分别为单顺、双顺、三顺，10**为飞机。
//*为0-9的任意数
//最后修订者:梅险,最后修订时间:15-03-01 
int CardsInfo::AnalyzeTypeCount(int iCards[])   
{
	int iCount = 0;
	iCount = CountCards(iCards);

	if (IsType0Pass(iCards))
		return 0 * 100 + iCount;

	if (IsType1Rocket(iCards))
		return 1 * 100 + iCount;

	if (IsType2Bomb(iCards))
		return 2 * 100 + iCount;

	if (IsType3Single(iCards))
		return 3 * 100 + iCount;

	if (IsType4Double(iCards))
		return 4 * 100 + iCount;

	if (IsType5Three(iCards))
		return 5 * 100 + iCount;

	if (IsType6ThreeOne(iCards))
		return 6 * 100 + iCount;

	if (IsType6ThreeDouble(iCards))
		return 6 * 100 + iCount;

	if (IsType11FourSingle(iCards))
		return 11 * 100 + iCount;

	if (IsType11FourDouble(iCards))
		return 11 * 100 + iCount;

	if (IsType7LinkSingle(iCards))
		return 7 * 100 + iCount;

	if (IsType8LinkDouble(iCards))
		return 8 * 100 + iCount;

	if (IsType9LinkThree(iCards))
		return 9 * 100 + iCount;

	if (IsType10LinkThreeSingle(iCards))
		return 10 * 100 + iCount;

	if (IsType10LinkThreeDouble(iCards))
		return 10 * 100 + iCount;


	return -1;
}
//I03-END

//I04-START计算主牌最小点数
//最后修订者:梅险,最后修订时间:15-03-01
int	CardsInfo::AnalyzeMainPoint(int iCards[])
{
	if (IsType0Pass(iCards))
		return -1;

	SortByMain(iCards);

	return iCards[0] / 4;
}
//I04-END



//计算主牌最大点数
//最后修订者:范晓梦，最后修订时间:16-08-08
//修订内容：单牌中大王点数比小王点数大

int CardsInfo::AnalyzeMainMaxPoint(int iCards[])
{
	if (IsType0Pass(iCards))
		return -1;

	int iTypeCount = AnalyzeTypeCount(iCards);

	int iCardsCount = iTypeCount % 100;

	SortByMuch(iCards);

	//或炸弹或火箭
	if (iTypeCount / 100 == 1 || iTypeCount / 100 == 2)
	{
		return iCards[0] / 4;
	}

	//如果是四带二
	if (1100 < iTypeCount &&iTypeCount< 1200)
	{
		//如果是带单牌
		if (IsType11FourSingle(iCards))
		{
			for (int i = 0; i < iCardsCount; i++)
			{
				if (iCards[i] / 4 == iCards[i + 1] / 4)
				{
					//找到连序起点
					return iCards[i] / 4; 
				}
			}
			return -1;
		}

		else if (IsType11FourDouble(iCards))
		{
			int iRepeat = 0;
			for (int i = 0; i < iCardsCount; i++)
			{

				if (iCards[i] / 4 == iCards[i + 1] / 4)
				{
					//找到连序起点
					iRepeat++;
				}
				else
				{
					iRepeat = 0;
				}
				if (iRepeat >= 2)
				{
					return iCards[i] / 4;
				}
			}
			return -1;
		}
	}
	//单牌，
	if (iTypeCount / 100 == 3)
	{
		//如果是大王
		if (iCards[0] == 53)
			return iCards[0] / 4 + 1;
		return iCards[0] / 4;
	}
	//对牌，三条,
	if (400 < iTypeCount &&iTypeCount< 600)
	{
		for (int i = 0; i < iCardsCount; i++)
		{
			if (iCards[i] / 4 == iCards[i + 1] / 4)
			{
				//找到连序起点
				return iCards[i] / 4;
			}
		}
		return -1;
	}
	//三带一
	if (600< iTypeCount &&iTypeCount< 700)
	{
		//如果三带单
		if (IsType6ThreeOne(iCards))
		{
			for (int i = 0; i < iCardsCount; i++)
			{
				if (iCards[i] / 4 == iCards[i + 1] / 4)
				{
					//找到连序起点
					return iCards[i] / 4;
				}
			}
		}
		//如果三带双
		else if (IsType6ThreeDouble(iCards))
		{
			int iRepeat = 0;
			for (int i = 0; i < iCardsCount; i++)
			{
				if (iRepeat >= 2)
				{
					return iCards[i] / 4;
				}
				if (iCards[i] / 4 == iCards[i + 1] / 4)
				{
					//找到连序起点
					iRepeat++;
				}
			}
		}
		return -1;
	}
	//如果是顺子：单顺、双顺、三顺
	//顺子的关键在于连续
	if (700 < iTypeCount &&iTypeCount< 1000)
	{
		int i = 0;
		for (i = 0; iCards[i] > -1; i++);
		return iCards[i - 1] / 4;
	}
	//如果是三顺带牌
	if (10 * 100 < iTypeCount&&iTypeCount < 11 * 100)
	{

		//如果是三顺带单
		//设有x单牌，则iCardsCount=x+3*x;则x=iCardsCount/4;
		//所以三顺有3*x=3*(iTypeCount/400)=0.0075*iTypeCount;
		if (IsType10LinkThreeSingle(iCards))
		{
			//三顺一定是连序的
			int count = 3 * (iCardsCount / 4);//三顺牌张数
			for (int i = 0; i < iCardsCount; i++)
			{
				if (iCards[i] / 4 == iCards[i + 1] / 4)
				{
					//找到连序起点
					i = i + count - 1;
					return iCards[i] / 4;
				}
			}
			return -1;

		}
		//如果是三顺带双
		//设有x对对牌，2*x+3*x=iCardsCount x=iCardsCount /5

		if (IsType10LinkThreeDouble(iCards))
		{
			int count = 3 * (iCardsCount / 5);//三顺牌张数
			for (int i = 0; i < iCardsCount; i++)
			{
				if (iCards[i] / 4 == iCards[i + 1] / 4)
				{
					//找到连序起点
					i = i + count - 1;
					return iCards[i] / 4;
				}
			}
		}
	}
	return -1;
}


//I01-START计算牌数
//增加计算 ***-2****-2-1型数组的牌数
//最后修订者:江文金,最后修订时间:18-07-20
int CardsInfo::CountCards(int iCards[])
{
	/*int iCount = 0;
	for (int i = 0; iCards[i] != -1; i++)
	{
		if (iCards[i] != -2 && iCards[i] != -1)
			iCount++;
	}
	return iCount;*/

	int iCount = 0;
	for (int i = 0; iCards[i]>=-2; i++)
	{
		if (iCards[i] != -2&&iCards[i]!=-1)
		{
			iCount++;
		}
		if (iCards[i] == -1)break;
	}
	return iCount;
}
//I01-END



//MO1-START按牌编码升序排列
//最后修订者:夏侯有杰&梅险,最后修订时间:15-02-09
void CardsInfo::SortById(int iCards[])
{
	int i, j;
	int iTemp;
	///冒泡排序，将牌从小到大排序
	for (i = 0; iCards[i] >= 0; i++)
	{
		for (j = i + 1; iCards[j] >= 0; j++)
			if (iCards[i]>iCards[j])
			{
				iTemp = iCards[i];
				iCards[i] = iCards[j];
				iCards[j] = iTemp;
			}
	}
}
//MO1-END

//M02-START按主牌(同点数牌)编码升序排列
//最后修订者:夏侯有杰,最后修订时间:2015-03-01
////由牌数多到少排序

void CardsInfo::SortByMuch(int iCards[])
{
	int i, j, k, n;
	// iTempArr[i][0]用来装点数为 i 有多少张牌，
	// iTempArr[i][1],iTempArr[i][2],iTempArr[i][3],iTempArr[i][4]用来装点数为 i 的牌的编码，
	
	int iTempArr[13][5];
	memset(iTempArr, -1, 13 * 5 * 4);

	SortById(iCards);    //先将iCards数组进行排序

	for (i = 0; iCards[i] > -1; i++)
	{
		///除去大小王
		if (iCards[i] < 52)
		{
			//出点数为 iCards[i]/4 的牌有多少张，用 j 来表示
			j = 1;
			while (-1 != iTempArr[iCards[i] / 4][j])
				j++;
			//将编码放入iTempArr[iCards[i]/4][j]里面，因为该编码的点数为iCards[i]/4
			
			iTempArr[iCards[i] / 4][j] = iCards[i];

			//将点数为 iCards[i]/4 的牌的张数 改为 j ；
			iTempArr[iCards[i] / 4][0] = j;
		}
	}

	n = 0;   //n为iCards的下标，重新将iTempArr中的数放入iCards中
	for (i = 4; i > 0; i--) //先找出iTempArr一样的四张牌，若有则写入原数组iCards【】中，然后再寻找三张的，一次类推
	{
		for (j = 0; j < 13; j++)
		{
			if (iTempArr[j][0] == i)    //判断该点数的牌是不是有 i 张
			{
				for (k = 1; k <= i; k++)    // 有的话，就把牌都放进iCards[ n ]中，然后 n++
				{
					iCards[n] = iTempArr[j][k];
					n++;
				}
			}
		}
	}
}
//MO2-END

//M03-START按管牌主要因素升序排列
//最后修订者:夏侯有杰,最后修订时间:15-03-08 18:00
void CardsInfo::SortByMain(int iCards[])
{

	if (IsType0Pass(iCards))
		return;
	if (IsType1Rocket(iCards))
		return;
	if (IsType2Bomb(iCards))
		return;
	if (IsType3Single(iCards))
		return;
	if (IsType4Double(iCards))
		return;
	if (IsType5Three(iCards))
		return;
	if (IsType6ThreeOne(iCards))
		return;
	if (IsType6ThreeDouble(iCards))
		return;
	if (IsType11FourSingle(iCards))
		return;
	if (IsType11FourDouble(iCards))
		return;
	if (IsType7LinkSingle(iCards))
		return;
	if (IsType8LinkDouble(iCards))
		return;
	if (IsType9LinkThree(iCards))
		return;
	if (IsType10LinkThreeSingle(iCards))
		return;
	if (IsType10LinkThreeDouble(iCards))
		return;

	return;
}
//MO3-END



//M05-START将牌码数组转换为字符串追加(-1或)
//最后修订者:梅险,最后修订时间:15-03-01
void CardsInfo::AppendCardsToS(int iCards[], char sMsg[])
{
	int i;
	char sCard[4] = "";
	char sCardString[90] = "";
	if (iCards[0] == -1)	// PASS
		strcat_s(sCardString, "-1");
	else					// 不是PASS
	{
		for (i = 0; iCards[i] >= 0; i++)
		{
			if (iCards[i] >= 10)
			{
				sCard[0] = iCards[i] / 10 + '0';
				sCard[1] = iCards[i] % 10 + '0';
				sCard[2] = ',';
				sCard[3] = '\0';
			}
			else
			{
				sCard[0] = iCards[i] % 10 + '0';
				sCard[1] = ',';
				sCard[2] = '\0';
			}
			strcat_s(sCardString, sCard);
		}
		sCardString[strlen(sCardString) - 1] = '\0';	//去掉多余的尾部逗号

	}
	strcat(sMsg, sCardString);

}
//MO5-END



//当前局面上一手玩家剩牌数
int CardsInfo::LastPlayerCardsCount(Ddz * pDdz)
{
	int count = 0;
	int m = pDdz->iOTmax - 1;

	//判断最近出牌玩家已出多少牌
	if (pDdz->iOTmax == 0)
		count = 0;
	else{
		while (m >= 0)
		{
			count += CountCards(pDdz->iOnTable[m]);
			m = m - 3;
		}
	}

	if (pDdz->cLastPlay == pDdz->cLandlord)
		count = 20 - count;
	else
		count = 17 - count;
	return count;
}


//去除重复的牌并从小到大排序
int CardsInfo::GetUnReaptedCards(int iCopyCards[])
{
	int i,j,iTemp;
	int iCount = CountCards(iCopyCards);

	for (i = 1; iCopyCards[i] >= 0; i++)
		if (iCopyCards[i] / 4 == iCopyCards[i - 1] / 4)
			iCopyCards[i - 1] = -1;

	//从大到小排序
	for (i = 0; i<iCount; i++)
		for (j = i + 1; j<iCount; j++)
			if (iCopyCards[i]<iCopyCards[j])
			{
				iTemp = iCopyCards[i];
				iCopyCards[i] = iCopyCards[j];
				iCopyCards[j] = iTemp;
			}
	//从小到大排序，去掉重复的牌（-1）
	for (i = 0; iCopyCards[i] >= 0; i++)
		for (j = i + 1; iCopyCards[j] >= 0; j++)
			if (iCopyCards[i]>iCopyCards[j])
			{
				iTemp = iCopyCards[i];
				iCopyCards[i] = iCopyCards[j];
				iCopyCards[j] = iTemp;
			}
			
	iCount = CountCards(iCopyCards);

	return iCount;
}
