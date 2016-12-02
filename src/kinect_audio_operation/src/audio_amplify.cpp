/*
 * audio_amolify.cpp
 *
 *  Created on: 2016年12月2日
 *      Author: hntea
 */

#include <ros/ros.h>
#include <audio_msgs/AudioData.h>
#include <vector>
#include <algorithm>

using namespace std;
class RosKinectAudio{
public:
	RosKinectAudio(){

		//配置节点
		_sb = _nh.subscribe("audio_data",50,chapterCallback);
		_pb = _nh.advertise<audio_msgs::AudioData>("audio_kinect_data",50);
		//等待回调
		ros::spin();

	}
	~RosKinectAudio(){}

	/*
	 * 注意：回调函数一般使用静态
	 * 			静态成员函数只能访问静态数据成员
	 * */
	static void  chapterCallback(const audio_msgs::AudioData &msgs){
		vector<int16_t> vec(msgs.data);
		vector<int16_t>::iterator iter;
		iter = std::max_element(vec.begin(),vec.end());
		int16_t max = *iter;
		iter = std::min_element(vec.begin(),vec.end());
		int16_t min = *iter;

		iter = vec.begin();
		audio_msgs::AudioData ampmsg;
		ampmsg.data.resize(msgs.data_size);
		ampmsg.data_size = msgs.data_size;
		for(int i=0; i<vec.size();i++){
			amplify(*iter,max,min);
			ampmsg.data[i] = *iter;
			iter++;
		}
		_pb.publish(ampmsg);

	}

	/*
	 * 函数功能：数据放大
	 * 策略：最大值、最小按原来数值放大10倍；如果上下限度在放大后还在 +-35000 之间，则所有数据全部放大5倍
	 * 并以此作为上下限
	 * 区域内：放大
	 */
	static void amplify(int16_t& data, int max, int min,int factor=10,int uper=35000,int down=-35000){
		data = data*factor;
		if(data > uper){
			data = uper;
		}else if(data < down){
			data = down;
		}
	}

private:
    ros::NodeHandle _nh;
    ros::Subscriber _sb;
    static ros::Publisher _pb;
};
ros::Publisher RosKinectAudio::_pb;


int main (int argc, char **argv){
     ros::init(argc, argv, "audio_amplify");
	RosKinectAudio amplifyer;
	return 0;
}


