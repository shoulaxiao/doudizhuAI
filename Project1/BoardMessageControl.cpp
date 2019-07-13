#include "BoardMessageControl.h"
#include<string>
#include "windows.h"
#include<iomanip>
using namespace std;
void printNum(int *temp);

BoardMessageControl::BoardMessageControl()
{
	Pma = new PokerModelAnalysis();
}

BoardMessageControl::~BoardMessageControl()
{
	delete(engine);
}

//初始化轮局信息
void BoardMessageControl::InitTurn(struct Ddz * pDdz)
{
	engine = new MonteCarloSearchEngine();
	CInfo = CardsInfo::Instance();
	pDdz->iTurnTotal =1;				//初始化总轮数
	pDdz->iTurnNow = 1;					//初始化当前轮次
	pDdz->iStatus = 1;       //初始化本轮引擎状态（增加赋初值，使初始化后一定能进入输入信息的循环里面）
	//Test
	if(!iFShowInConsole)
		pDdz->iRoundTotal = 100;			//初始化总局数
	else
		pDdz->iRoundTotal = 1;			//初始化总局数

	pDdz->dWinCount = 0;              //初始化AI赢的局数

	pDdz->iRoundNow = 1;				//初始化当前局次

	InitRound(pDdz);					//初始化本局数据
}
//P01-END

//M04-START重置本局数据初始值
//上一次修订者:范晓梦 上一次修订时间:16-07-31
//修改目的:otherPlayerCards初始化
//最后修订者：李讯航 最后修订时间：18-7-19
//修改内容及目的：添加版本号的初始化条件（iRoundNow=1），避免版本号被多次初始化
void BoardMessageControl::InitRound(struct Ddz * pDdz)
{
	int i, j;
	pDdz->iStatus = 1;					//初始化本局引擎状态
	strcpy_s(pDdz->sCommandIn, "");		//初始化本局通信输入内容
	strcpy_s(pDdz->sCommandOut, "");		//初始化本局通信输出内容
	for (i = 0; i < 21; i++)
	{		//初始化本局手牌
		pDdz->iOnHand[i] = -1;
	}
	//初始化otherPlayerCards
	for (i = 0; i < 54; i++)
	{
		pDdz->iOnOtherHand[i] = i;
	}
	pDdz->iOnOtherHand[54] = -1;
	for (i = 0; i<162; i++)				//初始化本局桌面牌
		for (j = 0; j<21; j++)
			pDdz->iOnTable[i][j] = -2;
	for (i = 0; i < 21; i++)			//初始化本局出牌
		pDdz->iToTable[i] = -1;
	if (pDdz->iRoundNow==1)
	strcpy_s(pDdz->sVer, "");				//初始化本局协议版本号
	//strcpy_s(pDdz->sVer, kPlayerName);		//初始化本局参赛选手称呼

	strcpy_s(pDdz->sName, kPlayerName);		//初始化本局参赛选手称呼

	pDdz->cDir = 'B';						//初始化本局玩家方位编号
	pDdz->cLandlord = 'B';				//初始化本局地主方位编号
	pDdz->cWinner = 'B';					//初始化本局胜者方位编号
	for (i = 0; i < 3; i++)				//初始化本局叫牌
		pDdz->iBid[i] = -1;
	pDdz->iLastPassCount = 2;		//当前桌面连续PASS数（值域[0,2],初值2，正常出牌取0，一家PASS取1，两家PASS取2）
	pDdz->iLastTypeCount = 0;		//当前桌面牌型张数（值域[0,1108],初值0，iLastPassCount=0时更新值，=1时保留原值，=2时值为0）
	pDdz->iLastMainPoint = -1;		//当前桌面主牌点数（值域[0,15],初值-1，iLastPassCount=0时更新值，，=1时保留原值，=2时值为-1）
	pDdz->iBmax = 0;					//初始化本局叫牌分数
	pDdz->iOTmax = 0;					//初始化本局桌面牌手数

	pDdz->haveHelpFriend = false;   //记录是否帮助过队友（当队友只有一张牌时，拿到出牌权，出最小单牌（判断队友是否打的起自己的最小单牌）） 初始化为false
}
//MO4-END

//P02-START输入信息
//上一次修订者：梅险，上一次修订时间:15-02-08
//最后修订者：李讯航 ，最后修订时间：18-07-21
//修订内容：增加在控制台上进行多局游戏以及调试的功能
//优化边界控制，并添加异常处理机制
void BoardMessageControl::InputMsg(struct Ddz * pDdz)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	if (!iFShowInConsole)
	{
		//此函数会一次读取输入进来的多个字符(包括空白字符）。
		//它以指定的地址为存放第一个读取的字符的位置，依次向后存放读取的字符，直到读满N - 1个
		//或者遇到指定的结束符为止。若不指定结束符，则默认结束符为'\n'。	
		cin.getline(pDdz->sCommandIn, 80);//读取平台输入的信息	
	}
	else
	{
	  //输出轮局信息
	  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
	  if (pDdz->iRoundNow > 0)
	  {
		  cout << endl << "总局数为 " << pDdz->iRoundTotal << " 局，当前为第 " << pDdz->iRoundNow << " 局\n"
		  << "A I 赢了 " << setiosflags(ios::fixed) << setprecision(0) << pDdz->dWinCount << " 局，胜率为：";
	  if(pDdz->iRoundNow>1)
	      cout<< setiosflags(ios::fixed) <<setprecision(2)<< 100*((double)(pDdz->dWinCount) / (pDdz->iRoundNow-1)) << "%" << endl;
	  else 
		  cout << setiosflags(ios::fixed) << setprecision(2) << "0 %" << endl;
	  }
	  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	  //18-07-21
	
	  cout << endl << "开始斗地主 （输入出牌请用空格隔开，只输入0为PASS，J Q K A 2 小王 大王分别为11 12 13 1 2 14 15，最后用0结束）" << endl << endl;
 {
		  
		//生成牌
		int out[55];//牌堆重新排序来发牌
		int temp[55];//牌堆（内容一直在变）
		int us[21];//我们的牌
		int latter[21];//下家的牌
		int former[21];//上家的牌

		srand((unsigned)time(NULL));//以时间作为种子

		//初始化三个数组
		for (int i = 0; i < 21; i++)
		{
			us[i] = -1;
			latter[i] = -1;
			former[i] = -1;
		}

		//随机生成牌
		if (isRandom){
		for (int i = 0; i < 54; i++)
		{
			temp[i] = i;//往数组里放进牌编号
		}
		for (int left = 53, i = 0; left>0; left--)//left为牌堆剩余牌数 
		{
			int b = rand() % left;//随机选取一张牌  （0 <= b <= 52）
			out[i++] = temp[b];//out[0-52]
			//确保剩余牌数一直在减少而且选取的牌不重复
			if (b != left) {
				temp[b] = temp[left];
			}
		}
		out[53] = temp[0];

		//从随机生成的牌中取出各家的牌
		for (int i = 0; i < 51; i++)
		{
			if (i < 17) 
			{
				us[i] = out[i];
			}
			else if(i<34)
			{
				latter[i - 17] = out[i];
			}
			else 
			{
				former[i - 34] = out[i];
			}
		}
		}//isRandom

		//固定生成牌（可自行输入想要的初始牌）
		else{
			int fixation[52] = { 29, 19, 39, 38, 33, 18, 3, 44, 46, 22, 4, 53, 11, 49, 24, 28, 17,      //上家的牌
				               43, 31, 36, 10, 35, 32, 30, 51, 13, 25, 47, 6, 1, 8, 26, 0, 37,    //AI的牌
							   41, 14, 21, 52, 48, 42, 20, 34, 9, 40, 2, 23, 16, 5, 27, 45, 7 };//下家的牌
			for (int i = 0; i < 51; i++)
				{
					if (i < 17)
					{
						former[i] = fixation[i];
					}
					else if (i<34)
					{
						us[i - 17] = fixation[i];
					}
					else
					{
						latter[i - 34] = fixation[i];
					}
					out[i] = fixation[i];
				}
			//底牌
			out[51] = 15;
			out[52] = 12;
			out[53] = 50; 
		}
	
		//用冒牌排序给三家的牌排个序
		for (int pass = 17,j; pass >0; pass--)
		{
			for (int i = 0; i < pass-1; i++)
			{
				if (us[i] > us[i + 1]&& us[i+1]!=-1) {
					j = us[i];
					us[i] = us[i + 1];
					us[i + 1] = j;
				}
				if (latter[i] > latter[i + 1] && latter[i + 1] != -1) {
					j = latter[i];
					latter[i] = latter[i + 1];
					latter[i + 1] = j;
				}
				if (former[i] > former[i + 1] && former[i + 1] != -1) {
					j = former[i];
					former[i] = former[i + 1];
					former[i + 1] = j;
				}
			}
		}

	
		//把数字转换成字符串
		char sCard[4] = "";
		char sCardString[90] = "";
		strcpy_s(sCardString, "DEAL B");//（把cat修改为cpy）
		for (int i = 0; i < 17; i++)
		{
			if (us[i] >= 10) {
				sCard[0] = us[i] / 10 + '0';
				sCard[1] = us[i] % 10 + '0';
				sCard[2] = ',';
				sCard[3] = '\0';
			}
			else {
				sCard[0] = us[i] % 10 + '0';
				sCard[1] = ',';
				sCard[2] = '\0';
			}
			strcat_s(sCardString, sCard);
		}
		sCardString[strlen(sCardString) - 1] = '\0';//去掉多余的尾部逗号
		strcpy(pDdz->sCommandIn, sCardString);//输出的是AI得到的牌的编码（把cat修改为cpy）
	

		//输出三家的手牌
		if(iFShowOtherCard)
		{
			cout << "上家的手牌";
			printCard(former);
			printNum(former);

			cout << "\nAI的手牌  ";
			printCard(us);
			printNum(us);

			cout << "\n下家的手牌";
			printCard(latter);
			printNum(latter);
		}
		
		
		//AI拿到牌后分析，存储，回答平台
		AnalyzeMsg(pDdz);
		OutputMsg(pDdz);
		//OK DEAL
		cout << endl;
		
		//叫分
		//上家叫分
		if (!iFShowOtherCard) {
			cout << "\n上家的手牌";
			printCard(former);
		}
		
		cout << "上家叫分： ";
		int formerbid;
		cin >> formerbid;

		char a[2];
		a[0]= formerbid % 10 + '0';//类型转换
		a[1] = '\0';
		strcpy(pDdz->sCommandIn,"BID A");//覆盖
		strcat(pDdz->sCommandIn, a);//连接两个字符：BID Aa 
		pDdz->sCommandIn[6] = '\0';
		AnalyzeMsg(pDdz);
		cout << endl;
		OutputMsg(pDdz);


		//AI叫分
		cout << "\nAI叫分 ";
		strcpy(pDdz->sCommandIn, "BID WHAT");
		AnalyzeMsg(pDdz);
		cout << endl;
		OutputMsg(pDdz);
		//BID B_
		cout << endl;


		//下家叫分
		if (!iFShowOtherCard) 
		{
			cout << "\n下家的手牌";
			printCard(latter);
		}

		cout << "下家叫分： ";
		int latterbid;
		cin >> latterbid;

		char aa[2];
		aa[1] = '\0';
		aa[0] = latterbid % 10 + '0';
		strcpy(pDdz->sCommandIn, "BID C");
		strcat(pDdz->sCommandIn, aa);
		pDdz->sCommandIn[6] = '\0';
		AnalyzeMsg(pDdz);
		cout << endl;
		OutputMsg(pDdz);


		//显示底牌，以及判断谁是地主 A B C
		int left[4];
		left[0] =out[51];
		left[1] = out[52];
		left[2] = out[53];
		left[3] = -1;

		//把底牌转换成字符串
		char info[90] = "";
		for (int i = 51; i < 54; i++)
		{
			if (out[i] >= 10) {
				sCard[0] = out[i] / 10 + '0';
				sCard[1] = out[i] % 10 + '0';
				sCard[2] = ',';
				sCard[3] = '\0';
			}
			else {
				sCard[0] = out[i] % 10 + '0';
				sCard[1] = ',';
				sCard[2] = '\0';
			}
			strcat_s(info, sCard);
		}

		if(strlen(info)>=1)
			info[strlen(info) - 1] = '\0';//去掉多余的尾部逗号
	
		//把底牌分给地主（添加了叫分的约束：1、都为0没人会成为地主；2、后者叫分要比前者高或为0）
		
		//AI为地主
		if (aiBid > latterbid && aiBid >= formerbid && aiBid !=0) {

			strcpy(pDdz->sCommandIn, "LEFTOVER B");
			strcat_s(pDdz->sCommandIn, info);
			
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
			cout << endl;
			cout << "                                                       AI为地主，底牌为";	
			printCard(left);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),  FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			AnalyzeMsg(pDdz);

			OutputMsg(pDdz);

			who = 1;
			who2 = 1;
			Pma->Gailvbiao(out, 2);	
		}

		//下家为地主
		else if (latterbid >= aiBid && latterbid > formerbid && latterbid!=0) {
			strcpy(pDdz->sCommandIn, "LEFTOVER C");
			strcat_s(pDdz->sCommandIn, info);

			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
			cout << endl;
			cout << "                                                       下家为地主，底牌为";
			
			printCard(left);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			AnalyzeMsg(pDdz);
			OutputMsg(pDdz);

			latter[17] = out[51];
			latter[18] = out[52];
			latter[19] = out[53];
			who = 2;
			who2 = 2;
			Pma->Gailvbiao(out, 3);
		}

		//上家为地主
		else if (formerbid > aiBid && formerbid > latterbid && formerbid !=0) {
			strcpy(pDdz->sCommandIn, "LEFTOVER A");
			strcat_s(pDdz->sCommandIn, info);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
			cout << endl;
			cout << "                                                       上家为地主，底牌为";
			
			printCard(left);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),  FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			AnalyzeMsg(pDdz);
			OutputMsg(pDdz);

			former[17] = out[51];
			former[18] = out[52];
			former[19] = out[53];
			who = 0;
			who2 = 0;
			Pma->Gailvbiao(out, 1);
		}

		//发完底牌后的手牌信息
		if (iFShowOtherCard)
		{
			cout << endl;
			cout << "上家的手牌";
			printCard(former);
			cout << "A I 的手牌";
			printCard(pDdz->iOnHand);
			cout << "下家的手牌";
			printCard(latter);
			
		}

		SplitCard *pSplitCards = new SplitCard();
		int **result = pSplitCards->startSplitCard(pDdz->iOnHand);

		while (pDdz->iStatus != 0)
		{
			int outOfHand[21];//出的牌

			//都不叫地主
		if ((aiBid == 0 || aiBid == '/') && formerbid == 0 && latterbid == 0){
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
			strcpy(pDdz->sCommandIn, "GAMEOVER D");
			break;
		}

			//初始化数组
			for (int i = 0; i < 21; i++)
			{
				outOfHand[i] = -1;
			}

			//轮到上家出牌
			if (who == 0) {
				if (!iFShowOtherCard)
				{
					cout << endl << "上家的手牌 ";
					printCard(former);
				}
			
				cout << "输入上家出牌： ";
				int a[21];  //存储要出的牌
				for (int i = 0; i < 20; i++)
				{
					a[i] = -1;
				}

				int i = 0, b=-1, k = 0; //修改：b赋初始值为-1

				//上家出牌
				while (cin >> b) {   //输入要出的牌
					if (b != 0 && b>0 && b<16) {
						a[i++] = b;  //i的值会变,出了几张i就为几
					}
					else {    //b为0结束循环
						break;
					}
				}

				if (i == 0) {   //不出牌:输入b=0,i不自增,此时i为0
					strcpy(pDdz->sCommandIn, "PLAY A-1");

					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
					cout << "                                                       上家PASS  " << endl;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
					//cout << " MSGtoAI " << pDdz->sCommandIn << endl;
					AnalyzeMsg(pDdz);		//分析处理信息
					OutputMsg(pDdz);		//输出信息
					cout << endl;
					who = 1;
					continue;
				}
				
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
				    cout << "                                                       上家出牌 ： ";

				//显示刚出的牌
				for (int i = 0; a[i] > -1; i++) {
					if (a[i] == 1)cout << "A ";
					else if(a[i]==11)cout <<"J ";
					else if (a[i] == 12)cout << "Q ";
					else if (a[i] == 13)cout << "K ";
					else if (a[i] == 14)cout << "小王 ";
					else if (a[i] == 15)cout << "大王 ";
					else cout << a[i] << " ";
				}
				cout << endl;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				
				//去掉刚出的牌
				for (int i = 0; a[i] != -1; i++) {   //从刚出的牌里面一张张搜索
					for (int j = 0; former[j] != -1; j++) {  //从手牌里面搜索
						if (former[j] / 4 == 0)//牌值为3
						{                          //这里的顺序??先检索手牌而不是出了的牌?工作量哪个大,不存在工作量的问题,先变的是j
							if (a[i] == 3) {   //出的牌里面有3
								outOfHand[k++] = former[j]; //出过的牌加上这张牌值为3的牌
								//	cout << " find 3";
								former[j] = -1000;      //这张牌在手牌中去掉,为什么是赋-1000?
								break;
								//cout << "catch 3";
							}
							//cout << 3 << " ";    //3
						}

						else if (former[j] / 4 == 1)//牌值为4
						{
							if (a[i] == 4) {
								outOfHand[k++] = former[j];
								//	cout << " find 4";
								former[j] = -1000;
								//cout << "catch 4";
								break;
							}
						}
						//cout << 4 << " ";   //4

						else if (former[j] / 4 == 2) {
							if (a[i] == 5) {
								outOfHand[k++] = former[j];
								//	cout << " find 5";
								former[j] = -1000;
								break;
								//cout << "catch 5";
							}
							//cout << 5 << " ";   //5
						}
						else if (former[j] / 4 == 3)
						{
							if (a[i] == 6) {
								outOfHand[k++] = former[j];
								//cout << " find 6";
								former[j] = -1000;
								break;
							}
							//cout << 6 << " ";    //6
						}
						else if (former[j] / 4 == 4) {
							if (a[i] == 7) {
								outOfHand[k++] = former[j];
								//cout << " find 7";
								former[j] = -1000;
								break;
							}
							//cout << 7 << " ";    //7
						}
						else if (former[j] / 4 == 5) {
							if (a[i] == 8) {
								outOfHand[k++] = former[j];
								//	cout << " find 8";
								former[j] = -1000;
								break;
							}
							//cout << 8 << " ";    //8
						}
						else if (former[j] / 4 == 6) {
							if (a[i] == 9) {
								outOfHand[k++] = former[j];
								//	cout << " find 9";
								former[j] = -1000;
								break;
							}
							//cout << 9 << " ";   //9
						}
						else if (former[j] / 4 == 7)
						{
							if (a[i] == 10) {
								outOfHand[k++] = former[j];
								//			cout << " find 10";
								former[j] = -1000;
								break;
							}
							//cout << 10 << " ";    //10
						}
						else if (former[j] / 4 == 8)
						{
							if (a[i] == 11) {
								outOfHand[k++] = former[j];
								//		cout << " find J";
								former[j] = -1000;
								break;
							}
							//cout << " J ";    //J
						}
						else if (former[j] / 4 == 9) {
							if (a[i] == 12) {
								outOfHand[k++] = former[j];
								//	cout << " find Q";
								former[j] = -1000;

								break;
							}
							//cout << " Q ";    //Q
						}
						else if (former[j] / 4 == 10) {
							if (a[i] == 13) {
								outOfHand[k++] = former[j];
								//			cout << " find K";
								former[j] = -1000;
								break;
							}
							//cout << " K ";   //K
						}
						else if (former[j] / 4 == 11)
						{
							if (a[i] == 1) {
								outOfHand[k++] = former[j];
								//			cout << " find A";
								former[j] = -1000;
								break;
							}
							//cout << " A ";    //A
						}
						else if (former[j] / 4 == 12)
						{
							if (a[i] == 2) {
								outOfHand[k++] = former[j];
								//				cout << " find 2";
								former[j] = -1000;
								break;
							}
							//cout << 2 << " ";   //2
						}
						else if (former[j] == 52)
						{
							if (a[i] == 14) {
								outOfHand[k++] = former[j];
								//					cout << " find 小王";
								former[j] = -1000;
								break;
							}
							//cout << " 小王 ";    //小王
						}
						else if (former[j] == 53)
						{
							if (a[i] == 15) {
								outOfHand[k++] = former[j];
								//					cout << " find 大王";
								former[j] = -1000;
								break;
							}
							//cout << " 大王 ";   //大王
						}
					}
				}

				//把出的牌转换为字符串并发送平台信息
				char info[90] = "";
				for (int i = 0; i <k; i++)
				{
					if (outOfHand[i] >= 10) {
						sCard[0] = outOfHand[i] / 10 + '0';
						sCard[1] = outOfHand[i] % 10 + '0';
						sCard[2] = ',';
						sCard[3] = '\0';
					}
					else {
						sCard[0] = outOfHand[i] % 10 + '0';
						sCard[1] = ',';
						sCard[2] = '\0';
					}
					strcat_s(info, sCard);
				}

				if (strlen(info) >= 1)
				info[strlen(info) - 1] = '\0';//去掉多余的尾部逗号

				strcpy(pDdz->sCommandIn, "PLAY A");
				strcat_s(pDdz->sCommandIn, info);

				//cout << " MSGtoAI " << pDdz->sCommandIn << endl;
				AnalyzeMsg(pDdz);		//分析处理信息
				OutputMsg(pDdz);		//输出信息
					who = 1;//准备到AI出牌
					Pma->Change(outOfHand, 1);

			}//轮到上家出牌
 
			//轮到AI出牌
			else if (who == 1) {

			//	cout << pDdz->sCommandIn << endl;
				cout << "AI thinking..." << endl<<endl;
				
				strcpy(pDdz->sCommandIn, "PLAY WHAT");//转到AI考虑出牌算法

			//	cout << " MSGtoAI " << pDdz->sCommandIn << endl;
				AnalyzeMsg(pDdz);		//分析处理信息
				OutputMsg(pDdz);		//输出信息

				who = 2;//准备到下家出牌
			}

			//轮到下家出牌，同上家出牌代码一致
			else if (who == 2) {
				if (!iFShowOtherCard)
				{
					cout << endl << "下家的手牌 ";
					printCard(latter);
				}
				cout << "输入下家出牌： ";
					int a[21];
					for (int i = 0; i < 20; i++)
					{
						a[i] = -1;
					}

					int i = 0, b, k = 0;

					while (cin >> b) {
						if (b != 0 && b>0 && b<16) {
							a[i++] = b;
						}
						else {
							break;
						}
					}

					//不出牌
					if (i == 0) {
						strcpy(pDdz->sCommandIn, "PLAY C-1");
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
						cout << "                                                       下家PASS " << endl;
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
						//cout << " MSGtoAI " << pDdz->sCommandIn << endl;
						AnalyzeMsg(pDdz);		//分析处理信息
						OutputMsg(pDdz);		//输出信息
						cout << endl;
						who = 0;
						continue;
					}

					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
					cout << "                                                       下家出牌 ： ";

					//若刚出了牌,则显示出了的牌
					for (int i = 0; a[i] > -1; i++) {
						if (a[i] == 1)cout << "A ";
						else if (a[i] == 11)cout << "J ";
						else if (a[i] ==12)cout << "Q ";
						else if (a[i] ==13)cout << "K ";
						else if (a[i] == 14)cout << "小王 ";
						else if (a[i] == 15)cout << "大王 ";
						else cout << a[i] << " ";
					}
					cout << endl;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				
					//显示手牌中的牌编号
					/*cout << endl<< "下家牌编码" << endl;
					for (int i = 0; latter[i]!=-1; i++)
					{
						cout << latter[i] << " ";
					}
					*/

					//在手牌中去掉刚出过的牌
					for (int i = 0; a[i] != -1; i++) {
						for (int j = 0; latter[j] != -1; j++) {
							if (latter[j] / 4 == 0)
							{
								if (a[i] == 3) {
									outOfHand[k++] = latter[j];
								//	cout << " find 3";
									latter[j] = -1000;
									break;
									//cout << "catch 3";
								}
								//cout << 3 << " ";    //3
							}

							else if (latter[j] / 4 == 1)
							{
								if (a[i] == 4) {
									outOfHand[k++] = latter[j];
								//	cout << " find 4";
									latter[j] = -1000;
									//cout << "catch 4";
									break;
								}
							}
							//cout << 4 << " ";   //4
							else if (latter[j] / 4 == 2) {
								if (a[i] == 5) {
									outOfHand[k++] = latter[j];
								//	cout << " find 5";
									latter[j] = -1000;
									break;
									//cout << "catch 5";
								}
								//cout << 5 << " ";   //5
							}
							else if (latter[j] / 4 == 3)
							{
								if (a[i] == 6) {
									outOfHand[k++] = latter[j];
									//cout << " find 6";
									latter[j] = -1000;
									break;
								}
								//cout << 6 << " ";    //6
							}
							else if (latter[j] / 4 == 4) {
								if (a[i] == 7) {
									outOfHand[k++] = latter[j];
									//cout << " find 7";
									latter[j] = -1000;
									break;
								}
								//cout << 7 << " ";    //7
							}
							else if (latter[j] / 4 == 5) {
								if (a[i] == 8) {
									outOfHand[k++] = latter[j];
								//	cout << " find 8";
									latter[j] = -1000;
									break;
								}
								//cout << 8 << " ";    //8
							}
							else if (latter[j] / 4 == 6) {
								if (a[i] == 9) {
									outOfHand[k++] = latter[j];
								//	cout << " find 9";
									latter[j] = -1000;
									break;
								}
								//cout << 9 << " ";   //9
							}
							else if (latter[j] / 4 == 7)
							{
								if (a[i] == 10) {
									outOfHand[k++] = latter[j];
						//			cout << " find 10";
									latter[j] = -1000;
									break;
								}
								//cout << 10 << " ";    //10
							}
							else if (latter[j] / 4 == 8)
							{
								if (a[i] == 11) {
									outOfHand[k++] = latter[j];
							//		cout << " find J";
									latter[j] = -1000;
									break;
								}
								//cout << " J ";    //J
							}
							else if (latter[j] / 4 == 9) {
								if (a[i] == 12) {
									outOfHand[k++] = latter[j];
								//	cout << " find Q";
									latter[j] = -1000;

									break;
								}
								//cout << " Q ";    //Q
							}
							else if (latter[j] / 4 == 10) {
								if (a[i] == 13) {
									outOfHand[k++] = latter[j];
						//			cout << " find K";
									latter[j] = -1000;
									break;
								}
								//cout << " K ";   //K
							}
							else if (latter[j] / 4 == 11)
							{
								if (a[i] == 1) {
									outOfHand[k++] = latter[j];
						//			cout << " find A";
									latter[j] = -1000;
									break;
								}
								//cout << " A ";    //A
							}
							else if (latter[j] / 4 == 12)
							{
								if (a[i] == 2) {
									outOfHand[k++] = latter[j];
					//				cout << " find 2";
									latter[j] = -1000;
									break;
								}
								//cout << 2 << " ";   //2
							}
							else if (latter[j] == 52)
							{
								if (a[i] == 14) {
									outOfHand[k++] = latter[j];
				//					cout << " find 小王";
									latter[j] = -1000;
									break;
								}
								//cout << " 小王 ";    //小王
							}
							else if (latter[j] == 53)
							{
								if (a[i] == 15) {
									outOfHand[k++] = latter[j];
				//					cout << " find 大王";
									latter[j] = -1000;
									break;
								}
								//cout << " 大王 ";   //大王
							}
						}
					}

					//把出的牌转换为字符串并发送平台信息
					char info[90] = "";

				/*	for (int i = 0; i < 4; i++)
					{
						sCard[i] = '\0';
					}
					*/
					for (int i = 0; i <k; i++)
					{
						if (outOfHand[i] >= 10) {
							sCard[0] = outOfHand[i] / 10 + '0';
							sCard[1] = outOfHand[i] % 10 + '0';
							sCard[2] = ',';
							sCard[3] = '\0';
						}
						else {
							sCard[0] = outOfHand[i] % 10 + '0';
							sCard[1] = ',';
							sCard[2] = '\0';
						}
						strcat_s(info, sCard);
					}

					if (strlen(info) >= 1)
					info[strlen(info) - 1] = '\0';//去掉多余的尾部逗号

					strcpy(pDdz->sCommandIn, "PLAY C");
					strcat_s(pDdz->sCommandIn, info);

					//cout << "final" << pDdz->sCommandIn << endl;
					//cout << " MSGtoAI " << pDdz->sCommandIn << endl;
					AnalyzeMsg(pDdz);		//分析处理信息
					OutputMsg(pDdz);		//输出信息
					Pma->Change(outOfHand, 2);
					who = 0;
				}//下家出牌
			
			//出牌后显示手牌信息
			if (iFShowOtherCard)
			{
				cout << endl;
				cout << "上家的手牌 ";
				printCard(former);
				cout << "AI的手牌   ";
				printCard(pDdz->iOnHand);
				cout << "下家的手牌 ";
				printCard(latter);

			}

			cout << "上家的牌数: " << pDdz->iUpPlayerCards << "   下家的牌数: " << pDdz->iDownPlayerCards;
			

			//计算AI手上的牌数
			int usCardCount=0;
			for (int i = 0; pDdz->iOnHand[i]!=-1; i++)
			{
				usCardCount++;
			}

			cout << "    A I 的牌数: " << usCardCount ;

			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
			if (who2 == 0)cout << "      上家为地主 "<<endl<<endl; 
			else if (who2 == 1)cout << "      AI为地主 " << endl << endl;
			else if (who2 == 2)cout << "      下家为地主 " << endl << endl;
			
		    //判断胜负
			if (pDdz->iUpPlayerCards == 0) {
				cout << endl<<"                                                       上家获胜  " << endl;
			//	cout << "				总共进行了 " << pDdz->iRoundNow << " 局, AI赢了 " << pDdz->dWinCount << " 局， 胜率为：" << pDdz->dWinCount / pDdz->iRoundNow <<"%"<< endl;

				strcpy(pDdz->sCommandIn, "GAMEOVER A");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				break;
			}
			else if (pDdz->iDownPlayerCards == 0) {
				cout << endl << "                                                       下家获胜  " << endl;

				//cout << "				总共进行了 " << pDdz->iRoundNow << " 局, AI赢了 " << pDdz->dWinCount << " 局， 胜率为：" << pDdz->dWinCount / pDdz->iRoundNow << "%" << endl;
				strcpy(pDdz->sCommandIn, "GAMEOVER C");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				break;
			}
			else if (usCardCount == 0) {
				cout << endl << "                                                       AI获胜  " << endl;
				
				strcpy(pDdz->sCommandIn, "GAMEOVER B");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				break;
			}
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}
		}
	}
}	
//P02-END

//输出牌的编码
void printNum(int *temp){
	cout << "牌的编码为  ";
	for (int i = 0; temp[i] != -1; i++)
	if (temp[i + 1] != -1)
		cout << temp[i] << ",";
	else
		cout << temp[i];
	cout << endl<<endl;
}

//把编号转换成牌的大小进行输出
void BoardMessageControl::printCard(int*temp) {

	int j;
	int a=0;
	for (int i = 0; temp[i]!=-1; i++)   //不能改成>-1，出了的牌是改成-1000的而不是调到后面
	{
		a++;

	}
	cout << "  ";
	//cout <<"还有"<< a<<"张牌 ";

	for (int pass = a; pass >0; pass--)  //a次循环
	{
		for (int i = 0; i < pass - 1; i++)
		{
			//按编号大小排序
			if (temp[i] > temp[i + 1]) {
				j = temp[i];
				temp[i] = temp[i + 1];
				temp[i + 1] = j;
			}
		}
	}

	//输出牌的实际大小
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
			cout << "J ";      //J
		else if (temp[i] / 4 == 9)
			cout << "Q ";        //Q
		else if (temp[i] / 4 == 10)
			cout << "K ";        //K
		else if (temp[i] / 4 == 11)
		{
			cout << "A ";        //A
		}
		else if (temp[i] / 4 == 12)
		{
			cout << 2 << " ";    //2
		}
		else if (temp[i] == 52)
		{
			cout << "小王 ";    //小王
		}
		else if (temp[i] == 53)
		{
			cout << "大王 ";   //大王
		}
		/*else if (temp[i] == -1000)
		{
			cout << temp[i] << " ";
		}*/
	}
	cout << endl<<endl;
}


//P0301-START获取并处理版本信息DOU
//sCommandIn==DOUDIZHUVER 1.0
//最后修订者:张洪民&梅险,最后修订时间:15-02-10 21:04
//修订内容及目的:修订kPlayerNmae
void BoardMessageControl::GetDou(struct Ddz * pDdz)
{
	int i;								//简单循环变量
	for (i = 0; pDdz->sCommandIn[i] != '\0'; i++)
		pDdz->sVer[i] = pDdz->sCommandIn[i]; //从平台获得的第一行版本信息：DOUUDIZHUVER 1.0
	pDdz->sVer[i] = '\0';
	strcpy_s(pDdz->sCommandOut, "NAME ");
	strcat_s(pDdz->sCommandOut, kPlayerName);
}
//P0301-END

//P0302-START获取并处理轮局信息INF
//sCommandIn==INFO 1,4,1,6,9,2100
//上一次修订者:张晨&梅险,上一次修订时间:15-02-10
//最后修订者:李讯航,最后修订时间:18-7-20
//修订内容及目的：去掉InitRound（）的重复调用，减少运算量
void BoardMessageControl::GetInf(struct Ddz * pDdz)		//轮次信息处理函数(处理数字信息，对应写入对象成员变量中):输入INFO 1/4 1/9 9 2450     输出OK INFO
{
	char c;					//存当前字节信息
	int iCount = 0;			//记录数据个数
	int iTemp = 0;			//中间变量
	int iMessage[7] = { 0 };		//记录数据数组
	int i;
	//逗号为分割符,多一个数字就往左边移一位,多的数字作为个位
	for (i = 5; pDdz->sCommandIn[i] != '\0'; i++)      //从i=5开始  info 1,4,1,9,9,3150
	{
		c = pDdz->sCommandIn[i];
		if (c >= '0' && c <= '9')											//当前字符为0-9
		{
			iTemp = iTemp * 10 + c - '0';                    
			iMessage[iCount] = iTemp;								//晋级选手数
		}
		if (c == ',')														//当前字符为逗号,分割作用
		{
			iCount++;
			iTemp = 0;
		}
	}
	
	
	pDdz->iTurnNow = iMessage[0];						//当前轮次
	pDdz->iTurnTotal = iMessage[1];						//总轮数
	pDdz->iRoundNow = iMessage[2];						//当前局次
	pDdz->iRoundTotal = iMessage[3];					//总局数
	pDdz->iLevelUp = iMessage[4];						//晋级数量
	pDdz->iScoreMax = iMessage[5];						//得分界限
	//InitRound(pDdz);                    //初始化牌的数据
	strcpy_s(pDdz->sCommandOut, "OK INFO");
}
//P0302-END

//P0303-START获取并处理牌套信息DEA
//sCommandIn==DEAL B0,4,5,7,9,10,17,21,25,33,34,39,41,43,44,45,46
//最后修订者:范晓梦 最后修订时间:16-07-31 17:03
//修订内容及目的:增加初始化上手和下手玩家手牌数的编码,更新其他玩家手牌信息
void BoardMessageControl::GetDea(struct Ddz * pDdz)
{
	int i;			      //简单循环变量
	int iNow = 0;		  //当前处理牌序号
	int iCardId = 0;	//当前处理牌编码
	char c;			      //当前指令字符
	pDdz->cDir = pDdz->sCommandIn[5];     //获取本家AI方位编号
	if (pDdz->cDir == 'B')
	{
		cUpPlayer = 'A';
		cDownPlayer = 'C';
	}
	if (pDdz->cDir == 'A')
	{
		cUpPlayer = 'C';
		cDownPlayer = 'B';
	}
	if (pDdz->cDir == 'C')
	{
		cUpPlayer = 'B';
		cDownPlayer = 'A';
	}
	pDdz->cLastPlay = cUpPlayer;
	pDdz->iUpPlayerCards = 0;      //初始化手牌数
	pDdz->iDownPlayerCards = 0;

	for (i = 0; i < 21; i++)//清理iOnhand[]
	{
		pDdz->iOnHand[i] = -1;
	}

	for (i = 6; pDdz->sCommandIn[i] != '\0'; i++)	//依次读取牌码指令,从sCommandIn[6]开始
	{
		c = pDdz->sCommandIn[i];			      //c为当前指令字符
		if (c >= '0' && c <= '9')				        //当前字符为0-9
		{
			iCardId = iCardId * 10 + c - '0';
			pDdz->iOnHand[iNow] = iCardId;
		}
		if (c == ',')							            //当前字符为逗号,分割作用
		{
			iCardId = 0;
			iNow++;
		}
	}
	//CInfo->SortById(pDdz->iOnHand);     //（多余去掉）
	OtherPlayerCards(pDdz, pDdz->iOnHand);//别人的手牌总和就是除去自己的牌
	strcpy_s(pDdz->sCommandOut, "OK DEAL");  //回复信息
	CInfo->SortById(pDdz->iOnHand);  //iOnHand[]从小到大排序
}
//P0303-END

//I02-START计算己方叫牌策略接口
int BoardMessageControl::CalBid(struct Ddz * pDdz)
{
	CallNum *CNum = new CallNum();
	int iMyBid = CNum->CallCardAnalyze(pDdz);		//叫牌
	aiBid = iMyBid;
	delete(CNum);
	return iMyBid;
}

//P0304-START获取并处理叫牌信息BID
//最后修订者:李思寒&梅险,最后修订时间:15-02-08
void BoardMessageControl::GetBid(struct Ddz * pDdz)
{
	//AI叫分,sCommandIn==BID WHAT
	if (pDdz->sCommandIn[4] == 'W')					
	{
		strcpy_s(pDdz->sCommandOut, "BID _0");
		pDdz->sCommandOut[4] = pDdz->cDir;  //叫分的玩家
		pDdz->iBid[pDdz->cDir - 'A'] = CalBid(pDdz);//调用叫牌函数 ，取值为0-3
		pDdz->sCommandOut[5] = pDdz->iBid[pDdz->cDir - 'A'] + '0';  //叫的分数（内部为char转化为int，外部为int转化为char类型）
		//这出现了一个问题，不叫的时候有可能变成 BID B/，而不是BID B0
		pDdz->sCommandOut[6] = '\0';
		return;
	}
	//人工叫分,sCommandIn==BID A/B/C*
	if (pDdz->sCommandIn[4] >= 'A'&&pDdz->sCommandIn[4] <= 'C')  //输入信息为BID A/B/C
	{
		pDdz->iBid[pDdz->sCommandIn[4] - 'A'] = pDdz->sCommandIn[5] - '0';
		strcpy_s(pDdz->sCommandOut, "OK BID");
		return;
	}
}
//P0304-END

//P0305-START获取并处理底牌信息LEF
//sCommandIn==LEFTOVER B27,48,53
//上次修订者:杨洋&梅险,上次修订时间:15-02-08
//最后修订者：李讯航，最后修订时间：18-7-20
//修订内容及目的：互换该函数中最后两行代码的位置，从而改变if语句的内容
//1、只要有人拿到底牌，AI就更新应答信息而不是AI拿到才更新
//2、只有AI拿到底牌才进行牌的重新排序，减少运算量
void BoardMessageControl::GetLef(struct Ddz * pDdz)
{
	int i, iCount = 0;
	char c;
	pDdz->cLandlord = pDdz->sCommandIn[9];    //确定地主方
	//初始化底牌
	pDdz->iLef[0] = 0;
	pDdz->iLef[1] = 0;
	pDdz->iLef[2] = 0;
	//sCommandIn[9]为方位,[10]为底牌
	for (i = 10; pDdz->sCommandIn[i] != '\0'; i++)
	{
		c = pDdz->sCommandIn[i];
		if (c >= '0' && c <= '9')
			pDdz->iLef[iCount] = pDdz->iLef[iCount] * 10 + c - '0';
		else
			iCount++;
	}
	//AI为地主,发底牌
	if (pDdz->sCommandIn[9] == pDdz->cDir)
	{
		pDdz->iOnHand[17] = pDdz->iLef[0];
		pDdz->iOnHand[18] = pDdz->iLef[1];
		pDdz->iOnHand[19] = pDdz->iLef[2];
		pDdz->iOnHand[20] = -1;
		OtherPlayerCards(pDdz, pDdz->iLef);
		pDdz->iUpPlayerCards = 17;
		pDdz->iDownPlayerCards = 17;
	}
	//上家为地主
	if (pDdz->sCommandIn[9] == cUpPlayer)
	{
		pDdz->iUpPlayerCards = 20;
		pDdz->iDownPlayerCards = 17;
	}
	//下家为地主
	if (pDdz->sCommandIn[9] == cDownPlayer)
	{
		pDdz->iUpPlayerCards = 17;
		pDdz->iDownPlayerCards = 20;
	}
	//AI回复收到底牌信息
	if (pDdz->sCommandIn[9] == pDdz->cDir)
		CInfo->SortById(pDdz->iOnHand);					//iOnHand[]从小到大排序
	
	strcpy_s(pDdz->sCommandOut, "OK LEFTOVER");
	
	
}
//P0305-END

//P0306-START 申请出牌
//最后修订者:范晓梦,最后修订时间:16-07-31
//修订内容:更新iOnOtherHand
void BoardMessageControl::CalPla(struct Ddz * pDdz)
{
	//int iMax = 0;
	//SearchEngine *engine = new SearchEngine();

	//memset()函数原型是extern void *memset(void *buffer, int c, int count)
	//buffer：为指针或是数组,c：是赋给buffer的值,count：是buffer的长度.
	//这个函数在socket中多用于清空数组.如:原型是memset(buffer, 0, sizeof(buffer))
	//这句话和用for循环作用一样,但更加简洁

	///////清空itoTable/////
	memset(pDdz->iToTable,-1,sizeof(pDdz->iToTable));
	///////////////
	//得到要出的牌
	engine->SearchAGoodMove(pDdz);
	int i=0;
	//更新己方要出的牌
	CInfo->SortById(engine->ibestMove);
	for (i = 0; engine->ibestMove[i] >= 0; i++)
		pDdz->iToTable[i] = engine->ibestMove[i];
	pDdz->iToTable[i] = -1;//以-1作为间隔.


}

//P030602-START根据己方出牌更新数据
//最后修订者:梅险&夏侯有杰,最后修订时间:15-03-01
//修订内容及目的:修改减少手中牌
void BoardMessageControl::UpdateMyPla(struct Ddz * pDdz)
{
	int i, j, k;
	if (pDdz->iToTable[0] == -1)	//Pass
	{
		pDdz->iOnTable[pDdz->iOTmax][0] = -1;//无出牌
		pDdz->iLastPassCount++;
		if (pDdz->iLastPassCount >= 2)	//连续两家PASS
		{
			pDdz->iLastPassCount = 0;
			pDdz->iLastTypeCount = 0;
			pDdz->iLastMainPoint = -1;
		}
		pDdz->iOTmax++;
	}
	else						//不是PASS
	{
		//增加桌面牌
		for (i = 0; pDdz->iToTable[i] >= 0; i++)
			pDdz->iOnTable[pDdz->iOTmax][i] = pDdz->iToTable[i];
		pDdz->iOnTable[pDdz->iOTmax][i] = -1;
		pDdz->iOTmax++;    //当前为本局第几次出牌
		//减少手中牌
		for (j = 0; pDdz->iToTable[j] >= 0; j++)
		{
			for (i = 0; pDdz->iOnHand[i] >= 0; i++)
				if (pDdz->iOnHand[i] == pDdz->iToTable[j])
				{
					for (k = i; pDdz->iOnHand[k] >= 0; k++)
						pDdz->iOnHand[k] = pDdz->iOnHand[k + 1];
					break;
				}
		}
		pDdz->iLastPassCount = 0;
		pDdz->iLastTypeCount = CInfo->AnalyzeTypeCount(pDdz->iToTable); //分析牌型和张数
		pDdz->iLastMainPoint = CInfo->AnalyzeMainPoint(pDdz->iToTable); //计算主牌最小点数
		pDdz->cLastPlay = pDdz->cDir;
	}
}
//PO30602-END

//P030603-START根据他人出牌更新数据
//最后修订者:范晓梦,最后修订时间:16-07-31
//修订内容：记录牌到iOnOtherHand
void BoardMessageControl::UpdateHisPla(struct Ddz * pDdz)
{
	int i;
	int iCardId;
	int iNow;
	char c;
	pDdz->cLastPlay = pDdz->sCommandIn[5];
	//减少手中牌
	if (pDdz->sCommandIn[6] == '-')// PLAY ?-1 即PASS
	{
		pDdz->iOnTable[pDdz->iOTmax][0] = -1;
		pDdz->iLastPassCount++;
		if (pDdz->iLastPassCount >= 2)	//连续两家PASS
		{
			pDdz->iLastPassCount = 0;
			pDdz->iLastTypeCount = 0;
			pDdz->iLastMainPoint = -1;
		}
		pDdz->iOTmax++;
	}
	else						// PLAY 出牌
	{
		for (i = 0; i<21; i++)							//清理iToTable[]
			pDdz->iToTable[i] = -1;
		iCardId = 0;
		iNow = 0;
		for (i = 6; pDdz->sCommandIn[i] != '\0'; i++)		//依次读取牌码
		{
			c = pDdz->sCommandIn[i];					//c为当前指令字符
			if (c >= '0' && c <= '9')				        //当前字符为0-9
			{
				iCardId = iCardId * 10 + c - '0';
				pDdz->iToTable[iNow] = iCardId;
			}
			if (c == ',')									//当前字符为逗号,
			{
				iCardId = 0;
				iNow++;
			}
		}
		//增加桌面牌
		for (i = 0; pDdz->iToTable[i] >= 0; i++)
		{
			pDdz->iOnTable[pDdz->iOTmax][i] = pDdz->iToTable[i];
		}
		pDdz->iOnTable[pDdz->iOTmax][i] = -1;//分割

		//减少iOnOtherHand牌
		//对于AI,只知道自己的牌、上下家牌数、底牌和出过的牌，在此基础上对上下家的牌进行推测
		if (pDdz->cLastPlay == cUpPlayer)
			pDdz->iUpPlayerCards -= i;
		else if (pDdz->cLastPlay == cDownPlayer)
			pDdz->iDownPlayerCards -= i;

		OtherPlayerCards(pDdz, pDdz->iToTable);//删除出过的牌，计算其他两位玩家手中的剩牌

		pDdz->iLastPassCount = 0;
		pDdz->iLastTypeCount = CInfo->AnalyzeTypeCount(pDdz->iToTable);
		pDdz->iLastMainPoint = CInfo->AnalyzeMainPoint(pDdz->iToTable);
		pDdz->iOTmax++;
	}
}
//PO30603-END

//P0306-START获取并处理出牌信息PLA
//sCommandIn==PLAY WHAT
//最后修订者:梅险,最后修订时间:15-02-08
void BoardMessageControl::GetPla(struct Ddz * pDdz)
{
	if (pDdz->sCommandIn[5] == 'W')					//接收信息为PLAY WHAT：应调用出牌计算函数计算出牌
	{
		//cout << "calPla" << endl;
		CalPla(pDdz);					//调用出牌计算函数计算出牌
		strcpy_s(pDdz->sCommandOut, "PLAY _");
		pDdz->sCommandOut[5] = pDdz->cDir;		//输出命令的预备信息准备到sCommandOut数组

		CInfo->AppendCardsToS(pDdz->iToTable, pDdz->sCommandOut);		//要出牌数组iToTable[]中的数字转化为字符并连接到sCommandOut中
		
			if (iFShowInConsole)
			{
				//Test
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
				if (pDdz->iToTable[0] != -1){
				cout << "\n                                                       AI 出牌 ： ";
				printCard(pDdz->iToTable);
				}
				else
					cout << "\n                                                       AI PASS"<<endl;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			}

		UpdateMyPla(pDdz);		//根据己方出牌更新数据
	}
	else										//否则收到信息为其它玩家出牌
	{
		if (pDdz->sCommandIn[5] == pDdz->cDir)//数据输错
			return;
		UpdateHisPla(pDdz);		//根据他人出牌更新数据
		strcpy_s(pDdz->sCommandOut, "OK PLAY");//回复收到
	}
	//当前手数加1
	//pDdz->iOTmax++;
}
//P0306-END

//P0307-START获取并处理胜负信息GAM
//sCommandIn==GAMEOVER C
//sCommandOut == OK GAMEOVER
//上一次修订者:梅险,上一次修订时间:15-02-08 00:13
//最后修订者：李讯航，最后修订时间：18-7-22
//修订内容：增加在控制台中调试的异常处理，以及添加游戏结束的输出信息
void BoardMessageControl::GetGam(struct Ddz * pDdz)
{
	pDdz->cWinner = pDdz->sCommandIn[9];			//胜利者方位编号

	//无人叫地主
	if (iFShowInConsole){
		if (pDdz->cWinner == 'D'){
			InitRound(pDdz);							//引擎状态变量设为重新开始下一局
			cout << endl;
			strcpy_s(pDdz->sCommandOut, "无人叫地主，重新开局");
			pDdz->iRoundNow--;   //减去一局
			pDdz->dWinCount--;
			OutputMsg(pDdz);		//输出信息
		}
	}
		//Test 累计胜利次数：dWinCount变量
		if (pDdz->cDir != pDdz->cLandlord )            //AI不是地主
		{
			if (pDdz->cLandlord != pDdz->cWinner)   //地主没有赢
			{
				pDdz->dWinCount++;    //AI赢的次数加一
			}
		}
		else                    //AI是地主
		{
			if (pDdz->cDir == pDdz->cWinner)   //AI赢了
			{
				pDdz->dWinCount++;
			}
		}


		if (pDdz->iRoundNow == pDdz->iRoundTotal)		//如果当前局数与每轮局相等时
		{
			pDdz->iStatus = 0;							//引擎状态变量设为结束
			if (iFShowInConsole){
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
				if (pDdz->iRoundNow > 0)
				{
					cout << endl << "总局数为 " << pDdz->iRoundTotal << " 局，当前游戏已结束\n"
						<< "A I 赢了 " << setiosflags(ios::fixed) << setprecision(0) << pDdz->dWinCount << " 局，胜率为：";
					if (pDdz->iRoundNow>1)
						cout << setiosflags(ios::fixed) << setprecision(2) << 100 * ((double)(pDdz->dWinCount) / (pDdz->iRoundTotal )) << "%" << endl;
					else
						cout << setiosflags(ios::fixed) << setprecision(2) << "0 %" << endl;
				}
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				cout << endl;
				system("pause");
			}
		}
		else											//否则
		{
			pDdz->iRoundNow++;							//当前局次加1
			InitRound(pDdz);							//引擎状态变量设为重新开始下一局
		}
		strcpy_s(pDdz->sCommandOut, "OK GAMEOVER");
	}

//PO307-END


//P03-START分析处理信息
//最后修订者:梅险,最后修订时间:15-02-08 00:13
void BoardMessageControl::AnalyzeMsg(struct Ddz * pDdz)
{
	char sShort[4];
	int i;
	for (i = 0; i<3; i++)
		sShort[i] = pDdz->sCommandIn[i]; //[0-2]
	sShort[3] = '\0';
	if (strcmp(sShort, "DOU") == 0)					//版本信息
	{
		GetDou(pDdz);
		return;
	}
	if (strcmp(sShort, "INF") == 0)					//轮局信息
	{
		GetInf(pDdz);
		return;
	}
	if (strcmp(sShort, "DEA") == 0)					//牌套信息
	{
		GetDea(pDdz);
		return;
	}
	if (strcmp(sShort, "BID") == 0)					//叫牌过程
	{
		GetBid(pDdz);
		return;
	}
	if (strcmp(sShort, "LEF") == 0)					//底牌信息
	{
		GetLef(pDdz);
		return;
	}
	if (strcmp(sShort, "PLA") == 0)					//出牌过程
	{
		GetPla(pDdz);
		return;
	}
	if (strcmp(sShort, "GAM") == 0)					//胜负信息
	{
		GetGam(pDdz);
		return;
	}
	if (strcmp(sShort, "EXI") == 0)					//强制退出
	{
		exit(0);
	}
	
	strcpy_s(pDdz->sCommandOut, "ERROR at module AnalyzeMsg,sCommandIn without match");
	return;
}
//P03-END



//P04-START输出信息
//最后修订者:梅险,最后修订时间:15-02-08 00:13
void BoardMessageControl::OutputMsg(struct Ddz * pDdz)
{
	//Test
	/*cout <<endl<< "LastPassCount " << pDdz->iLastPassCount << endl;
	cout << "LastMainPoint " << pDdz->iLastMainPoint << endl;
	cout << "LastTypeCount " << pDdz->iLastTypeCount << endl << endl;*/
	//cout对于char[]有重载，这是一个特例，只有字符串数组会输出整个数组，如果是int数组或者别的数组，则不会。
	cout << pDdz->sCommandOut << endl;
}
//P04-END

//P05-START计算其它数据
//最后修订者:梅险,最后修订时间:15-02-08
/*void BoardMessageControl::CalOthers(struct Ddz * pDdz)
{
	pDdz->iVoid = 0;
}
//P05-END*/


//传入要删除的牌，计算两位玩家手中剩牌，并存储在iOnOtherHand中
//最后修订者：范晓梦
//最后修改时间：2016/7/31
//看不懂
void BoardMessageControl::OtherPlayerCards(Ddz *pDdz, int *iCards)
{
	//内置函数，在已经排好序的情况下调用
	auto deleteElem = [](int *iOnHand, int elem){  //auto可以在声明变量的时候根据变量初始值的类型自动为此变量选择匹配的类型
		int i = 0, j = 0;
		for (j = 0; j < 55; j++)
		{

			if (iOnHand[j] == elem)  //匹配
				i++;     
			else                   //不匹配
			{
				iOnHand[j - i] = iOnHand[j];  //删去i张牌
			}
		}
	};
	
	//start
	int j;

	//传入底牌
	if (iCards == pDdz->iLef)
	{
		for (j = 0; j < 3; j++)
		{
			deleteElem(pDdz->iOnOtherHand, iCards[j]);
		}
	}
	else
	{
		for (j = 0; iCards[j] >= 0; j++)
		{
			deleteElem(pDdz->iOnOtherHand, iCards[j]);
		}
	}
}