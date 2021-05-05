//Mouse Events
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;
Mat im1, im2, im3;
int buttonDown = 0;
int slider1 = 50;
int slider2 = 50;

static void onMouse(int event, int x, int y, int flags, void*) //處理滑鼠事件的函式
{
	switch (event) {
	case EVENT_LBUTTONDOWN: //滑鼠左鍵按下事件
		imshow("手繪", im1); //在手繪視窗秀出im1
		buttonDown = 1; //設定buttonDown為1
		break;
	case EVENT_LBUTTONUP: //滑鼠左鍵彈起事件
		buttonDown = 0; //設定buttonDown為0
		break;
	case EVENT_MOUSEMOVE: //滑鼠移動事件
		if (buttonDown) {
			circle(im1, Point(x, y), 6, CV_RGB(255, 255, 255), -1, LINE_AA); //繪製半徑為6的實心圓圈
			imshow("手繪", im1); //在手繪視窗秀出im1
		}
		break;
	case EVENT_RBUTTONUP: //滑鼠右鍵彈起事件
		destroyWindow("手繪"); //關閉手繪視窗
		break;
	}
}

void on_trackbar(int, void *) {
	Mat dst, roi; //宣告一個輸出影像跟要貼上的roi
	int new_cols, new_rows; //宣告放大縮小後圖片的長寬
	if (slider2 == 0) slider2 = 1; //若slider2的值為0 則設成1

	new_cols = int(im2.cols*slider2 / 100.0); //計算新的col
	new_rows = int(im2.rows*slider2 / 100.0); //計算新的row

	im3.create(im2.rows, im2.cols, CV_8UC3); //建立一個跟im2一樣大小的im3
	im3.setTo(0); //im3底色設成黑色
	resize(im1, roi, Size(new_cols, new_rows)); //resize im1到roi，大小為新的長寬

	int position_x = (im2.cols / 2) - (roi.cols / 2); //計算要貼入的圖片的左上角的x座標
	int position_y = (im2.rows / 2) - (roi.rows / 2); //計算要貼入的圖片的左上角的y座標

	Rect roi_rect = Rect(position_x, position_y, roi.cols, roi.rows); //宣告一個Rect變數，放入左上角的x, y座標，以及圖片要的長跟寬
	roi.copyTo(im3(roi_rect)); //將roi影像copy到im3的對應位置

	addWeighted(im3, max(1, slider1)/100.0, im2, 1- max(1, slider1) / 100.0, 0.0, dst); //做圖片融合

	imshow("融合", dst); //將結果圖呈現在"融合"的視窗上
}

int main(int, char** argv)
{

	im1.create(400, 400, CV_8UC3); //建立一個400*400的im1
	im1.setTo(0); //im1底色設成黑色
	im2 = imread("data/baboon.jpg", 1); //

	if (im1.empty()) cout << "Could not load image file" << endl;
	else {

		namedWindow("手繪"); //命名一個視窗為"手繪"
		imshow("手繪", im1); //秀出手繪視窗
		setMouseCallback("手繪", onMouse); //建立setMouseCallback

		waitKey();
	}

	on_trackbar(0, 0); //初始化"融合"視窗

	createTrackbar("影像融合", "融合", &slider1, 100, on_trackbar); //建立第一個trackbar，控制影像融合
	createTrackbar("大小", "融合", &slider2, 100, on_trackbar); //建立第二個trackbar，控制大小

	waitKey(); //按任意鍵結束

	return 0;
}
