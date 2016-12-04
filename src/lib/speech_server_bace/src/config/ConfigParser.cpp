/*
 * ConfigParser.cpp
 *
 *  Created on: 2016年11月18日
 *      Author: hntea
 */

#include "../../include/config/ConfigParser.h"

ConfigParser::ConfigParser(){}

ConfigParser::ConfigParser(string& file): _config_file(file){}


ConfigParser::~ConfigParser() {}
/*
 * 函数功能：读取配置文件信息
 * */
void  ConfigParser::process(){
//	const char* path =  _config_file.c_str();
	std::ifstream ifs(_config_file);
	if(!ifs)
	{
		cerr<<"Can not open "<<_config_file<<" !!!"<<endl;
		return;
	}
	json jload;
	ifs>>jload;
	ifs.close();

	_user_words_file = jload["userWordsFilePath"];
	_robot_name = jload["robotName"];

	bool usekinect =  jload["audioDevice"]["recorder"]["usekinect"];
	if(usekinect ){
//		cout<<"Use kinect audio device!" << endl;
		_device.recorder =	jload["audioDevice"]["recorder"]["kinect"];
	}else{
		_device.recorder = jload["audioDevice"]["recorder"]["logitech"];
	}
//	cout<<"_device.recorder is :"<<_device.recorder<<endl;
	_device.player = jload["audioDevice"]["player"];

	_info.channel = jload["audioInfo"]["channel"];
	_info.rate = jload["audioInfo"]["rate"];
	_info.sample = jload["audioInfo"]["sample"];



	bool usexfei =  jload["speechServer"]["xunfei"]["use"];
	if (usexfei){
		_xfei_cfg.loginParams = jload["speechServer"]["xunfei"]["loginParams"];
		_xfei_cfg.session_begin_params = jload["speechServer"]["xunfei"]["onlineasr"]["session_begin_params"];

		_localasr.asr_res_path = jload["speechServer"]["xunfei"]["localasr"][	"asr_res_path"];
		_localasr.engine_type  = jload["speechServer"]["xunfei"]["localasr"]["engine_type"];
		_localasr.grm_build_path = jload["speechServer"]["xunfei"]["localasr"]["grm_build_path"];
		_localasr.result_encoding = jload["speechServer"]["xunfei"]["localasr"]["result_encoding"];
		_localasr.sample_rate = jload["speechServer"]["xunfei"]["localasr"][	"sample_rate"];
		_localasr.result_type = jload["speechServer"]["xunfei"]["localasr"]["result_type"];
		_localasr.bnf_file_path = jload["speechServer"]["xunfei"]["localasr"][ "bnf_file_path" ];

	}else{
		_baidu_cfg.apikey =  jload["speechServer"]["baidu"]["apikey"];
		_baidu_cfg.appid = jload["speechServer"]["baidu"][ "appid" ];
		_baidu_cfg.secretkey =  jload["speechServer"]["baidu"][ "secretkey" ];
		_baidu_cfg.asrURL =  jload["speechServer"]["baidu"]["serverForASR"];
		_baidu_cfg.ttsURL	=  jload["speechServer"]["baidu"]["serverForTTS"];

		_tuling_cfg.apikey = jload["speechServer"]["tuling"]["apikey"];
		_tuling_cfg.serverURL = jload["speechServer"]["tuling"]["server"];
	}

//	// add an array that is stored as std::vector (using an initializer list)
//	auto tmp  = jload["offLineCmd"];
//	//将控制命令加入容器中。
//	for(int i=0;i< tmp.size();i++){
//		_offline_cmd.push_back(tmp[i]);
//	}


}
/*
 * 函数功能：打印配置参数
 * */
void ConfigParser::prinrInfo(){
	cout<<"audio config information is:  ";
	cout<<"channle = "<<getAudioChannel()<<"    "<<"rate = " << getAudioRate()<<"    "<<"sample = "<<getAudioSample()<<endl;
	cout<<"Recorder device is:  "<<getRecorder()<<endl;
	cout<<"Player device is:  "<<getPlayer()<<endl;
	cout<<"Robot name is:  "<<getRobotName()<<endl;
	cout<<"Xfei login params is:  "<<getXFLoginParams()<<endl;
	cout<<"user words path is : " << _user_words_file <<endl;

	cout<<"local_asr_rec_path: " <<_localasr.asr_res_path <<endl;
	cout<<"_localasr.engine_type = "<<_localasr.engine_type<<endl;
	cout<<"_localasr.grm_build_path = "<<_localasr.grm_build_path<<endl;
	cout<<"_localasr.result_encoding = "<<_localasr.result_encoding<<endl;
	cout<<"_localasr.result_type = "<<_localasr.result_type<<endl;
	cout<<"_localasr.sample_rate = "<<_localasr.sample_rate<<endl;
}

void  ConfigParser::setCfgFile(string file){
	_config_file = file;
}
string  ConfigParser::getRobotName(){
	return _robot_name;
}
string  ConfigParser::getRecorder(){
	return this->_device.recorder;
}
string  ConfigParser::getPlayer(){
	return this->_device.player;
}
unsigned int  ConfigParser::getAudioRate(){
	return this->_info.rate;
}
int   ConfigParser::getAudioSample(){
	return this->_info.sample;
}
int   ConfigParser::getAudioChannel(){
	return this->_info.channel;
}
string  ConfigParser::getXFLoginParams(){
	return this->_xfei_cfg.loginParams;
}
string  ConfigParser::getXFonlineSessionBeginParams(){
	return this->_xfei_cfg.session_begin_params;
}

string  ConfigParser::getUserWordFile(){
	return this->_user_words_file;
}
//vector<string>  ConfigParser::getLocalCMD(){
//	return this->_offline_cmd;
//}

string  ConfigParser::getLocalAsrResPath(){
	return _localasr.asr_res_path;
}
string  ConfigParser::getLocalAsrEngineType(){
	return _localasr.engine_type;
}
string  ConfigParser::getLocalAsrGrmBuildPath(){
	return _localasr.grm_build_path;
}
string  ConfigParser::getLocalAsrResultType(){
	return _localasr.result_type;
}
string  ConfigParser::getLocalAsrResultEncoding(){
	return _localasr.result_encoding;
}
int  ConfigParser::getLocalAsrSampleRate(){
	return _localasr.sample_rate;
}
string ConfigParser::getLocalAsrBNFfile(){
	return _localasr.bnf_file_path;
}
