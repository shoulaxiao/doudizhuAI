#ifndef _CardsInfo
#define _CardsInfo

#include "Ddz.h"
//////////////////////////////////
//////手牌信息分析///////////////
//功能：用于初始化轮局，更新牌局////
////////////////////////////////

class CardsInfo
{
public:
	CardsInfo();
	~CardsInfo();
	static CardsInfo* Instance();

	///判断牌型////
	int IsType0Pass(int iCs[]);						//B00 判断Pass，返回1是，0不是

	int IsType1Rocket(int iCs[]);					//B01 判断火箭，返回1是，0不是

	int IsType2Bomb(int iCs[]);						//B02 判断炸弹，返回1是，0不是

	int	IsType3Single(int iCs[]);					//B03 判断单牌，返回1是，0不是

	int	IsType4Double(int iCs[]);					//B04 判断对牌，返回1是，0不是

	int	IsType5Three(int iCs[]);					//B05 判断三条，返回1是，0不是

	int IsType6ThreeOne(int iCs[]);					//B0601 判断三带一单，返回1是，0不是

	int IsType6ThreeDouble(int iCs[]);				//B0602 判断三带一对，返回1是，0不是

	int IsType7LinkSingle(int iCs[]);				//B07 判断单顺，返回1是，0不是

	int IsType8LinkDouble(int iCs[]);				//B08 判断双顺，返回1是，0不是

	int	IsType9LinkThree(int iCs[]);				//B09 判断三顺，返回1是，0不是

	int	IsType10LinkThreeSingle(int iCs[]);			//B1001 判断三顺带单，返回1是，0不是

	int	IsType10LinkThreeDouble(int	iCs[]);			//B1002 判断三顺带对，返回1是，0不是

	int IsType11FourSingle(int iCs[]);				//B1101 判断四带二单，返回1是，0不是

	int IsType11FourDouble(int iCs[]);				//B1102 判断四带二对，返回1是，0不是


	///分析牌型及点数//
	int	AnalyzeTypeCount(int iCards[]);				//I03  分析牌型和张数

	int	AnalyzeMainPoint(int iCards[]);				//I04 计算主牌最小点数


	///////手牌数计算、排序/////
	int CountCards(int iCards[]);					//I01 计算牌张数

	void SortById(int iCards[]);					//M01 按牌编码升序排列

	void SortByMuch(int iCards[]);					//M02 按同点数牌优先降序排列

	void SortByMain(int iCards[]);					//M03 按牌型主牌升序排列


	//其它//
	void AppendCardsToS(int iCards[], char sMsg[]);	//M05 将牌码数组转换为字符串追加

	// 计算主牌最大点数
	int AnalyzeMainMaxPoint(int iCards[]);
	
	int LastPlayerCardsCount(Ddz * pDdz);
	//去除重复的牌并从小到大排序
	int GetUnReaptedCards(int iCards[]);
};


#endif _CardsInfo