/*
 * ConfigResolver.h
 *
 *  Created on: 2016年12月28日
 *      Author: hntea
 */

#ifndef SRC_ASR_SERVER_LIB_CONFIGRESOLVER_H_
#define SRC_ASR_SERVER_LIB_CONFIGRESOLVER_H_

#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"
#include <vector>
#include <algorithm>
#include "xfparams.h"

namespace Hntea {
using namespace std;
class ConfigResolver {
public:
	ConfigResolver(std::string file);
	virtual ~ConfigResolver();
	void process();
	XfBasic getXfBasic();
	XfOnlineASR getXfOnlineParams();
	XfLocalASR getXfLocalParams();

private:
	XfBasic _basic;
	XfOnlineASR _online;
	XfLocalASR _local;
	std::string _file;
};

} /* namespace Hntea */

#endif /* SRC_ASR_SERVER_LIB_CONFIGRESOLVER_H_ */
