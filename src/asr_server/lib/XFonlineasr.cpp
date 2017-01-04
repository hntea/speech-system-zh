/*
 * XFonlineasr.cpp
 *
 *  Created on: 2017年1月3日
 *      Author: hntea
 */

#include "XFonlineasr.h"

namespace Hntea {

XFonlineasr::XFonlineasr() {
	// TODO Auto-generated constructor stub

}
XFonlineasr::XFonlineasr(const XfOnlineASR& params){
	composeBeginParams(params);
	uploadUserWords(params);
	reset();
}
XFonlineasr::~XFonlineasr() {
	// TODO Auto-generated destructor stub
}

/*
 *合成启动会话参数
 * */
void XFonlineasr::composeBeginParams(const XfOnlineASR& asrparams){
	_begin_params = asrparams.params;
}


/*
 * 获取会话语句柄
 * */
void XFonlineasr::getSid(){
	_sid = QISRSessionBegin(NULL,_begin_params.c_str(),&_state.errcode);
	if (MSP_SUCCESS != _state.errcode)
	{
		printf("\nQISRSessionBegin failed! error code:%d\n",_state.errcode);
		QISRSessionEnd(_sid.c_str(),NULL);
	}
}

/*
 * 会话复位
 * */
void XFonlineasr::reset(){
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
 * 上传用户词汇表
 * */
void XFonlineasr::uploadUserWords(const XfOnlineASR& asrparams){

	//获取文件大小
	ifstream t;
	unsigned int length;
	cout<<"userword file is: "<< asrparams.userWord <<endl;
	t.open(asrparams.userWord.c_str());
	t.seekg(0, std::ios::end);
	length = t.tellg();			//获取文件长度
	t.seekg(0, std::ios::beg);
	char* buffer = new char[length+1];
	t.read(buffer, length);		//读取文件内容
	buffer[length] = '\0';		//文件末尾添加结束符
	t.close();

	//上传用户表
	int err;
	MSPUploadData("userwords", buffer, length, "sub = uup, dtt = userword", &err); //上传用户词表
	if (MSP_SUCCESS != err)
	{
		printf("\nMSPUploadData failed ! errorCode: %d \n", err);
	}else{
		printf("UploadDada success!\n");
	}
	delete []buffer;
}

/*
 * 等待语音识别结束,
 * 注意：若wifi不稳定，则会出现无效语句柄10108错误！
 * */
void XFonlineasr::waitAsrComplete(std::string& result){
	//写入结束帧
	int err = QISRAudioWrite(_sid.c_str(), NULL, 0, MSP_AUDIO_SAMPLE_LAST,	&_state.ep_stat,&_state.rec_stat);
	if (MSP_SUCCESS != err)
	{
		printf("\n waitAsrComplete() 2-QISRAudioWrite failed, error code:%d\n",err);
		reset();
		return;
	}
	while (MSP_REC_STATUS_COMPLETE != _state.rec_stat)
	{
		const char* rslt =  QISRGetResult(_sid.c_str(), &_state.rec_stat, 0, &_state.errcode);
		if (MSP_SUCCESS != _state.errcode)
		{
			printf("\n waitAsrComplete() 3-QISRGetResult failed, error code: %d\n", _state.errcode);
			result = "Null";
			break;
		}
		if (NULL != rslt)
		{
			_result = rslt;
			result += _result;
		}
		usleep(150*1000);
	}
	if(result.empty()){
		result = "Null";
	}
	reset();
}


/*
 * 语音识别
 * 参数说明：
 * 			file:识别的音频文件
 * */
void XFonlineasr::runasr(std::string file,std::string& result){
	ifstream t;
	//获取文件内容
	unsigned int length;
	cout<<"asr file is: "<< file <<endl;
	t.open(file.c_str());
	t.seekg(0, std::ios::end);
	length = t.tellg();
	if(length < 64){
		reset();			//文件出错，退出并复位
		t.close();
		cerr<<"File is too short! "<<endl;
		return;
	}
	t.seekg(0, std::ios::beg);
	char* buffer = new char[length+1];
	t.read(buffer, length);
	buffer[length] = '\0';
	t.close();



	//初始化
	int err = 0;
	int frame_cur = 0;
	const int frame_size = 64;

	//发送起始帧
	err = QISRAudioWrite(_sid.c_str(),(const void*)&buffer[frame_cur],
					frame_size,MSP_AUDIO_SAMPLE_FIRST,&_state.ep_stat,&_state.rec_stat);

	if (MSP_SUCCESS != err){
		printf("\n runasr-file-0 QISRAudioWrite failed, error code:%d\n",err);
		reset();
	}

	frame_cur+=frame_size;

	//发送剩余数据
	err = QISRAudioWrite(_sid.c_str(),(const void*)&buffer[frame_cur],
			(length-frame_size),MSP_AUDIO_SAMPLE_CONTINUE,&_state.ep_stat,&_state.rec_stat);
	if (MSP_SUCCESS != err){
		printf("\n runasr-file-1 QISRAudioWrite failed, error code:%d\n",err);
		reset();
	}
	delete []buffer;
	frame_cur = 0;
	waitAsrComplete(result);

}
/*
 * 语音识别
 * 参数说明：
 * 			input:识别的音频块
 * */
void XFonlineasr::runasr(std::vector<int16_t>& input,std::string& result,bool end){

	//发送起始帧
	int err;
	static bool first_frame = true;
	_state.aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;
	if(first_frame == true){
		_state.aud_stat = MSP_AUDIO_SAMPLE_FIRST;
		first_frame = false;
	}
	//发送连续帧
	err = QISRAudioWrite(_sid.c_str(),(const void*)&input[0],
			input.size()*2,_state.aud_stat,&_state.ep_stat,&_state.rec_stat);


	if (MSP_SUCCESS != err){
		cout<<_sid<<endl;
		QISRSessionEnd(_sid.c_str(), NULL);
		cerr<<"runasr<stream> 1-QISRAudioWrite fail ,err = "<<err<<endl;
		first_frame = true;
		reset();
		return;
	}

	//查询结果-只能边发边收
	if(_state.rec_stat == MSP_REC_STATUS_SUCCESS){
		const char* rslt= QISRGetResult( _sid.c_str(),&_state.rec_stat,0,&_state.errcode);
		if(NULL != rslt){
			result += rslt;
		}
	}

	//结束会话并获取结果
	if (end){
		waitAsrComplete(result);
		end = false;
		first_frame = true;
		if(result.empty()){
			result = "Null";
		}
	}


}

} /* namespace Hntea */
