#pragma once
#include<Windows.h>
#include"card.h"

enum HAND {
	e_None,
	e_Suit2,
	e_Strt3,
	e_Suit3,
	e_One,
	e_Strt4,
	e_Suit4,
	e_Two,
	e_Three_J,
	e_Three,
	e_Straight_J,
	e_Straight,
	e_Flush_J,
	e_Flush,
	e_FullHouse_J,//全てホールド
	e_FullHouse,//全てホールド
	e_Four_J,//cntNumが3のカードとJoker
	e_Four,	//cntNumが4のカード
	e_StrtFrush,//全てホールド
	e_Five_J,//全てホールド
	e_Five,	//全てホールド
	e_RSF	//全てホールド
};

class infoHoldCard {
public:
	int p[5];

	bool p1;
	bool p2;
	bool p3;
	bool p4;
	bool p5;


	int num1, num2;
	int mark1, mark2;

public:
	infoHoldCard() :num1(-1), num2(-1), mark1(e_NoMark), mark2(e_NoMark) {
		for (int i = 0; i < 5; i++) {
			p[i] = 0;
		}
	}
	void SetState(bool s1, bool s2, bool s3, bool s4, bool s5, int n1, int n2, int mark1, int mark2) {
		this->p[0] = s1;
		this->p[1] = s2;
		this->p[2] = s3;
		this->p[3] = s4;
		this->p[4] = s5;

		this->p1 = s1;
		this->p2 = s2;
		this->p3 = s3;
		this->p4 = s4;
		this->p5 = s5;

		this->num1 = n1 - 1;
		this->num2 = n2 - 1;

		this->mark1 = mark1;
		this->mark2 = mark2;
	}
	void Reset() {
		this->SetState(false, false, false, false, false, NONE, NONE, e_NoMark, e_NoMark);
	}

	void ShowInfo() {
		for (int i = 0; i < 5; i++)
			std::cout << p[i] << std::endl << std::endl;

		std::cout << p1 << std::endl;
		std::cout << p2 << std::endl;
		std::cout << p3 << std::endl;
		std::cout << p4 << std::endl;
		std::cout << p5 << std::endl;

		std::cout << "num1:" << num1 << std::endl;
		std::cout << "num2:" << num2 << std::endl;
		std::cout << "mark1:" << mark1 << std::endl;
		std::cout << "mark2:" << mark2 << std::endl;

	}
};


//ポーカーを行うクラス
class poker {
private:
	infoHoldCard state;
	infoHoldCard tempInfo;

	bool judgeHold;

	bool Joker;		//ワイルドカードのフラグ
	int suitPearIndex = 0;	//ペアの組の添字
	int suitPear[2] = { 0 };	//ペアの記憶変数
	int suitFive = 0, suitFour = 0, suitThree = 0;	//4カード、3カードのフラグ

	int numPearIndex = 0;	//...数字のペアの組の添字
	int numPear[2] = { 0 }; //...ペアの数字を記録
	int numFive = 0, numFour = 0, numThree = 0;

	int strtCheck[4] = { 0 };	//...ストレートの判別用

	int cntMark[e_Num] = { 0 };	//...手札にある数字の枚数を記録。添字が数字、値が枚数
	int cntNum[13] = { 0 };		//...手札に存在するマークの枚数を記録。添字がマーク、値が枚数
	card hand[5];	//手札
	std::vector<card> cemetery;	//墓地
public:
	poker();
	void setHand(int index, int num);
	void drawHand(void);	//手札を引く
	void holdHand(void);	//ホールドするカードの選択
	void dropHand(void);	//手札を捨てる
	void showHand(void);	//手札の参照
	int checkPokerHand(void);	//役の確認
	void JudgeHoldCard(HWND hwnd,RECT rc,int w, int h, int delayRange);
	void Reset(void);		//ゲームのリセット
	void initPokerHand();	//初期化
	void DEBUG_DRAWHAND();	//デバッグ用の役
private:
	bool checkCemetery(card* hand);	//手札を引く際に墓地の確認

	/*----- 役判定の関数 -----*/
	void cntNum_and_Mark();	//カードの枚数とマークを確認	2015/12/21 実装
	int RoyalStraightFrush();//ロイヤル・ストレート・フラッシュの確認
	int checkFiveCard();	//ファイブカードの確認	2015/12/21 実装
	int checkStraightFrush();//ストレート・フラッシュの確認
	int checkFourCard();	//フォーカードの確認		2015/12/21 実装
	int checkFullHouse();	//フルハウスの確認
	int checkFlush();		//フラッシュの確認
	int checkStraight();	//ストレートの確認		2016/01/05 「デバッグ対象_TwoPearにならない.png」時の動作がおかしい
	int checkThreeCard();	//スリーカードの確認		2015/12/21 実装
	int checkTwoPear();		//ツーペアの確認			2015/12/21 実装		
	int checkOnePear();		//ワンペアの確認			2015/12/21 実装		2016/01/04 動作が改善済み

	/*----- カードのホールドを行う関数 -----*/
	void DoHoldCard(infoHoldCard* info,HWND hwnd,RECT rc, int w, int h, int delayRange);

	/*----- DEBUG用関数群 -----*/
	void DebugHand();
};