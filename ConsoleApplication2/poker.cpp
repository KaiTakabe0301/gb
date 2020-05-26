#include<iostream>
#include<random>
#include<opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include"poker.h"

//�g�����v�̃N���b�N�J�n�͈�
int scrX[5] = {64,180,291,402,514};
int scrY[5] = {430,430,430,430,430};
//��D������
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

//...qsort�p
int compCardNum(const void *a, const void *b) {
	const card* aa = (const card*)a;
	const card* bb = (const card*)b;
	return aa->num - bb->num;
}

poker::poker() {
	initPokerHand();
}

//��n�̊m�F
bool poker::checkCemetery(card* hand) {
	for (auto& e : cemetery) {
		if (e.equal(hand))
			return true;	//��x�������J�[�h�ł����true
	}
	return false; //��x�����������ƂłȂ��J�[�h�ł����false
}

//...�R�D�����Z�b�g����B�ēx53������X�^�[�g�B
void poker::Reset(void) {
	Joker = false;
	judgeHold = false;
	for (int i = 0; i < 5; i++)
		hand[i].holdFlag = false;
	dropHand();
	cemetery.clear();
}

//��D������
void poker::drawHand(void) {
	int i, j;
	card temp;
	std::random_device rnd;
	std::mt19937 mt(rnd());
	std::uniform_int_distribution<> randNum(0, 12);	//�����̗����͈͐ݒ� ���ۂ̐������P������
	std::uniform_int_distribution<> randMark(0, 4);	//�}�[�N�̗����͈͐ݒ�
	std::uniform_int_distribution<> randTrump(0, 52);//4�Ŋ����������������A�]�肪�}�[�N 52���W���[�J�[

	if (cemetery.size() < 48) {	//��n��48���ȉ��Ȃ��
		for (i = 0; i < 5;) {
			if (!hand[i].holdFlag) {	//�J�[�h���z�[���h����Ă��Ȃ����
										/*
										temp.num = randNum(mt);
										temp.mark = randMark(mt);
										temp.holdFlag = false;
										*/

				int tempCard = randTrump(mt);
				temp.num = tempCard / 4;	//0�`13�̐������o�Ă��� 13�̂�Joker
				temp.mark = tempCard % 4 + 1; //Diamond(0) , Clover(1) , Spade(2) , Heart(3) �����o�Ȃ�
				temp.holdFlag = false;

				//...���������J�[�h��Joker�Ȃ�
				if (temp.num == 13) {
					if (Joker == false) {		//�܂���x��Joker��������Ă��Ȃ��Ȃ�
						Joker = true;	//Joker�̃t���O�𗧂Ă�
						temp.mark = e_Joker;	//Joker�̂�13
						hand[i++] = temp;
					}
				}
				//...���������J�[�h��Joker�ȊO�Ȃ�
				else if (!checkCemetery(&temp)) {		//��n�̊m�F
					for (j = 0; j < i; j++) {
						if (hand[j].equal(&temp))	//��D���ł̊m�F
							break;
					}
					if (i == j)	//��D���Ŕ���Ă��Ȃ����
						hand[i++] = temp;	//���̃J�[�h����D�ɉ�����
				}

			}
			else {	//�z�[���h����Ă�����
				hand[i].holdFlag = false;
				i++;//���̎D��
			}
		}
	}
	else {
		std::cout << "END POKER" << std::endl;
	}

	//��D�𐔎��̑傫�����ɕ��ёւ���
	qsort(hand, sizeof(hand) / sizeof(*hand), sizeof(card), compCardNum);
}

//...������
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

//...��D���̂Ă�
void poker::dropHand(void) {
	for (auto& h : hand) {
		if (h.holdFlag == false) {
			h.holdFlag = false;
			cemetery.push_back(h);
		}
	}
}

//��D�̎Q��
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

//...�f�o�b�O�p
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

//...���𔻒f�A�ǂ̃J�[�h���z�[���h���邩�l����
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

//...checkPokerHand�ł̔��f����Ɏ��ۂɃJ�[�h���z�[���h����
void poker::JudgeHoldCard(HWND hwnd,RECT rc, int w, int h, int delayRange) {
	if (judgeHold == false) {
		qsort(hand, sizeof(hand) / sizeof(*hand), sizeof(card), compCardNum);
		int hand = checkPokerHand();

		switch (hand)
		{
		case e_None:
			std::cout << "�Y���Ȃ�" << std::endl << std::endl;
			break;
		case e_Strt3:
			std::cout << "�X�g���[�g��_���ׂ�" << std::endl << std::endl;
			break;
		case e_Suit2:
			std::cout << "�t���b�V����_���ׂ�" << std::endl << std::endl;
			break;
		case e_Suit3:
			std::cout << "�t���b�V����_���ׂ�" << std::endl << std::endl;
			break;
		case e_One:
			std::cout << "�����y�A���z�[���h" << std::endl;
			std::cout << "�Ώ�:" << numPear[0] << std::endl << std::endl;
			break;
		case e_Strt4:
			std::cout << "�X�g���[�g��_���ׂ�" << std::endl << std::endl;
			break;
		case e_Suit4:
			std::cout << "�t���b�V����_���ׂ�" << std::endl << std::endl;
			break;
		case e_Two:
			std::cout << "�c�[�y�A���z�[���h" << std::endl;
			std::cout << "�Ώ�:" << numPear[0] << "&" << numPear[1] << std::endl << std::endl;
			break;
		case e_Three_J:
			std::cout << "�X���[�J�[�h���z�[���h" << std::endl << std::endl;
			std::cout << "�Ώ�:" << numPear[0] << std::endl << std::endl;
			break;
		case e_Three:
			std::cout << "�X���[�J�[�h���z�[���h" << std::endl << std::endl;
			std::cout << "�Ώ�:" << numThree << std::endl << std::endl;
			break;
		case e_Straight_J:
			std::cout << "�X�g���[�g�F�S�ăz�[���h" << std::endl << std::endl;
			break;
		case e_Straight:
			std::cout << "�X�g���[�g�F�S�ăz�[���h" << std::endl << std::endl;
			break;
		case e_Flush_J:
			std::cout << "�t���b�V���F�S�ăz�[���h" << std::endl << std::endl;
			break;
		case e_Flush:
			std::cout << "�t���b�V���F�S�ăz�[���h" << std::endl << std::endl;
			break;
		case e_FullHouse_J:
			std::cout << "�t���n�E�X�F�S�ăz�[���h" << std::endl << std::endl;
			break;
		case e_FullHouse:
			std::cout << "�t���n�E�X�F�S�ăz�[���h" << std::endl << std::endl;
			break;
		case e_Four_J:
			std::cout << "�t�H�[�J�[�h���z�[���h" << std::endl << std::endl;
			std::cout << "�Ώ�:" << numThree << std::endl << std::endl;
			break;
		case e_Four:
			std::cout << "�t�H�[�J�[�h���z�[���h" << std::endl << std::endl;
			std::cout << "�Ώ�:" << numFour << std::endl << std::endl;
			break;
		case e_StrtFrush:
			std::cout << "�X�g���[�g�E�t���b�V���F�S�ăz�[���h" << std::endl << std::endl;
			break;
		case e_Five_J:
			std::cout << "�t�@�C�u�J�[�h�F�S�ăz�[���h" << std::endl << std::endl;
			break;
		case e_Five:
			std::cout << "�t�@�C�u�J�[�h�F�S�ăz�[���h" << std::endl << std::endl;
			break;
		case e_RSF:
			std::cout << "���C�����E�X�g���[�g�E�t���b�V���F�S�ăz�[���h" << std::endl << std::endl;
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
	std::uniform_int_distribution<> randW(0, w);	//0.0~�N���b�N����mt�@
	std::uniform_int_distribution<> randH(0, h);	//0.0~�N���b�N����mt�@
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
	/*----- �J�[�h�̃z�[���h -----*/
	for (int i = 0; i < 5; i++) {
		if (hand[i].holdFlag) {
			SetCursorPos(rc.left + hand[i].x + randW(mt), rc.top + hand[i].y + randH(mt));
			cvWaitKey(randD(mt));
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		}
	}
}

//�J�[�h�̐����ƃ}�[�N�̃J�E���g
void poker::cntNum_and_Mark(void) {

	//�J�[�h�𕿂Ɛ������i�[
	for each (card var in hand)
	{
		cntMark[var.mark]++;	//�Y�����}�[�N�ɑΉ��A�l������
		if (var.mark != e_Joker)	//Joker�̏ꍇ�̓J�E���g���Ȃ�
			cntNum[var.num]++;		//�Y���������ɑΉ��A�l������
	}

	//Joker�̊m�F�AJoker�����݂������΂Ƀz�[���h���s��
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

	//���̊m�F
	for (int i = 0; i < e_Num; i++) {
		switch (cntMark[i]) {
		case 5:	//�S�ē������Ȃ�
			suitFive = i;
			break;
		case 4:	//4���������Ȃ�
			suitFour = i;	//�����L�^
			break;
		case 3:	//3���������Ȃ�
			suitThree = i;	//�����m�F
			break;
		case 2:	//2�����������m�F�ł����
			suitPear[suitPearIndex++] = i;	//1�g�ڂ̕����L�^���A2�g�ڂ̋L�^�Ɉڂ�
			break;
		default:
			break;
		}
	}
}

//�X�g���[�g�E�t���b�V��
int poker::checkStraightFrush(void) {
	if (checkFlush() >= e_Flush_J && checkStraight() >= e_Straight_J) {
		tempInfo.SetState(1, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
		return e_StrtFrush;		//�X�g���[�g�E�t���b�V���̊���	//�S�ăz�[���h
	}
	return 0;
}

//�t���n�E�X 3�p�^�[��
int poker::checkFullHouse(void) {
	if (checkThreeCard() == e_Three && numPearIndex >= 1) {
		tempInfo.SetState(1, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
		return e_FullHouse;		//�t���n�E�X�̊���	//�S�ăz�[���h
	}
	else if (checkThreeCard() == e_Three_J && numPearIndex == 2) {
		tempInfo.SetState(1, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
		return e_FullHouse_J;	//Joker���Ńt���n�E�X�̊���	//�S�ăz�[���h
	}
	else
		return 0;
}

//�t���b�V�� 5�p�^�[��
int poker::checkFlush(void) {
	/*----- �t���b�V�������������ꍇ -----*/
	if (suitFive > 0) {	//���C���h�J�[�h���܂܂Ȃ��ꍇ
		tempInfo.SetState(1, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
		return e_Flush;		//�t���b�V���̊���	//�S�ăz�[���h
	}
	else if (suitFour > 0 && Joker == true) {	//���C���h�J�[�h���܂ޏꍇ
		tempInfo.SetState(1, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
		return e_Flush_J;	//Joker���̃t���b�V������		//�S�ăz�[���h
	}

	/*----- 4���������̏ꍇ -----*/
	if (suitFour > 0) {	//���C���h�J�[�h���܂܂Ȃ�4�J�[�h
		tempInfo.SetState(0, 0, 0, 0, 0, NONE, NONE, suitFour, e_NoMark);
		return e_Suit4;		//20.4%�̊m���Ńt���b�V��		//suitFour�Ńz�[���h
	}
	else if (suitThree > 0 && Joker == true) {	//���C���h�J�[�h���܂�4�J�[�h
		tempInfo.SetState(0, 0, 0, 0, 1, NONE, NONE, suitThree, e_NoMark);
		return e_Suit4;		//20.4%�̊m���Ńt���b�V��		//������suitThree�Ńz�[���h
	}
	/*----- 3���������̏ꍇ -----*/
	if (suitThree > e_NoMark) {	//���C���h�J�[�h���܂܂Ȃ�3�J�[�h
		tempInfo.SetState(0, 0, 0, 0, 0, NONE, NONE, suitThree, e_NoMark);
		return e_Suit3;		//suitThree�Ńz�[���h
	}
	else if (suitPear[0] > e_NoMark && Joker == true) {	//���C���h�J�[�h���܂�3�J�[�h
		tempInfo.SetState(0, 0, 0, 0, 1, NONE, NONE, suitPear[0], e_NoMark);
		return e_Suit3;		//suitPear[0]�Ɩ����Ńz�[���h
	}
	/*----- 2���������̏ꍇ -----*/
	if (suitPear[0] > e_NoMark && suitPear[1] == e_NoMark) {
		tempInfo.SetState(0, 0, 0, 0, 0, NONE, NONE, suitPear[0], e_NoMark);
		return e_Suit2;	//suitPear[0] �Ńz�[���h
	}
	else if (suitPear[0] > e_NoMark && suitPear[1] > e_NoMark) {
		tempInfo.SetState(0, 0, 0, 0, 0, NONE, NONE, suitPear[0], e_NoMark);
		return e_Suit2; //suitPeat[0] �Ńz�[���h
	}
	/*----- ���������ꖇ���Ȃ��ꍇ -----*/
	return 0;
}

//�X�g���[�g 2016/01/07/ A 10 J Q K �̏������������Ă��Ȃ�
int poker::checkStraight(void) {
	//�W���[�J�[���Ȃ��ꍇ
	if (Joker == false) {
		for (int i = 1; i < 5; i++) {
			strtCheck[i - 1] = hand[i].num - hand[i - 1].num;
			//std::cout << "DEBUG:" << strtCheck[i - 1] << std::endl;
		}

		//�X�g���[�g�Ȃ�
		if (strtCheck[0] == 1 && strtCheck[1] == 1 && strtCheck[2] == 1 && strtCheck[3] == 1) {
			tempInfo.SetState(1, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Straight;	//�S�ăz�[���h
		}

		/*--- 4���A�Ȃ��Ă���̂Ȃ� ��15%�̊m���ŃX�g���[�g ---*/
		//�[�ǂ��炩�̂ݘA�ԂłȂ��ꍇ
		if ((strtCheck[1] == 1 && strtCheck[2] == 1) && strtCheck[0] == 1) {
			tempInfo.SetState(1, 1, 1, 1, 0, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt4;		//�O4���z�[���h
		}
		if ((strtCheck[1] == 1 && strtCheck[2] == 1) && strtCheck[3] == 1) {
			tempInfo.SetState(0, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt4;		//���4���z�[���h
		}

		/*----- �������̏ꍇ -----*/
		//1 2 3 5 X �Ȃǂ̏ꍇ
		if (strtCheck[0] == 1 && strtCheck[1] == 1 && strtCheck[2] == 2) {
			tempInfo.SetState(1, 1, 1, 1, 0, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt4;		//�O4���z�[���h
		}
		//1 2 4 5 X �Ȃǂ̏ꍇ
		if (strtCheck[0] == 1 && strtCheck[1] == 2 && strtCheck[2] == 1) {
			tempInfo.SetState(1, 1, 1, 1, 0, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt4;		//�O4���z�[���h
		}
		//1 3 4 5 X �Ȃǂ̏ꍇ
		if (strtCheck[0] == 2 && strtCheck[1] == 1 && strtCheck[2] == 1) {
			tempInfo.SetState(1, 1, 1, 1, 0, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt4;		//�O4���z�[���h
		}

		//X 7 9 10 11 �Ȃǂ̏ꍇ
		if (strtCheck[1] == 2 && strtCheck[2] == 1 && strtCheck[3] == 1) {
			tempInfo.SetState(0, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt4;		//���4���z�[���h
		}
		//X 7 8 10 11 �Ȃǂ̏ꍇ
		if (strtCheck[0] == 1 && strtCheck[1] == 2 && strtCheck[2] == 1) {
			tempInfo.SetState(0, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt4;		//���4���z�[���h
		}
		//X 7 8 9 11 �Ȃǂ̏ꍇ
		if (strtCheck[0] == 1 && strtCheck[1] == 1 && strtCheck[2] == 2) {
			tempInfo.SetState(0, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt4;		//���4���z�[���h
		}


		/*--- 3���A�Ȃ��Ă���̂Ȃ� ---*/
		// X 2 3 4 X �Ȃǂ̏ꍇ
		if (strtCheck[1] == 1 && strtCheck[2] == 1) {
			tempInfo.SetState(0, 1, 1, 1, 0, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt3;		//�^��3���z�[���h
		}
		// 1 2 3 X X �Ȃǂ̏ꍇ
		if (strtCheck[0] == 1 && strtCheck[1] == 1) {
			tempInfo.SetState(1, 1, 1, 0, 0, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt3;		//�O3���z�[���h
		}
		// X X 3 4 5 �Ȃǂ̏ꍇ
		if (strtCheck[2] == 1 && strtCheck[3] == 1) {
			tempInfo.SetState(0, 0, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt3;		//���3���z�[���h
		}

		return e_None;
	}
	//�W���[�J�[������ꍇ
	else {
		//������strtCheck[3]�͎g�p���Ȃ��B
		for (int i = 1; i < 4; i++) {
			strtCheck[i - 1] = hand[i].num - hand[i - 1].num;
			//std::cout << "DEBUG:" << strtCheck[i - 1] << std::endl;
		}
		//...�O4�����A�ԂŃX�g���[�g�Ȃ�
		if (strtCheck[0] == 1 && strtCheck[1] == 1 && strtCheck[2] == 1) {
			tempInfo.SetState(1, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Straight_J;	//�S�ăz�[���h
		}
		/*----- 1����΂��ŘA�Ԃ̏ꍇ -----*/
		//1 2 3 5 J �Ȃǂ̏ꍇ
		if (strtCheck[0] == 1 && strtCheck[1] == 1 && strtCheck[2] == 2) {
			tempInfo.SetState(1, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Straight_J;	//�S�ăz�[���h
		}
		//1 2 4 5 J �Ȃǂ̏ꍇ
		if (strtCheck[0] == 1 && strtCheck[1] == 2 && strtCheck[2] == 1) {
			tempInfo.SetState(1, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Straight_J;	//�S�ăz�[���h
		}
		//1 3 4 5 J �Ȃǂ̏ꍇ
		if (strtCheck[0] == 2 && strtCheck[1] == 1 && strtCheck[2] == 1) {
			tempInfo.SetState(1, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Straight_J;	//�S�ăz�[���h
		}
		/*--- 3���A�Ȃ��Ă���̂Ȃ� ---*/
		// X 2 3 4 J �Ȃǂ̏ꍇ
		if (strtCheck[1] == 1 && strtCheck[2] == 1) {
			tempInfo.SetState(0, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt4;		//���4���z�[���h
		}
		// 1 2 3 X J �Ȃǂ̏ꍇ
		if (strtCheck[0] == 1 && strtCheck[1] == 1) {
			tempInfo.SetState(1, 1, 1, 0, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Suit4;		//�O3���Ɩ����z�[���h
		}
		// X 3 4 5 J �Ȃǂ̏ꍇ	<-----���̏�������Ȃ����
		if (strtCheck[2] == 1 && strtCheck[3] == 1) {
			tempInfo.SetState(0, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Suit4;		//���4���z�[���h
		}

		/*----- 2���A�Ȃ��Ă���ꍇ -----*/
		if (strtCheck[0] == 1) {
			tempInfo.SetState(1, 1, 0, 0, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt3; //�O2���Ɩ����z�[���h
		}
		if (strtCheck[1] == 1) {
			tempInfo.SetState(0, 1, 1, 0, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt3; //2���ڂ�3���Ɩ����z�[���h
		}
		if (strtCheck[2] == 1) {
			tempInfo.SetState(0, 0, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Strt3;	//���3���z�[���h
		}
		return e_None;
	}
	return 0;
}

//�t�@�C�u�J�[�h
int poker::checkFiveCard(void) {
	if (Joker == true) {
		if (numFour > 0) {
			tempInfo.SetState(1, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Five_J;	//�S�ăz�[���h
		}
	}
	else {
		if (numFive > 0) {
			tempInfo.SetState(1, 1, 1, 1, 1, NONE, NONE, e_NoMark, e_NoMark);
			return e_Five;		//�S�ăz�[���h
		}
	}
	return 0;
}

//�t�H�[�J�[�h
int poker::checkFourCard(void) {
	if (Joker == true) {
		if (numThree > 0) {
			tempInfo.SetState(0, 0, 0, 0, 1, numThree, NONE, e_NoMark, e_NoMark);
			return e_Four_J;	//������numThree�̃z�[���h
		}
	}
	else {
		if (numFour > 0) {
			tempInfo.SetState(0, 0, 0, 0, 0, numFour, NONE, e_NoMark, e_NoMark);
			return e_Four;		//numFour�̃z�[���h
		}
	}
	return 0;
}

//�X���[�J�[�h
int poker::checkThreeCard(void) {
	if (Joker == true) {
		if (numPearIndex >= 1) {
			tempInfo.SetState(0, 0, 0, 0, 1, numPear[0], NONE, e_NoMark, e_NoMark);
			return e_Three_J;	//numPear[0]�Ɩ���
		}
	}
	else {
		if (numThree > 0) {
			tempInfo.SetState(0, 0, 0, 0, 0, numThree, NONE, e_NoMark, e_NoMark);
			return e_Three;		//numThree�̃z�[���h
		}
	}

	return 0;
}

//�c�[�y�A�̊m�F
int poker::checkTwoPear(void) {
	if (numPearIndex == 2) {
		tempInfo.SetState(0, 0, 0, 0, 0, numPear[0], numPear[1], e_NoMark, e_NoMark);
		return e_Two;	//numPear[0]��numPear[1]�̃z�[���h
	}
	else
		return 0;
}

//�����y�A�̊m�F
int poker::checkOnePear(void) {
	if (numPearIndex == 1) {
		tempInfo.SetState(0, 0, 0, 0, 0, numPear[0], NONE, e_NoMark, e_NoMark);
		return e_One;  //numPear[0]�̃z�[���h
	}
	else {
		tempInfo.SetState(0, 0, 0, 0, 0, NONE, NONE, suitPear[0], e_NoMark);
		return 0;
	}
}
