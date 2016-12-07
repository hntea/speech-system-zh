/*
 * log_energy.cpp
 *
 *  Created on: 2016年12月6日
 *      Author: hntea
 */

#include <ros/ros.h>
#include "audio_msgs/AudioData.h"
#include "audio_msgs/TimeFeature.h"
#include <vector>
#include <aquila/source/window/HammingWindow.h>

class RosLogEnergy{
public:
	RosLogEnergy(){
		//初始化窗体
		RosLogEnergy::_win_len = 320;				//默认窗口长度为 320 = 20ms
		_step_size = _win_len/2;
		_pbuf.resize(_step_size);
		_lbuf.resize(_step_size);

		HammingWindow(_win_len);
		_hanming_iter = _hanming_win_data.begin();

		_sb = _nh.subscribe("audio_pre_emphasis",50,chapterCallback);
		_pub = _nh.advertise<audio_msgs::TimeFeature>("audio_log_energy",1000);
		ros::spin();
	}
	~RosLogEnergy(){}


	static void  chapterCallback(const audio_msgs::AudioData &msgs){
		ROS_INFO_THROTTLE(60, "Audio [audio_log_energy], everything is OK ! ");

		audio_msgs::TimeFeature energy;

		std::vector<int16_t> src(msgs.data);
		std::vector<int16_t>::iterator src_iter = src.begin();
		std::vector<int16_t>::iterator p_iter = _pbuf.begin();
		std::vector<int16_t>::iterator l_iter = _lbuf.begin();

		//取缓存帧数据与下一块的前10ms数据
		std::vector<int16_t> merge;
		merge.resize(msgs.data_size);

		//取源数据前半部填充移动船的后半部
		for(int i=0;i<_step_size;i++){
			l_iter[i] = src_iter[i];
		}

		//链接生成新容器
		newBoard(_pbuf,_lbuf,merge);

		//计算能量并发布能量
		energy.feature = logEnergy(merge,_hanming_win_data);
		_pub.publish(energy);


		//board与本次数据块完全重叠
		energy.feature = logEnergy(src,_hanming_win_data);
		_pub.publish(energy);

		//更新移动船前半部：将源数据后半部填充到船的前半部
		for(int i=0;i<_step_size;i++){
			p_iter[i] =  src_iter[i+_step_size];
		}


//		audio_msgs::TimeFeature energy;
//	    //分割数据
//	    for(int i=0;i<(msgs.data_size/320);i++){
//	    	for(int j=0;j<_win_len;j++){
//	    		 energy.feature += ( ( iter[j+_win_len*i]*iter[j+_win_len*i] ) * (_hanming_iter[j]) );
//	    	}
//	    	energy.feature = log(energy.feature);
//	    	_pub.publish(energy);
//	    	energy.feature=0;
//	    }
	}


	static float logEnergy(std::vector<int16_t> &src,std::vector<double> &window){
		float energy = 0.0;

		std::vector<int16_t>::iterator src_iter = src.begin();
		std::vector<double>::iterator win_iter = window.begin();

	    for(int j=0;j<_win_len;j++){
	    	energy += ( ( src_iter[j]*src_iter[j] ) * (window[j]) );
	    }

		return log(energy);
	}

	/*
	 * 将两个容器链接到一个容器中。
	 * */
	static void newBoard(std::vector<int16_t> &pbuf,std::vector<int16_t> &lbuf,std::vector<int16_t> &board){
		std::vector<int16_t>::iterator p_iter = pbuf.begin();
		std::vector<int16_t>::iterator l_iter = lbuf.begin();
		std::vector<int16_t>::iterator b_iter = board.begin();
		for(int i=0;i<_step_size;i++){
			b_iter[i] = p_iter[i];
			b_iter[i+_step_size] = l_iter[i];
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

    static uint64_t		_step_size;
    static std::vector<int16_t> _pbuf;
    static std::vector<int16_t> _lbuf;


    static std::vector<double> _hanming_win_data;
    static std::vector<double>::iterator _hanming_iter;


};

uint64_t   RosLogEnergy::_step_size ;
std::vector<int16_t> RosLogEnergy::_pbuf;
std::vector<int16_t> RosLogEnergy::_lbuf;
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

