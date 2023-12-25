/*

@All right reserved.
@summary: used to track the black line to correct the diretion of the car
@author: LiHao
@date: 2018.7.24
@refer: https://www.opencv-python-tutroals.readthedocs.io
@location: DJI
*/
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

Mat frame;
VideoCapture capture;

//要发送的数据集合
struct CV_Data
{
	int distance[2];
	float slope[2];
};

//实例化集合
CV_Data CD;

string IntToStr(int data)
{
	char ch_str[10] = { 0 };
	sprintf(ch_str, "%d", data);
	string  str_data(ch_str);
	return str_data;
}
/*@brief: 求点到直线距离
*         点斜式大法好
*/
int dst_calc(int x1, int y1, int x2, int y2, int tar_x = 320, int tar_y = 240)
{
	line(frame, Point(x1, y1), Point(x2, y2), Scalar(0, 155, 155), 3);
	double A, B, C, dst;
	A = y2 - y1;
	B = x1 - x2;
	C = x2 * y1 - x1 * y2;
	dst = abs(A*tar_x + B * tar_y + C) / sqrt(A*A + B * B);
	return int(dst);
}

/*
*@brief: 位置分析函数
*/
string pos_calc(Point pt1, Point  pt2, Point  pt3, Point  pt4, Point  center_point, float &dst, int camID)
{
	dst = 0;
	int  line_x1, line_x2, line_y1, line_y2;
	float slope_up = 0;
	float height = 0.1, width = 0.1;
	cout << "pt2 : " << pt2 << endl;
	if (pt2.y + 10 > 200)
	{
		
		putText(frame, "p1", Point(pt1.x, pt1.y + 10), FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 200), 1);//p1 位置
		putText(frame, "p2", Point(pt2.x, pt2.y - 5), FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 200), 1);//p2位置
		putText(frame, "p3 ", Point(pt3.x, pt3.y - 5), FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 200), 1);//p3位置
		putText(frame, "p4 ", Point(pt4.x, pt4.y + 10), FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 200), 1);//p4位置
		line_x1 = (pt1.x + pt4.x) / 2;
		line_x2 = (pt2.x + pt3.x) / 2;
		line_y1 = (pt1.y + pt4.y) / 2;
		line_y2 = (pt2.y + pt3.y) / 2;
		dst = dst_calc(line_x1, line_y1, line_x2, line_y2);

		height = float(pt1.y - pt2.y);
		width = float(-pt1.x + pt2.x);
	}
	else if (pt2.y + 10 < 100)
	{
		//修正摄像头左移
		putText(frame, "p1 ", Point(pt1.x, pt1.y + 10), FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 200), 1); //p1位置
		putText(frame, "p2 ", Point(pt2.x, pt2.y + 10), FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 200), 1);  //p2位置
		putText(frame, "p3 ", Point(pt3.x, pt3.y - 5), FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 200), 1); //p3位置
		putText(frame, "p4 ", Point(pt4.x, pt4.y - 5), FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 200), 1); //p4位置

		line_x1 = (pt1.x + pt2.x) / 2;
		line_x2 = (pt4.x + pt3.x) / 2;
		line_y1 = (pt1.y + pt2.y) / 2;
		line_y2 = (pt4.y + pt3.y) / 2;
		dst = -dst_calc(line_x1, line_x2, line_y1, line_y2);

		height = float(pt2.y - pt3.y);
		width = float(-pt2.x + pt3.x);
	}
	else
	{
		float x = abs(320 - center_point.x) * abs(320 - center_point.x);//获取镜头中心与质点的横距离并平方
		float y = abs((240 - center_point.y)) * abs((240 - center_point.y)); //获取镜头中心与质点的纵距离，并开平方
		dst = sqrt(x + y);
	}

	//初中数学斜率法
	if (height != 0 && width != 0)
		slope_up = float(height / width);
	else if (true)
		slope_up = 0x3f3f3f3f;//让斜率大于阈值

	cout << "slope_up : " << slope_up << endl;
	CD.slope[1] += slope_up;
	//根据角度分析行进方向
	if (slope_up >= 13 || slope_up <= -13)
	{
		printf("%f\n", slope_up);
		return "true";
	}
	else if (slope_up > 0)//车车偏离黑线向左行驶
	{
		printf("%f\n", slope_up);
		return "left";
	}
	else if (slope_up < 0)
	{
		printf("%f\n", slope_up);
		return "right";
	}
}


/*
*@brief:
*/
void drawLine(string shape, vector<Point> & approx)
{
	Point pt[13];
	if (shape == "rectangle" || shape == "sqaure")
	{
		pt[1] = approx[0];
		pt[2] = approx[1];
		pt[3] = approx[2];
		pt[4] = approx[3];
		line(frame, pt[1], pt[2], Scalar(0, 200, 0), 3);
		line(frame, pt[3], pt[2], Scalar(0, 200, 0), 3);
		line(frame, pt[4], pt[3], Scalar(0, 200, 0), 3);
		line(frame, pt[4], pt[1], Scalar(0, 200, 0), 3);
	}
	else if (shape == "pentagon")
	{
		pt[1] = approx[0];
		pt[2] = approx[1];
		pt[3] = approx[2];
		pt[4] = approx[3];
		pt[5] = approx[4];
		line(frame, pt[1], pt[2], Scalar(0, 200, 0), 3);
		line(frame, pt[3], pt[2], Scalar(0, 200, 0), 3);
		line(frame, pt[4], pt[3], Scalar(0, 200, 0), 3);
		line(frame, pt[4], pt[5], Scalar(0, 200, 0), 3);
		line(frame, pt[5], pt[1], Scalar(0, 200, 0), 3);
	}
	else if (shape == "cross")
	{
		pt[1] = approx[0];
		pt[2] = approx[1];
		pt[3] = approx[2];
		pt[4] = approx[3];
		pt[5] = approx[4];
		pt[6] = approx[5];
		line(frame, pt[1], pt[2], Scalar(0, 200, 0), 3);
		line(frame, pt[3], pt[2], Scalar(0, 200, 0), 3);
		line(frame, pt[4], pt[3], Scalar(0, 200, 0), 3);
		line(frame, pt[4], pt[5], Scalar(0, 200, 0), 3);
		line(frame, pt[5], pt[6], Scalar(0, 200, 0), 3);
		line(frame, pt[1], pt[1], Scalar(0, 200, 0), 3);
	}

}

//初始化图形并圈出
string detect(vector<Point> cnts_single, vector<Point> & approx)
{
	string shape = "undentified";
	double peri = arcLength(cnts_single, true);
	approxPolyDP(cnts_single, approx, 0.045 * peri, true);
	if (approx.size() == 3)
	{
		shape = "triangle";
	}
	else if (approx.size() == 4)
	{
		shape = "rectangle";
		/*待完成*/
	}
	else if (approx.size() == 5)
	{
		shape = "pentagon";
	}
	else
	{
		shape = "circle";
	}

	return shape;
}




bool CV_Close = false; //用于关闭摄像头CV


int main()
{

	capture.open("..//图片1.png");
	capture.set(cv::CAP_PROP_FRAME_WIDTH, 640); // 宽度
	capture.set(cv::CAP_PROP_FRAME_HEIGHT, 480); // 高度
	
											   //检测1
	if (!capture.isOpened())
	{
		printf("--(!)Error opening video capture\n"); return -1;
	}
	//计时5次， 取平均值
	int num = 0;
	while (capture.read(frame))
	{
		capture.set(cv::CAP_PROP_FRAME_WIDTH, 640); // 宽度
		capture.set(cv::CAP_PROP_FRAME_HEIGHT, 480); // 高度
		resize(frame, frame, Size(640, 480));
		cout << "x : " << frame.cols << " y : " << frame.rows << endl;

		num++;
		//检测2
		if (frame.empty())
		{
			printf(" --(!) No captured frame -- Break!");
			break;
		}

		//格式变换
		Mat imgHSV_ori, imgHSV;
		cvtColor(frame, imgHSV_ori, COLOR_BGR2HSV);
		imshow("frame", frame);
		waitKey(0);
		Rect rect(0, 150, 640, 330); 
		imgHSV = imgHSV_ori(rect);
		imshow("imgHSV",imgHSV);
		waitKey(0);
		frame = imgHSV;
		Scalar lower_black = Scalar(0, 0, 0);
		Scalar upper_black = Scalar(180, 255, 80);
		//取颜色范围
		Mat imgThresHold;
		inRange(imgHSV, lower_black, upper_black, imgThresHold);

		Mat erode_element = getStructuringElement(MORPH_RECT, Size(2, 2));
		Mat erode_out;     //腐蚀后的图像
		erode(imgThresHold, erode_out, erode_element, Point(-1, -1), 1);

		Mat dilate_element = getStructuringElement(MORPH_RECT, Size(3, 3));
		Mat dilate_out;     //膨胀后的图像
		dilate(erode_out, dilate_out, dilate_element, Point(-1, -1), 4);
		imshow("Dilate Frame", dilate_out);
		waitKey(0);
		vector<vector<Point>>cnts;//获取了一堆又一堆点
		findContours(dilate_out, cnts, RETR_EXTERNAL, CHAIN_APPROX_NONE);

		for (int i = 0; i < cnts.size(); i++)
		{
			vector<Point> cnts_single = cnts[i];//获取了上面一堆点中的一个点
			if (cnts_single.size() > 300)
			{
				vector<Point> approx;
				string shape = detect(cnts_single, approx);
				Moments M = moments(cnts_single);
				int cX, cY;
				if (M.m10 != 0)
				{
					//表示图像重心
					cX = int((M.m10 / M.m00));
					cY = int((M.m01 / M.m00));
				}
				else
				{
					cX = cY = 0;
				}
				line(frame, Point(320, 0), Point(320, 480), (200, 200, 200), 4);//画中位线

				cout << shape << endl;

				drawLine(shape, approx);  //画矩形

				line(frame, Point(cX, cY), Point(0, cY), Scalar(200, 200, 200), 1);//画质心线
				line(frame, Point(cX, cY), Point(cX, 0), Scalar(200, 200, 200), 1);

				putText(frame, "black line " + shape, Point(cX, cY), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 200), 1);
				line(frame, Point(cX, cY), Point(cX, cY), (200, 200, 200), 5);
				putText(frame, "position " + IntToStr(cX) + " , " + IntToStr(cY), Point(cX, cY + 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(200, 0, 200), 1);  //质心位置

				float dst = 0;
				
				//避免出现三角形时pos_calc()报错
				if (shape == "rectangle" || shape == "square")
				{
					cout << "shape: " << shape << endl;
					string angle_status = "";
					
					angle_status = pos_calc(approx[0], approx[1], approx[2], approx[3], Point(cX, cY), dst, 1);
					cout << "cX " << cX << endl;
			
					//小车跑到右边去啦
					if (cX > 340)
					{
						CD.distance[1] += int(dst);
						putText(frame, "Left Off", Point(340, 100), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 200), 1);  //偏离状态
						putText(frame, "Distance " + IntToStr(int(dst)), Point(cX, cY + 40), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 40, 205), 1);  //偏离状态
					}
					else if (cX < 300)
					{
					cout << "too fucing much segmentation fault!!" << endl;
						CD.distance[1] += int(dst);
						putText(frame, "Right Off", Point(340, 100), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 200), 1);  //偏离状态
						putText(frame, "Distance" + IntToStr(int(dst)), Point(cX, cY + 40), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 40, 205), 1);  //偏离状态
					cout << "not enough" << endl;
					}
					else
					{
						CD.distance[1] += int(dst);
						putText(frame, "Distance" + IntToStr(int(dst)), Point(cX, cY + 40), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 200), 1);  //偏离状态
						if (angle_status == "true")
						{

							putText(frame, "Correct direction!", Point(320, 100), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 200), 2);
						}
						else if (angle_status == "left")
						{

							putText(frame, "Go right!", Point(320, 100), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 200), 2);
						}
						else if (angle_status == "right")
						{

							putText(frame, "Go left!", Point(320, 100), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 200), 2);
						}
						else
						{
							putText(frame, "No rectangle!!!", Point(320, 100), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 200), 2);
						}
					}
					
				}
			}
			else
			{
				printf("too fucking small %d\n", cnts_single.size());
			}
		}
		imshow("Result Frame1", frame);
		waitKey(0);
		cvtColor(frame, frame, COLOR_HSV2BGR);
		imshow("Result Frame2", frame);
		waitKey(0);
		//waitKey(50);
		if (waitKey(50) == 'q' || CV_Close == true)
		{
			capture.release();
			break;
		} // escape
	}
	// CD.distance[1] = CD.distance[1] / 5;
	// CD.slope[1] = CD.slope[1] / 5;
	
	cout << "All is well " << endl;
	cout << "CD.distance : "  << CD.distance[1] << "      CD.slope : " << CD.slope[1] << endl;
	//system("pause");
	return 0;
}