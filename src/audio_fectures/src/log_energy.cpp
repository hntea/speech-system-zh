/*
 * log_energy.cpp
 *
 *  Created on: 2016年12月6日
 *      Author: hntea
 */

#include <ros/ros.h>
#include <audio_msgs/AudioData.h>
#include <audio_msgs/TimeFeature.h>
#include <vector>
#include <aquila/source/window/HammingWindow.h>

class RosLogEnergy{
public:
	RosLogEnergy(){
		//初始化窗体
		RosLogEnergy::_win_len = 320;	//默认窗口长度为 320 = 20ms
		HammingWindow(_win_len);
		_hanming_iter = _hanming_win_data.begin();

		_sb = _nh.subscribe("audio_pre_emphasis",50,chapterCallback);
		_pub = _nh.advertise<audio_msgs::TimeFeature>("audio_log_energy",1000);
		ros::spin();
	}
	~RosLogEnergy(){}


	static void  chapterCallback(const audio_msgs::AudioData &msgs){
		ROS_INFO_THROTTLE(60, "Audio [audio_log_energy], everything is OK ! ");

		std::vector<int16_t> src(msgs.data);
		std::vector<int16_t>::iterator iter = src.begin();

		audio_msgs::TimeFeature energy;
	    //分割数据
	    for(int i=0;i<(msgs.data_size/320);i++){
	    	for(int j=0;j<_win_len;j++){
	    		 energy.feature += ( (iter[j+_win_len*i]*iter[j+_win_len*i]) * (_hanming_iter[j]) );
	    	}
	    	energy.feature = log(energy.feature);
//	    	std::cout<<"log(energy) = "<<log(energy.feature)<<std::endl;
	    	_pub.publish(energy);
	    	energy.feature=0;
	    }
	}

	static void HammingWindow(std::size_t size){
		_hanming_win_data.reserve(size);
		for (std::size_t n = 0; n < size; ++n)
		{
			_hanming_win_data.push_back(
				0.53836 - 0.46164 * std::cos(2.0 * M_PI * n / double(size - 1))
			);
		}
	 }

private:
	ros::NodeHandle _nh;
    ros::Subscriber _sb;
    static ros::Publisher _pub;
    static std::size_t _win_len;
    static std::vector<double> _hanming_win_data;
    static std::vector<double>::iterator _hanming_iter;

};
ros::Publisher RosLogEnergy::_pub;
std::size_t RosLogEnergy::_win_len;
std::vector<double> RosLogEnergy::_hanming_win_data;
std::vector<double>::iterator RosLogEnergy::_hanming_iter;

int main (int argc, char **argv)
{
  ROS_INFO ("Ros Node Name : audio_log_energy");
  ROS_INFO ("Ros Node Subscribe : audio_pre_emphasis");
  ROS_INFO ("Ros Node Publish Topic : audio_log_energy");
  ros::init(argc, argv, "audio_log_energy");
  RosLogEnergy energy;
}

