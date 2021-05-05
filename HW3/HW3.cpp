//Mouse Events
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;
Mat im1, im2, im3;
int buttonDown = 0;
int slider1 = 50;
int slider2 = 50;

static void onMouse(int event, int x, int y, int flags, void*) //�B�z�ƹ��ƥ󪺨禡
{
	switch (event) {
	case EVENT_LBUTTONDOWN: //�ƹ�������U�ƥ�
		imshow("��ø", im1); //�b��ø�����q�Xim1
		buttonDown = 1; //�]�wbuttonDown��1
		break;
	case EVENT_LBUTTONUP: //�ƹ�����u�_�ƥ�
		buttonDown = 0; //�]�wbuttonDown��0
		break;
	case EVENT_MOUSEMOVE: //�ƹ����ʨƥ�
		if (buttonDown) {
			circle(im1, Point(x, y), 6, CV_RGB(255, 255, 255), -1, LINE_AA); //ø�s�b�|��6����߶��
			imshow("��ø", im1); //�b��ø�����q�Xim1
		}
		break;
	case EVENT_RBUTTONUP: //�ƹ��k��u�_�ƥ�
		destroyWindow("��ø"); //������ø����
		break;
	}
}

void on_trackbar(int, void *) {
	Mat dst, roi; //�ŧi�@�ӿ�X�v����n�K�W��roi
	int new_cols, new_rows; //�ŧi��j�Y�p��Ϥ������e
	if (slider2 == 0) slider2 = 1; //�Yslider2���Ȭ�0 �h�]��1

	new_cols = int(im2.cols*slider2 / 100.0); //�p��s��col
	new_rows = int(im2.rows*slider2 / 100.0); //�p��s��row

	im3.create(im2.rows, im2.cols, CV_8UC3); //�إߤ@�Ӹ�im2�@�ˤj�p��im3
	im3.setTo(0); //im3����]���¦�
	resize(im1, roi, Size(new_cols, new_rows)); //resize im1��roi�A�j�p���s�����e

	int position_x = (im2.cols / 2) - (roi.cols / 2); //�p��n�K�J���Ϥ������W����x�y��
	int position_y = (im2.rows / 2) - (roi.rows / 2); //�p��n�K�J���Ϥ������W����y�y��

	Rect roi_rect = Rect(position_x, position_y, roi.cols, roi.rows); //�ŧi�@��Rect�ܼơA��J���W����x, y�y�СA�H�ιϤ��n������e
	roi.copyTo(im3(roi_rect)); //�Nroi�v��copy��im3��������m

	addWeighted(im3, max(1, slider1)/100.0, im2, 1- max(1, slider1) / 100.0, 0.0, dst); //���Ϥ��ĦX

	imshow("�ĦX", dst); //�N���G�ϧe�{�b"�ĦX"�������W
}

int main(int, char** argv)
{

	im1.create(400, 400, CV_8UC3); //�إߤ@��400*400��im1
	im1.setTo(0); //im1����]���¦�
	im2 = imread("data/2_L.jpg", 1); //

	if (im1.empty()) cout << "Could not load image file" << endl;
	else {

		namedWindow("��ø"); //�R�W�@�ӵ�����"��ø"
		imshow("��ø", im1); //�q�X��ø����
		setMouseCallback("��ø", onMouse); //�إ�setMouseCallback

		waitKey();
	}

	on_trackbar(0, 0); //��l��"�ĦX"����

	createTrackbar("�v���ĦX", "�ĦX", &slider1, 100, on_trackbar); //�إ߲Ĥ@��trackbar�A����v���ĦX
	createTrackbar("�j�p", "�ĦX", &slider2, 100, on_trackbar); //�إ߲ĤG��trackbar�A����j�p

	waitKey(); //�����N�䵲��

	return 0;
}
