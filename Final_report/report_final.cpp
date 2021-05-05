#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>
using namespace cv;
using namespace std;

Mat inputImage;
Mat inputImage_mosaic;
Mat inputImage_clone;
Mat img_gray, dst;
int height, width;

//馬賽克大小
int neightbourhood = 10;
//記錄滑鼠狀態，0為滑鼠左鍵未按下或彈起，1為滑鼠左鍵按下
int mouseStatus = 0;
void onMouse(int events, int x, int y, int flag, void* ustg);
//創建馬賽克區域
void createMosaicImage(Mat inputMat, Mat& outputMat, int size);
//設置馬賽克區域
void setMosaic(Mat& inputMat, Rect rect);
//油畫特效
void oil_painting(Mat inputMat);

int main() {
	int option = 0;
	cout << "0為油畫特效，1為馬賽克: ";
	cin >> option;

	inputImage = imread("data/building.jpg");
	
	switch (option) {
	case 0:
		oil_painting(inputImage);
		return 0;

		break;
	case 1:
		inputImage_clone = inputImage.clone();
		createMosaicImage(inputImage, inputImage_mosaic, neightbourhood);
		namedWindow("showImage");
		setMouseCallback("showImage", onMouse);
		waitKey();
		return 0;

		break;
	default:
		break;
	}
}

void createMosaicImage(Mat inputMat, Mat& outputMat, int size) {
	RNG rng;
	height = inputMat.rows;
	width = inputMat.cols;
	Mat padding;
	Mat tempMat;
	//為了方便後面計算，將輸入的影像大小擴充到寬高都是size的倍數
	copyMakeBorder(inputMat, padding, 0, size - inputMat.rows % size, 0, size - inputMat.cols % size, BORDER_REPLICATE);
	tempMat = padding.clone();
	for (int row = 0; row < padding.rows; row += size) {
		for (int col = 0; col < padding.cols; col += size) {
			int rand_x = rng.uniform(0, size);
			int rand_y = rng.uniform(0, size);
			Rect rect = Rect(col, row, size, size);
			Mat roi = tempMat(rect);
			Scalar color = Scalar(padding.at<Vec3b>(row + rand_y, col + rand_x)[0], \
				padding.at<Vec3b>(row + rand_y, col + rand_x)[1], \
				padding.at<Vec3b>(row + rand_y, col + rand_x)[2]);
			roi.setTo(color);
		}
	}
	outputMat = tempMat(Rect(0, 0, width, height)).clone();
}
void setMosaic(Mat& inputMat, Rect rect) {
	Mat roi = inputMat(rect);
	Mat tempRoi = inputImage_mosaic(rect);
	tempRoi.copyTo(roi);
}
void onMouse(int events, int x, int y, int flag, void* ustg) {
	//當滑鼠移出圖片區域時不做操作
	if (x < 0 || x > inputImage.cols || y < 0 || y > inputImage.rows) {
		return;
	}
	//馬賽克塊的位置訊息
	int x_left, x_right, y_top, y_bottom;
	x - neightbourhood <= 0 ? x_left = 0 : x_left = x - neightbourhood;
	x + neightbourhood > inputImage.cols ? x_right = inputImage.cols : x_right = x + neightbourhood;
	y - neightbourhood <= 0 ? y_top = 0 : y_top = y - neightbourhood;
	y + neightbourhood > inputImage.rows ? y_bottom = inputImage.rows : y_bottom = y + neightbourhood;
	if (events == EVENT_LBUTTONDOWN) {
		mouseStatus = 1;
		setMosaic(inputImage_clone, Rect(x_left, y_top, x_right - x_left, y_bottom - y_top));
	}
	else if (events == EVENT_MOUSEMOVE) {
		if (mouseStatus == 1) {
			setMosaic(inputImage_clone, Rect(x_left, y_top, x_right - x_left, y_bottom - y_top));
		}
		else {
			//nothing
		}
	}
	else if (events == EVENT_LBUTTONUP) {
		mouseStatus = 0;
	}
	else {
		//cout << "nothing" << endl;
	}
	imshow("showImage", inputImage_clone);
}

void oil_painting(Mat inputMat) {
	cvtColor(inputMat, img_gray, COLOR_BGR2GRAY);

	int level, currentMax, max_level;
	height = inputMat.rows;
	width = inputMat.cols;

	copyMakeBorder(inputMat, inputMat, 4, 4, 4, 4, BORDER_REFLECT);

	dst = Mat::zeros(height + 8, width + 8, CV_8UC3);


	for (int i = 4; i < inputMat.rows - 4; i++) {
		for (int j = 4; j < inputMat.cols - 4; j++) {
			int arr[32] = { 0 };
			for (int m = -4; m < 4; m++) {
				for (int n = -4; n < 4; n++) {
					level = ((int)img_gray.at<uchar>(i + m, j + n) / 8);
					arr[level] += 1;
				}
			}

			currentMax = arr[0];
			max_level = 0;
			for (int k = 0; k < 32; k++) {
				if (currentMax < arr[k]) {
					currentMax = arr[k];
					max_level = k;
				}
			}

			for (int m = -4; m < 4; m++) {
				for (int n = -4; n < 4; n++) {
					if ((int)img_gray.at<uchar>(i + m, j + n) >= max_level * 8 && (int)img_gray.at<uchar>(i + m, j + n) <= (max_level + 1) * 8) {
						dst.at<Vec3b>(i, j)[0] = (int)inputMat.at<Vec3b>(i + m, j + n)[0];
						dst.at<Vec3b>(i, j)[1] = (int)inputMat.at<Vec3b>(i + m, j + n)[1];
						dst.at<Vec3b>(i, j)[2] = (int)inputMat.at<Vec3b>(i + m, j + n)[2];
					}
				}
			}
		}
	}

	dst = dst(Rect(4, 4, width, height));
	inputMat = inputMat(Rect(4, 4, width, height));

	imshow("Original", inputMat);
	imshow("dst", dst);
	waitKey(0);
}