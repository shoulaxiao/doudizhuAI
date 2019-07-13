/* 出牌生成器*/
/*
* 最后修订者: 江文金  2018.7.20
* 本文件功能： 出牌生成器
*/
#include "CardsInfo.h"
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include "SplitCard.h"
#include "PokerModelAnalysis.h"

//节点数上限
#define PlaMax 100

//存储牌以及桌面信息
struct fDdz
{
	int iOnHand[21];		//手中牌（所有值初始化为-1）
	int *iLastOnTable;		//最近一次非Pass出牌数组，以-1结尾，Pass记为-1
	int iUpPlayerCards;		//上手玩家手牌数
	int iDownPlayerCards;	//下手玩家手牌数
	char cDir;				//玩家方位编号
	char cLandlord;			//地主玩家方位编号
	char cLastPlay;         //上位出牌玩家方位编号
	int iPlaCount;			//多种出牌可行解数量（值域[0，kPlaMax-1]）
	int iPlaArr[PlaMax][21];//多种出牌可行解集（各出牌解由牌编号升序组成-1间隔,-2收尾）//PASS首项为-3；//pass为-2-1;
	int iLastPassCount;		//当前桌面连续PASS数（值域[0,2],初值2，正常出牌取0，一家PASS取1，两家PASS取2）
	int iLastMainPoint;		//当前桌面主牌点数（值域[0,15],初值-1，iLastPassCount=0时更新值，，=1时保留原值，=2时值为-1）
	int iLastTypeCount;		//当前桌面牌型张数（值域[0,1108],初值0，iLastPassCount=0时更新值，=1时保留原值，=2时值为0）//出的牌型以及点数
	bool haveHelpFriend;	//记录是否帮助队友（当队友只有一张牌时，拿到出牌权，出最小单牌（判断队友是否打的起自己的最小单牌）） 初始化为false
	int iOnHandNum;			//AI剩余手数   //手牌数和手数不一样
};
class RecommendCards
{
public:
	RecommendCards();
	~RecommendCards();
	static RecommendCards* Instance();
	int HelpPla(struct fDdz * pDdz, bool IsSim);    //根据牌型调用下面的推荐出牌函数

	int Help0Pass(fDdz * pDdz);						//推荐出牌应对两家Pass

	int Help1Rocket(struct fDdz * pDdz);			//H01 推荐出牌应对火箭

	int Help2Bomb(struct fDdz * pDdz);				//H02 推荐出牌应对炸弹

	int Help3Single(struct fDdz * pDdz);			//H03 推荐出牌应对单牌

	int Help4Double(struct fDdz * pDdz);			//H04 推荐出牌应对对牌

	int Help4DoubleSim(struct fDdz * pDdz);			//无规则应对对子

	int Help5Three(struct fDdz * pDdz);				//H05 推荐出牌应对三条

	int Help6ThreeOne(struct fDdz * pDdz);			//H0601 推荐出牌应对三带一单

	int Help6ThreeOneSim(struct fDdz * pDdz);		//无规则应对三带一

	int Help6ThreeDouble(struct fDdz * pDdz);		//H0602 推荐出牌应对三带一对

	int Help6ThreeDoubleSim(struct fDdz * pDdz);	//无规则应对三带二
		
	int Help7LinkSingle(struct fDdz * pDdz);		//H07 推荐出牌应对单顺

	int Help8LinkDouble(struct fDdz * pDdz);		//H08 推荐出牌应对双顺

	int Help9LinkThree(struct fDdz * pDdz);			//H09 推荐出牌应对三顺

	int Help10LinkThreeSingle(struct fDdz * pDdz);	//H1001 推荐出牌应对三顺带单

	int Help10LinkThreeDouble(struct fDdz * pDdz);	//H1002 推荐出牌应对三顺带对

	int Help11FourSingle(struct fDdz * pDdz);		//H1101 推荐出牌应对四带二单

	int Help11FourDouble(fDdz * pDdz);				//H1102 推荐出牌应对四带二对

	int GetPlayerType(fDdz *pDdz);					//得到其他玩家的类型

	CardsInfo* CInfo;								

	int iPlaCount;									//本次模拟出牌生成器产生的多种出牌可行解数量

	SplitCard *pSplitCards;							//拆牌类变量

	PokerModelAnalysis *pMoldel;

	int OtherPlayersCards(fDdz* pDdz, Ddz *Ddz);	//计算其他玩家手中剩牌数（未被调用）

	int* pCardsOnHand;

	char cAI;//记录AI方位

	int InitPlaGenerate(fDdz* pDdz);				//出牌生成器初始化

	int iTmpSave[100];

	int iLastMaxPoint;//上家最大主牌点数

	
	bool IfPassTwice(fDdz *pDdz);   // 判断前面两家是否PASS两次

	
	int GetLegalCards(fDdz *pDdz, int *iCards, int *iSave);// 从iCards里面分离出能打过上家出牌的牌型存到iplaArr里面

	int GetLegalCards(fDdz *pDdz, int *iCards);
	int AddInPlaArr(fDdz *pDdz, int *iCards);    //将合法牌型放进iPlaArr
	
	int AnalyzeSplitedCards(int* iCards, int *iSave); // 解析拆牌类产生的数组

	void AppendCards(int* from, int* to);
	int Help3SingleSim(fDdz* pDdz);


	int MinHandNum(int* iOnHand);	// 计算手牌最小手数
	int iHandNum;  //我的手数
	int iUpHandNum;//上家的手数
	int iDownHandNum;//下家的手数
	int playerType; //玩家类型  1为AI是地主,2为下家是地主,3为上家是地主
	bool ifNeedPass;
	Ddz *pRealDdz;
	bool isSim;		   //isSim=1无规则应对,isSim=0有规则应对
	int iUpCards[21];  //上家的牌
	int iDownCards[21];//下家的牌
	bool GetModel(Ddz * prealDdz);
	bool AnalyzeCondition(fDdz* pDdz);
	void DeleteElementInCards(int* iCards, int iCardsCount, int iPoint);
	int AnalyzeSingleLink(int Length, int iLastMainPoint,int *iCards, int* iTmpShunzi);
	int CardsValue(int* iCards); //计算牌力值


	bool judgeFriend(fDdz* pDdz);
	int deleteTypeLeftSingleCount(int*iCopyCard, int*type);//计算手中的单牌数 在拆掉某一牌型前后的差值
	int judgeTypeAndAddInArr(fDdz *pDdz, int*iCards);//判断拆出的牌中，剩下的牌是否会产生很多单牌，如果不会，判断是否合法，如果合法加入可行解数组
	int iLastTwoHand[21];//存储最后两手牌
};

