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

void ConfigResolver::process(){
	std::ifstream ifs(_file);
	if(!ifs)
	{
		cerr<<"Can not open "<<_file<<"!"<<endl;
		return;
	}
	json jload;
	ifs>>jload;
	ifs.close();
	_basic.userWord 		= jload["speechServer"]["xunfei"]["userWordsFilePath"];
	_basic.loginParams 		= jload["speechServer"]["xunfei"]["loginParams"];
	_online.params 			= jload["speechServer"]["xunfei"]["onlineasr"]["params"];

	_local.asr_model 		= jload["speechServer"]["xunfei"]["localasr"]["asr_model"];
	_local.engine_type  	= jload["speechServer"]["xunfei"]["localasr"]["engine_type"];
	_local.grm_build_path 	= jload["speechServer"]["xunfei"]["localasr"]["grm_build_path"];
	_local.result_encoding 	= jload["speechServer"]["xunfei"]["localasr"]["result_encoding"];
	_local.sample_rate 		= jload["speechServer"]["xunfei"]["localasr"]["sample_rate"];
	_local.result_type 		= jload["speechServer"]["xunfei"]["localasr"]["result_type"];
	_local.bnf_file 		= jload["speechServer"]["xunfei"]["localasr"]["bnf_file"];
}
} /* namespace Hntea */
