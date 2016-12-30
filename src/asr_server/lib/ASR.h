/*
 * ASR.h
 *
 *  Created on: 2016年12月28日
 *      Author: hntea
 */

#ifndef SRC_ASR_SERVER_LIB_ASR_H_
#define SRC_ASR_SERVER_LIB_ASR_H_
#include <iostream>
#include <vector>

namespace Hntea {
class ASR {
public:
	ASR();
	virtual ~ASR();
	virtual void runasr(std::string input);
	virtual void runasr(std::vector<int16_t>& input,bool end);
	virtual std::string getResult();
protected:
	std::string _result;
};

} /* namespace Hntea */

#endif /* SRC_ASR_SERVER_LIB_ASR_H_ */
