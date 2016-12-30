/*
 * XFlocalasr.h
 *
 *  Created on: 2016年12月29日
 *      Author: hntea
 */

#ifndef SRC_ASR_SERVER_LIB_XFLOCALASR_H_
#define SRC_ASR_SERVER_LIB_XFLOCALASR_H_

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

class XFlocalasr:public ASR {
public:
	XFlocalasr();
	virtual ~XFlocalasr();

	/*
	 * 状态初始化
	 * */
	void stateRest();

	/*
	 * 会话复位
	 * */
	void rest();

	/*
	 * 合成语法参数 QISRBuildGrammar() 中的 params
	 * 用于构建语法网络
	 * */
	void composeBuildGrammarParams(XfLocalASR& asrparams);

	/*
	 * 构建语法网络
	 * */
	void buildGrammar(const XfLocalASR& asrparams);

	/*
	 * 构建语法网络回调
	 * */
	static int bgCallback(int ecode, const char *info, void *udata);

	/*
	 *合成启动会话参数
	 * */
	void composeBeginParams(const XfLocalASR& asrparams);

	/*
	 * 获取会话语句柄
	 * */
	void getSid();

	/*
	 * 更新离线语法词典
	 * */
	void updateDictionary();

	/*
	 * 等待语音识别结束
	 * */
	void waitAsrComplete();

	/*
	 * 语音识别
	 * 参数说明：
	 * 			file:识别的音频文件
	 * */
	void runasr(std::string file);

	/*
	 * 语音识别
	 * 参数说明：
	 * 			bkinput:识别的音频块
	 * */
	void runasr(std::vector<int16_t>& input,bool end);

	/*
	 * 获取识别结果
	 * */
	std::string getResult();

private:
	XfAsrState 	_state;			//离线命令识别状态
	string _bgparms;			//语法构建回调ID
	string _begin_params;		//第一个会话参数
	string _sid;				//识别会话语句柄
	static XfUserData  _userdata;

};

} /* namespace Hntea */

#endif /* SRC_ASR_SERVER_LIB_XFLOCALASR_H_ */
