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
#include <sys/time.h>
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
		_pub2 = _nh.advertise<std_msgs::String>("asr_brige/cache_state",10);
		ros::spin();
	}
	~CacheManger(){

	}



	static void  audioDataCallback(const audio_msgs::AudioData &msgs){
		std::vector<int16_t> vec(msgs.data);
		audio_msgs::AudioData delay_msgs;
		delay_msgs.data.resize(msgs.data_size);
		delay_msgs.data_size = msgs.data_size;

		createDelayMessage(delay_msgs,vec);
		if(_start){
			pubStateMessage("cache_start");
			_start = false;
		}
		if(_end){
			_end_setp += msgs.data_size;
			if(_end_setp >= _lcache.size()){
				pubStateMessage("cache_end");
				_end = false;
				_end_setp = 0;
			}
		}

		_pub1.publish(delay_msgs);
	}
	/*
	 * 端点检测回调
	 * 注意：
	 * 		如果两个消息回调频率过快:如抖动
	 * 		则认为是误判,连续上次状态
	 * */
	static void  vadCallback(const std_msgs::String &msgs){
		if(std::strcmp(msgs.data.c_str(),"start") == 0){
			if(isTooFast(500))
				ROS_INFO("==VAD Translate too false! Noise==");
			else
				_start = true;
		}
		else{
			_end = true;
			flashTime();
		}
	}

	/*
	 * 函数功能：发布状态消息
	 * 参数说明：
	 * 			flag: start or end
	 * */
	static void pubStateMessage(std::string flag){
		std_msgs::String msgs;
		msgs.data = flag;
		ROS_INFO("[CacheManger]:%s",flag.c_str());
		_pub2.publish(msgs);
	}

	static void createDelayMessage(audio_msgs::AudioData& delay_msgs,std::vector<int16_t>& vec){
		//发送缓存
		std::list<int16_t>::iterator liter;
		//构造延迟消息-取链表头数据
		for(int i=0;i<delay_msgs.data_size;i++){
			std::list<int16_t>::iterator liter = _lcache.begin();
			delay_msgs.data[i] = *liter;
			_lcache.pop_front();
			_lcache.push_back(vec[i]);
		}
	}



	/*
	 * 判决两个端点出现的时间频率依据
	 * 前一段结束时间与后一段结束时间
	 * 刷新历史时间在后端点出现之后
	 * */
	static void flashTime(){
		timeval tv;
		gettimeofday(&tv, 0);
		_psec = tv.tv_sec;
		_pus = tv.tv_usec;
	}



	/*
	 * 回调频率过快判定
	 * */
	static bool isTooFast(uint64_t ms){
//		ROS_INFO("Pre time:[%ld]",_psec);
		bool isfast;
		timeval tv;
		gettimeofday(&tv, 0);

		uint64_t subs = tv.tv_sec - _psec;
		uint64_t cur_us = tv.tv_usec;
		uint64_t sub_us = subs*1000*1000+cur_us - _pus;
		ROS_INFO("sub_ms time:[%ld]",sub_us/1000);

		if (sub_us<(ms*1000)){
			isfast = true;
		}else{
			isfast = false;
		}
//		ROS_INFO("Cur time:[%ld]",_psec);
		return isfast;
	}


private:
    ros::NodeHandle _nh;
    ros::Subscriber _sb1,_sb2;

    std::string _sub_name1,_sub_name2;
    int _cache_time;
    static uint64_t _psec,_pus;	//前一次进入回调的时间
    static bool _start,_end;
    static float _end_setp;
    static ros::Publisher _pub1,_pub2;
    static std::list<int16_t> _lcache;
};

bool CacheManger::_end = false,
	 CacheManger::_start = false;

float  CacheManger::_end_setp = 0;

uint64_t CacheManger::_psec = 0,
		CacheManger::_pus = 0;

ros::Publisher  CacheManger::_pub1,
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



