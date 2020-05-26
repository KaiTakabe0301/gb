#include <iostream>
#include<string>
#include<random>
#include<Windows.h>
#include<opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include"bmp.h"
#include"ScreenShot.h"
#include"poker.h"
#include"KeyBoard.h"

using namespace std;

//シーン判定の切り抜きサイズ
#define PHASE_W 344
#define PHASE_H 182 /*75*/

//トランプ判定の切り抜きサイズ
#define TRUMP_W 42
#define TRUMP_H 68

//ダブルアップの切り抜きサイズ
#define DOUBLE_W 60
#define DOUBLE_H 100

/*------ クリックするときの許容幅 -------*/
#define TRUMP_CLICK_W 63
#define TRUMP_CLICK_H 89

#define OK_CLICK_W 135
#define OK_CLICK_H 65

#define YorN_CLICK_W 126
#define YorN_CLICK_H 40

#define DOUBLE_CLICK_W
#define DOUBLE_CLICK_H

#define DEBUG_IMG 0
#define DEBUG_POINT 0
#define RECT_TRUMP 1

//ここらへんも、いつかヘッダに宣言するべき
enum e_Scene { e_Poker, e_DoubleUp ,e_PkrReTry,e_PkrStart,e_DoubleLOSE, e_DoubleSuccess, e_DbleTry,e_JudgeDouble,e_SceneNum,e_SceneNone};

//座標格納用の構造体 
struct Position {
	int x;
	int y;
};

Position posOK = { 247,713 };
Position posYES = { 354,730 };
Position posNO = { 158,727 };


//モニターサイズ1980*1080におけるトランプの配置
//切り出し開始座標
Position posTrump[5] = { { 56,416 },{ 170,416 },{ 281,416 },{ 393,416 },{ 505,416 } };
Position posDouble = {159,362};
Position posPhase = { 162,180 /*282*/ };

void MyClick(HWND hwnd,RECT* rc,int x,int y,int width,int height,int delayRange) {
	std::random_device rnd;
	std::mt19937 mt(rnd());
	std::uniform_int_distribution<> randW(0, width);	//0.0~クリック幅でmt法
	std::uniform_int_distribution<> randH(0, height);	//0.0~クリック幅でmt法
	std::uniform_int_distribution<> randD(230, 230+delayRange);

	int w = randW(mt);
	int h = randH(mt);
	int d = randD(mt);

	SetForegroundWindow(hwnd);
	SetCursorPos(rc->left + x + w, rc->top + y + h);
	cvWaitKey(d);		//...カーソル移動後に若干ディレイ挟まないとクリックイベントが失敗しやすくなる
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);


}

//閾値
double threshold_stop = 0.998;

int nowPhase = -1;
int formerPhase = -1;

bool cutout = false;
bool matchFlag = false;

bool dCutout = false;
bool dMatchFlag = false;

bool f_PkrRetry = false;

poker casino;

HWND GetWindowHandle(	// 戻り値: 成功 望みのHWND / 失敗 NULL
	const DWORD TargetID)	// プロセスID
{
	HWND hWnd = GetTopWindow(NULL);
	do {
		if (GetWindowLong(hWnd, GWL_HWNDPARENT) != 0 || !IsWindowVisible(hWnd))
			continue;
		DWORD ProcessID;
		GetWindowThreadProcessId(hWnd, &ProcessID);
		if (TargetID == ProcessID)
			return hWnd;
	} while ((hWnd = GetNextWindow(hWnd, GW_HWNDNEXT)) != NULL);

	return NULL;
}

void LoadPhase(cv::Mat* Phase, bool graySwith) {
	cv::Mat tempImg;
	string filename;
	for (int i = 0; i < e_SceneNum; i++) {
		filename = "Phase/" + std::to_string(i) + ".bmp";
		tempImg = cv::cvarrToMat(cvLoadImage(filename.c_str(), CV_LOAD_IMAGE_COLOR));

		if (tempImg.data == NULL)
			exit(1);
		if (graySwith == true)
			cv::cvtColor(tempImg, Phase[i], CV_RGB2GRAY);
		else
			Phase[i] = tempImg;
	}
}

void LoadTrump(cv::Mat* trump,bool graySwith) {
	cv::Mat tempImg;
	string filename;
	for (int i = 0; i <= 52; i++) {
		filename = "trump-origin-/" + std::to_string(i) + ".bmp";
		tempImg = cv::cvarrToMat(cvLoadImage(filename.c_str(), CV_LOAD_IMAGE_COLOR));

		if (tempImg.data == NULL)
			exit(1);
		if (graySwith == true)
			cv::cvtColor(tempImg, trump[i], CV_RGB2GRAY);
		else
			trump[i] = tempImg;
	}
}

void LoadDoubleTrump(cv::Mat* trump, bool graySwith) {
	cv::Mat tempImg;
	string filename;
	for (int i = 0; i < 52; i++) {
		filename = "doubleUp/" + std::to_string(i) + ".bmp";
		tempImg = cv::cvarrToMat(cvLoadImage(filename.c_str(), CV_LOAD_IMAGE_COLOR));

		if (tempImg.data == NULL)
			exit(1);
		if (graySwith == true)
			cv::cvtColor(tempImg, trump[i], CV_RGB2GRAY);
		else
			trump[i] = tempImg;
	}
}

void iLoadTrump(IplImage** temp) {
	string fname;
	for (int i = 0; i <= 52; i++) {
		fname = "trump-origin-/" + std::to_string(i) + ".bmp";
		temp[i] = cvLoadImage(fname.c_str(), CV_LOAD_IMAGE_COLOR);
	}
}

//状態の確認　2015/11/3 正しく動作している
int CheckPhase(cv::Mat* screen, cv::Mat* phase_img) {
	cv::Mat tempPhase;
	double maxValue;
	cv::Point maxPt;

	//フェーズ判定画像の切り抜き
	cv::cvtColor(cv::Mat(*screen, cv::Rect(posPhase.x, posPhase.y, PHASE_W, PHASE_H)), tempPhase, CV_RGB2GRAY);

	for (int i = 0; i < e_SceneNum; i++) {
		cv::Ptr<cv::Mat> result_img = new cv::Mat(cvSize(tempPhase.cols - phase_img[i].cols + 1, tempPhase.rows - phase_img[i].rows + 1), CV_8UC3);
		cv::matchTemplate(tempPhase, phase_img[i], *result_img, CV_TM_CCORR_NORMED);
		minMaxLoc(*result_img, 0, &maxValue, 0, &maxPt);
		if (maxValue > threshold_stop) {
			if (i != formerPhase) {
				std::cout << "Scene:" << i << endl;
				formerPhase = nowPhase;
				nowPhase = i;
			}
			return i;
		}
	}
	//std::cout << "Scene:None" << endl;
	return e_SceneNone;
}

//トランプの切り出し　2015/11/3 正しく動作している
void CutoutTrump(cv::Mat* screen,cv::Mat* trump) {
	cv::Mat tempTrump;
	for (int i = 0; i < 5; i++) {
		tempTrump = cv::Mat(*screen, cv::Rect(posTrump[i].x, posTrump[i].y, TRUMP_W, TRUMP_H));
		cv::cvtColor(tempTrump, trump[i], CV_RGB2GRAY);
	}
}

void CutoutDoubleTrump(cv::Mat* screen, cv::Mat* double_img) {
	cv::Mat tempTrump;
	tempTrump = cv::Mat(*screen, cv::Rect(posDouble.x, posDouble.y, DOUBLE_W, DOUBLE_H));
	cv::cvtColor(tempTrump, *double_img, CV_RGB2GRAY);
	dCutout = true;
}

int matchDouble(cv::Mat* DoubleHand, cv::Mat* double_img) {	// DoubleHand 60x64 double_img 31x59
	double maxValue;
	cv::Point maxPt;
	cv::Ptr<cv::Mat> result_img = new cv::Mat(cvSize(DoubleHand->cols - double_img[0].cols + 1, DoubleHand->rows - double_img[0].rows + 1), CV_8UC3);
	
	/*
	std::cout << DoubleHand->cols << std::endl;
	std::cout << DoubleHand->rows << std::endl<< std::endl;

	std::cout << double_img->cols << std::endl;
	std::cout << double_img->cols << std::endl;
	*/

	
	for (int i = 0; i < 52; i++) {
		cv::matchTemplate(*DoubleHand, double_img[i], *result_img, CV_TM_CCORR_NORMED);
		minMaxLoc(*result_img, 0, &maxValue, 0, &maxPt);
		if (maxValue > threshold_stop) {
			dMatchFlag = true;
			return i/4+1;
		}
	}
	return 8;
}

//カードを認識
void matchTrump(cv::Mat* Trump,cv::Mat* trump_img){	//Trump 42x68  trump_img 26x52
	int key = 0;
	double maxValue;
	cv::Point maxPt;
	cv::Ptr<cv::Mat> result_img = new cv::Mat(cvSize(Trump->cols - trump_img->cols + 1, Trump->rows - trump_img->rows + 1), CV_8UC3);

	for (int num = 0; num < 5; num++) {
		for (int i = 0; i <= 52; i++) {
			cv::matchTemplate(Trump[num], trump_img[i], *result_img, CV_TM_CCORR_NORMED);
			minMaxLoc(*result_img, 0, &maxValue, 0, &maxPt);
			if (maxValue > threshold_stop) {
				key++;
				std::cout << num <<"枚目:" << i/4+1 << " | ";
				casino.setHand(num, i);
				break;
			}
		}
	}
	if (key == 5)
		matchFlag = true;
	else
		matchFlag = false;

	std::cout << endl;
}

int main(){
	

	ScreenShot test;
	PBITMAP testImg;

	HWND gbf;
	RECT rc;
	HDC hdc, hMemDC;
	
	PBITMAPINFO bmpInfo;
	BITMAP bmBitmap;
	HBITMAP hBitmap;
	LPDWORD lpPixel;

	int width, height;

	cv::Ptr<IplImage> searchScreen, resultScreen;
	IplImage* temp[53];
	CvPoint min_loc, max_loc;
	CvSize dst_size;
	double min_val, max_val;

	cv::Mat *scr_img;
	cv::Mat gray_img;
	cv::Mat Trump[5];
	cv::Mat DoubleHand;
	cv::Mat trump_img[53];
	cv::Mat double_img[52];
	cv::Mat *result_img;
	cv::Point maxPt;

	cv::Mat phase_img[e_SceneNum];

	double maxValue;
	int nowScene=e_None;

	char *window_output = "output";
	cv::namedWindow(window_output, CV_WINDOW_AUTOSIZE);
	//char *window_result = "result";
	//cv::namedWindow(window_result, CV_WINDOW_AUTOSIZE);
	//char *window_rect = "rect";
	//cv::namedWindow(window_rect, CV_WINDOW_AUTOSIZE);


	gbf = FindWindow(L"Chrome_WidgetWin_1",L"グランブルーファンタジー[ChromeApps版]");
	GetWindowRect(gbf, &rc);
	
	width = rc.right - rc.left;
	height = rc.bottom - rc.top;

	/* BITMAPの情報を設定 */
	bmpInfo = bmp::LoadBITMAPINFO(width,-height,24,BI_RGB);

	LoadTrump(trump_img,true);		//ポーカーのトランプをロード

	LoadDoubleTrump(double_img, true);//ダブルアップのトランプをロード

	LoadPhase(phase_img, true);
	
	scr_img = new cv::Mat(cvSize(width, height), CV_8UC3);
	
	result_img = new cv::Mat(cvSize(width - trump_img[0].cols + 1, height - trump_img[0].rows + 1), CV_8UC3);
	
	

	/* ---------------- */
	if (!test.SelectHWND(gbf))
		exit(1);
	
	while (1) {
		MygetKeyState();

		if (keyState[VK_ESCAPE]) {
			std::cout << "終了します" << endl;
			break;
		}


		//正しく動作している 2015/10/30
		searchScreen = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);//メモリ確保
		test.Capture(searchScreen);
#if 1
		* scr_img = cv::cvarrToMat(searchScreen);

		nowScene = CheckPhase(scr_img, phase_img);

		switch (nowScene) {
		case e_Poker:		//ホールドするカードを判断
			//2回目の頭：画像は真っ黒
			//cutout = false, matchflag = false;
			if (cutout == false) {
				CutoutTrump(scr_img, Trump);	//2015 11/03 正しく動作している
				cutout = true;
			}
			if (cutout == true) {
				if (matchFlag == false)
					matchTrump(Trump, trump_img);	//2015 11/03 正しく動作している
				if (matchFlag == true) {
					casino.JudgeHoldCard(gbf,rc,TRUMP_CLICK_W,TRUMP_CLICK_H,230);	//カードをクリック
					cout << "NOW:" << nowPhase << endl;
					cout << "FORMER:" << formerPhase << endl;
					MyClick(gbf, &rc, posOK.x, posOK.y, OK_CLICK_W, OK_CLICK_H, 200);

					if (nowPhase != formerPhase) {
						//cout << "NOW:" << nowPhase << endl;
						//cout << "FORMER:" << formerPhase << endl;

						//MyClick(gbf, &rc, posOK.x, posOK.y, OK_CLICK_W, OK_CLICK_H, 200);
					}
						
					f_PkrRetry = false;
				}
				else
					cutout = false;
			}
			break;
		case e_DoubleUp:	//ダブルアップに挑戦する
			if (nowPhase != formerPhase) {
				MyClick(gbf, &rc, posYES.x, posYES.y, YorN_CLICK_W, YorN_CLICK_H, 200);
			}
			break;
		case e_PkrReTry:	//ポーカー失敗時(ダブルアップ失敗時ではない)
			if (f_PkrRetry == false) {
				cutout = false;
				matchFlag = false;

				/*----- クリックするときの処理 -----*/
				casino.Reset();		//カードの情報をまっさらに
				MyClick(gbf, &rc, posOK.x, posOK.y, OK_CLICK_W, OK_CLICK_H, 200);
				f_PkrRetry = true;
				/*----- クリックするときの処理 -----*/

				cvWaitKey(1200);	//画面にトランプが表示されるのを待つ
			}
			break;
		case e_DbleTry:

			dCutout = false;
			dMatchFlag = false;
			
			MyClick(gbf, &rc, posYES.x, posYES.y, YorN_CLICK_W, YorN_CLICK_H, 200);
			/*
			SetForegroundWindow(gbf);
			SetCursorPos(rc.left + 432, rc.top + 747);
			cvWaitKey(230);		//...カーソル移動後に若干ディレイ挟まないとクリックイベントが失敗しやすくなる
			
			MyClick(gbf, &rc, posOK.x, posOK.y, OK_CLICK_W, OK_CLICK_H, 200);

			//mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
			//mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			//cvWaitKey(1200);	//画面にトランプが表示されるのを待つ
			*/
			break;
		case e_JudgeDouble:
			if(dCutout==false)
				CutoutDoubleTrump(scr_img,&DoubleHand);
			if (dCutout == true) {
				if (dMatchFlag == false) {
					int value=matchDouble(&DoubleHand, double_img);
					if (dMatchFlag == true) {
						//std::cout << "value:" << value << endl;

						if (1 <= value && value < 7) {
							//Highをクリック
							std::cout << "High->" << value << endl;
							MyClick(gbf, &rc, posNO.x, posNO.y, YorN_CLICK_W, YorN_CLICK_H, 200);
						}
						else if (7 < value && value <= 13) {
							//Lowをクリック
							std::cout << "Low->" << value << endl;
							MyClick(gbf, &rc, posYES.x, posYES.y, YorN_CLICK_W, YorN_CLICK_H, 200);
						}
						else {
							std::random_device rnd;
							std::mt19937 mt(rnd());
							std::uniform_real_distribution<> randNum(0.0, 1.0);	//0.0~1.0でmt法
							double prob = randNum(mt);

							if (0 <= prob && prob<0.5) {
								//Highをクリック
								std::cout << "High:" << value << endl;
								MyClick(gbf, &rc, posYES.x, posYES.y, YorN_CLICK_W, YorN_CLICK_H, 200);
							}
							else {
								//Lowをクリック
								std::cout << "Low:" << value << endl;
								MyClick(gbf, &rc, posNO.x, posNO.y, YorN_CLICK_W, YorN_CLICK_H, 200);
							}
						}

						/*
						std::random_device rnd;
						std::mt19937 mt(rnd());
						std::uniform_real_distribution<> randNum(0.0, 1.0);	//0.0~1.0でmt法

						double prob = randNum(mt);

						std::cout << 1.0 - ((value-1) / 13.0) << " %の確率でHigh" << endl;
						std::cout << ((value-1) / 13.0) << " %の確率でLow" << endl;

						if (0 < prob && prob < (1.0 - ((value-1) / 13.0))) {
							//Highをクリック
							std::cout << "High:" << value << endl;
							SetForegroundWindow(gbf);
							SetCursorPos(rc.left + 243, rc.top + 757);
						}
						else {
							//Lowをクリック
							std::cout << "Low:" << value << endl;
							SetForegroundWindow(gbf);
							SetCursorPos(rc.left + 430, rc.top + 766);
						}
						*/
					}
				}
			}
			break;
		case e_DoubleSuccess:
		case e_DoubleLOSE:		//ダブルアップ失敗時
		case e_PkrStart:		//ゲームを始めた最初期の画面
			cutout = false;
			matchFlag = false;
			dCutout = false;
			dMatchFlag = false;

			casino.Reset();		//カードの情報をまっさらに
			MyClick(gbf, &rc, posOK.x, posOK.y, OK_CLICK_W, OK_CLICK_H, 200);
			//cvWaitKey(2000);
			break;
		case e_SceneNone:
			break;
		}

	#if DEBUG_IMG
		std::cout << scr_img->step << ":" << trump_img[0].step <<":"<< result_img->step <<endl;
		std::cout << scr_img->depth() << ":" << trump_img[0].depth() << ":" << result_img->depth() << endl;
	#endif


	#if RECT_TRUMP	//正しく動作している 2015/10/30
		//CutoutTrump(scr_img, Trump);
	#endif

		/*--- テンプレートマッチング開始 ---*/ //正しく動作している 2015/10/30
	#if RECT_TRUMP
		/*for (int i = 0; i <= 52; i++) {
			cv::matchTemplate(Trump[0], trump_img[i], *result_img, CV_TM_CCORR_NORMED);
			minMaxLoc(*result_img, 0, &maxValue, 0, &maxPt);
			if (maxValue > threshold_stop) {
				cout << "num:" << i << endl;
				break;
			}
		}*/
		//matchTrump(Trump, trump_img);
	#else
		cv::matchTemplate(*scr_img, trump_img[0], *result_img, CV_TM_CCORR_NORMED);
	#endif	

		

	#if DEBUG_POINT
		std::cout << (maxPt.x+=posTrump[0].x) << ":" << (maxPt.y+=posTrump[0].y) << endl;
		std::cout << maxValue << endl;
	#endif

	#if RECT_TRUMP //正しく動作している 2015/10/30
		//cv::rectangle(*scr_img, maxPt, cv::Point(maxPt.x + trump_img[0].cols, maxPt.y + trump_img[0].rows), cv::Scalar(255, 0, 0), 2, 8, 0);
	#else
		cv::rectangle(*scr_img, maxPt, cv::Point(maxPt.x + trump_img[0].cols, maxPt.y + trump_img[0].rows), cv::Scalar(0, 255, 0), 2, 8, 0);
	#endif
		/*--- テンプレートマッチング終了 ---*/
		//cv::imshow(window_result, *scr_img);
		//cv::imshow(window_output, double_img[0]);
		//cv::imshow(window_rect, DoubleHand);

#else
		dst_size = cvSize(searchScreen->width - temp[0]->width + 1, searchScreen->height - temp[0]->height + 1);
		resultScreen = cvCreateImage(dst_size, IPL_DEPTH_32F, 1);

		cvMatchTemplate(searchScreen,temp[0],resultScreen, CV_TM_CCORR_NORMED);
		cvMinMaxLoc(resultScreen, &min_val, &max_val, &min_loc, &max_loc, NULL);
		cvRectangle(searchScreen, max_loc,cvPoint(max_loc.x + temp[0]->width, max_loc.y + temp[0]->height), CV_RGB(255, 0, 0), 3);

		cvShowImage(window_result, searchScreen);
		cvShowImage(window_output, temp[0]);
#endif
		cvWaitKey(17);
	}
	std::cin >> formerPhase;
	// 終了処理
	cvDestroyWindow("Desptop Image");
	std::free(scr_img);
	PostQuitMessage(0);

	return 0;
}