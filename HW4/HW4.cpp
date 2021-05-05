/*�@�~4: �H�y�ϰ�B�z*/
#include "opencv2/opencv.hpp"
#include <iostream>
#include <time.h>

using namespace cv; //�ŧi opencv �禡�w���R�W�Ŷ�
using namespace std; //�ŧi C++�禡�w���R�W�Ŷ�

/** Function Headers */
void detectAndDisplay(void); //�����H�y���禡�ŧi

/** Global variables */
String face_cascade_name = "data/haarcascade_frontalface_alt.xml";//�����H�y�r�������V�m�ƾ�
//��M�פU�G"haarcascade_frontalface_alt.xml��(��sln�ɩ�@�_)
//�۹���|�G"data/haarcascade_frontalface_alt.xml��(��b�U�@�h��data�ɮק�)
//������|�G"D: / AAA / BBB / CCC / haarcascade_frontalface_alt.xml��(���N��m)

CascadeClassifier face_cascade; //�إ��r������������

Mat im; //��J�v��
int option = 0; //�w�]�ﶵ
int width, height; //�ŧi���T�����e

//�ŧi�I���ϥH�έI����ROI
Mat background, background_ROI;

//�w�q�ƹ������禡 mouse_callback
static void mouse_callback(int event, int x, int y, int, void *)
{
	// ��ƹ����U����A�ھ��I�諸(x,y)��m�A�o��ﶵ (option) �ƭ� 
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
	VideoCapture cap("data/sleepy.mp4"); //Ū���v���ά۾�
	//VideoCapture cap(0);
	//��M�פU�G"sleepy.mp4��(��sln�ɩ�@�_)
	//�۹���|�G"data/sleepy.mp4��(��b�U�@�h��data�ɮק�)
	//������|�G"D: / AAA / BBB / CCC / sleepy.mp4��(���N��m)

	if (!cap.isOpened()) return 0; //����Ū���T���B�z

	//�e�B�z
	//Ū�����T���e width
	width = cap.get(CAP_PROP_FRAME_WIDTH);
	//Ū�����T���� height
	height = cap.get(CAP_PROP_FRAME_HEIGHT);
	//��ܵ��T���e width
	cout << width << endl;
	//��ܵ��T���� height
	cout << height << endl;

	//���o�Ĥ@�V�v����@�I����
	cap >> background;
	

	//�פJ�H�y�r�������V�m�ƾ�
	if (!face_cascade.load(face_cascade_name)) { printf("--(!)Error loading face cascade\n"); waitKey(0); return -1; };

	while (char(waitKey(1)) != 27 && cap.isOpened()) //����L�S�� Esc�A�H�ε��T���󦨥\�}�ҮɡA������� while �j��
	{
		cap >> im; //������T���e��
		if (im.empty()) //�p�G�S��� �e��
		{
			printf(" --(!) No captured im -- Break!");  //��ܿ��~�T��
			break;
		}
		//�w�q�����W�� namedWindow
		namedWindow("window");
		imshow("window", im);

		//�]�w�ƹ������禡 setMouseCallback
		setMouseCallback("window", mouse_callback);

		/*�����H�y�A�����AR�Ϲ��ĦX���G*/
		detectAndDisplay();
	}
	return 0;
}

/** @function detectAndDisplay */
void detectAndDisplay(void)
{
	/*�H�y��������*/
	vector<Rect> faces; //�إߤH�yROI �V�q
	Mat im_gray; //�Ƕ��v������

	cvtColor(im, im_gray, COLOR_BGR2GRAY); //�m��v����Ƕ�
	equalizeHist(im_gray, im_gray); //�Ƕ��Ȥ�ϵ���(���۰ʼW�j)�C�Y���T�~��n�A�i����

	//�H�y�r������
	face_cascade.detectMultiScale(im_gray, faces, 1.1, 4, 0, Size(80, 80));

	//�ŧi�����I�����ݭn���ܼ�
	vector<Point2f> corners;
	double qualityLevel = 0.01;
	double minDistance = 10;
	int blockSize = 3;
	bool useHarrisDetector = false;
	double k = 0.04;
	int maxCorners = 20;
	int r = 4;

	//�ŧi�@���H�����Ʀr�ͦ���
	RNG rng(12345);

	

	//��o�̤j�H�y�� ROI�ƾ�
	if (faces.size() > 0) {
		int largest_area = -999;
		int largest_i;
		for (int i = 0; i < faces.size(); i++) //�ΰj��Ū���Ҧ��H�y ROI
		{
			//�w�q�v������ ROI
			if (largest_area < faces[i].height)
			{
				largest_area = faces[i].height;
				largest_i = i;
			}
		}

		Rect faceROI = faces[largest_i]; //�N�̤j�H�y�� ROI�ƾڦs�J faceROI

		// �y����jROI�A�Ϸs�y��������л\���T�����H�y(�i����)
		int d = 25;
		faceROI.x = faceROI.x - d;
		faceROI.y = faceROI.y - d;
		faceROI.width = faceROI.width + 2 * d;
		faceROI.height = faceROI.height + 2 * d;

		//�קKfaceROI�W�X�e��
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


		// ø�s�H�y�ϰ�x�ή�
		Point leftTop(faceROI.x, faceROI.y);
		Point rightButtom(faceROI.x + faceROI.width, faceROI.y + faceROI.height);
		rectangle(im, leftTop, rightButtom, CV_RGB(255, 0, 0), 2);
		putText(im, "XXXXXXXXX", Point(faceROI.x, faceROI.y - 15), FONT_HERSHEY_COMPLEX, 0.8, CV_RGB(255, 0, 0), 2);


		//ø�swindow�U�誺�ﶵ��r
		putText(im, "Skin", Point(50, height-50), FONT_HERSHEY_COMPLEX, 1, CV_RGB(255, 255, 0), 2);
		putText(im, "Corners", Point(226, height - 50), FONT_HERSHEY_COMPLEX, 1, CV_RGB(255, 255, 0), 2);
		putText(im, "Disappear", Point(452, height - 50), FONT_HERSHEY_COMPLEX, 1, CV_RGB(255, 255, 0), 2);


		//�ŧiHSV�����H�νվ㽧�⪺mask
		Mat im_HSV, skin_mask;
		Mat element = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));

		//�N���Ѭ��y�����ϩǰ�-100���B�z�ƻs��new_face_ROI
		Mat new_face_ROI = im(faceROI) - 100;

		//�ŧi�Ψӽƻs�H�y�϶��A�H�αN���ର�Ƕ���Mat
		Mat copy, copy_gray;


		//�ھ� option �ﶵ�A�B�zROI�v��
		switch (option) {
		case 1:
			//�N�y���϶��নHSV
			cvtColor(im(faceROI), im_HSV, COLOR_BGR2HSV);
			//�NHSV�b(0, 40, 40)��(50, 180, 240)�϶���pixel�]��1��L�]��0 �s�Jskin_mask
			inRange(im_HSV, Scalar(0, 40, 40), Scalar(50, 180, 240), skin_mask);
			//�N����B�n�v������
			dilate(skin_mask, skin_mask, element); 
			//�N����B�n�v���G�k
			erode(skin_mask, skin_mask, element); 
			//�Q��skin_mask�N�s�վ㪺�y�ƻs���쥻��image�������϶�
			new_face_ROI.copyTo(im(faceROI), skin_mask);
			break;
		case 2:
			//�N�y���϶��ƻs�X�ӵ�copy
			copy = im(faceROI);
			//�Ncopy�ର�Ƕ�
			cvtColor(copy, copy_gray, COLOR_BGR2GRAY);
			//�j�M�y�����I�y�Цs�Jcorners�}�C
			goodFeaturesToTrack(copy_gray, corners, maxCorners, qualityLevel, minDistance, Mat(), blockSize, useHarrisDetector, k);
			//�N�Ҧ������I���H�����C��e�X��
			for (size_t i = 0; i < corners.size(); i++)
			{
				circle(copy, corners[i], r, Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)), -1, 8, 0);
			}
			break;
		case 3:
			//�N�I����ROI�]���I�����y�����Ѫ��϶�
			background_ROI = background(faceROI);
			//�N���I��ROI�ƻs���Ϥ�
			background_ROI.copyTo(im(faceROI));
			break;
		default:
			break;
		}

		//��ܼv��
		imshow("window", im);
	}
}

