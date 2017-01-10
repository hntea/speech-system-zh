/*
 * xf_local_asr.cpp
 *
 *  Created on: 2016年12月29日
 *      Author: hntea
 */


#include "../../lib/xunfei/lib/ConfigResolver.h"
#include "../../lib/xunfei/lib/XFlocalasr.h"
#include "../../lib/xunfei/lib/XFonlineasr.h"
#include <unistd.h>
#include <ros/ros.h>
#include "audio_msgs/AudioData.h"
#include "std_msgs/String.h"
#include <string>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

Hntea::XFlocalasr local;
ros::Publisher pub;
bool start = false;
bool end = false;

/*
 * 端点状态
 * */
void  stateCallback(const std_msgs::String &msgs){
	if(!strcmp(msgs.data.c_str(),"cache_start")){
		start = true;
	}else{
		end = true;
	}
}

/*
 * 使用流识别回调
 * */
void streamCallback(const audio_msgs::AudioData& msg)
{
	std::vector<int16_t> vec(msg.data);
	std::string result;
	if(end){
		local.runasr(vec,result,true);
		if(result.empty()){
			std_msgs::String msgs;
			msgs.data = "Null";
			pub.publish(msgs);
		}else{
			std_msgs::String msgs;
			msgs.data = result;
			pub.publish(msgs);
		}
		start = false;
		end = false;
	}
	if(start){
		local.runasr(vec,result,false);
	}
}



/*
 * 获取默认配置文件
 * */
std::string defaultFile(){
	//获取home 路径
	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;
	std::string path = homedir;
	path+="/.SpeechSystem/config/config.json";
	return path;
}


int main(int argc, char **argv)
{
	ros::init(argc, argv, "xf_localasr_stream");


	// 依照配置文件配置服务
	std::string configfile;
	ros::param::param<std::string>("~configfile",configfile,defaultFile());
	Hntea::ConfigResolver parser(configfile);
	local = Hntea::XFlocalasr(parser.getXfLocalParams(),parser.getXfBasic());


	ros::NodeHandle n;
	ros::Subscriber sub1 = n.subscribe("asr_brige/cache_state", 1000, stateCallback);
	ros::Subscriber sub2 = n.subscribe("asr_brige/cache_data", 1000, streamCallback);
	pub = n.advertise<std_msgs::String>("asr_server/xf/local_s_res",1000);
	ros::spin();
	return 0;
}


