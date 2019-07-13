#include <iostream>
#include "math.h"
#include <algorithm>
#include "windows.h"
#include <vector>
#include "time.h"
#include "SplitCard.h"
#include  "CardsInfo.h"

using namespace std;
class PokerModelAnalysis
{
public:
	PokerModelAnalysis();
	~PokerModelAnalysis();

	SplitCard *A;
	CardsInfo *B;
	Ddz *pMyDdz;

	int Standard1;
	int Standard2;//牌力鉴定依据
	int InitialSum;
	int iUtimate[8][40];//8行40列
	int iMyUtimate[8][40];
	int iOneUtimate[21];//上家的牌
	int iOneDe[21];//上家必有的牌
	int iTwoUtimate[21];//下家的牌
	int iTwoDe[21];//下家必有的牌
	int Buffer[38];
	int iOnHand[21];
	int Hc[21];
	int UpWrong;
	int DownWrong;
	float BiaoA[15][5];/*上家的概率表*/
	float  BiaoB[15][5];/*下家的概率表*/

	void Init();
	void GetOtherPokersCards(int *iUpCards, int *iDownCards, Ddz *pMyDdz);
	void GameStart();
	void Gaming();
	void CardsValue(int *pCards);
	void LordBest();
	void Equality();
	void PartnerBest();
	void SaveCards(int i, int *iResult);
	void DeleteCards(int *iResult);
	void ConfirmCards(int *iResult);
	void Analysis(int t);

	void Type1(int *iCs, int k);//火箭  也就是双王
	void Type2(int *iCs, int k);//炸弹 AAAA类型
    void Type3(int *iCs, int k);//单牌 B型
	void Type4(int *iCs, int k);//对牌 BB型
	void Type5(int *iCs, int k);//三条 BBB型
 
	void Type6_11(int *iCs, int k);
   
	void Random(int *Buffer);
	void DeleteB(int *iCs, int a);
	void Fendipai();
	void DeleteC();

	void Gailvbiao(int *Out, int p);/*概率表的初始化*/
	void Change(int *iCs,int t);/*概率表的变化*/
	
};

