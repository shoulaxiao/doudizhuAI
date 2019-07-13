//////////////////////////////////
//////叫牌分析///////////////
//功能：根据手牌信息叫牌////
////////////////////////////////
#ifndef _CallNum
#define _CallNum
#include <stdio.h>
#include <stdlib.h>
#include "CardsInfo.h"
#include "SplitCard.h"

class CallNum
{
public:
	CallNum();
	~CallNum();
	static CallNum* Instance();

	CardsInfo* CInfo;
	int CardForce(int iCards[]);
	int CallCardAnalyze(Ddz* pDdz);
	SplitCard *pSplitCards;						//拆牌类变量
};

#endif _CallNum