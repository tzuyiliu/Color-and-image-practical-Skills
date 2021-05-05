/*作業4: 人臉區域處理*/
#include "opencv2/opencv.hpp"
#include <iostream>
#include <time.h>

using namespace cv; //宣告 opencv 函式庫的命名空間
using namespace std; //宣告 C++函式庫的命名空間

/** Function Headers */
void detectAndDisplay(void); //偵測人臉的函式宣告

/** Global variables */
String face_cascade_name = "data/haarcascade_frontalface_alt.xml";//正面人臉瀑布偵測訓練數據
//放專案下："haarcascade_frontalface_alt.xml”(跟sln檔放一起)
//相對路徑："data/haarcascade_frontalface_alt.xml”(放在下一層的data檔案夾)
//絕對路徑："D: / AAA / BBB / CCC / haarcascade_frontalface_alt.xml”(任意位置)

CascadeClassifier face_cascade; //建立瀑布分類器物件

Mat im; //輸入影像
int option = 0; //預設選項
int width, height; //宣告視訊的長寬

//宣告背景圖以及背景的ROI
Mat background, background_ROI;

//定義滑鼠反應函式 mouse_callback
static void mouse_callback(int event, int x, int y, int, void *)
{
	// 當滑鼠按下左鍵，根據點選的(x,y)位置，得到選項 (option) 數值 
	if (event == EVENT_LBUTTONDOWN)
	{
		cout << x << " " << y << endl;
		if (x < width / 3 && y > height * 3 / 4) {
			option = 1; //blur
			cout << "option 1" << endl;
		}
		else if (x < width * 2 / 3 && y > height * 3 / 4) {
			option = 2;
			cout << "option 2" << endl;
		}
		else if (y > height * 3 / 4) {
			option = 3;
			cout << "option 3" << endl;
		}
		else
			option = 0;
	}
}

/**
 * @function main
 */
int main(void)
{
	VideoCapture cap("data/sleepy.mp4"); //讀取影片或相機
	//VideoCapture cap(0);
	//放專案下："sleepy.mp4”(跟sln檔放一起)
	//相對路徑："data/sleepy.mp4”(放在下一層的data檔案夾)
	//絕對路徑："D: / AAA / BBB / CCC / sleepy.mp4”(任意位置)

	if (!cap.isOpened()) return 0; //不能讀視訊的處理

	//前處理
	//讀取視訊的寬 width
	width = cap.get(CAP_PROP_FRAME_WIDTH);
	//讀取視訊的高 height
	height = cap.get(CAP_PROP_FRAME_HEIGHT);
	//顯示視訊的寬 width
	cout << width << endl;
	//顯示視訊的高 height
	cout << height << endl;

	//取得第一幀影像當作背景圖
	cap >> background;
	

	//匯入人臉瀑布偵測訓練數據
	if (!face_cascade.load(face_cascade_name)) { printf("--(!)Error loading face cascade\n"); waitKey(0); return -1; };

	while (char(waitKey(1)) != 27 && cap.isOpened()) //當鍵盤沒按 Esc，以及視訊物件成功開啟時，持續執行 while 迴圈
	{
		cap >> im; //抓取視訊的畫面
		if (im.empty()) //如果沒抓到 畫面
		{
			printf(" --(!) No captured im -- Break!");  //顯示錯誤訊息
			break;
		}
		//定義視窗名稱 namedWindow
		namedWindow("window");
		imshow("window", im);

		//設定滑鼠反應函式 setMouseCallback
		setMouseCallback("window", mouse_callback);

		/*偵測人臉，並顯示AR圖像融合結果*/
		detectAndDisplay();
	}
	return 0;
}

/** @function detectAndDisplay */
void detectAndDisplay(void)
{
	/*人臉偵測部分*/
	vector<Rect> faces; //建立人臉ROI 向量
	Mat im_gray; //灰階影像物件

	cvtColor(im, im_gray, COLOR_BGR2GRAY); //彩色影像轉灰階
	equalizeHist(im_gray, im_gray); //灰階值方圖等化(對比自動增強)。若視訊品質好，可不用

	//人臉瀑布偵測
	face_cascade.detectMultiScale(im_gray, faces, 1.1, 4, 0, Size(80, 80));

	//宣告做角點偵測需要的變數
	vector<Point2f> corners;
	double qualityLevel = 0.01;
	double minDistance = 10;
	int blockSize = 3;
	bool useHarrisDetector = false;
	double k = 0.04;
	int maxCorners = 20;
	int r = 4;

	//宣告一個隨機的數字生成器
	RNG rng(12345);

	

	//獲得最大人臉的 ROI數據
	if (faces.size() > 0) {
		int largest_area = -999;
		int largest_i;
		for (int i = 0; i < faces.size(); i++) //用迴圈讀取所有人臉 ROI
		{
			//定義影像中的 ROI
			if (largest_area < faces[i].height)
			{
				largest_area = faces[i].height;
				largest_i = i;
			}
		}

		Rect faceROI = faces[largest_i]; //將最大人臉的 ROI數據存入 faceROI

		// 稍為放大ROI，使新臉能夠完整覆蓋視訊中的人臉(可不做)
		int d = 25;
		faceROI.x = faceROI.x - d;
		faceROI.y = faceROI.y - d;
		faceROI.width = faceROI.width + 2 * d;
		faceROI.height = faceROI.height + 2 * d;

		//避免faceROI超出畫面
		if (faceROI.y < 0)
			faceROI.y = 0;
		if (faceROI.y > height)
			faceROI.y = height;
		if (faceROI.height > height)
			faceROI.height = height;
		if (faceROI.x < 0)
			faceROI.x = 0;
		if (faceROI.x > width)
			faceROI.x = width;
		if (faceROI.width > width)
			faceROI.width = width;


		// 繪製人臉區域矩形框
		Point leftTop(faceROI.x, faceROI.y);
		Point rightButtom(faceROI.x + faceROI.width, faceROI.y + faceROI.height);
		rectangle(im, leftTop, rightButtom, CV_RGB(255, 0, 0), 2);
		putText(im, "XXXXXXXXX", Point(faceROI.x, faceROI.y - 15), FONT_HERSHEY_COMPLEX, 0.8, CV_RGB(255, 0, 0), 2);


		//繪製window下方的選項文字
		putText(im, "Skin", Point(50, height-50), FONT_HERSHEY_COMPLEX, 1, CV_RGB(255, 255, 0), 2);
		putText(im, "Corners", Point(226, height - 50), FONT_HERSHEY_COMPLEX, 1, CV_RGB(255, 255, 0), 2);
		putText(im, "Disappear", Point(452, height - 50), FONT_HERSHEY_COMPLEX, 1, CV_RGB(255, 255, 0), 2);


		//宣告HSV平面以及調整膚色的mask
		Mat im_HSV, skin_mask;
		Mat element = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));

		//將辨識為臉部的區怪做-100的處理複製給new_face_ROI
		Mat new_face_ROI = im(faceROI) - 100;

		//宣告用來複製人臉區塊，以及將其轉為灰階的Mat
		Mat copy, copy_gray;


		//根據 option 選項，處理ROI影像
		switch (option) {
		case 1:
			//將臉部區塊轉成HSV
			cvtColor(im(faceROI), im_HSV, COLOR_BGR2HSV);
			//將HSV在(0, 40, 40)到(50, 180, 240)區間的pixel設為1其他設為0 存入skin_mask
			inRange(im_HSV, Scalar(0, 40, 40), Scalar(50, 180, 240), skin_mask);
			//將膚色遮罩影像膨脹
			dilate(skin_mask, skin_mask, element); 
			//將膚色遮罩影像腐蝕
			erode(skin_mask, skin_mask, element); 
			//利用skin_mask將新調整的臉複製給原本的image的對應區塊
			new_face_ROI.copyTo(im(faceROI), skin_mask);
			break;
		case 2:
			//將臉部區塊複製出來給copy
			copy = im(faceROI);
			//將copy轉為灰階
			cvtColor(copy, copy_gray, COLOR_BGR2GRAY);
			//搜尋臉部角點座標存入corners陣列
			goodFeaturesToTrack(copy_gray, corners, maxCorners, qualityLevel, minDistance, Mat(), blockSize, useHarrisDetector, k);
			//將所有的角點用隨機的顏色畫出圓
			for (size_t i = 0; i < corners.size(); i++)
			{
				circle(copy, corners[i], r, Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), -1, 8, 0);
			}
			break;
		case 3:
			//將背景的ROI設成背景的臉部辨識的區塊
			background_ROI = background(faceROI);
			//將此背景ROI複製到原圖中
			background_ROI.copyTo(im(faceROI));
			break;
		default:
			break;
		}

		//顯示影像
		imshow("window", im);
	}
}

