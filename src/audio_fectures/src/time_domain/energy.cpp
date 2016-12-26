/*
 * energy.cpp
 *
 *  Created on: 2016年12月10日
 *      Author: hntea
 *
 *
 *  将原先的对数能量文件更改为 energy.cpp
 *  订阅窗口节点
 */


#include <ros/ros.h>
#include "audio_msgs/AudioData.h"
#include "audio_msgs/AudioFeature.h"
#include <vector>


namespace  Hntea{
class RosEnergy{
public:
	RosEnergy(){

		ros::param::param<std::string>("~window",_subname,"hamming_window");
		_sb = _nh.subscribe(_subname,50,chapterCallback);
		_pub = _nh.advertise<audio_msgs::AudioFeature>("audio_energy",1000);
		ros::spin();
	}
	~RosEnergy(){}


	static void  chapterCallback(const audio_msgs::AudioData &msgs){
		ROS_INFO_THROTTLE(60, "Audio [audio_energy], everything is OK ! ");

		audio_msgs::AudioFeature energy;

		std::vector<int16_t> src(msgs.data);
		std::vector<int16_t>::iterator src_iter = src.begin();

		energy.feature = logEnergy(src);
		_pub.publish(energy);
	}


	static float logEnergy(std::vector<int16_t> &src){
		float energy = 0.0;
		std::vector<int16_t>::iterator src_iter = src.begin();

	    for(int j=0;j<src.size();j++){
	    	energy += ( ( src_iter[j]*src_iter[j] ));
	    }

		return log(energy);
	}


private:
	ros::NodeHandle _nh;
    ros::Subscriber _sb;
    std::string _subname;
    static ros::Publisher _pub;

};

ros::Publisher RosEnergy::_pub;
}
int main (int argc, char **argv)
{
  ROS_INFO ("Ros Node Name : audio_energy");
  ROS_INFO ("Ros Node Subscribe : audio_window");
  ROS_INFO ("Ros Node Publish Topic : audio_energy");
  ros::init(argc, argv, "audio_energy");
  Hntea::RosEnergy energy;
}

