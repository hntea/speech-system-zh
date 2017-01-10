/*
 * XFonlineasr.h
 *
 *  Created on: 2017年1月3日
 *      Author: hntea
 */

#ifndef SRC_ASR_SERVER_LIB_XFONLINEASR_H_
#define SRC_ASR_SERVER_LIB_XFONLINEASR_H_

#include "../include/xfApiHead/qisr.h"
#include "../include/xfApiHead/msp_cmn.h"
#include "../include/xfApiHead/msp_errors.h"
#include "xfparams.h"
#include "ASR.h"
#include <unistd.h>
#include <string.h>
#include <fstream>
#include <iostream>

namespace Hntea {

class XFonlineasr :public ASR{
public:
	XFonlineasr();
	XFonlineasr(const XfOnlineASR& params,const XfBasic& lg);
	virtual ~XFonlineasr();


	/*
	 * 上传用户词汇表
	 * */
	void uploadUserWords(const XfOnlineASR& asrparams);


	/*
	 * 语音识别
	 * 参数说明：
	 * 			file:识别的音频文件
	 * 			result:识别结果
	 * */
	void runasr(std::string file,std::string& result);

	/*
	 * 语音识别
	 * 参数说明：
	 * 			input:识别的音频块
	 * 			result:识别结果，识别空:Null
	 * 			end: 结束帧标志
	 * */
	void runasr(std::vector<int16_t>& input,std::string& result,bool end);

private:

	/*
	 *登录
	 * */
	void login(const XfBasic& params);

	/*
	 *合成启动会话参数
	 * */
	void composeBeginParams(const XfOnlineASR& asrparams);

	/*
	 * 获取会话语句柄
	 * */
	void getSid();

	/*
	 * 会话复位
	 * */
	void reset();


	/*
	 * 等待语音识别结束
	 * */
	void waitAsrComplete(std::string& result);


	XfAsrState 	_state;			//离线命令识别状态
	string _begin_params;		//第一个会话参数
	string _sid;				//识别会话语句柄
};

} /* namespace Hntea */

#endif /* SRC_ASR_SERVER_LIB_XFONLINEASR_H_ */
