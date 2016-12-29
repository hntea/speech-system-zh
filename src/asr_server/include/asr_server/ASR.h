/*
 * ASR.h
 *
 *  Created on: 2016年12月28日
 *      Author: hntea
 */

#ifndef SRC_ASR_SERVER_LIB_ASR_H_
#define SRC_ASR_SERVER_LIB_ASR_H_

namespace Hntea {

class ASR {
public:
	ASR();
	virtual ~ASR();

	template<typename T>
	virtual void runasr(T& input){}

protected:
	std::string _result;
};

} /* namespace Hntea */

#endif /* SRC_ASR_SERVER_LIB_ASR_H_ */
