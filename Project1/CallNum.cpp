
#include "CallNum.h"

CallNum::CallNum()//不用修改
{
	CInfo = CardsInfo::Instance();
	pSplitCards = new SplitCard();
}

CallNum::~CallNum()//不用修改
{
}

CallNum* CallNum::Instance()//不用修改
{
	static CallNum instance;

	return &instance;
}
/*
最后修订：闵子煜 18-07-22
*/

int CallNum::CardForce(int iCards[])
{
	int iMyCards[18] = { 0 };//手牌牌型

	int CopyiMyCards[18] = { 0 };//复制手牌

	//计算重牌，偶数为牌的大小，奇数为该牌的数量
	int iRepeat[27] = { 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 };//（仅仅只是重牌及张数） 

	int iBRepeat[27] = { 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 };//针对Q以上的炸弹单独的数组

	int iOrder[18] = { 0 };//连牌 

	int iStraight[9] = { 0 };//顺子（每三位表示一个顺子）第一位为开始，第二位为结束，第三位为开始和结束的差值

	int iSingle = 0;//调张（单牌张数）

	int iForce = 0;//牌力

	bool bLock = true;//锁机制


	int*single = pSplitCards->startSplitCard(iCards, 7);//对应拆牌中的接口函数中的单牌命令
	//小于A的单牌的权值
	for (int i = 0; single[i] != -1; i++)
	{
		if (single[i] != -2 && single[i] < 48)
		{
			iSingle++;
			iForce = single[i] - 10;
		}
	}

	//整理牌型，转换为实际点数
	for (int i = 0; i < 17; i++)
	{

		if (iCards[i] / 4 == 0)
		{
			iMyCards[i] = 3;    //3
		}
		if (iCards[i] / 4 == 1)
		{
			iMyCards[i] = 4;    //4
		}
		if (iCards[i] / 4 == 2)
		{
			iMyCards[i] = 5;    //5
		}
		if (iCards[i] / 4 == 3)
		{
			iMyCards[i] = 6;    //6
		}
		if (iCards[i] / 4 == 4)
		{
			iMyCards[i] = 7;    //7
		}
		if (iCards[i] / 4 == 5)
		{
			iMyCards[i] = 8;    //8
		}
		if (iCards[i] / 4 == 6)
		{
			iMyCards[i] = 9;    //9
		}
		if (iCards[i] / 4 == 7)
		{
			iMyCards[i] = 10;    //10
		}
		if (iCards[i] / 4 == 8)
		{
			iMyCards[i] = 11;    //J
		}
		if (iCards[i] / 4 == 9)
		{
			iMyCards[i] = 12;    //Q
		}
		if (iCards[i] / 4 == 10)
		{
			iMyCards[i] = 13;    //K
		}
		if (iCards[i] / 4 == 11)
		{
			iMyCards[i] = 14;    //A
		}
		if (iCards[i] / 4 == 12)
		{
			iMyCards[i] = 22;    //2
		}
		if (iCards[i] == 52)
		{
			iMyCards[i] = 66;    //小王
		}
		if (iCards[i] == 53)
		{
			iMyCards[i] = 99;    //大王
		}
	}

	//重新定义一个重牌函数，用于Q以上炸弹的记录

	for (int i = 0, j = 0; i < 17; i++)//统计重牌数及牌面大小
	{
		if (iMyCards[i] == iMyCards[i + 1])
		{
			iBRepeat[j] = iMyCards[i + 1];
			iBRepeat[j + 1]++;
		}
		else
		{
			j += 2;//在iRepeat中移动记录
		}
	}

	//先将Q以上的炸弹直接计算权值
	for (int i = 1; i < 26; i += 2)
	{
		//Q以上炸弹的权值计算，用iBRepeat来判断
		if (iBRepeat[i] == 4 && iBRepeat[i - 1] > 11)
		{
			iForce = iBRepeat[i - 1] - 4;
		}
	}
	//顺子的第一次筛选
	for (int i = 0, j = 0; i < 17; i++)
	{
		if (iMyCards[i] + 1 == iMyCards[i + 1])//判断是否相连
		{
			iOrder[j] = iMyCards[i];    //如果两张相邻的牌递增，这里只记录了前一张
			j++;
		}
		//两张相邻的牌，后者大于前者2点及以上；或者为最后一张牌
		else if (iMyCards[i] + 1 < iMyCards[i + 1] || i == 16)
		{

			if (iOrder[j - 1] + 1 == iMyCards[i])
			{
				iOrder[j] = iMyCards[i];//把本应该相连的最后一张牌输入iOrder中
				j += 2;//在iOrder中另起一个序列来存储下一个顺子
			}
		}
		else
		{
			continue;//出现相邻相等的牌不管
		}
	}
	//顺子的第二次筛选
	for (int i = 0, j = 0; i < 17; i++)
	{
		if (iOrder[i] != 0)
		{
			if (bLock)
			{
				iStraight[j] = iOrder[i];//顺子开始的第一张放入iStraight
				j++;
				bLock = false;
			}
		}
		else
		{
			if (iOrder[i - 1] == 0 && iOrder[i + 1] == 0)//前后均为0表示iOreder中无顺子，则跳出
			{
				break;
			}
			else
			{
				if (iOrder[i - 1] - iStraight[j - 1] >= 4)
				{
					iStraight[j] = iOrder[i - 1];//顺子的第二位为最后一张牌
					j++;
					iStraight[j] = iStraight[j - 1] - iStraight[j - 2];//第三位表示从开始到结束需要做几次加法
					j++;
					bLock = true;
				}
				else//不符合顺子的条件，重新筛选
				{
					iStraight[j - 1] = 0;
					j--;
					bLock = true;
				}
			}
		}
	}

	//顺子的权值计算,后面会改变顺子数组的值
	for (int i = 1; i < 8; i += 3)
	{
		if (iStraight[i + 1] >= 7)
		{
			iForce = iStraight[i] - 7;//iStraight[i] - 10 + 3
		}
		else
		{
			iForce = iStraight[i] - 8;//iStraight[i] - 10 + 2
		}
	}

	//对子、三条、炸弹的第一次筛选（之后的带牌都是在这些的基础上进行组合）
	for (int i = 0, j = 0; i < 17; i++)//统计重牌数及牌面大小
	{
		if (iMyCards[i] == iMyCards[i + 1])
		{
			iRepeat[j] = iMyCards[i + 1];
			iRepeat[j + 1]++;
		}
		else
		{
			j += 2;//在iRepeat中移动记录
		}
	}

	//在顺子的前提下重新对重牌进行筛选(由于这里计算之后会使顺子数组发生改变，所以应在前面进行顺子权值的计算)
	for (int i = 0; i < 26; i += 2)
	{
		for (int x = 0, y = 1; x < 8; x += 3)
		{
			for (int z = 2; iStraight[z] > 0; iStraight[z]--)
			{
				if (iStraight[x] == iRepeat[i])
				{
					iRepeat[i + 1]--;
				}
				else if (iStraight[x] < iStraight[y])
				{
					iStraight[x] = iStraight[x] + 1;
				}
			}
		}
	}

	//第二次筛选并进行权值的计算
	for (int i = 1; i < 26; i += 2)
	{
		//炸弹（小于Q的）
		if (iRepeat[i] == 4 && iRepeat[i - 1] < 11)
		{
			iForce = iRepeat[i - 1] - 4;
		}
		//对子的权值
		if (iRepeat[i] == 2 && iRepeat[i - 1] < 48)//剩余对子（小于2）的权值计算
		{
			iForce = iRepeat[i - 1] - 10;
		}
		//三条和三顺的权值
		if (iRepeat[i] == 3 && iRepeat[i - 1] < 48)//剩余三条和三顺（小于2）的权值计算
		{
			iForce = iRepeat[i - 1] - 8;
			/*可以防止重复计算一个三条的值
			eg:444555999
			先算444的权值，然后满足三顺的条件，多+1，再进行555的计算
			这样444的权值+555的权值+1==444555的权值
			*/
			if (iRepeat[i - 1] + 1 == iRepeat[i + 1] && iRepeat[i + 2] == 3)
			{
				iForce++;
			}
		}
	}


	//牌力判断函数
	/*
	牌力计算思想：（待定）
	1.每张牌都首先有个基础的牌力值，以10为中间值0，小于10的依次递减，大于10的依次递增到A，2（6）小王（7）大王（8））
	2.单牌就是每张牌的基础值
	3.对子在基础权值的基础上+1
	4.三条、三带单和带对，一般认为都是带10及以下的牌，所以权值为基础权值+2
	5.单顺和双顺的权值计为最后一张牌的基础权值+2到3（暂定为单顺7张及以上+3，双顺4对及以上+3）
	6.三顺和三顺带牌（认为带的牌均为10以下的牌），权值为基础权值+5（防止拆分后和三条的的权值一样甚至更小）
	7.炸弹和四带二（认为炸弹的翻倍和带牌的效用一样），权值均为基础权值+6
	8.火箭权值定义为16
	PS：三顺为三个情况不是很多，暂时不定义其权值
	*/


	//牌力的计算(增加及修改权值)
	for (int i = 0; i < 17; i++)
	{
		//如果单牌数小于3，增加牌力值(可以考虑删除)
		/*if (iSingle < 3)
		{
			iForce += 3 - iSingle;
		}*/
		if (iFShowInConsole)
			cout << " SingleNumber " << iSingle << endl;

		return iForce;
	}
}


/*
最后修订：闵子煜 18-07-22
*/
//判断叫牌点数ipip和ipnum改为float
int CallNum::CallCardAnalyze(Ddz* pDdz)
{
	int iInHand = 0;//进手张
	int iBig = 0;//统计手中A以上的大牌有多少
	float iPip = 0.0;//大牌权值
	int bomb = 0;//判断是否有双王（双王只在循环计算单牌中的价值体现不出来）
	int iCallPoint = -1;//最终叫牌数
	int iHeadCallPointSecond;//上家叫牌总点数
	int iHeadCallPointFirst;//上上家叫牌点数

	/*//获取上家叫牌数
	if (pDdz->iBid[0] == -1)
	{
		iHeadCallPointFirst = -1;//上上家叫牌点数
		iHeadCallPointSecond = -1;//上家叫牌点数
	}
	else if (pDdz->iBid[1] == -1)
	{
		iHeadCallPointFirst = -1;//上上家叫牌点数
		iHeadCallPointSecond = pDdz->iBid[0];//上家叫牌点数
	}
		else if (pDdz->iBid[2] == -1)
		{
			iHeadCallPointFirst = pDdz->iBid[0];//上上家叫牌点数
			iHeadCallPointSecond = pDdz->iBid[1];//上家叫牌点数
		}
		*/
		//设置玩家的编号并准备记录其叫牌点数
	if (pDdz->cDir == 'A')//0 1 2 
	{
		iHeadCallPointFirst = pDdz->iBid[1];
		iHeadCallPointSecond = pDdz->iBid[2];
	}
	else if (pDdz->cDir == 'B')
	{
		iHeadCallPointFirst = pDdz->iBid[0];//iBid[2]原
		iHeadCallPointSecond = pDdz->iBid[2];//iBid[0]原
	}
	else if (pDdz->cDir == 'C')
	{
		iHeadCallPointFirst = pDdz->iBid[0];
		iHeadCallPointSecond = pDdz->iBid[1];
	}


	//牌点判断，即A以上的单牌的价值计算
	for (int i = 0; i < 17; i++)
	{
		iInHand = pDdz->iOnHand[i];
		if (iInHand >= 53)		//大王 iPip += 8;
		{
			bomb += 2;
			iBig += 3;
		}
		else if (iInHand >= 52)	//小王 iPip +=7;
		{
			bomb += 1;
			iBig += 3;
		}
		else if (iInHand >= 48)	//2
		{
			iPip += 6.0;
			iBig += 2;
		}
		else if (iInHand >= 44) //A
		{
			iBig++;
		}
	}

	if (bomb == 3)//双王
		iPip += 16.0;//iPip += 16;
	if (bomb == 1)//小王
		iPip += 7.0;//iPip += 7;
	if (bomb == 2)//大王
		iPip += 8.0;//iPip += 8;

	//叫牌分析
	//叫牌的一般情况的基本条件至少需要有几张大牌
	if (iBig >= 5)
	{
		float iPipNum = CardForce(pDdz->iOnHand) + iPip;//一副手牌的最终价值

		if (iFShowInConsole)
		{
			//Test
			cout << "牌型的权值（不包含 2 大小王）:  " << iPipNum - iPip << endl;
			cout << "大牌的权值（2 大小王）:  " << iPip << endl;
			cout << "总的权值:  " << iPipNum << endl;
			cout << "大牌的数目表示:  " << iBig << endl;
		}

		//表示自己是A的时候
		if (iHeadCallPointFirst == -1 && iHeadCallPointSecond == -1)//可以直接叫牌，不用考虑其它人
		{
			if (iPipNum >= 18.0)
			{
				if (3 / 2 * iPip >= iPipNum)
				{
					iCallPoint = 3;
				}
				else if (3 / 2 * iPip >= 3 / 4 * iPipNum)
				{
					iCallPoint = 2;
				}
				else
				{
					iCallPoint = 1;
				}
			}
			else if (iPipNum >= 10.0)
			{
				if (iPip == iPipNum)
				{
					iCallPoint = 0;
				}
				else if (iPip >= 1 / 2 * iPipNum)
				{
					iCallPoint = 1;
				}
				else
				{
					iCallPoint = 0;
				}
			}
			else
			{
				iCallPoint = 0;
			}
		}
		//表示自己为B的时候,且当准备叫分与A一样时，选择不叫
		if (iHeadCallPointFirst != -1 && iHeadCallPointSecond == -1)//即上家叫了牌，轮到我们了
		{
			if (iPipNum >= 18.0)
			{
				if (iPipNum >= 18.0)
				{
					if (3 / 2 * iPip >= iPipNum && iHeadCallPointFirst < 3)
					{
						iCallPoint = 3;
					}
					else if (3 / 2 * iPip >= 3 / 4 * iPipNum && iHeadCallPointFirst < 2)
					{
						iCallPoint = 2;
					}
					else if (iHeadCallPointFirst < 1)
					{
						iCallPoint = 1;
					}
					else
					{
						iCallPoint = 0;
					}
				}
				else if (iPipNum >= 10.0)
				{
					if (iPip == iPipNum)
					{
						iCallPoint = 0;
					}
					else if (iPip >= 1 / 2 * iPipNum && iHeadCallPointFirst < 1)
					{
						iCallPoint = 1;
					}
					else
					{
						iCallPoint = 0;
					}
				}
				else
				{
					iCallPoint = 0;
				}
			}
		}
		//表示自己为C的时候,且当准备叫分与B一样时，选择不叫
		if (iHeadCallPointFirst != -1 && iHeadCallPointSecond != -1)
		{
			if (iPipNum >= 18.0)
			{
				if (3 / 2 * iPip >= iPipNum && iHeadCallPointFirst < 3 && iHeadCallPointSecond < 3)
				{
					iCallPoint = 3;
				}
				else if (3 / 2 * iPip >= 3 / 4 * iPipNum && iHeadCallPointFirst < 2 && iHeadCallPointSecond < 2)
				{
					iCallPoint = 2;
				}
				else if (iHeadCallPointFirst < 1 && iHeadCallPointSecond < 1)
				{
					iCallPoint = 1;
				}
				else
				{
					iCallPoint = 0;
				}
			}
			else if (iPipNum >= 10.0)
			{
				if (iPip == iPipNum)
				{
					iCallPoint = 0;
				}
				else if (iPip >= 1 / 2 * iPipNum && iHeadCallPointFirst < 1 && iHeadCallPointSecond < 1)
				{
					iCallPoint = 1;
				}
				else
				{
					iCallPoint = 0;
				}
			}
			else
			{
				iCallPoint = 0;
			}
		}
	}
		else //是iBig太小表示没有多少大牌，某些特殊情况可以考虑叫1分
		{
			int iPipNum = CardForce(pDdz->iOnHand) + iPip;//一副手牌的最终价值
			if (iFShowInConsole)
			{
				//Test
				cout << "牌型的权值（不包含2和大小王的权值）:  " << iPipNum - iPip << endl;
				cout << "大牌的权值（A 2 大小王）:  " << iPip << endl;
				cout << "总的权值:  " << iPipNum << endl;
				cout << "大牌的数目表示:  " << iBig << endl;
			}

			//表示自己是A的时候(需要重新弄)
			if (iHeadCallPointFirst == -1 && iHeadCallPointSecond == -1)
			{
				if (iPipNum >= 10 && iPip <= 1 / 2 * iPipNum)
				{
					iCallPoint = 1;
				}
				else
				{
					iCallPoint = 0;
				}
			}

			//表示自己为B的时候,且当准备叫分与A一样时，选择不叫
			if (iHeadCallPointFirst != -1 && iHeadCallPointSecond == -1)
			{
				if (iPipNum >= 10 && iPip <= 1 / 2 * iPipNum && iHeadCallPointFirst < 1)
				{
					iCallPoint = 1;
				}
				else
				{
					iCallPoint = 0;
				}
			}

			//表示自己为C的时候,且当准备叫分与B一样时，选择不叫
			if (iHeadCallPointFirst != -1 && iHeadCallPointSecond != -1)
			{
				if (iPipNum >= 10 && iPip <= 1 / 2 * iPipNum && iHeadCallPointFirst < 1 && iHeadCallPointSecond < 1)
				{
					iCallPoint = 1;
				}
				else
				{
					iCallPoint = 0;
				}
			}

		}



		//判断是否有2和王，叫三分的前提，至少要有1个王，2个二 或者两个王一个二
		/*int j = 0,k=0;
		if (iCallPoint == 3)
		{
		for (int i = 0; i < 17; i++)
		{
		if (pDdz->iOnHand[i]/4==12)
		{
		j++;//2
		}
		else if (pDdz->iOnHand[i] / 4 == 13)
		{
		k++;//王
		}
		}
		//没有两个二或者没有王
		if (j < 2 || k < 1)
		{
		if (iHeadCallPointSecond != 2 && iHeadCallPointFirst != 2)
		iCallPoint = 2;
		else
		iCallPoint = 0;
		}
		//有两个王和一个及以上的二
		if (j >= 1 && k == 2)
		{
		iCallPoint = 3;
		}
		}*/

		//返回叫牌点数
		return iCallPoint;
	}