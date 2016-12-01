/*
 * XunFeiServer.h
 *
 *  Created on: 2016年11月17日
 *      Author: hntea
 */

#ifndef SRC_KINECT_SPEECH_SRC_XUNFEI_ASR_XUNFEISERVER_H_
#define SRC_KINECT_SPEECH_SRC_XUNFEI_ASR_XUNFEISERVER_H_


#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <cstdio>
#include <unistd.h>

#include <iostream>
#include <vector>
#include <list>
#include <queue>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include "../../include/config/ConfigParser.h"
/*
 * 讯飞SDK头文件
 * */
#include "qisr.h"
#include "msp_cmn.h"
#include "msp_errors.h"

using namespace std;

#define	RESULT_BUFFER_SIZE	4096*2
#define AUDIO_FRAME_LEN	640
#define USER_HINTS_SIZE  100				//自定义存放错误状态缓存大小

#define SAMPLE_RATE_16K     (16000)
#define SAMPLE_RATE_8K    		  (8000)
#define MAX_GRAMMARID_LEN   (32)
#define MAX_PARAMS_LEN      (1024)




typedef struct asrState{
	int 				aud_stat ;				//音频状态-连续识别
	int 				ep_stat ;					//端点检测
	int 				rec_stat ;					//识别状态
	int 				errcode ;
}AsrState;

//离线语音识别
typedef struct _UserData {
	int     	build_fini; 													//标识语法构建是否完成
	int     	update_fini;							 					//标识更新词典是否完成
	int     	errcode; 														//记录语法构建或更新词典回调错误码
	char    grammar_id[MAX_GRAMMARID_LEN];		 //保存语法构建返回的语法ID
}UserData;


class XunFeiServer {
public:

	XunFeiServer();
	XunFeiServer(ConfigParser &cfg,bool );
	virtual ~XunFeiServer();

	void init(ConfigParser &cfg,bool);

	/*****************************************
	 *********** 离线命令词识别函数************
	 * ***************************************/

	//构建离线识别语法网络
	static  int buildGrammarCallback(int ecode, const char *info, void *udata);
	static int buildGrammar();
	void runLocalAsr(const void* block,int len,bool& condition);
	//更新离线语法词典
	static int update_lex_cb(int ecode, const char *info, void *udata);
	static int update_lexicon();


	/*****************************************
	 *****************在线函数*****************
	 * ****************************************/
	//获取听写会话
	void  getOnlineSID();
	//上传用户表
	int uploadUserwords(const char* filepath );
	//在线实时听写
	void  runOnlineAsr(const  void* block,int len);
	void  getOnlineAsrResult(string& tmp);

	/*****************************************
	 ***************参数获取与配置*************
	 * ****************************************/
	void setLoginParams(string login);
	void setOnlineSeccionParams(string begin);
	void setLocalASR(string localasr);
	string  getBNFfile();
	void reset();

protected:
	void getLocalSID();
	//网络文件参数初始化
	void localasrGRMParamasInit(ConfigParser& cfg);
	//离线语法识别
	void localasrBeginparamsInit(ConfigParser& cfg);

	void onlineSessionBeginparamsInit(ConfigParser& cfg);


private:
	 char* _sidbuf;														//存放会话语句柄
	 string _resultstr;													//识别结果缓存
	 bool _new_session;												//会话刷新标志
//	 vector<string> _result_vec;									//使用循环队列存放识别结果-初始化10
//	 vector<string>::iterator  inter;
//	 queue<string,vector<string>> _result_que;

	string  _login_params;												//登录参数 - 带上工作目录
	string _online_session_begin_params;					//在线识别服务参数
	string _localasr;															//离线命令词识别服务参数
	string  _hints; 															//hints为结束本次会话的原因描述，由用户自定义
	string  _upload_file;													//上传文件 用户数据

	static string _bnf_file;											//语法文件
	static string _localasr_grm_params;					//语法构建参数
	static string _localasr_begin_params;				//获取会话语句柄参数
	static UserData  _userdata;								//离线命令识别状态
	static bool 	_localasr_firsr_frame;						//离线识别第一帧标志
	static bool   _onlineasr_first_frame;					//在线识别第一z帧标志


	AsrState _asr_state;												//讯飞服务器语音处理状态状态

};
#endif /* SRC_KINECT_SPEECH_SRC_XUNFEI_ASR_XUNFEISERVER_H_ */
