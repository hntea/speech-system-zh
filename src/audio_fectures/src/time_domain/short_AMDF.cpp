/*
 * short_AMDF.cpp
 *
 *  Created on: 2016年12月8日
 *      Author: hntea
 *
 * 文件功能：计算短时平均幅度
 *
 * 作用：对浊音语音可以用自相关函数求出语音波形序列的【基音周期】。
 *		 在进行语信号的线性预测分析时,也要用到自相关函数。
 *		 在浊音的基频周期【迅速下降】;在清音的基频周期没有可比较的下降。
 *
 */
#include <ros/ros.h>
#include "audio_msgs/AudioData.h"
#include "audio_msgs/AudioFeature.h"

class RosAMDF{
public:
	RosAMDF(){
		_sb = _nh.subscribe("audio_pre_emphasis",50,chapterCallback);
		_pub = _nh.advertise<audio_msgs::AudioFeature>("audio_AMDF",1000);
		ros::spin();
	}
	~RosAMDF(){}

	static void  chapterCallback(const audio_msgs::AudioData &msgs){
		f_AMDF(msgs);
	}

	static void f_AMDF(const audio_msgs::AudioData &msgs){
		std::vector<int16_t> msg(msgs.data);
		std::vector<int16_t>::iterator msg_iter = msg.begin();
		audio_msgs::AudioFeature amdf;
		//计算完就发送
		for(int k=0;k<msgs.data_size;k++){
			for(int m=0;m<msgs.data_size-1-k;m++){
				amdf.feature += abs((msg_iter[m] - msg_iter[m-k]));
			}
			_pub.publish(amdf);
			usleep(62.5);
		}
	}
private:
	ros::NodeHandle _nh;
    ros::Subscriber _sb;
    static ros::Publisher _pub;

};


ros::Publisher RosAMDF::_pub;

int main (int argc, char **argv)
{
  ROS_INFO ("Ros Node Name : audio_AMDF");
  ROS_INFO ("Ros Node Subscribe : audio_pre_emphasis");
  ROS_INFO ("Ros Node Publish Topic : audio_AMDF");
  ros::init(argc, argv, "audio_AMDF");
  RosAMDF amdf;
}




