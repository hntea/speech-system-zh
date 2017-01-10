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
	/*
	 * 参数说明：
	 * 			input:输入文件
	 * 			result:存放识别结果
	 * */
	virtual void runasr(std::string input,std::string& result);

	/*
	 * 参数说明：
	 * 			input:输入音频块
	 * 			result:存放识别结果
	 * 			end:结束标志
	 * */
	virtual void runasr(std::vector<int16_t>& input,std::string& result,bool end);
protected:
	std::string _result;
};

} /* namespace Hntea */

#endif /* SRC_ASR_SERVER_LIB_ASR_H_ */
