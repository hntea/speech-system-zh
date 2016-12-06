/*
 * audio_pre_emphasis.cpp
 *
 *  Created on: 2016年12月5日
 *      Author: hntea
 */

#include <ros/ros.h>
#include <audio_msgs/AudioData.h>
#include <vector>
class RosAudioPreEmphasis{
public:
	RosAudioPreEmphasis(){
        ros::param::param<float>("~u", _u, 0.9378);
		_sb = _nh.subscribe("audio_data",50,chapterCallback);
		_pub = _nh.advertise<audio_msgs::AudioData>("audio_pre_emphasis",50);
		ros::spin();
	}
	~RosAudioPreEmphasis(){

	}
	static void  chapterCallback(const audio_msgs::AudioData &msgs){
		ROS_INFO_THROTTLE(60, "Audio pre_emphasis, everything is OK ! ");
		audio_msgs::AudioData des;
		des.data_size = msgs.data_size;
		des.data.resize(des.data_size);
		preEmphasis(des,msgs,_u);
		_pub.publish(des);

	}

	/*
	 * 函数功能：一阶滤波器 H(Z) = 1-uZ^{-1}
	 * 离散卷积响应表示：
	 * 			y[n] = x[n] - ux[n-1]
	 * */
	static void preEmphasis(audio_msgs::AudioData &des,const audio_msgs::AudioData &src,float u=_u){
		for(int i=0;i<des.data_size;i++){
			if(i==0){
				des.data[i] = src.data[i];
			}else{
				des.data[i] = src.data[i] - src.data[i-1] * u;
			}
		}
	}
private:
    ros::NodeHandle _nh;
    ros::Subscriber _sb;
    static ros::Publisher _pub;
    static float _u;
};
ros::Publisher RosAudioPreEmphasis::_pub;
float RosAudioPreEmphasis::_u = 0.9378;

int main (int argc, char **argv)
{
  ROS_INFO ("Ros Node Name : audio_pre_emphasis");
  ROS_INFO ("Ros Node Subscriber : audio_data");
  ROS_INFO ("Ros Node Publish Topic : audio_pre_emphasis");
  ros::init(argc, argv, "audio_pre_emphasis");
  RosAudioPreEmphasis emphises;
}



