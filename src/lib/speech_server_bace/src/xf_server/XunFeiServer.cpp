/*
 * XunFeiServer.cpp
 *
 *  Created on: 2016年11月17日
 *      Author: hntea
 */

#include "../../include/xf_server/XunFeiServer.h"

//静态成员初始化
UserData 	 XunFeiServer::_userdata ;
string  		XunFeiServer::_bnf_file;											//语法文件
string  		XunFeiServer::_localasr_grm_params;					//语法构建参数
string 		XunFeiServer::_localasr_begin_params;				//获取会话语句柄参数
bool 			XunFeiServer::_localasr_firsr_frame = false;
bool 			XunFeiServer::_onlineasr_first_frame = true;

XunFeiServer::XunFeiServer(){}
XunFeiServer::XunFeiServer(ConfigParser &cfg,bool local)
{
	//初始化状态参数
	this->_asr_state.aud_stat 		= MSP_AUDIO_SAMPLE_FIRST;				//音频状态
	this->_asr_state.ep_stat 		= MSP_EP_LOOKING_FOR_SPEECH;		//端点检测
	this->_asr_state.rec_stat 		= MSP_REC_STATUS_SUCCESS;				//识别状态
	this->_asr_state.errcode 		= MSP_SUCCESS;										//错误码
	this->_sidbuf = NULL;
	this->_new_session = false;
	//读取配置文件登录参数
	_login_params = cfg.getXFLoginParams();
	//登录
	int ret = MSPLogin(NULL, NULL,this->_login_params.c_str()); //第一个参数是用户名，第二个参数是密码，均传NULL即可，
	if (MSP_SUCCESS != ret){
		cout<<"MSPLogin failed , Error code:"<<ret<<endl;
		exit;
	}
	//依照本地服务与在线服务进行不同初始化
	if(local == true){
																//初始化本地服务
		localasrGRMParamasInit(cfg);		//1、初始化命令词识别网络构建参数
		buildGrammar();								//2、构建语法识别网络
		localasrBeginparamsInit(cfg);		//3、初始化会话参数
		getLocalSID();									//4、获取会话语句柄
	}
	else{
		//初始化在线服务
		onlineSessionBeginparamsInit(cfg);	//1、初始化在线会话参数
		getOnlineSID();									//2、获取在线会话语句柄
	}
}
XunFeiServer::~XunFeiServer() {
	//当对象销毁时登出
	cout<<"MSPLogout..."<<endl;
	MSPLogout();

}

void XunFeiServer::init(ConfigParser &cfg,bool local){
	//初始化状态参数
	this->_asr_state.aud_stat 		= MSP_AUDIO_SAMPLE_FIRST;				//音频状态
	this->_asr_state.ep_stat 		= MSP_EP_LOOKING_FOR_SPEECH;		//端点检测
	this->_asr_state.rec_stat 		= MSP_REC_STATUS_SUCCESS;				//识别状态
	this->_asr_state.errcode 		= MSP_SUCCESS;										//错误码
	this->_sidbuf = NULL;
	this->_new_session = false;
	//读取配置文件登录参数
	_login_params = cfg.getXFLoginParams();
	_upload_file = cfg.getUserWordFile();
	//登录
	int ret = MSPLogin(NULL, NULL,this->_login_params.c_str()); //第一个参数是用户名，第二个参数是密码，均传NULL即可，
	if (MSP_SUCCESS != ret){
		cout<<"MSPLogin failed , Error code:"<<ret<<endl;
		exit;
	}


	//依照本地服务与在线服务进行不同初始化
	if(local == true){
																//初始化本地服务
		localasrGRMParamasInit(cfg);		//1、初始化命令词识别网络构建参数
		buildGrammar();								//2、构建语法识别网络
		localasrBeginparamsInit(cfg);		//3、初始化会话参数
		getLocalSID();									//4、获取会话语句柄
	}
	else{
		//上传用户词条
		  if(_upload_file == "~/.SpeechSystem/xf-source/userwords.txt"){
			  struct passwd *pw = getpwuid(getuid());
			  const char *homedir = pw->pw_dir;
			  string path = homedir;
			  path+="/.SpeechSystem/xf-source/userwords.txt";
			  _upload_file = path;
			  cout<<"upload file path: "<<_upload_file<<endl;
		  }
		uploadUserwords(_upload_file.c_str());
		//初始化在线服务
		onlineSessionBeginparamsInit(cfg);	//1、初始化在线会话参数
		getOnlineSID();									//2、获取在线会话语句柄
	}
}
void XunFeiServer::setLoginParams(string login){
	this->_login_params = login;
}
void XunFeiServer::setOnlineSeccionParams(string begin){
	this->_online_session_begin_params = begin;
}
void XunFeiServer::setLocalASR(string localasr){
	this->_localasr = localasr;
}
string  XunFeiServer::getBNFfile(){
	cout<<_bnf_file<<endl;
	return _bnf_file;
}
void XunFeiServer::reset(){
	this->_asr_state.aud_stat 		= MSP_AUDIO_SAMPLE_FIRST;				//音频状态
	this->_asr_state.ep_stat 		= MSP_EP_LOOKING_FOR_SPEECH;		//端点检测
	this->_asr_state.rec_stat 		= MSP_REC_STATUS_SUCCESS;				//识别状态
	this->_asr_state.errcode 		= MSP_SUCCESS;										//错误码
	this->_sidbuf = NULL;
	XunFeiServer::_localasr_firsr_frame = false;
	XunFeiServer::_onlineasr_first_frame = true;
}

/******************************************在线识别相关函数********************************************************/
/******************************************在线识别相关函数********************************************************/
/******************************************在线识别相关函数********************************************************/

void  XunFeiServer::onlineSessionBeginparamsInit(ConfigParser& cfg){
	this->_online_session_begin_params =cfg.getXFonlineSessionBeginParams();
}
/*
 * 函数功能：上传用户表
 * 参数说明：
 * 					filepath：文件所在绝对路径
 * */
int XunFeiServer::uploadUserwords(const char* filepath)
{

	char*			userwords	=	NULL;
	unsigned int	len			=	0;
	unsigned int	read_len	=	0;
	FILE*			fp			=	NULL;
	int				ret			=	-1;

	fp = fopen(filepath, "rb");
	if (NULL == fp)
	{
		printf("\nopen %s failed! \n",filepath);
		goto upload_exit;
	}

	fseek(fp, 0, SEEK_END);
	len = ftell(fp); //获取音频文件大小
	fseek(fp, 0, SEEK_SET);

	userwords = (char*)malloc(len + 1);
	if (NULL == userwords)
	{
		printf("\nout of memory! \n");
		goto upload_exit;
	}

	read_len = fread((void*)userwords, 1, len, fp); //读取用户词表内容
	if (read_len != len)
	{
		printf("\nread [userwords.txt] failed!\n");
		goto upload_exit;
	}
	userwords[len] = '\0';

	MSPUploadData("userwords", userwords, len, "sub = uup, dtt = userword", &ret); //上传用户词表
	if (MSP_SUCCESS != ret)
	{
		printf("\nMSPUploadData failed ! errorCode: %d \n", ret);
		goto upload_exit;
	}else{
		printf("UploadDada success!\n");
	}

upload_exit:
	if (NULL != fp)
	{
		fclose(fp);
		fp = NULL;
	}
	if (NULL != userwords)
	{
		free(userwords);
		userwords = NULL;
	}

	return ret;
}
/*
 * 函数功能：获取会话语句柄；
 * */
void XunFeiServer::getOnlineSID(){

    //设置业务请求参数,准备听写 - 每一次听写只需要一个会话。
	const char*sid= QISRSessionBegin(NULL,
			this->_online_session_begin_params.c_str(),
			&this->_asr_state.errcode);
	if (MSP_SUCCESS != _asr_state.errcode)
	{
		printf("\n getOnlineSID() ---- QISRSessionBegin failed! error code:%d\n", _asr_state.errcode);
		QISRSessionEnd( sid,NULL);
		exit;
	}else{
		int len=strlen(sid);
		if(_sidbuf != NULL){
			cout<<"allocate new mem "<<endl;
			delete []_sidbuf;
		}
		_sidbuf = new char[len];
		strcpy(_sidbuf,sid);
	}
}

/*
 * 函数功能：无限识别结果。
 * */
 void XunFeiServer::runOnlineAsr(const void* block,int len){
	 	 int ret = 0;
		_asr_state.aud_stat 	= MSP_AUDIO_SAMPLE_CONTINUE;
		if(_onlineasr_first_frame){
			_asr_state.aud_stat 	= MSP_AUDIO_SAMPLE_FIRST;
			_onlineasr_first_frame = false;
		}

		//写入数据
		ret = QISRAudioWrite(_sidbuf,(const void *)block,len,_asr_state.aud_stat,&_asr_state.ep_stat,&_asr_state.rec_stat);
		if (MSP_SUCCESS != ret){
			printf("\nQISRAudioWrite failed! error code:%d\n", ret);
			QISRSessionEnd(_sidbuf,NULL);
		}

//		查询结果-只能边发边收
//		if(_asr_state.rec_stat == MSP_REC_STATUS_INCOMPLETE){
//						cout<< ">"<<flush;
//		}
		if(_asr_state.rec_stat == MSP_REC_STATUS_SUCCESS){
//			cout<<"\n有部分识别结果"<<endl;
			const char* rslt= QISRGetResult( _sidbuf,&_asr_state.rec_stat,0,&_asr_state.errcode);
			if(NULL != rslt){
				_resultstr += rslt;
			}
		}else if(_asr_state.rec_stat == MSP_REC_STATUS_NO_MATCH){
			cout<<"识别结束-->没有识别结果"<<endl;
		}

		//判断最后端点出现：获取最后结果
		if (this->_asr_state.ep_stat == MSP_EP_AFTER_SPEECH )
		{
//			cout<<"后端点出现.获取最后识别结果"<<endl;
			//发送最后一帧
			_asr_state.errcode = QISRAudioWrite(_sidbuf,NULL,0,MSP_AUDIO_SAMPLE_LAST,&_asr_state.ep_stat,&_asr_state.rec_stat);
			if (MSP_SUCCESS != _asr_state.errcode){
				printf("\n At the end of Frame;QISRAudioWrite failed, error code:%d\n",_asr_state.errcode);
				QISRSessionEnd(_sidbuf,_hints.c_str());
				exit;
			}
			//获取最后识别结果 - 这里有时会陷入死循环
			while (MSP_REC_STATUS_COMPLETE != _asr_state.rec_stat)
			{
				const char* rslt= QISRGetResult( _sidbuf,&_asr_state.rec_stat,0,&_asr_state.errcode);
				if(NULL != rslt){
					_resultstr += rslt;
				}
			}

			//这里使用刷新操作，重新申请新的会话
			_new_session = true;
			//结束会话
			QISRSessionEnd(_sidbuf,_hints.c_str());
		}
 }

void XunFeiServer::getOnlineAsrResult(string& tmp){
	if(_new_session){	//当识别结果出来后-会话更新
		 reset();
		 getOnlineSID();
		_new_session = false;
		if(!_resultstr.empty()){
			tmp = _resultstr;
			_resultstr = " ";
		}else{
			 tmp = " ";
			_resultstr = " ";
		}
	 }
}
 /*************************************离线识别相关函数***********************************/
 /*************************************离线识别相关函数***********************************/
 /*************************************离线识别相关函数***********************************/
 /*************************************离线识别相关函数***********************************/

void XunFeiServer::localasrGRMParamasInit(ConfigParser& cfg){
	//1、读取配置文件
	string asr_rec_path = cfg.getLocalAsrResPath();
	string engine_type = cfg.getLocalAsrEngineType();
	string grm_buile_path = cfg.getLocalAsrGrmBuildPath();
	string result_type = cfg.getLocalAsrResultType();
	string  result_encoding = cfg.getLocalAsrResultEncoding();
	int sample_rate = cfg.getLocalAsrSampleRate();
	char* buf = new char[MAX_PARAMS_LEN];
    snprintf(buf, MAX_PARAMS_LEN - 1,
        "engine_type  = %s, "
        "sample_rate = %d, "
		"asr_res_path = %s, "
		"grm_build_path = %s,"
    		"vad_eos = 500",
		engine_type.c_str(),
		sample_rate,
		asr_rec_path.c_str(),
		grm_buile_path.c_str()
		);
    this->_localasr_grm_params = buf;
    delete []buf;
    this->_bnf_file = cfg.getLocalAsrBNFfile();
}
/*
 * 函数功能：语法构建网络回调函数
 * 参数说明:
 *
 * 注意：多线程应该使用静态函数作为入口，不然会出错
 */
 int  XunFeiServer::buildGrammarCallback(int ecode, const char *info, void *udata)
 {
		UserData *grm_data = (UserData *)udata;
		cout<<"grm_data->grammar_id"<<grm_data->grammar_id<<endl;

		if (NULL != grm_data) {
			grm_data->build_fini = 1;
			grm_data->errcode = ecode;
		}

		if (MSP_SUCCESS == ecode && NULL != info) {
			printf("构建语法成功！ 语法ID:%s\n", info);
			if (NULL != grm_data)
				snprintf(grm_data->grammar_id, MAX_GRAMMARID_LEN - 1, info);
			//	cout<<"_userdata.grammar_id = "<<_userdata.grammar_id<<endl;
		}
		else
			printf("构建语法失败！%d\n", ecode);

		return 0;
 }

 /*
  * 函数功能：构建离线识别语法网络
  * 参数说明:
  */
int XunFeiServer::buildGrammar(){
	ifstream t;
	//获取文件长度
	unsigned int length;
	t.open(_bnf_file.c_str());
	t.seekg(0, std::ios::end);    // go to the end
	length = t.tellg();           		// report location (this is the length)
	cout<<"file length is: "<<length<<endl;
	//获取文件内容
	t.seekg(0, std::ios::beg);    // go back to the beginning
	char* buffer = new char[length];    // allocate memory for a buffer of appropriate dimensio
	t.read(buffer, length);       // read the whole file into the buffer
	buffer[length] = '\0';
	t.close();                    			// close file handle

	//构建网络
	int ret = QISRBuildGrammar("bnf",buffer,length,
			_localasr_grm_params.c_str(),
			buildGrammarCallback,
			(void*)&_userdata
			);
	//等待构建网络成功
	while(!_userdata.build_fini);
	delete []buffer;

	return ret;
}


void XunFeiServer::localasrBeginparamsInit(ConfigParser& cfg){
	string asr_rec_path = cfg.getLocalAsrResPath();
	string engine_type = cfg.getLocalAsrEngineType();
	string grm_buile_path = cfg.getLocalAsrGrmBuildPath();
	string result_type = cfg.getLocalAsrResultType();
	string  result_encoding = cfg.getLocalAsrResultEncoding();
	int sample_rate = cfg.getLocalAsrSampleRate();


	char* buf = new char[MAX_PARAMS_LEN];
	//离线语法识别参数设置
	snprintf(buf, MAX_PARAMS_LEN - 1,
		"engine_type = %s, "
		"asr_res_path = %s, "
		"sample_rate = %d, "
		"grm_build_path = %s, "
		"local_grammar = %s, "
		"result_type = %s, "
		"result_encoding = %s, "
		"asr_denoise = 1,"
		"vad_eos  = 1000,",
		engine_type.c_str(),
		asr_rec_path.c_str(),
		sample_rate,
		grm_buile_path.c_str(),
		_userdata.grammar_id,
		result_type.c_str(),
		result_encoding.c_str()
		);

    this->_localasr_begin_params = buf;
    delete []buf;
}

void XunFeiServer::getLocalSID(){
	  //设置业务请求参数,准备听写 - 每一次听写只需要一个会话。
		const char*sid= QISRSessionBegin(NULL,
				this->_localasr_begin_params.c_str(),
				&this->_asr_state.errcode);
		if (MSP_SUCCESS != _asr_state.errcode)
		{
			printf("\nQISRSessionBegin failed! error code:%d\n", _asr_state.errcode);
			QISRSessionEnd( sid,NULL);
		}else{
			int len=strlen(sid);
			if(_sidbuf != NULL){
				cout<<"allocate new memory for next session ID... "<<endl;
				delete []_sidbuf;
			}
			_sidbuf = new char[len];
			strcpy(_sidbuf,sid);
			cout<<_sidbuf<<endl;
		}
}


/*
 * 离线语音识别存在一些问题：前段点检测不稳定，后端点也是。
 *
 * */
void XunFeiServer::runLocalAsr(const void* block,int len,bool& condition){
	//发送起始帧
	cout<<">"<<flush;
	_asr_state.aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;
	if(_localasr_firsr_frame == false){
		_asr_state.aud_stat = MSP_AUDIO_SAMPLE_FIRST;
		_localasr_firsr_frame = true;
	}
	//发送连续帧
	int errcode = QISRAudioWrite(_sidbuf, block, len, _asr_state.aud_stat, &_asr_state.ep_stat, &_asr_state.rec_stat);
	if (MSP_SUCCESS != errcode){
		//结束本次会话
		QISRSessionEnd(_sidbuf, NULL);
		return;
	}

	//检测端点状态
	if (_asr_state.ep_stat ==MSP_EP_AFTER_SPEECH ){

		cout<<"后端点出现"<<endl;
		QISRAudioWrite(_sidbuf, (const void *)NULL, 0, MSP_AUDIO_SAMPLE_LAST,  &_asr_state.ep_stat, &_asr_state.rec_stat);		//发送结束帧
		while (MSP_REC_STATUS_COMPLETE != _asr_state.rec_stat  && MSP_SUCCESS == errcode) {
			const char* rec_rslt = QISRGetResult(_sidbuf, & _asr_state.rec_stat, 0, &errcode);
			if(rec_rslt != NULL){
				_resultstr = rec_rslt;
			}
//			usleep(150 * 1000);
		}
		const char* rec_rslt = QISRGetResult(_sidbuf, & _asr_state.rec_stat, 0, &errcode);
		if(rec_rslt != NULL){
			_resultstr = rec_rslt;
		}
		cout<<_resultstr<<endl;
		condition = false;
		QISRSessionEnd(_sidbuf, NULL);
	}
}
//更新离线语法词典
//int XunFeiServer::update_lex_cb(){
//
//}
//int XunFeiServer::update_lexicon(){
//
//}
