/*
 * ASR.cpp
 *
 *  Created on: 2016年12月28日
 *      Author: hntea
 */

#include "ASR.h"

namespace Hntea {

ASR::ASR() {
	_result = " ";
}

ASR::~ASR() {
	// TODO Auto-generated destructor stub
}
void ASR::runasr(std::string input,std::string& result){
	result = _result;
}
void ASR::runasr(std::vector<int16_t>& input,std::string& result,bool end){
	result = _result;
}

} /* namespace Hntea */
