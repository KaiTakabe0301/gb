#pragma once
#define JOKER 0
#define NONE -1

enum Mark { e_NoMark, e_Diamond, e_Clover, e_Spade, e_Heart, e_Joker, e_Num };

//�g�����v�̃N���X�B
class card {
public:
	int num;	//1~13�܂ł̐���
	int mark;	//�J�[�h�̕�
	bool holdFlag;	//�J�[�h���z�[���h���邩�ǂ���
	int x, y;
public:
	card::card() :holdFlag(false) {};
	bool card::equal(card* judge) {
		if (this->num == judge->num && this->mark == judge->mark)
			return true;
		else if (this->mark == e_Joker && judge->mark == e_Joker)
			return true;
		else
			return false;
	}
};