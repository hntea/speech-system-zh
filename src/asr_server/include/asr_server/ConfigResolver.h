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
#include "xfparams.h"
using json = nlohmann::json;
using namespace std;


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
