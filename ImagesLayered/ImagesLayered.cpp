// ImagesLayered.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"//���ú����ͷ��
#include "string"
#include<opencv2/opencv.hpp> 
#include <opencv2/core.hpp> //ͼ�����ݽṹ�ĺ���ͷ�ļ�
#include <opencv2/highgui.hpp> //ͼ�νӿں�����highguiͷ�ļ�
#include "ImagesLayered.h"
#include <fstream>

using namespace std;
using namespace cv;

#ifdef DEBUG
#pragma comment(lib,"opencv_world343d.lib")
#endif // RELEASE
#pragma comment(lib,"opencv_world343.lib")

//�ָ��һ��ͼƬ
void wrapper::func_ImageLayer_First(layerParams &layer_params) {
	string firstName = "1";
	string _originFilePath = layer_params.originFilePath + layer_params.fileName;
	string _outputPath = layer_params.reFilePath + layer_params.fileName+ firstName + ".jpg";
	//�ȱ���С
	int w,h;
	if (layer_params.cols > layer_params.rows) {//��ͼ
		w = layer_params.lay_01_cols;
		h = layer_params.rows * w / layer_params.cols;
	}
	else if (layer_params.cols <= layer_params.rows) {//��ͼ
		h = layer_params.lay_01_rows;
		w = layer_params.cols * h / layer_params.rows;
	}
	//��ԭͼ
	cv::Mat gray_origin_image = imread(_originFilePath);
	cv::Mat new_image;
	resize(gray_origin_image, new_image, cv::Size(w, h), 0, 0, INTER_LINEAR);
	imwrite(_outputPath,new_image);
};

//�ָ�ڶ���ͼƬ
void wrapper::func_ImageLayer_Second(layerParams &layer_params) {
	string secondName = "2";
	string _originFilePath = layer_params.originFilePath + layer_params.fileName;
	cv::Mat originImage = cv::imread(_originFilePath);
	if (!(layer_params.cols < layer_params._cols) || !(layer_params.rows < layer_params._rows)) {
		//��͸������ж�û�г�����������֮һ������ 
		vector<int> w_arr;
		vector<int> h_arr;
		char buffer[256];
		if (layer_params.cols < layer_params._cols) {
			layer_params._cols = layer_params.cols;
		}
		if (layer_params.rows < layer_params._rows) {
			layer_params._rows = layer_params.rows;
		}
		float scale_width = (float)layer_params.cols / layer_params._cols;
		float ceil_width = ceil(scale_width);
		if (scale_width) {
			for (int i = 1; i <= ceil_width; i++) {
				if (i == ceil_width) {					
					w_arr.push_back(layer_params.cols - layer_params._cols * (i - 1));
				}
				else {
					w_arr.push_back(layer_params._cols);
				}
			}
		}
		float scale_height = (float)layer_params.rows / layer_params._rows;
		float ceil_height = ceil(scale_height);
		//cout << "ceil_width ::" << ceil_width << " ceil_height ::" << ceil_height << endl;
		if (scale_height) {
			for (int i = 1; i <= ceil_height; i++) {
				if (i == ceil_height) {//˵�����һ��ĸ߶�ΪͼƬ�ĸ߶�
					h_arr.push_back(layer_params.rows - layer_params._rows * (i - 1));
				}
				else {
					h_arr.push_back(layer_params._rows);
				}
			}
		}
		int sum = 0;
		vector<int> sign;
		for (int w = 0; w < w_arr.size(); w++) {
			for (int h = 0; h < h_arr.size(); h++) {
				sum++;
				sign.push_back(sum);
				cv::Mat layer_02_image;
				layer_02_image = originImage(Rect(w * layer_params._cols, h * layer_params._rows, w_arr[w], h_arr[h]));
				string _outputPath = layer_params.reFilePath + layer_params.fileName + secondName;
				sprintf_s(buffer, "%s%d.jpg", _outputPath.c_str(),(int)sum);
				cv::imwrite(buffer, layer_02_image);
			}
		}
		//����ָ����� ��x,y,w,h�� end
	}else {
		//cout << "cols: _cols" << layer_params.cols << " " << layer_params._cols << endl;
		//cout << "rows: _rows" << layer_params.rows << " " << layer_params._rows << endl;
		cout << "�ڶ��㲻��Ҫ�ֲ�" << endl;
	}
};

//�ָ�ͼƬ�������
int wrapper::func_ImageLayer_all(string origin_dirpath, string file_name, string set_dirpath,int _cols,int _rows) {
	cv::Mat originImage = cv::imread(origin_dirpath + file_name,0);
	if (originImage.empty()) {
		cout << "please input file" << endl;
		return -1;
	};

	//1.��ȡͼƬ�Ĵ�С
	//2.��� ͼƬ��� >= ���ÿ�� || ͼƬ�߶� >= ���ø߶�
	if (originImage.cols >= 1280 || originImage.rows <= 960) {
		//2.1 ��Ҫ�ֲ�  ��һ�� �ڶ���
		// ����ֲ����
		layerParams layer_params;
		layer_params.originFilePath = origin_dirpath;
		layer_params.fileName = file_name;
		layer_params.reFilePath = set_dirpath;
		layer_params._cols = _cols;//json ���õĿ� 2560 // �ÿͻ��˴�����
		layer_params._rows = _rows;//json ���õĸ� 1820 // �ÿͻ��˴�����
		layer_params.lay_01_cols = 1280;//json ���õĿ�
		layer_params.lay_01_rows = 960;//json ���õĸ�
		layer_params.rows = originImage.rows;//ͼƬ�ĸ߶�
		layer_params.cols = originImage.cols;//ͼƬ�Ŀ��
		//���ɵ�һ������ÿ��
		func_ImageLayer_First(layer_params);
		//���ɵ�һ������ø߶�
		func_ImageLayer_Second(layer_params);
	}
	else { //3. ���� ����Ҫ�ֲ�
		cout << "ͼƬ����Ҫ�ֲ�" << endl;
	}
	return 0;
};