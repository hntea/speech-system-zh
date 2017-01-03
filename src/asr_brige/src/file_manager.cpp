/*
 * file_manager.cpp
 *
 *  Created on: 2016年12月26日
 *      Author: hntea
 */

#include <ros/ros.h>
#include "audio_msgs/AudioData.h"
#include "std_msgs/String.h"
#include "aquila/global.h"
#include "aquila/source/WaveFile.h"
#include <fstream>
#include <list>
#include <vector>
#include <atomic>
#include <pthread.h>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

namespace Hntea{

/*
 * 文件管理者：
 * 功能：生成 pcm 文件 与 wav 文件
 * */

class FileManager{
public:
	FileManager(){

		//获取硬件配置。
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

		//获取工作空间
		std::string path = getDefaultWorkSpace();
		ros::param::param("~workspace",_workspace,path);
		//创建工作空间
		creatWorkspace(_workspace);
		_signal.setSampleFrequency(rate);


		_sb1 = _nh.subscribe("asr_brige/cache_data",50,dataCallback);
		_sb2 = _nh.subscribe("asr_brige/cache_state",50,stateCallback);
		_pub1 = _nh.advertise<std_msgs::String>("asr_brige/pcm_file",20);
		_pub2 = _nh.advertise<std_msgs::String>("asr_brige/wav_file",20);
		ros::spin();
	}
	~FileManager(){

	}

	static void dataCallback(const audio_msgs::AudioData &msgs){
		//将数据缓存在 vector 中，最后再写入
		if(_start){
			std::vector<int16_t> src(msgs.data);
			for(int i=0;i<src.size();i++){
				_64cache.push_back(src[i]);
				_16cache.push_back(src[i]);
			}
		}
		if(_end){
			//写入文件
			allocalFile(_workspace);
			Aquila::SignalSource signal(_64cache,_signal.getSampleFrequency());
			Aquila::WaveFile wav(_wavfile);
			wav.save(signal,_wavfile);
			writePcm(_pcmfile,_16cache);
			ROS_INFO("[FileManger]:[%d] created success!\n",_fileID-1);
			std_msgs::String pcm_msgs;
			pcm_msgs.data = _pcmfile.c_str();
			std_msgs::String wav_msgs;
			wav_msgs.data = _wavfile.c_str();

			_pub1.publish(pcm_msgs);
			_pub2.publish(wav_msgs);
			_64cache.clear();
			_16cache.clear();
			_start = false;
			_end = false;
		}
	}

	/*
	 * 问题：如果连续两个结束标志收到，会导致文件空！
	 *
	 * */
	static void  stateCallback(const std_msgs::String &msgs){
		if(std::strcmp(msgs.data.c_str(),"cache_start") == 0){
			_start = true;
		}else{
			_end = true;
		}
	}

	/*
	 * 函数功能：分配文件名
	 * 参数说明：
	 * 			workspace:工作目录
	 * 			max_id:缓存大小
	 * */
	static void allocalFile(std::string workspace,const int max_id = 10){

		std::string wav = ".wav";
		std::string pcm = ".pcm";
		std::stringstream s_wav;
		std::stringstream s_pcm;

		if(_fileID == max_id){
			_fileID = 0;
		}
		//这部分不是线程安全的。
		s_wav<<workspace<<_fileID<<wav;
		s_pcm<<workspace<<_fileID<<pcm;
		_wavfile = s_wav.str();
		_pcmfile = s_pcm.str();
		_fileID++;

//		char *wavbuf = new char[128];
//		char *pcmbuf = new char[128];
//		std::snprintf(wavbuf,16,"%s%d.wav",workspace.c_str(),_fileID);
//		std::snprintf(pcmbuf,16,"%s%d.wav",workspace.c_str(),_fileID);
//		if(_fileID == max_id){
//			_fileID = 0;
//		}
//		_wavfile = wavbuf;
//		_pcmfile = pcmbuf;
//		delete []wavbuf;
//		delete []pcmbuf;
//		_fileID++;
	}

	static void writePcm(std::string file,std::vector<int16_t>& src){
        std::ofstream fs;
		fs.open(file.c_str(), std::ios::out | std::ios::binary);
	    short* data = &src[0];
	    fs.write((const char*)data, src.size()*2);
	    fs.close();
	}

	static std::string getDefaultWorkSpace(){
		//获取home 路径
		struct passwd *pw = getpwuid(getuid());
		const char *homedir = pw->pw_dir;
		std::string path = homedir;
		path+="/.SpeechSystem/audio-file/";
		return path;
	}

	static void creatWorkspace(std::string path){
		int err;
		err = mkdir(_workspace.c_str(),0775);
		if(err == 0){
			printf("Create dir success!\n");
		}
		if(err == EEXIST){
			printf("dir already exit!\n");
		}
	}

private:
    ros::NodeHandle _nh;
    ros::Subscriber _sb1,_sb2;
    static bool _start,_end;
    static int _fileID;
    static std::string _wavfile,_pcmfile,_workspace;
    static std::vector<int16_t> _16cache;
    static std::vector<double> _64cache;
    static ros::Publisher _pub1,_pub2;

    static Aquila::SignalSource _signal;
};
bool FileManager::_start = false,
	 FileManager::_end = false;

int  FileManager::_fileID = 0;


std::string FileManager::_wavfile,
		    FileManager::_pcmfile,
			FileManager::_workspace;

std::vector<int16_t> FileManager::_16cache;
std::vector<double> FileManager::_64cache;

ros::Publisher FileManager::_pub1,
			   FileManager::_pub2;

Aquila::SignalSource FileManager::_signal;
}

int main (int argc, char **argv)
{
  ROS_INFO ("Ros Node Name : file_manager");
  ROS_INFO("Ros Node Publish Topic : asr_brige/pcm_file and asr_brige/wav_file");

  ros::init(argc, argv, "file_manager");
  Hntea::FileManager manager;
}


