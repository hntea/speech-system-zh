/*
 * ConfigResolver.h
 *
 *  Created on: 2016年12月28日
 *      Author: hntea
 */

#ifndef SRC_ASR_SERVER_LIB_CONFIGRESOLVER_H_
#define SRC_ASR_SERVER_LIB_CONFIGRESOLVER_H_

namespace Hntea {

#include "nlohmann/json.hpp"
using json = nlohmann::json;
using namespace std;

typedef struct basic{
	string  loginParams;
	string  userWord;
}XfBasic;

typedef struct online{
	string  params;
}XfOnlineASR;

typedef struct local{
	string engine_type;
	int sample_rate;
	string result_type;
	string result_encoding;
	string asr_model;
	string grm_build_path;
	string bnf_file;
}XfLocalASR;

class ConfigResolver {
public:
	ConfigResolver(string file);
	virtual ~ConfigResolver();
	void process();
protected:
	XfBasic _basic;
	XfOnlineASR _online;
	XfLocalASR _local;
private:
	std::string _file;
};

} /* namespace Hntea */

#endif /* SRC_ASR_SERVER_LIB_CONFIGRESOLVER_H_ */
