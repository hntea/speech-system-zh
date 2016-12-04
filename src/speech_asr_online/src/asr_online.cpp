#include <ros/ros.h>
#include <std_msgs/String.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include "audio_msgs/AudioData.h"
#include "../../lib/speech_server_bace/include/xf_server/XunFeiServer.h"
#include "../../lib/speech_server_bace/include/config/ConfigParser.h"


std::string path;

class RosAsrOnline{
public:
	RosAsrOnline(){
		//获取配置信息
		ros::param::param("~cfgpath",_cfgfile,path);
		RosAsrOnline::asrInit(_cfgfile);
		//配置节点
		string hw;
		ros::param::get("/audio_capture/name",hw);

		if(hw == "plughw:3,0"){
			cout<<"Subscribe from [ audio_kinect_data ]      Using kinect2.0 [ plughw:3,0] !"<<endl;
			_sb = _nh.subscribe("audio_kinect_data",50,chapterCallback);
		}else{
			_sb = _nh.subscribe("audio_data",50,chapterCallback);
		}

		_pb = _nh.advertise<std_msgs::String>("asr_online_result",10);
		//等待回调
		ros::spin();

	}
	~RosAsrOnline(){}

	/*
	 * 注意：回调函数一般使用静态
	 * 			静态成员函数只能访问静态数据成员
	 * */
	static void  chapterCallback(const audio_msgs::AudioData &msgs){
		string result ;
		ROS_INFO_THROTTLE(60, "Speech to Text : XunFei Server runing!");
	    _server.runOnlineAsr(&msgs.data[0],msgs.data_size);
	    _server.getOnlineAsrResult(result);
	    if(!result.empty()){
	    	printf("ASR Result [ %s ]  Publish\n",result.c_str());
	    	std_msgs::String msg;
			msg.data= result.c_str();
	    	_pb.publish(msg);
	    }
	}

	/*
	 * 使用静态成员函数管理静态成员
	 * 注意: 使用前必须在类外初始化静态成员
	 * */
	static void asrInit(std::string path){
		RosAsrOnline::_cfg = ConfigParser(path);
		RosAsrOnline::_cfg.process();
		//初始化在线服务
		RosAsrOnline::_server.init(_cfg,false);
	}


private:
	ros::NodeHandle _nh;
	static ros::Publisher		_pb;
	ros::Subscriber 	_sb;
	std::string 					_cfgfile;
	static ConfigParser _cfg;
	static XunFeiServer _server;

};
ConfigParser RosAsrOnline::_cfg;
XunFeiServer RosAsrOnline::_server;
ros::Publisher	RosAsrOnline::_pb;

int main (int argc, char **argv){
	  ros::init(argc, argv, "asr_online");

	  printf("\n=============================================================\n"
			      "                       ROS Topic: asr_online                               \n"
			      "                       ROS Sub : audio_data\audio_kinect_data                               \n"
			      "                       ROS Pub : asr_online_result                       "
			      "\n=============================================================\n");

	  //获取home 路径
	  struct passwd *pw = getpwuid(getuid());
	  const char *homedir = pw->pw_dir;

	  path = homedir;
	  path+="/.SpeechSystem/config/config.json";

	  RosAsrOnline asr_online;
	return 0;
}
