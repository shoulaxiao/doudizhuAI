//主函数：实现与平台之间协议的交互
#include "BoardMessageControl.h"
#include<fstream>
int	main()
{
	//freopen("t.log","w",stdout);
	/*ofstream outfile;
	outfile.open("game.txt"); 
	outfile.close();*/
	struct Ddz tDdz, *pDdz = &tDdz;
	BoardMessageControl * pBoardControl = new BoardMessageControl();
	pBoardControl->InitTurn(pDdz);			//初始化数据
	
	//修订者：李讯航 修订时间：18-7-22
	//增加部分，在控制台中模拟在平台上进行游戏
	if (iFShowInConsole){
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED |FOREGROUND_GREEN);
	    strcpy(pDdz->sCommandIn, "DOUDIZHUVER 1.0"); //模拟平台发送版本信息
	    pBoardControl->AnalyzeMsg(pDdz);
	    pBoardControl->OutputMsg(pDdz);
	    cout << "\n请设置游戏总局数：" << endl;
	    cin >> pDdz->iRoundTotal;
	    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	//NAME 
	}
	//

	while (pDdz->iStatus != 0)            //循环，每次从平台获得一行信息，然后经历分析和输出的过程
	{
		pBoardControl->InputMsg(pDdz);			//输入信息
		pBoardControl->AnalyzeMsg(pDdz);		//分析处理信息
		pBoardControl->OutputMsg(pDdz);		    //输出信息
		//pBoardControl->CalOthers(pDdz);		//计算其它数据
	}
	delete(pBoardControl);
	return 0;
}
	