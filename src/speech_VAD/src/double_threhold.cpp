/*
 * double_threhold.cpp
 *
 *  Created on: 2016年12月14日
 *      Author: hntea
 */
#include <ros/ros.h>
#include "audio_msgs/AudioData.h"
#include "audio_msgs/FreqData.h"
#include "audio_msgs/AudioFeature.h"
#include "std_msgs/String.h"
#include <vector>
#include <list>
#include <map>
#include <cmath>
#include <mutex>
#include <ros/timer.h>


/*
 * 能量：用于限制声场距离。
 * 过零：用于区分人声，不过对于清辅音没效果不太好。
 *
 * */
namespace Hntea{

static const float _IF = 120.0 ;	//检测清音帧的全局域值（基于对清音过零率的长时间统计）
static const float _ITU = 17.0;		//能量高保守阈值。历史平均加上 4～5


//存放历史均值最高与最低
typedef struct _NoiseAverage{
	float low;
	float up;
}NoiseAverage;


class RosDoubleThrehold{
public:
	RosDoubleThrehold(){
		int len = 0;

		ros::param::param<int>("~history_size",len,10);
		_zclist.resize(len);
		_eglist.resize(len);


		_sb = _nh.subscribe("audio_zero_crossing",50,zeroCall);
		_sb2 = _nh.subscribe("audio_energy",50,energyCall);
		_pub = _nh.advertise<std_msgs::String>("double_threshold_result",1000);

	}
	virtual ~RosDoubleThrehold(){

	}



	static void  energyCall(const audio_msgs::AudioFeature &msgs){
		//刷新缓冲数据-用队列方式实现循环buff
		_eglist.pop_front();
		_eglist.push_back(msgs.feature);
		_eg_flash = true;

	}

	static void  zeroCall(const audio_msgs::AudioFeature &msgs){
		//ROS_INFO_THROTTLE(60,"ZeroCrossing detector is ok!");
		_zclist.pop_front();
		_zclist.push_back(msgs.feature);
		_zc_flash = true;

	}

	/*
	 * 函数功能：根据历史信息计修正域值  _IZCT = std::max(_IF,avg - 10*sig);
	 * 参数说明：
	 * 		_IZCT：低清音阈值：语音过程：静音-清音-浊音-清音-静音
	 * 		_ITR : 低能量阈值：用于调节灵敏度
	 * 		avg：开始100ms的均值
	 * 		sig：标准差
	 *
	 * 		list：缓存历史数据的链表
	 * 		flag: 选择设置阈值
	 * */
	void setThreshold( std::list<float>& eglist, std::list<float>& zclist){
		std::list<float>::iterator eg_iter = eglist.begin();
		std::list<float>::iterator zc_iter = zclist.begin();
		float eg_sum = 0,eg_avg=0,zc_sum=0,zc_avg=0;
		int N = eglist.size();

		//求均值
		for(eg_iter,zc_iter;eg_iter != eglist.end();eg_iter++,zc_iter++){
			eg_avg += *eg_iter;
			zc_avg += *zc_iter;
		}
		eg_avg = eg_avg/N;
		zc_avg = zc_avg/N;

		//求方差
		for(eg_iter,zc_iter;eg_iter != eglist.end();eg_iter++,zc_iter++){
			eg_sum += (*eg_iter - eg_avg) * (*eg_iter - eg_avg);
			zc_sum += (*zc_iter - zc_avg) * (*zc_iter - zc_avg);
		}
		zc_sum = std::sqrt(zc_sum/N);
	    eg_sum = std::sqrt(eg_sum/N);

		_IZCT = std::max(_IF,zc_avg - 10*zc_sum);

		_ITR = std::max(_ITU-4,eg_avg+eg_sum);

//		ROS_INFO("\n<<<< Dynamic low threshold level >>>>");
//		ROS_INFO("_IZCT = %f ,_ITR = %f\n",_IZCT,_ITR);
	}



	/*
	 * 函数功能：前端点判决
	 * 策略：	 局部搜索，连续三帧的值大于低门限且最新帧大于高门限
	 * 参数说明：
	 * 			list: 局部缓存
	 * 			up : 高保守门限
	 * 			low : 低保守门限
	 * */
	bool isStartPoint( std::list<float> &eglist,const float eg_up,const float eg_low,
							  std::list<float> &zclist,const float zc_up,const float zc_low)
	{
		static bool is_start = false,eg_start = false,zc_start = false;
		static float eg_avg=0,zc_avg=0,id=0;
		std::list<float>::iterator eg_iter = eglist.end();
		std::list<float>::iterator zc_iter = zclist.end();

		//搜索能量集中区域:集中区域中有3帧超过即判定为有效能量场。
		if((*--eg_iter > eg_low) && _eg_flash){
			_eglow_over = true;
			_eg_flash = false;
		}
		if(_eglow_over){
			_eglow_over = false;

			if(*eg_iter > eg_up)
				_egover_id++;
			else
				_egover_id = 0;

			if(_egover_id >= 3){
				eg_start = true;
				//printf("Energy,Recording....\n");
			}
		}

//		//局部过零 连续三帧过零均值判决
		if(_zc_flash){
			_zc_flash = false;
			for(id=0;id<2;++id,--zc_iter)
			{
				zc_avg += *zc_iter;
			}
			zc_avg = zc_avg/id;

			if((zc_avg < zc_low) && (*--zclist.end() < zc_up)){
				zc_start = true;
				//printf("Zero crossing ,Recording....zc_avg = %f , new = %f\n",zc_avg,*--zclist.end());
			}
		}

//		printf("\n<<<<<<<< IS START POINT >>>>>>>>\n"
//				"eg_avg = %f,zc_avg = %f\n"
//				"eg_low = %f,zc_low = %f\n",
//				eg_avg,zc_avg,eg_low,zc_low);


		//门限判别
		if( eg_start && zc_start){
			is_start = true;
			zc_start = false;
			eg_start = false;
			printf("\n<<<<<<<< IS START POINT >>>>>>>>\n");
		}else{
			is_start = false;
		}

		return is_start;
	}



	/*
	 * 函数功能：后端点判决
	 * 参数说明：
	 * 			list：历史缓存
	 * 			up :  历史局部静音的平均最高值
	 * 			low : 历史局部静音的平均最低值
	 * 策略： 假设能量值与过零同时落入静音历史区间
	 * */
	bool isEndpoint( std::list<float>& eglist,
					const float eg_up,const float eg_low,
					std::list<float>& zclist,
					const float zc_up,const float zc_low)
	{

		float eg = 0 ,zc = 0, eg_average=0,zc_average = 0;
		bool is_endpoint = false ;


		//局部遍历计算缓存均值,每次移动10个缓存
		std::list<float>::iterator egiter = eglist.end();
		std::list<float>::iterator zciter = zclist.end();

		for(egiter,zciter;egiter != eglist.begin();--egiter,--zciter){
			 eg += *(egiter);
			 zc += *(zciter);
		}
		eg_average = eg/eglist.size();
		zc_average = zc/zclist.size();




//		printf("\n<<<<<<<< IS End POINT >>>>>>>>\n"
//				"eg_avg = %f,zc_avg = %f\n"
//				"eg_low = %f,zc_up = %f\n"
//				"eg_up = %f,zc_low = %f\n",
//				eg_average,zc_average,eg_low,zc_low,eg_up,zc_up);


		//静音判别,局部均值位于历史静音区间则判定为静音
		if(((eg_low < eg_average) && (eg_average<eg_up)) &&
				((zc_low < zc_average) && (zc_average<zc_up))){
				printf("\n================Ending===============\n");
				is_endpoint = true;
				//调整门限
				searchNoiseSection(eglist,zclist);

		}


		return is_endpoint;
	}
	/*
	 * 函数功能：局部遍历过零与能量噪声区间
	 * 参数说明：
	 * 			list：局部缓存
	 *
	 * */
	void searchNoiseSection( std::list<float>& eglist, std::list<float>& zclist){
		 _enoise.up = *(std::max_element(eglist.begin(),eglist.end()));
		 _enoise.low = *(std::min_element(eglist.begin(),eglist.end()));
		 _znoise.up = *(std::max_element(zclist.begin(),zclist.end()));
		 _znoise.low = *(std::min_element(zclist.begin(),zclist.end()));

		 printf("\n========== History Noise Leve =========\n"
				 "_znoise.up = %f    _znoise.low = %f\n"
				 "_enoise.up = %f     _enoise.low = %f\n",
				 _znoise.up,_znoise.low,_enoise.up,_enoise.low);
	}

	/*
	 * 函数功能：缓冲一段时间并设置噪声水平
	 * 启动时先让缓存装满
	 * */
	void noistLevelSet(){
		int count = 0;
		while(ros::ok()){
			if(_zc_flash){
				_zc_flash = false;
				usleep(500);
				count++;
			}
			if(count == 200){
				printf("OK! Ready to listen\n");
				break;
			}
			ros::spinOnce();
		}
		searchNoiseSection(_eglist,_zclist);
	}


	/*
	 *函数功能：端点检测
	 * */
	void runDetector(){

		noistLevelSet();

		static bool flash = false;
		while(ros::ok()){
			//前端检测
			if(isStartPoint(_eglist,_ITU,_ITR,_zclist,_IF,_IZCT) && !flash)
			{
				flash = true;
			}

			if (flash){
				if(isEndpoint(_eglist,_enoise.up,_enoise.low,
						_zclist,_znoise.up,_znoise.low)){
					flash = false;
				}
			}

			//调整低阈值
			setThreshold(_eglist,_zclist);
			ros::spinOnce();
		}
	}


private:
	ros::NodeHandle _nh;
    ros::Subscriber _sb,_sb2;
    NoiseAverage _enoise;
    NoiseAverage _znoise;


    static bool _eg_flash , _eglow_over,_zc_flash,_zrlow_over;
    static int _count,_egover_id,_zcover_id;
    static float _IZCT, _ITR;					//低保守阈值
    static std::list<float> _zclist,_eglist;		//数据缓存
    static ros::Publisher _pub;

};
ros::Publisher RosDoubleThrehold::_pub;


bool  RosDoubleThrehold::_eg_flash = false,
      RosDoubleThrehold::_eglow_over = false,
	  RosDoubleThrehold::_zc_flash = false,
	  RosDoubleThrehold::_zrlow_over = false;


float RosDoubleThrehold::_IZCT,
      RosDoubleThrehold::_ITR;

int RosDoubleThrehold::_count,
	RosDoubleThrehold::_egover_id,
	RosDoubleThrehold::_zcover_id;

std::list<float>  RosDoubleThrehold::_zclist,
				  RosDoubleThrehold::_eglist;



//std::mutex  RosDoubleThrehold::mt1,RosDoubleThrehold::mt2;
}

int main (int argc, char **argv)
{

  printf ("\nRos Node Name : double_threshold_VAD\n");
  printf ("Ros Node Subscribe : audio_energy and audio_zero_crossing\n");
  printf ("Ros Node Publish Topic : double_threhold_result\n\n");
  ros::init(argc, argv, "double_threshold_VAD");
  Hntea::RosDoubleThrehold factory;
  factory.runDetector();
}

