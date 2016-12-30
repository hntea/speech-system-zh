/*
 * ASR.cpp
 *
 *  Created on: 2016年12月28日
 *      Author: hntea
 */

#include "ASR.h"

namespace Hntea {

ASR::ASR() {
	// TODO Auto-generated constructor stub
}

ASR::~ASR() {
	// TODO Auto-generated destructor stub
}
void ASR::runasr(std::string input){
	 _result = " ";
}
void ASR::runasr(std::vector<int16_t>& input,bool end){
	 _result = " ";
}
std::string ASR::getResult(){
	return _result;
}
} /* namespace Hntea */
