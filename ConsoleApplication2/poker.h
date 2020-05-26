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
	e_FullHouse_J,//�S�ăz�[���h
	e_FullHouse,//�S�ăz�[���h
	e_Four_J,//cntNum��3�̃J�[�h��Joker
	e_Four,	//cntNum��4�̃J�[�h
	e_StrtFrush,//�S�ăz�[���h
	e_Five_J,//�S�ăz�[���h
	e_Five,	//�S�ăz�[���h
	e_RSF	//�S�ăz�[���h
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


//�|�[�J�[���s���N���X
class poker {
private:
	infoHoldCard state;
	infoHoldCard tempInfo;

	bool judgeHold;

	bool Joker;		//���C���h�J�[�h�̃t���O
	int suitPearIndex = 0;	//�y�A�̑g�̓Y��
	int suitPear[2] = { 0 };	//�y�A�̋L���ϐ�
	int suitFive = 0, suitFour = 0, suitThree = 0;	//4�J�[�h�A3�J�[�h�̃t���O

	int numPearIndex = 0;	//...�����̃y�A�̑g�̓Y��
	int numPear[2] = { 0 }; //...�y�A�̐������L�^
	int numFive = 0, numFour = 0, numThree = 0;

	int strtCheck[4] = { 0 };	//...�X�g���[�g�̔��ʗp

	int cntMark[e_Num] = { 0 };	//...��D�ɂ��鐔���̖������L�^�B�Y���������A�l������
	int cntNum[13] = { 0 };		//...��D�ɑ��݂���}�[�N�̖������L�^�B�Y�����}�[�N�A�l������
	card hand[5];	//��D
	std::vector<card> cemetery;	//��n
public:
	poker();
	void setHand(int index, int num);
	void drawHand(void);	//��D������
	void holdHand(void);	//�z�[���h����J�[�h�̑I��
	void dropHand(void);	//��D���̂Ă�
	void showHand(void);	//��D�̎Q��
	int checkPokerHand(void);	//���̊m�F
	void JudgeHoldCard(HWND hwnd,RECT rc,int w, int h, int delayRange);
	void Reset(void);		//�Q�[���̃��Z�b�g
	void initPokerHand();	//������
	void DEBUG_DRAWHAND();	//�f�o�b�O�p�̖�
private:
	bool checkCemetery(card* hand);	//��D�������ۂɕ�n�̊m�F

	/*----- �𔻒�̊֐� -----*/
	void cntNum_and_Mark();	//�J�[�h�̖����ƃ}�[�N���m�F	2015/12/21 ����
	int RoyalStraightFrush();//���C�����E�X�g���[�g�E�t���b�V���̊m�F
	int checkFiveCard();	//�t�@�C�u�J�[�h�̊m�F	2015/12/21 ����
	int checkStraightFrush();//�X�g���[�g�E�t���b�V���̊m�F
	int checkFourCard();	//�t�H�[�J�[�h�̊m�F		2015/12/21 ����
	int checkFullHouse();	//�t���n�E�X�̊m�F
	int checkFlush();		//�t���b�V���̊m�F
	int checkStraight();	//�X�g���[�g�̊m�F		2016/01/05 �u�f�o�b�O�Ώ�_TwoPear�ɂȂ�Ȃ�.png�v���̓��삪��������
	int checkThreeCard();	//�X���[�J�[�h�̊m�F		2015/12/21 ����
	int checkTwoPear();		//�c�[�y�A�̊m�F			2015/12/21 ����		
	int checkOnePear();		//�����y�A�̊m�F			2015/12/21 ����		2016/01/04 ���삪���P�ς�

	/*----- �J�[�h�̃z�[���h���s���֐� -----*/
	void DoHoldCard(infoHoldCard* info,HWND hwnd,RECT rc, int w, int h, int delayRange);

	/*----- DEBUG�p�֐��Q -----*/
	void DebugHand();
};