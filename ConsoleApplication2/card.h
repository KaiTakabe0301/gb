#pragma once
#define JOKER 0
#define NONE -1

enum Mark { e_NoMark, e_Diamond, e_Clover, e_Spade, e_Heart, e_Joker, e_Num };

//トランプのクラス。
class card {
public:
	int num;	//1~13までの数字
	int mark;	//カードの柄
	bool holdFlag;	//カードをホールドするかどうか
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