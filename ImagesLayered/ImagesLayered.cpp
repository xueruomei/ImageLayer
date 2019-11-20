// ImagesLayered.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"//配置后必引头部
#include "string"
#include<opencv2/opencv.hpp> 
#include <opencv2/core.hpp> //图像数据结构的核心头文件
#include <opencv2/highgui.hpp> //图形接口函数的highgui头文件
#include "ImagesLayered.h"
#include <fstream>

using namespace std;
using namespace cv;

#ifdef DEBUG
#pragma comment(lib,"opencv_world343d.lib")
#endif // RELEASE
#pragma comment(lib,"opencv_world343.lib")

//分割第一层图片
void wrapper::func_ImageLayer_First(layerParams &layer_params) {
	string firstName = "1";
	string _originFilePath = layer_params.originFilePath + layer_params.fileName;
	string _outputPath = layer_params.reFilePath + layer_params.fileName+ firstName + ".jpg";
	//等比缩小
	int w,h;
	if (layer_params.cols > layer_params.rows) {//横图
		w = layer_params.lay_01_cols;
		h = layer_params.rows * w / layer_params.cols;
	}
	else if (layer_params.cols <= layer_params.rows) {//竖图
		h = layer_params.lay_01_rows;
		w = layer_params.cols * h / layer_params.rows;
	}
	//读原图
	cv::Mat gray_origin_image = imread(_originFilePath);
	cv::Mat new_image;
	resize(gray_origin_image, new_image, cv::Size(w, h), 0, 0, INTER_LINEAR);
	imwrite(_outputPath,new_image);
};

//分割第二层图片
void wrapper::func_ImageLayer_Second(layerParams &layer_params) {
	string secondName = "2";
	string _originFilePath = layer_params.originFilePath + layer_params.fileName;
	cv::Mat originImage = cv::imread(_originFilePath);
	if (!(layer_params.cols < layer_params._cols) || !(layer_params.rows < layer_params._rows)) {
		//宽和高两者中都没有超过或者两者之一超过了 
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
				if (i == ceil_height) {//说明最后一块的高度为图片的高度
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
		//计算分割坐标 （x,y,w,h） end
	}else {
		//cout << "cols: _cols" << layer_params.cols << " " << layer_params._cols << endl;
		//cout << "rows: _rows" << layer_params.rows << " " << layer_params._rows << endl;
		cout << "第二层不需要分层" << endl;
	}
};

//分割图片的总入口
int wrapper::func_ImageLayer_all(string origin_dirpath, string file_name, string set_dirpath,int _cols,int _rows) {
	cv::Mat originImage = cv::imread(origin_dirpath + file_name,0);
	if (originImage.empty()) {
		cout << "please input file" << endl;
		return -1;
	};

	//1.获取图片的大小
	//2.如果 图片宽度 >= 配置宽度 || 图片高度 >= 配置高度
	if (originImage.cols >= 1280 || originImage.rows <= 960) {
		//2.1 需要分层  第一层 第二层
		// 构造分层参数
		layerParams layer_params;
		layer_params.originFilePath = origin_dirpath;
		layer_params.fileName = file_name;
		layer_params.reFilePath = set_dirpath;
		layer_params._cols = _cols;//json 配置的宽 2560 // 让客户端传进来
		layer_params._rows = _rows;//json 配置的高 1820 // 让客户端传进来
		layer_params.lay_01_cols = 1280;//json 配置的宽
		layer_params.lay_01_rows = 960;//json 配置的高
		layer_params.rows = originImage.rows;//图片的高度
		layer_params.cols = originImage.cols;//图片的宽度
		//生成第一层的配置宽度
		func_ImageLayer_First(layer_params);
		//生成第一层的配置高度
		func_ImageLayer_Second(layer_params);
	}
	else { //3. 否则 不需要分层
		cout << "图片不需要分层" << endl;
	}
	return 0;
};