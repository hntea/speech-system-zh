/*
 * audio_window.cpp
 *
 *  Created on: 2016年12月10日
 *      Author: hntea
 */
#include <ros/ros.h>
#include "audio_msgs/AudioData.h"
#include "audio_msgs/TimeFeature.h"
#include <vector>
#include <list>
#include <aquila/source/window/HammingWindow.h>
#include <aquila/global.h>

/*
 * 帧移动相当于一个先进先出的队列，每个队列长度与窗口长度相等。
 * 窗长度和帧移可通过启动参数配置
 *
 * 注意：帧移最好是可以被块大小整除
 * */
namespace Hntea{

class Hamming: public Aquila::HammingWindow{
public:
	Hamming(std::size_t size):HammingWindow(size){
		_winlen = size;
		std::cout<<"The Window length is: "<<_winlen<<std::endl;
		ros::param::param<int>("~step",_step,_winlen/2);
		std::cout<<"The Step length is: "<<_step<<std::endl;

		_pr_source_buf.resize(_winlen);

		_windata = m_data.begin();


		_sb = _nh.subscribe("audio_pre_emphasis",50,chapterCallback);
		_pub = _nh.advertise<audio_msgs::AudioData>("hamming_window",1000);
		ros::spin();
	}
	~Hamming(){}

	static void  chapterCallback(const audio_msgs::AudioData &msgs){
		int id = 0;
		audio_msgs::AudioData new_msg;
		new_msg.data.resize(msgs.data_size);
		new_msg.data_size = msgs.data_size;
		//获取源数据
		std::vector<int16_t> src(msgs.data);
		src.resize(msgs.data_size);
		std::vector<int16_t>::iterator src_iter = src.begin();

		for(int i=0,cur = 0;(cur +=_step) <= msgs.data_size;i++){
			//每次更新缓存数据，更新长度即为帧移
			for(int j=0;j<_step;j++){
				_pr_source_buf.pop_front();						//删除缓存头部
				_pr_source_buf.push_back(src_iter[j+_step*i]);	//尾部添加新元素，保持缓存数据长度不变
			}

			listToMsg(_pr_source_buf,new_msg);						//每次移动完成后计算加窗值
			_pub.publish(new_msg);
		}


	}


	/*
	 * 函数功能：将队列中的数据加窗计算后填充到消息中
	 * */
	static void listToMsg(std::list<int16_t>& list,audio_msgs::AudioData& msgs){
		std::list<int16_t>::iterator liter;
		int i=0;
		for(liter =list.begin(); liter != list.end(); ++liter,++i){
			msgs.data[i] = *liter * _windata[i];
		}
	}



private:
	ros::NodeHandle _nh;
    ros::Subscriber _sb;
    static ros::Publisher _pub;
    static int _winlen;						//窗长
    static int _step;						//帧移
    static std::list<int16_t> _pr_source_buf;	//存放前个状态的缓存数据
    static std::vector<Aquila::SampleType>::iterator _windata;	//汉民窗数据迭代器
};


int Hamming::_step ;
int Hamming::_winlen;
std::list<int16_t> Hamming::_pr_source_buf;
ros::Publisher Hamming::_pub;
std::vector<Aquila::SampleType>::iterator Hamming::_windata;

}

int main (int argc, char **argv)
{
  ROS_INFO ("Ros Node Name : hamming_window");
  ROS_INFO ("Ros Node Subscribe : audio_pre_emphasis");
  ROS_INFO ("Ros Node Publish Topic : hamming_window");
  int winlen;
  ros::init(argc, argv, "hamming_window");
  ros::param::param<int>("~winlen",winlen,512);
  Hntea::Hamming window(winlen);
}


