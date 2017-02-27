/*
 * FeatureSet.h
 *
 *  Created on: 2017年2月27日
 *      Author: hntea
 */

#ifndef SRC_AUDIO_FECTURES_SRC_FEATURE_SET_FEATURESET_H_
#define SRC_AUDIO_FECTURES_SRC_FEATURE_SET_FEATURESET_H_

#include <iostream>
#include <vector>
#include <numeric>
#include "aquila/global.h"
#include "aquila/source/SignalSource.h"

namespace Hntea {

class FeatureSet: public Aquila::SignalSource
{
public:
	FeatureSet();
	FeatureSet(const std::vector<int16_t>& source,double frequence);
	virtual ~FeatureSet();


	/*
	 * 函数功能：均值统计
	 * 参数说明：
	 * 返回值：均值
	 * */
	float mean();

	/*
	 * 函数功能：统计过零率
	 * 参数说明：
	 * 返回值：过零率
	 * */
	float zeroCrossing();

	/*
	 * 函数功能：统计对数能量
	 * 参数说明：
	 * 返回值：对数能量
	 * */
	float logEnergy();

private:


};

} /* namespace Hntea */

#endif /* SRC_AUDIO_FECTURES_SRC_FEATURE_SET_FEATURESET_H_ */
