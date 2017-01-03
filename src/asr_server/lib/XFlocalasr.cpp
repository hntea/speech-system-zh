/*
 * XFlocalasr.cpp
 *
 *  Created on: 2016年12月29日
 *      Author: hntea
 */

#include "XFlocalasr.h"

namespace Hntea {

XfUserData  XFlocalasr::_userdata;

XFlocalasr::XFlocalasr():ASR() {

}
XFlocalasr::XFlocalasr(XfLocalASR& params):ASR(){
	composeBuildGrammarParams(params);
	buildGrammar(params);
	composeBeginParams(params);
	rest();
}
XFlocalasr::~XFlocalasr(){}


XFlocalasr& XFlocalasr::operator=(const XFlocalasr& another){
	if(this == &another)
		return *this; 				//防止自赋值
	ASR::operator =(another); 		// 调用父类的赋值运算符重载
	this->_begin_params = another._begin_params;
	this->_bgparms = another._bgparms;
	this->_result = another._result;
	this->_sid = another._sid;
	this->_state = another._state;
	this->_userdata = another._userdata;
	return *this;
}

/*
 * 启动/新 会话都必须复位
 * */
void XFlocalasr::rest(){
	_state.aud_stat = MSP_AUDIO_SAMPLE_FIRST;			//音频状态
	_state.ep_stat 	= MSP_EP_LOOKING_FOR_SPEECH;		//端点检测
	_state.rec_stat = MSP_REC_STATUS_SUCCESS;			//识别状态
	_state.errcode 	= MSP_SUCCESS;					//错误码
	if(!_sid.empty()){
		QISRSessionEnd(_sid.c_str(), NULL);
		_sid.clear();
	}
	if(!_result.empty())
		_result.clear();
	getSid();
}

/*
 * 合成语法参数 QISRBuildGrammar()
 * 中的 params
 * */
void XFlocalasr::composeBuildGrammarParams(XfLocalASR& asrparams){
	char* buf = new char[512];
	snprintf(buf, 512,
        "engine_type  = %s, "
        "sample_rate = %d, "
		"asr_res_path = %s, "
		"grm_build_path = %s,",
		asrparams.engine_type.c_str(),
		asrparams.sample_rate,
		asrparams.asr_res_path.c_str(),
		asrparams.grm_build_path.c_str()
		);
	_bgparms = buf;
	std::cout<<"composeBuildGrammarParams : "<<_bgparms<< std::endl;
	delete []buf;
}
/*
 * 函数功能：语法构建网络回调函数
 * 参数说明:
 * 			ecode:错误码
 * 			info:语法id
 *			udata:存放用户数据
 */
 int  XFlocalasr::bgCallback(int ecode, const char *info, void *udata)
 {
	 XfUserData *grm_data = (XfUserData *)udata;
	if (MSP_SUCCESS == ecode && NULL != info)
	{
		printf("构建语法成功！ 语法ID:%s\n\n", info);
		if (NULL != grm_data){
			_userdata.local_grammar = info;
			_userdata.errcode = ecode;
			_userdata.build_fini = 1;
		}
	}
	else
		printf("构建语法失败！%d\n\n", ecode);
	return 0;
 }

/*
 * 构建语法网络
 * */
void XFlocalasr::buildGrammar(const XfLocalASR& asrparams){
	ifstream t;
	//获取文件长度
	unsigned int length;
	cout<<"build file is: "<< asrparams.bnf_file <<endl;
	t.open(asrparams.bnf_file.c_str());
	t.seekg(0, std::ios::end);
	length = t.tellg();
	t.seekg(0, std::ios::beg);
	char* buffer = new char[length];
	t.read(buffer, length);
	buffer[length] = '\0';
	t.close();

	//构建网络
	int ret = QISRBuildGrammar("bnf",buffer,length,
			_bgparms.c_str(),
			bgCallback,
			(void*)&_userdata);
	_userdata.build_fini = 0;
	while(!_userdata.build_fini);
	delete []buffer;
}

/*
 *合成启动会话参数
 * */
void XFlocalasr::composeBeginParams(const XfLocalASR& asrparams){
	char* buf = new char[512];
	//离线语法识别参数设置
	snprintf(buf, 512,
		"engine_type = %s, "
		"asr_res_path = %s, "
		"sample_rate = %d, "
		"grm_build_path = %s, "
		"local_grammar = %s, "
		"result_type = %s, "
		"result_encoding = %s, "
		"asr_denoise = %d,"
		"vad_eos  = %d,"
		"vad_bos = %d,"
		"vad_enable = %d,"
		"language = %s,"
		"accent = %s",
		asrparams.engine_type.c_str(),
		asrparams.asr_res_path.c_str(),
		asrparams.sample_rate,
		asrparams.grm_build_path.c_str(),
		_userdata.local_grammar.c_str(),
		asrparams.result_type.c_str(),
		asrparams.result_encoding.c_str(),
		asrparams.asr_denoise,
		asrparams.vad_eos,
		asrparams.vad_bos,
		asrparams.vad_enable,
		asrparams.language.c_str(),
		asrparams.accent.c_str()
		);

	_begin_params = buf;
	cout<<endl<<"_begin_params = "<<_begin_params<<endl;
    delete []buf;
}


/*
 * 获取会话语句柄
 * */
void XFlocalasr::getSid(){
	_sid = QISRSessionBegin(NULL,_begin_params.c_str(),&_state.errcode);
	if (MSP_SUCCESS != _state.errcode)
	{
		printf("\nQISRSessionBegin failed! error code:%d\n",_state.errcode);
		QISRSessionEnd(_sid.c_str(),NULL);
	}
}

/*
 * 等待语音识别结束
 * */
void XFlocalasr::waitAsrComplete(std::string& result){
	//写入结束帧
	int err = QISRAudioWrite(_sid.c_str(), NULL, 0, MSP_AUDIO_SAMPLE_LAST,	&_state.ep_stat,&_state.rec_stat);
	if (MSP_SUCCESS != err)
	{
		printf("\n waitAsrComplete() 2-QISRAudioWrite failed, error code:%d\n",err);
		rest();
		return;
	}
	while (MSP_REC_STATUS_COMPLETE != _state.rec_stat)
	{
		const char* rslt =  QISRGetResult(_sid.c_str(), &_state.rec_stat, 0, &_state.errcode);
		if (MSP_SUCCESS != _state.errcode)
		{
			printf("\n waitAsrComplete() 3-QISRGetResult failed, error code: %d\n", _state.errcode);
		}
		if (NULL != rslt)
		{
			_result = rslt;
			result = _result;
		}
		usleep(150*1000);
	}
	rest();
}

/*
 * 语音识别
 * 参数说明：
 * 			file:识别的音频文件
 * */
void XFlocalasr::runasr(std::string file,std::string& result){
	ifstream t;
	//获取文件长度
	unsigned int length;
	cout<<"asr file is: "<< file <<endl;
	t.open(file.c_str());
	t.seekg(0, std::ios::end);
	length = t.tellg();
	if(length < 1024){
		rest();			//文件出错，退出并复位
		return;
	}
	t.seekg(0, std::ios::beg);
	char* buffer = new char[length];
	t.read(buffer, length);
	buffer[length] = '\0';
	t.close();

	int err = 0;

	//写入识别器
	while (true)
	{
		//写入第一帧
		static int frame_cur = 0;
		const int frame_size = 1024;
		cout<<"."<<flush;
		_state.aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;
		if(frame_cur == 0){
			_state.aud_stat = MSP_AUDIO_SAMPLE_FIRST;
		}
		if(frame_cur > length - frame_size){
			frame_cur = 0;
			delete []buffer;
			break;
		}
		err = QISRAudioWrite(
				_sid.c_str(),
				(const void*)&buffer[frame_cur],
				frame_size,
				_state.aud_stat,
				&_state.ep_stat,
				&_state.rec_stat);
		if (MSP_SUCCESS != err)
		{
			printf("\nQISRAudioWrite failed, error code:%d\n",err);
			break;
		}
		frame_cur += frame_size;
	}

	waitAsrComplete(result);
}

/*
 * 语音识别：
 * 参数说明：
 * 			input:识别的音频块
 * 			end:結束幀標誌
 * 			result：存放识别结果
 * */
void XFlocalasr::runasr(std::vector<int16_t>& input,std::string& result,bool end){
	//发送起始帧
	int err;
	static bool first_frame = true;
	_state.aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;
	if(first_frame == true){
		_state.aud_stat = MSP_AUDIO_SAMPLE_FIRST;
		first_frame = false;
	}
	//发送连续帧
	err = QISRAudioWrite(
			_sid.c_str(),
			(const void*)&input[0],
			input.size()*2,
			_state.aud_stat,
			&_state.ep_stat,
			&_state.rec_stat);

	//结束本次会话
	if (MSP_SUCCESS != err){
		QISRSessionEnd(_sid.c_str(), NULL);
		cerr<<"runasr<stream> 1-QISRAudioWrite fail ,err = "<<err<<endl;
		return;
	}
	if (end){
		waitAsrComplete(result);
		end = false;
		first_frame = true;
	}
}

/*
 * 更新离线语法词典
 * */
void XFlocalasr::updateDictionary(){}

} /* namespace Hntea */
