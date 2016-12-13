/*
 * power_spectrum.cpp
 *
 *  Created on: 2016年12月13日
 *      Author: hntea
 */


/*
 * fft_factory.cpp
 *
 *  Created on: 2016年12月9日
 *      Author: hntea
 */


#include <ros/ros.h>
#include "audio_msgs/AudioData.h"
#include "audio_msgs/AudioFeature.h"
#include "audio_msgs/FreqData.h"
#include <vector>

namespace Hntea{
/*
 * 需要定义一个循环缓冲区，每次只变换512字节。
 * */
class RosPowerSpectrum{
public:
	RosPowerSpectrum(){

		_sb = _nh.subscribe("fft_result",50,chapterCallback);
		_pub = _nh.advertise<audio_msgs::TimeFeature>("audio_power",1000);
		ros::spin();
	}
	virtual ~RosPowerSpectrum(){

	}

	static void  chapterCallback(const audio_msgs::FreqData &msgs){


	}

private:
	ros::NodeHandle _nh;
    ros::Subscriber _sb;
    static ros::Publisher _pub;

};
ros::Publisher RosPowerSpectrum::_pub;

}
int main (int argc, char **argv)
{
  ROS_INFO ("Ros Node Name : audio_fft_factory");
  ROS_INFO ("Ros Node Subscribe : audio_pre_emphasis");
  ROS_INFO ("Ros Node Publish Topic : fft_result");
  ros::init(argc, argv, "audio_power");

}


