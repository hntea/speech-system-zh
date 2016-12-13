/*
 * zero_crossing.cpp
 *
 *  Created on: 2016年12月7日
 *      Author: hntea
 */
#include <ros/ros.h>
#include "audio_msgs/AudioData.h"
#include "audio_msgs/AudioFeature.h"
#include <vector>


class RosZeroCrossing{
public:
	RosZeroCrossing(){
		_sb = _nh.subscribe("audio_pre_emphasis",50,chapterCallback);
		_pub = _nh.advertise<audio_msgs::AudioFeature>("audio_zero_crossing",1000);
		ros::spin();
	}
	~RosZeroCrossing(){}

	static void  chapterCallback(const audio_msgs::AudioData &msgs){
		audio_msgs::AudioFeature zero_rate;
		std::vector<int16_t> msg(msgs.data);
		msg.resize(msgs.data_size);
		zero_rate.feature = zerroRate(msg);
		_pub.publish(zero_rate);
	}

	/*
	 * 过零率采用矩型窗且帧移为0
	 * */
	static float zerroRate(std::vector<int16_t> &msgs){
		std::vector<int16_t>::iterator msg_iter = msgs.begin();
		float prev = 0.0;
		float count = 0.0;
		for(int i=0;i<msgs.size();i++){
			bool crossing = false;
			float sample = msg_iter[i];
			if(sample >= 0.0){
				if(prev <0.0) crossing = true;
			}else if(sample < 0.0){
				if(prev>=0.0) crossing = true;
			}
			if(crossing == true){
				count++;
			}
			prev = sample;
		}
		//放大20倍，不然太小不好分析
		return 20*count/(2*msgs.size());
	}


private:
	ros::NodeHandle _nh;
    ros::Subscriber _sb;
    static ros::Publisher _pub;

};
ros::Publisher RosZeroCrossing::_pub;

int main (int argc, char **argv)
{
  ROS_INFO ("Ros Node Name : audio_zero_crossing");
  ROS_INFO ("Ros Node Subscribe : audio_pre_emphasis");
  ROS_INFO ("Ros Node Publish Topic : audio_zero_crossing");
  ros::init(argc, argv, "audio_zero_crossing");
  RosZeroCrossing zerocrossing;
}



