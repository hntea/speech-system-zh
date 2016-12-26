/*
 * audio_cache_manager.cpp
 *
 *  Created on: 2016年12月22日
 *      Author: hntea
 *
 *
 * 文件功能：延迟发送原始音频数据、提前与延后端点检测状态
 *
 */

#include <ros/ros.h>
#include "audio_msgs/AudioData.h"
#include "std_msgs/String.h"
#include <fstream>
#include <list>
#include <vector>
namespace Hntea{

class CacheManger{
public:
	CacheManger(){
		//获取硬件配置-初始化时间缓存。
		float rate;
		float depth;
		float channel;
		if(!ros::param::get("/audio_capture/sample_rate",rate)){
			rate = 16000;
		}
		if(!ros::param::get("/audio_capture/depth",depth)){
			depth = 16;
		}
		if(!ros::param::get("/audio_capture/channels",channel)){
			channel = 1;
		}

		//初始化变量
		ros::param::param<int>("~delay_ms",_cache_time,200);
		ros::param::param<std::string>("~signal_subscriber",_sub_name1,"audio_data");
		ros::param::param<std::string>("~vad_subscriber",_sub_name2,"double_threshold_result");

		//开辟缓存空间
		float cache_size = _cache_time * rate/1000;
		_lcache.resize(cache_size);

		_sb1 = _nh.subscribe(_sub_name1,50,audioDataCallback);
		_sb2 = _nh.subscribe(_sub_name2,50,vadCallback);
		_pub1 = _nh.advertise<audio_msgs::AudioData>("asr_brige/cache_data",1000);
		_pub2 = _nh.advertise<std_msgs::String>("asr_brige/cache_state",20);
		ros::spin();
	}
	~CacheManger(){

	}



	static void  audioDataCallback(const audio_msgs::AudioData &msgs){
		ROS_INFO_THROTTLE(60,"Cache manager,everything is ok!");
		std::vector<int16_t> vec(msgs.data);
		audio_msgs::AudioData delay_msgs;
		delay_msgs.data.resize(msgs.data_size);
		delay_msgs.data_size = msgs.data_size;
		//发送缓存
		std::list<int16_t>::iterator liter;
		//构造延迟消息-取链表头数据
		for(int i=0;i<delay_msgs.data_size;i++){
			//拿头
			std::list<int16_t>::iterator liter = _lcache.begin();
			delay_msgs.data[i] = *liter;
			//去头
			_lcache.pop_front();
			//尾部更新
			_lcache.push_back(vec[i]);
		}
		//在接到后端点状态后，延时发送状态
		if(_end_state){
			_end_setp+=msgs.data_size;
			if(_end_setp >= _lcache.size()){
				std_msgs::String end;
				end.data = "cache_end";
				_pub2.publish(end);
				_end_setp = 0;
				_end_state = false;
			}
		}
		_pub1.publish(delay_msgs);

	}

	static void  vadCallback(const std_msgs::String &msgs){
		if(std::strcmp(msgs.data.c_str(),"start") == 0){
			//发送起始信号
			std_msgs::String start;
			start.data = "cache_start";
			_pub2.publish(start);
		}else{
			_end_state = true;
		}
	}

private:
    ros::NodeHandle _nh;
    ros::Subscriber _sb1,_sb2;

    std::string _sub_name1,_sub_name2;
    int _cache_time;
    static bool _end_state;
    static float _end_setp;
    static ros::Publisher _pub1,_pub2;
    static std::list<int16_t> _lcache;
};
bool CacheManger::_end_state = false;
float  CacheManger::_end_setp = 0;
ros::Publisher CacheManger::_pub1,
				CacheManger::_pub2;
std::list<int16_t> CacheManger::_lcache;
}

int main (int argc, char **argv)
{
  ROS_INFO ("Ros Node Name : cache_manager");
  ROS_INFO("Ros Node Publish Topic : asr_brige/cache_state and asr_brige/cache_data");

  ros::init(argc, argv, "cache_manager");
  Hntea::CacheManger manager;
}



