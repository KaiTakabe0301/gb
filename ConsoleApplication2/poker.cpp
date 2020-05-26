#include<iostream>
#include<random>
#include<opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include"poker.h"

//トランプのクリック開始範囲
int scrX[5] = {64,180,291,402,514};
int scrY[5] = {430,430,430,430,430};
//手札を引く
void poker::setHand(int index, int num) {
	hand[index].num = num / 4;
	if (hand[index].num == 13)
		hand[index].mark = e_Joker;
	else
		hand[index].mark = num % 4 + 1;
	hand[index].holdFlag = false;

	hand[index].x = scrX[index];
	hand[index].y = scrY[index];

}

void showMark(int mark) {
	switch (mark)
	{
	case e_NoMark:
		std::cout << "NoMark" << std::endl;
		break;
	case e_Diamond:
		std::cout << "Diamond" << std::endl;

		break;
	case e_Clover:
		std::cout << "Clover" << std::endl;

		break;
	case e_Spade:
		std::cout << "Spade" << std::endl;

		break;
	case e_Heart:
		std::cout << "Heart" << std::endl;

		break;
	case e_Joker:
		std::cout << "Joker" << std::endl;

		break;
	default:
		std::cout << "" << std::endl;
		break;
	}
}

//...qsort用
int compCardNum(const void *a, const void *b) {
	const card* aa = (const card*)a;
	const card* bb = (const card*)b;
	return aa->num - bb->num;
}

poker::poker() {
	initPokerHand();
}

//墓地の確認
bool poker::checkCemetery(card* hand) {
	for (auto& e : cemetery) {
		if (e.equal(hand))
			return true;	//一度引いたカードであればtrue
	}
	return false; //一度も引いたことでないカードであればfalse
}

//...山札をリセットする。再度53枚からスタート。
void poker::Reset(void) {
	Joker = false;
	judgeHold = false;
	for (int i = 0; i < 5; i++)
		hand[i].holdFlag = false;
	dropHand();
	cemetery.clear();
}

//手札を引く
void poker::drawHand(void) {
	int i, j;
	card temp;
	std::random_device rnd;
	std::mt19937 mt(rnd());
	std::uniform_int_distribution<> randNum(0, 12);	//数字の乱数範囲設定 実際の数字より１小さい
	std::uniform_int_distribution<> randMark(0, 4);	//マークの乱数範囲設定
	std::uniform_int_distribution<> randTrump(0, 52);//4で割った答えが数字、余りがマーク 52がジョーカー

	if (cemetery.size() < 48) {	//墓地が48枚以下ならば
		for (i = 0; i < 5;) {
			if (!hand[i].holdFlag) {	//カードがホールドされていなければ
										/*
										temp.num = randNum(mt);
										temp.mark = randMark(mt);
										temp.holdFlag = false;
										*/

				int tempCard = randTrump(mt);
				temp.num = tempCard / 4;	//0～13の数字が出てくる 13のみJoker
				temp.mark = tempCard % 4 + 1; //Diamond(0) , Clover(1) , Spade(2) , Heart(3) しか出ない
				temp.holdFlag = false;

				//...生成したカードがJokerなら
				if (temp.num == 13) {
					if (Joker == false) {		//まだ一度もJokerが引かれていないなら
						Joker = true;	//Jokerのフラグを立てる
						temp.mark = e_Joker;	//Jokerのみ13
						hand[i++] = temp;
					}
				}
				//...生成したカードがJoker以外なら
				else if (!checkCemetery(&temp)) {		//墓地の確認
					for (j = 0; j < i; j++) {
						if (hand[j].equal(&temp))	//手札内での確認
							break;
					}
					if (i == j)	//手札内で被っていなければ
						hand[i++] = temp;	//そのカードを手札に加える
				}

			}
			else {	//ホールドされていたら
				hand[i].holdFlag = false;
				i++;//次の札へ
			}
		}
	}
	else {
		std::cout << "END POKER" << std::endl;
	}

	//手札を数字の大きい順に並び替える
	qsort(hand, sizeof(hand) / sizeof(*hand), sizeof(card), compCardNum);
}

//...初期化
void poker::initPokerHand() {
	Joker = false;
	tempInfo.Reset();

	suitPearIndex = 0;
	memset(suitPear, 0, sizeof(suitPear));
	suitFive = suitFour = suitThree = 0;

	numPearIndex = 0;
	memset(numPear, -1, sizeof(numPear));
	numFive = numFour = numThree = 0;

	memset(strtCheck, 0, sizeof(strtCheck));

	memset(cntMark, 0, sizeof(cntMark));
	memset(cntNum, 0, sizeof(cntNum));
}

//...手札を捨てる
void poker::dropHand(void) {
	for (auto& h : hand) {
		if (h.holdFlag == false) {
			h.holdFlag = false;
			cemetery.push_back(h);
		}
	}
}

//手札の参照
void poker::showHand(void) {
	std::cout << "------------------------" << std::endl;
	for (int i = 0; i < 5; i++) {
		switch (hand[i].mark)
		{
		case e_Clover: std::cout << "C ";
			break;
		case e_Diamond: std::cout << "D ";
			break;
		case e_Heart: std::cout << "H ";
			break;
		case e_Spade: std::cout << "S ";
			break;
		case e_Joker: std::cout << "J ";
			break;
		default: std::cout << "error ";
			break;
		}
		std::cout << hand[i].num + 1 << " ";
		std::cout << hand[i].holdFlag << std::endl;
	}
	std::cout << "------------------------" << std::endl;

	//cntNum_and_Mark();
	//DebugHand();
}

//...デバッグ用
void poker::DebugHand() {
	std::cout << "-------------- Joker Flag DEBUG -------------------" << std::endl;
	std::cout << "Joker Flag:" << Joker << std::endl;

	std::cout << "-------------- Num DEBUG -------------------" << std::endl;
	for (int i = 0; i < 13; i++) {
		std::cout << i + 1 << ":" << cntNum[i] << std::endl;
	}

	std::cout << "-------------- Mark DEBUG -------------------" << std::endl;
	for (int i = 0; i < e_Num; i++) {
		switch (i) {
		case e_Clover:
			std::cout << "Clover:" << cntMark[i] << std::endl;
			break;
		case e_Diamond:
			std::cout << "Diamond:" << cntMark[i] << std::endl;
			break;
		case e_Spade:
			std::cout << "Spade:" << cntMark[i] << std::endl;
			break;
		case e_Heart:
			std::cout << "Heart:" << cntMark[i] << std::endl;
			break;
		case e_Joker:
			std::cout << "Joker:" << cntMark[i] << std::endl;
			break;
		}
	}

	std::cout << "-------------- Mark Pear DEBUG -------------------" << std::endl;
	std::cout << "Pear Mark:";
	switch (suitPear[0])
	{
	case e_Clover:
		std::cout << "Clover" << std::endl;
		break;
	case e_Diamond:
		std::cout << "Diamond" << std::endl;
		break;
	case e_Spade:
		std::cout << "Spade" << std::endl;
		break;
	case e_Heart:
		std::cout << "Heart" << std::endl;
		break;
	case e_None:
		std::cout << "None" << std::endl;
		break;
	}

	std::cout << "-------------- Num Pear DEBUG -------------------" << std::endl;
	std::cout << "Pear Num:" << numPear[0] << std::endl;

	std::cout << "-------------- Suit Hand DEBUG -------------------" << std::endl;
	std::cout << "suitFive:" << suitFive << std::endl;
	std::cout << "suitFour:" << suitFour << std::endl;
	std::cout << "suitThree:" << suitThree << std::endl;

	std::cout << "-------------- Num Hand DEBUG -------------------" << std::endl;
	std::cout << "numFive:" << numFive << std::endl;
	std::cout << "numFour:" << numFour << std::endl;
	std::cout << "numThree:" << numThree << std::endl;

	std::cout << "-------------- NumPearIndex DEBUG -------------------" << std::endl;
	std::cout << "numPearIndex:" << numPearIndex << std::endl;

	std::cout << "-------------- MarkPearIndex DEBUG -------------------" << std::endl;
	std::cout << "MarkPearIndex:" << suitPearIndex << std::endl;

	std::cout << "-------------- Straight DEBUG -------------------" << std::endl;
	for (int i = 0; i < 4; i++)
		std::cout << "Strt[" << i << "]:" << strtCheck[i] << std::endl;



	std::cout << "-------------- END DEBUG -------------------" << std::endl;

}

//...役を判断、どのカードをホールドするか考える
int poker::checkPokerHand(void) {
	int weight = 0;

	initPokerHand();
	cntNum_and_Mark();

	//DebugHand();
	/*
	if ((weight = checkFiveCard())) {
	//std::cout << "Five Card" << std::endl;
	//std::cout << "weight:" <<weight << std::endl;
	}
	if ((weight=checkStraightFrush())) {
	//std::cout << "Straight Frush" << std::endl;
	//std::cout << "weight:" << weight << std::endl;
	}
	if ((weight=checkFourCard()) >= e_Four_J) {
	//std::cout << "Four Card" << std::endl;
	//std::cout << "weight:" << weight << std::endl;
	}
	if ((weight=checkFullHouse())) {
	//std::cout << "Full House" << std::endl;
	//std::cout << "weight:" << weight << std::endl;
	}
	if ( weight < checkFlush() ) {
	std::cout << "Flush" << std::endl;
	//std::cout << "weight:" << weight << std::endl;
	}
	if ((weight=checkStraight()) >= e_Straight_J) {
	//std::cout << "weight:" << weight << std::endl;
	//std::cout << "Straight" << std::endl;
	//std::cout << "weight:" << weight << std::endl;
	}
	if ((weight = checkThreeCard())) {
	//std::cout << "Three Card" << std::endl;
	//std::cout << "weight:" << weight << std::endl;
	}
	if ((weight=checkTwoPear())) {
	//std::cout << "Two Pear" << std::endl;
	//std::cout << "weight:" << weight << std::endl;
	}
	if ((weight=checkOnePear())) {
	//std::cout << "One Pear" << std::endl;
	//std::cout << "weight:" << weight << std::endl;
	}*/

	weight = checkFiveCard();

	if (weight < checkStraightFrush()) {
		weight = checkStraightFrush();
		state = tempInfo;
	}

	if (weight < checkFourCard()) {
		weight = checkFourCard();
		state = tempInfo;
	}

	if (weight < checkFullHouse()) {
		weight = checkFullHouse();
		state = tempInfo;
	}

	if (weight < checkFlush()) {
		weight = checkFlush();
		state = tempInfo;
	}

	if (weight < checkStraight()) {
		weight = checkStraight();
		state = tempInfo;
	}

	if (weight < checkThreeCard()) {
		weight = checkThreeCard();
		state = tempInfo;
	}

	if (weight < checkTwoPear()) {
		weight = checkTwoPear();
		state = tempInfo;
	}

	if (weight < checkOnePear()) {
		weight = checkOnePear();
		state = tempInfo;
	}

#ifdef DEBUG_MODE
	DebugHand();
#endif

	std::cout << "weight:" << weight << std::endl;
	return weight;
}

//...checkPokerHandでの判断を基準に実際にカードをホールドする
void poker::JudgeHoldCard(HWND hwnd,RECT rc, int w, int h, int delayRange) {
	if (judgeHold == false) {
		qsort(hand, sizeof(hand) / sizeof(*hand), sizeof(card), compCardNum);
		int hand = checkPokerHand();

		switch (hand)
		{
		case e_None:
			std::cout << "該当なし" << std::endl << std::endl;
			break;
		case e_Strt3:
			std::cout << "ストレートを狙うべき" << std::endl << std::endl;
			break;
		case e_Suit2:
			std::cout << "フラッシュを狙うべき" << std::endl << std::endl;
			break;
		case e_Suit3:
			std::cout << "フラッシュを狙うべき" << std::endl << std::endl;
			break;
		case e_One:
			std::cout << "ワンペアをホールド" << std::endl;
			std::cout << "対象:" << numPear[0] << std::endl << std::endl;
			break;
		case e_Strt4:
			std::cout << "ストレートを狙うべき" << std::endl << std::endl;
			break;
		case e_Suit4:
			std::cout << "フラッシュを狙うべき" << std::endl << std::endl;
			break;
		case e_Two:
			std::cout << "ツーペアをホールド" << std::endl;
			std::cout << "対象:" << numPear[0] << "&" << numPear[1] << std::endl << std::endl;
			break;
		case e_Three_J:
			std::cout << "スリーカードをホールド" << std::endl << std::endl;
			std::cout << "対象:" << numPear[0] << std::endl << std::endl;
			break;
		case e_Three:
			std::cout << "スリーカードをホールド" << std::endl << std::endl;
			std::cout << "対象:" << numThree << std::endl << std::endl;
			break;
		case e_Straight_J:
			std::cout << "ストレート：全てホールド" << std::endl << std::endl;
			break;
		case e_Straight:
			std::cout << "ストレート：全てホールド" << std::endl << std::endl;
			break;
		case e_Flush_J:
			std::cout << "フラッシュ：全てホールド" << std::endl << std::endl;
			break;
		case e_Flush:
			std::cout << "フラッシュ：全てホールド" << std::endl << std::endl;
			break;
		case e_FullHouse_J:
			std::cout << "フルハウス：全てホールド" << std::endl << std::endl;
			break;
		case e_FullHouse:
			std::cout << "フルハウス：全てホールド" << std::endl << std::endl;
			break;
		case e_Four_J:
			std::cout << "フォーカードをホールド" << std::endl << std::endl;
			std::cout << "対象:" << numThree << std::endl << std::endl;
			break;
		case e_Four:
			std::cout << "フォーカードをホールド" << std::endl << std::endl;
			std::cout << "対象:" << numFour << std::endl << std::endl;
			break;
		case e_StrtFrush:
			std::cout << "ストレート・フラッシュ：全てホールド" << std::endl << std::endl;
			break;
		case e_Five_J:
			std::cout << "ファイブカード：全てホールド" << std::endl << std::endl;
			break;
		case e_Five:
			std::cout << "ファイブカード：全てホールド" << std::endl << std::endl;
			break;
		case e_RSF:
			std::cout << "ロイヤル・ストレート・フラッシュ：全てホールド" << std::endl << std::endl;
			break;
		default:
			std::cout << "Error" << std::endl << std::endl;
			break;
		}
		DoHoldCard(&state,hwnd,rc,w,h,delayRange);
		judgeHold = true;
		//state.ShowInfo();
	}
}

void poker::DoHoldCard(infoHoldCard* info,HWND hwnd,RECT rc,int w,int h,int delayRange) {
	std::random_device rnd;
	std::mt19937 mt(rnd());
	std::uniform_int_distribution<> randW(0, w);	//0.0~クリック幅でmt法
	std::uniform_int_distribution<> randH(0, h);	//0.0~クリック幅でmt法
	std::uniform_int_distribution<> randD(230, 230 + delayRange);

	for (int i = 0; i < 5; i++)
		hand[i].holdFlag = info->p[i];

	for (int i = 0; i < 5; i++) {
		if (hand[i].num == info->num1 || hand[i].num == info->num2)
			hand[i].holdFlag = true;
		if (hand[i].mark == info->mark1 || hand[i].mark == info->mark2)
			hand[i].holdFlag = true;
	}
	showHand();

	SetForegroundWindow(hwnd);
	/*----- カードのホールド -----*/
	for (int i = 0; i < 5; i++) {
		if (hand[i].holdFlag) {
			SetCursorPos(rc.left + hand[i].x + randW(mt), rc.top + hand[i].y + randH(mt));
			cvWaitKey(randD(mt));
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		}
	}
}

//カードの数字とマークのカウント
void poker::cntNum_and_Mark(void) {

	//カードを柄と数字を格納
	for each (card var in hand)
	{
		cntMark[var.mark]++;	//添字がマークに対応、値が枚数
		if (var.mark != e_Joker)	//Jokerの場合はカウントしない
			cntNum[var.num]++;		//添字が数字に対応、値が枚数
	}

	//Jokerの確認、Jokerが存在したら絶対にホールドを行う
	if (cntMark[e_Joker])
		Joker = true;

	for (int i = 0; i < 13; i++) {
		switch (cntNum[i])
		{
		case 5:
			numFive = i + 1;
			break;
		case 4:
			numFour = i + 1;
			break;
		case 3:
			numThree = i + 1;
			break;
		case 2:
			numPear[numPearIndex++] = i + 1;
			break;
		default:
			break;
		}
	}

	//柄の確認
	for (int i = 0; i < e_Num; i++) {
		switch (cntMark[i]) {
		case 5:	//全て同じ柄なら
			suitFive = i;
			break;
		case 4:	//4枚同じ柄なら
			suitFour = i;	//柄を記録
			break;
		case 3:	//3枚同じ柄なら
			suitThree = i;	//柄を確認
			break;
		case 2:	//2枚同じ柄が確認できれば
			suitPear[suitPearIndex++] = i;	//1組目の柄を記録し、2組目の記録に移る
			break;
		default:
			break;
		}
	}
}

//ストレート・フラッシュ
int poker::checkStraightFrush(void) {
	if (checkFlush() >= e_Flush_J && checkStraight() >= e_Straight_J) {
		tempInfo.SetState(1, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
		return e_StrtFrush;		//ストレート・フラッシュの完成	//全てホールド
	}
	return 0;
}

//フルハウス 3パターン
int poker::checkFullHouse(void) {
	if (checkThreeCard() == e_Three && numPearIndex >= 1) {
		tempInfo.SetState(1, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
		return e_FullHouse;		//フルハウスの完成	//全てホールド
	}
	else if (checkThreeCard() == e_Three_J && numPearIndex == 2) {
		tempInfo.SetState(1, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
		return e_FullHouse_J;	//Joker込でフルハウスの完成	//全てホールド
	}
	else
		return 0;
}

//フラッシュ 5パターン
int poker::checkFlush(void) {
	/*----- フラッシュが成立した場合 -----*/
	if (suitFive > 0) {	//ワイルドカードを含まない場合
		tempInfo.SetState(1, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
		return e_Flush;		//フラッシュの完成	//全てホールド
	}
	else if (suitFour > 0 && Joker == true) {	//ワイルドカードを含む場合
		tempInfo.SetState(1, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
		return e_Flush_J;	//Joker込のフラッシュ完成		//全てホールド
	}

	/*----- 4枚同じ柄の場合 -----*/
	if (suitFour > 0) {	//ワイルドカードを含まない4カード
		tempInfo.SetState(0, 0, 0, 0, 0, NONE, NONE, suitFour, e_NoMark);
		return e_Suit4;		//20.4%の確率でフラッシュ		//suitFourでホールド
	}
	else if (suitThree > 0 && Joker == true) {	//ワイルドカードを含む4カード
		tempInfo.SetState(0, 0, 0, 0, 1, NONE, NONE, suitThree, e_NoMark);
		return e_Suit4;		//20.4%の確率でフラッシュ		//末尾とsuitThreeでホールド
	}
	/*----- 3枚同じ柄の場合 -----*/
	if (suitThree > e_NoMark) {	//ワイルドカードを含まない3カード
		tempInfo.SetState(0, 0, 0, 0, 0, NONE, NONE, suitThree, e_NoMark);
		return e_Suit3;		//suitThreeでホールド
	}
	else if (suitPear[0] > e_NoMark && Joker == true) {	//ワイルドカードを含む3カード
		tempInfo.SetState(0, 0, 0, 0, 1, NONE, NONE, suitPear[0], e_NoMark);
		return e_Suit3;		//suitPear[0]と末尾でホールド
	}
	/*----- 2枚同じ柄の場合 -----*/
	if (suitPear[0] > e_NoMark && suitPear[1] == e_NoMark) {
		tempInfo.SetState(0, 0, 0, 0, 0, NONE, NONE, suitPear[0], e_NoMark);
		return e_Suit2;	//suitPear[0] でホールド
	}
	else if (suitPear[0] > e_NoMark && suitPear[1] > e_NoMark) {
		tempInfo.SetState(0, 0, 0, 0, 0, NONE, NONE, suitPear[0], e_NoMark);
		return e_Suit2; //suitPeat[0] でホールド
	}
	/*----- 同じ柄が一枚もない場合 -----*/
	return 0;
}

//ストレート 2016/01/07/ A 10 J Q K の処理を実装していない
int poker::checkStraight(void) {
	//ジョーカーがない場合
	if (Joker == false) {
		for (int i = 1; i < 5; i++) {
			strtCheck[i - 1] = hand[i].num - hand[i - 1].num;
			//std::cout << "DEBUG:" << strtCheck[i - 1] << std::endl;
		}

		//ストレートなら
		if (strtCheck[0] == 1 && strtCheck[1] == 1 && strtCheck[2] == 1 && strtCheck[3] == 1) {
			tempInfo.SetState(1, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Straight;	//全てホールド
		}

		/*--- 4枚連なっているのなら 約15%の確率でストレート ---*/
		//端どちらかのみ連番でない場合
		if ((strtCheck[1] == 1 && strtCheck[2] == 1) && strtCheck[0] == 1) {
			tempInfo.SetState(1, 1, 1, 1, 0, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt4;		//前4枚ホールド
		}
		if ((strtCheck[1] == 1 && strtCheck[2] == 1) && strtCheck[3] == 1) {
			tempInfo.SetState(0, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt4;		//後ろ4枚ホールド
		}

		/*----- 歯抜けの場合 -----*/
		//1 2 3 5 X などの場合
		if (strtCheck[0] == 1 && strtCheck[1] == 1 && strtCheck[2] == 2) {
			tempInfo.SetState(1, 1, 1, 1, 0, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt4;		//前4枚ホールド
		}
		//1 2 4 5 X などの場合
		if (strtCheck[0] == 1 && strtCheck[1] == 2 && strtCheck[2] == 1) {
			tempInfo.SetState(1, 1, 1, 1, 0, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt4;		//前4枚ホールド
		}
		//1 3 4 5 X などの場合
		if (strtCheck[0] == 2 && strtCheck[1] == 1 && strtCheck[2] == 1) {
			tempInfo.SetState(1, 1, 1, 1, 0, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt4;		//前4枚ホールド
		}

		//X 7 9 10 11 などの場合
		if (strtCheck[1] == 2 && strtCheck[2] == 1 && strtCheck[3] == 1) {
			tempInfo.SetState(0, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt4;		//後ろ4枚ホールド
		}
		//X 7 8 10 11 などの場合
		if (strtCheck[0] == 1 && strtCheck[1] == 2 && strtCheck[2] == 1) {
			tempInfo.SetState(0, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt4;		//後ろ4枚ホールド
		}
		//X 7 8 9 11 などの場合
		if (strtCheck[0] == 1 && strtCheck[1] == 1 && strtCheck[2] == 2) {
			tempInfo.SetState(0, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt4;		//後ろ4枚ホールド
		}


		/*--- 3枚連なっているのなら ---*/
		// X 2 3 4 X などの場合
		if (strtCheck[1] == 1 && strtCheck[2] == 1) {
			tempInfo.SetState(0, 1, 1, 1, 0, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt3;		//真ん中3枚ホールド
		}
		// 1 2 3 X X などの場合
		if (strtCheck[0] == 1 && strtCheck[1] == 1) {
			tempInfo.SetState(1, 1, 1, 0, 0, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt3;		//前3枚ホールド
		}
		// X X 3 4 5 などの場合
		if (strtCheck[2] == 1 && strtCheck[3] == 1) {
			tempInfo.SetState(0, 0, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt3;		//後ろ3枚ホールド
		}

		return e_None;
	}
	//ジョーカーがある場合
	else {
		//末尾のstrtCheck[3]は使用しない。
		for (int i = 1; i < 4; i++) {
			strtCheck[i - 1] = hand[i].num - hand[i - 1].num;
			//std::cout << "DEBUG:" << strtCheck[i - 1] << std::endl;
		}
		//...前4枚が連番でストレートなら
		if (strtCheck[0] == 1 && strtCheck[1] == 1 && strtCheck[2] == 1) {
			tempInfo.SetState(1, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Straight_J;	//全てホールド
		}
		/*----- 1枚飛ばしで連番の場合 -----*/
		//1 2 3 5 J などの場合
		if (strtCheck[0] == 1 && strtCheck[1] == 1 && strtCheck[2] == 2) {
			tempInfo.SetState(1, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Straight_J;	//全てホールド
		}
		//1 2 4 5 J などの場合
		if (strtCheck[0] == 1 && strtCheck[1] == 2 && strtCheck[2] == 1) {
			tempInfo.SetState(1, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Straight_J;	//全てホールド
		}
		//1 3 4 5 J などの場合
		if (strtCheck[0] == 2 && strtCheck[1] == 1 && strtCheck[2] == 1) {
			tempInfo.SetState(1, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Straight_J;	//全てホールド
		}
		/*--- 3枚連なっているのなら ---*/
		// X 2 3 4 J などの場合
		if (strtCheck[1] == 1 && strtCheck[2] == 1) {
			tempInfo.SetState(0, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt4;		//後ろ4枚ホールド
		}
		// 1 2 3 X J などの場合
		if (strtCheck[0] == 1 && strtCheck[1] == 1) {
			tempInfo.SetState(1, 1, 1, 0, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Suit4;		//前3枚と末尾ホールド
		}
		// X 3 4 5 J などの場合	<-----この処理いらない問題
		if (strtCheck[2] == 1 && strtCheck[3] == 1) {
			tempInfo.SetState(0, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Suit4;		//後ろ4枚ホールド
		}

		/*----- 2枚連なっている場合 -----*/
		if (strtCheck[0] == 1) {
			tempInfo.SetState(1, 1, 0, 0, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt3; //前2枚と末尾ホールド
		}
		if (strtCheck[1] == 1) {
			tempInfo.SetState(0, 1, 1, 0, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt3; //2枚目と3枚と末尾ホールド
		}
		if (strtCheck[2] == 1) {
			tempInfo.SetState(0, 0, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt3;	//後ろ3枚ホールド
		}
		return e_None;
	}
	return 0;
}

//ファイブカード
int poker::checkFiveCard(void) {
	if (Joker == true) {
		if (numFour > 0) {
			tempInfo.SetState(1, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Five_J;	//全てホールド
		}
	}
	else {
		if (numFive > 0) {
			tempInfo.SetState(1, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Five;		//全てホールド
		}
	}
	return 0;
}

//フォーカード
int poker::checkFourCard(void) {
	if (Joker == true) {
		if (numThree > 0) {
			tempInfo.SetState(0, 0, 0, 0, 1, numThree, NONE, e_NoMark, e_NoMark);
			return e_Four_J;	//末尾とnumThreeのホールド
		}
	}
	else {
		if (numFour > 0) {
			tempInfo.SetState(0, 0, 0, 0, 0, numFour, NONE, e_NoMark, e_NoMark);
			return e_Four;		//numFourのホールド
		}
	}
	return 0;
}

//スリーカード
int poker::checkThreeCard(void) {
	if (Joker == true) {
		if (numPearIndex >= 1) {
			tempInfo.SetState(0, 0, 0, 0, 1, numPear[0], NONE, e_NoMark, e_NoMark);
			return e_Three_J;	//numPear[0]と末尾
		}
	}
	else {
		if (numThree > 0) {
			tempInfo.SetState(0, 0, 0, 0, 0, numThree, NONE, e_NoMark, e_NoMark);
			return e_Three;		//numThreeのホールド
		}
	}

	return 0;
}

//ツーペアの確認
int poker::checkTwoPear(void) {
	if (numPearIndex == 2) {
		tempInfo.SetState(0, 0, 0, 0, 0, numPear[0], numPear[1], e_NoMark, e_NoMark);
		return e_Two;	//numPear[0]とnumPear[1]のホールド
	}
	else
		return 0;
}

//ワンペアの確認
int poker::checkOnePear(void) {
	if (numPearIndex == 1) {
		tempInfo.SetState(0, 0, 0, 0, 0, numPear[0], NONE, e_NoMark, e_NoMark);
		return e_One;  //numPear[0]のホールド
	}
	else {
		tempInfo.SetState(0, 0, 0, 0, 0, NONE, NONE, suitPear[0], e_NoMark);
		return 0;
	}
}
