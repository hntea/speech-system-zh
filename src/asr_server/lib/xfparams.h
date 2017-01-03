/*
 * xfparams.h
 *
 *  Created on: 2016年12月29日
 *      Author: hntea
 */

#ifndef SRC_ASR_SERVER_INCLUDE_ASR_SERVER_XFPARAMS_H_
#define SRC_ASR_SERVER_INCLUDE_ASR_SERVER_XFPARAMS_H_
#include <iostream>
namespace Hntea{
using namespace std;

typedef struct basic{
	string  loginParams;			//登录参数
}XfBasic;

typedef struct online{
	string  params;					//在线识别参数
	string  userWord;				//用户数据表
}XfOnlineASR;

typedef struct local{
	string engine_type;					//识别引擎
	string result_type;					//识别结果类型
	string result_encoding;				//结果编码
	string asr_res_path;				//声学模型库所在路径
	string grm_build_path;				//语法网络存储路径
	string bnf_file;					//语法文件
	string language;					//识别语言类型
	string accent;						//方言
	int sample_rate;					//采样率
	int asr_denoise;					//是否降噪
	int vad_enable;						//是否开启端点检测
	int vad_bos;						//前端点静音判别
	int vad_eos;						//后端点静音判别
}XfLocalASR;

typedef struct state{
	int 	aud_stat ;				//音频状态-连续识别
	int 	ep_stat ;				//端点检测
	int 	rec_stat ;				//识别状态
	int 	errcode ;
}XfAsrState;

//离线语音识别
typedef struct userData {
	int     	build_fini; 			//标识语法构建是否完成
	int     	update_fini;			//标识更新词典是否完成
	int     	errcode; 				//记录语法构建或更新词典回调错误码
	string    	local_grammar ;		 	//保存语法构建返回的语法ID
}XfUserData;
}
#endif /* SRC_ASR_SERVER_INCLUDE_ASR_SERVER_XFPARAMS_H_ */

