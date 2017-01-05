/*
 * xf_onlineasr_file.cpp
 *
 *  Created on: 2017年1月3日
 *      Author: hntea
 */


#include "../lib/xfparams.h"
#include "../lib/ASR.h"
#include "../lib/ConfigResolver.h"
#include "../lib/XFlocalasr.h"
#include "../lib/XFonlineasr.h"
#include <unistd.h>
#include <ros/ros.h>
#include "audio_msgs/AudioData.h"
#include "std_msgs/String.h"
#include <string>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

Hntea::XFonlineasr online;
ros::Publisher p;
bool start = false;
bool end = false;

void  stateCallback(const std_msgs::String &msgs){
	if(!strcmp(msgs.data.c_str(),"cache_start")){
		start = true;
	}else{
		end = true;
	}
}

void fileCallback(const std_msgs::String& msg){
	std::string online_res;
	online.runasr(msg.data,online_res);
	printf("[XFonlineASR-FILE] result = %s\n",online_res.c_str());
	std_msgs::String res;
	res.data = online_res;
	p.publish(res);
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
	ros::init(argc, argv, "xf_onlineasr_file");

	// 依照配置文件配置服务
	std::string configfile;
	ros::param::param<std::string>("~configfile",configfile,defaultFile());
	Hntea::ConfigResolver parser(configfile);
	int ret = MSPLogin(NULL, NULL,parser.getXfBasic().loginParams.c_str());
	if (MSP_SUCCESS != ret)
		std::cerr<<"MSPLogin failed , Error code:"<<ret<<std::endl;

	online = Hntea::XFonlineasr(parser.getXfOnlineParams());


	ros::NodeHandle n;
	ros::Subscriber sub1 = n.subscribe("asr_brige/cache_state", 1000, stateCallback);
	ros::Subscriber sub2 = n.subscribe("asr_brige/pcm_file", 10, fileCallback);
	p = n.advertise<std_msgs::String>("asr_server/xf/online_f_res",1000);
	ros::spin();
	return 0;
}


