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
		_pub = _nh.advertise<audio_msgs::AudioFeature>("audio_power",1000);
		ros::spin();
	}
	virtual ~RosPowerSpectrum(){

	}

	static void  chapterCallback(const audio_msgs::FreqData &msgs){
		std::vector<double> image(msgs.image);
		std::vector<double> real(msgs.real);
		audio_msgs::AudioFeature power;

		/*每个点的功率强度占用 1/N * Fs 的带宽，处理时可以使用前N/2，因为是对称的*/
		for(int i=0;i<image.size();i++){
			power.feature = 20*log(std::pow(image[i],2)+std::pow(real[i],2))/(image.size()/2);
			usleep(62.5);
			_pub.publish(power);
		}

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
  ROS_INFO ("Ros Node Name : audio_power_spectrum");
  ROS_INFO ("Ros Node Subscribe : fft_result");
  ROS_INFO ("Ros Node Publish Topic : audio_power");
  ros::init(argc, argv, "audio_power_spectrum");
  Hntea::RosPowerSpectrum power;

}


