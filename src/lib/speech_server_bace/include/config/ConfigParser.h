/*
 * ConfigParser.h
 *
 *  Created on: 2016年11月18日
 *      Author: hntea
 */

#ifndef SRC_KINECT_SPEECH_SRC_CONFIG_CONFIGPARSER_H_
#define SRC_KINECT_SPEECH_SRC_CONFIG_CONFIGPARSER_H_

#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"
#include <vector>
#include <algorithm>
using namespace std;
using json = nlohmann::json;

typedef struct audioInfo{
	unsigned int rate;
	int  sample;
	int  channel;
}AudioInfo;

typedef struct audioDevice{
	string recorder;
	string player;
}AudioDevice;

typedef struct xunfeiServer{
	bool use;
	string  loginParams;
	string session_begin_params;
}XFei;

typedef struct xflocalasr{
	string engine_type;
	int sample_rate;
	string result_type;
	string result_encoding;
	string asr_res_path;
	string  grm_build_path;
	string bnf_file_path;
}XFeiLocalAsr;

typedef struct baiduServer{
	bool use;
	string    appid;
	string 	apikey;
	string		secretkey;
	string		asrURL;
	string 	ttsURL;
}BaiDu;

typedef struct tulingServer{
	bool use;
	string serverURL;
	string apikey;
}Tuling;

class ConfigParser {
public:
	ConfigParser();
	ConfigParser(string& file);
	virtual ~ConfigParser();
	void process();
	void prinrInfo();

	void setCfgFile(string file);
	string getRobotName();
	string getRecorder();
	string getPlayer();
	unsigned int getAudioRate();
	int  getAudioSample();
	int  getAudioChannel();
	string getXFLoginParams();
	string getXFonlineSessionBeginParams();
	string getUserWordFile();

	string getLocalAsrResPath();
	string getLocalAsrEngineType();
	string getLocalAsrGrmBuildPath();
	string getLocalAsrResultType();
	string getLocalAsrResultEncoding();
	int getLocalAsrSampleRate();
	string getLocalAsrBNFfile();


	vector<string> getLocalCMD();
private:
	string  _config_file;
	string 	_user_words_file;
	string _robot_name;
	AudioDevice _device;
	AudioInfo  _info;
	XFei	_xfei_cfg;
	XFeiLocalAsr _localasr;
	BaiDu _baidu_cfg;
	Tuling _tuling_cfg;
//	vector<string> _offline_cmd;
};

#endif /* SRC_KINECT_SPEECH_SRC_CONFIG_CONFIGPARSER_H_ */
