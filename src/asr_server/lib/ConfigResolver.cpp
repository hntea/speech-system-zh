/*
 * ConfigResolver.cpp
 *
 *  Created on: 2016年12月28日
 *      Author: hntea
 */

#include "ConfigResolver.h"

namespace Hntea {

ConfigResolver::ConfigResolver(string file):_file(file) {
	process();
}
ConfigResolver::~ConfigResolver() {}

XfBasic ConfigResolver::getXfBasic(){
	return this->_basic;
}
XfOnlineASR ConfigResolver::getXfOnlineParams(){
	return this->_online;
}
XfLocalASR ConfigResolver::getXfLocalParams(){
	return this->_local;
}

void ConfigResolver::process(){
	std::ifstream ifs(_file);
	if(!ifs)
	{
		cerr<<"Can not open "<<_file<<"!"<<endl;
		return;
	}
	nlohmann::json jload;
	ifs>>jload;
	ifs.close();

	_basic.loginParams 		= jload["speechServer"]["xunfei"]["loginParams"];
	_online.params 			= jload["speechServer"]["xunfei"]["onlineasr"]["params"];
	_online.userWord 		= jload["speechServer"]["xunfei"]["onlineasr"]["userWordsFilePath"];

	_local.asr_res_path 	= jload["speechServer"]["xunfei"]["localasr"]["asr_res_path"];
	_local.engine_type  	= jload["speechServer"]["xunfei"]["localasr"]["engine_type"];
	_local.grm_build_path 	= jload["speechServer"]["xunfei"]["localasr"]["grm_build_path"];
	_local.result_encoding 	= jload["speechServer"]["xunfei"]["localasr"]["result_encoding"];
	_local.result_type 		= jload["speechServer"]["xunfei"]["localasr"]["result_type"];
	_local.bnf_file 		= jload["speechServer"]["xunfei"]["localasr"]["bnf_file"];
	_local.language			= jload["speechServer"]["xunfei"]["localasr"]["language"];
	_local.accent			= jload["speechServer"]["xunfei"]["localasr"]["accent"];
	_local.sample_rate 		= jload["speechServer"]["xunfei"]["localasr"]["sample_rate"];
	_local.asr_denoise		= jload["speechServer"]["xunfei"]["localasr"]["asr_denoise"];
	_local.vad_enable       = jload["speechServer"]["xunfei"]["localasr"]["vad_enable"];
	_local.vad_bos			= jload["speechServer"]["xunfei"]["localasr"]["vad_bos"];
	_local.vad_eos			= jload["speechServer"]["xunfei"]["localasr"]["vad_eos"];

}
} /* namespace Hntea */
