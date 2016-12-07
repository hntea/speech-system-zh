/*
 * audio_data_plot.cpp
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
		//接收到的消息以16kHz发布
		_sb = _nh.subscribe("audio_data",50,chapterCallback);
		_pub = _nh.advertise<audio_msgs::PlotData>("audio_data_plot",3200);
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
//			_loop_rate.sleep();
			usleep(62.5);
		}
	}

private:
	ros::NodeHandle _nh;
    ros::Subscriber _sb;
  //  static ros::Rate _loop_rate;
    static ros::Publisher _pub;
};
ros::Publisher RosAudioPlot::_pub;
//ros::Rate RosAudioPlot::_loop_rate(16000);

int main (int argc, char **argv)
{
  ROS_INFO ("Ros Node Name : audio_data_plot");
  ROS_INFO ("Ros Node Subscribe : audio_data");
  ROS_INFO ("Ros Node Publish Topic : audio_data_plot");

  ros::init(argc, argv, "audio_data_plot");
  RosAudioPlot plot;
}
