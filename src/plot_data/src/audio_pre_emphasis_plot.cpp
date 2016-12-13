/*
 * audio_pre_emphasis_plot.cpp
 *
 *  Created on: 2016年12月7日
 *      Author: hntea
 */
#include <ros/ros.h>
#include "audio_msgs/AudioData.h"
#include "audio_msgs/PlotData.h"
#include <vector>
class RosAudioPlot{
public:
	RosAudioPlot(){
		_sb = _nh.subscribe("audio_pre_emphasis",50,chapterCallback);
		_pub = _nh.advertise<audio_msgs::PlotData>("audio_pre_emphasis_plot",5120);
		ros::spin();
	}
	~RosAudioPlot(){}

	static void  chapterCallback(const audio_msgs::AudioData &msgs){
		std::vector<int16_t> vec(msgs.data);
		vec.resize(msgs.data_size);
		std::vector<int16_t>::iterator iter = vec.begin();

		audio_msgs::PlotData plot;
		for(int i=0;i<msgs.data_size;i++){
			plot.src_data = iter[i];
			_pub.publish(plot);
			usleep(62.5);		//接收到的消息以16kHz发布
		}
	}

private:
	ros::NodeHandle _nh;
    ros::Subscriber _sb;
    static ros::Publisher _pub;
};
ros::Publisher RosAudioPlot::_pub;


int main (int argc, char **argv)
{
  ROS_INFO ("Ros Node Name : audio_pre_emphasis_plot");
  ROS_INFO ("Ros Node Subscribe : audio_pre_emphasis");
  ROS_INFO ("Ros Node Publish Topic : audio_pre_emphasis_plot");

  ros::init(argc, argv, "audio_pre_emphasis_plot");
  RosAudioPlot plot;
}



