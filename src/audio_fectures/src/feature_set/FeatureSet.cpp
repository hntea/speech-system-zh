/*
 * FeatureSet.cpp
 *
 *  Created on: 2017年2月27日
 *      Author: hntea
 */

#include "FeatureSet.h"

namespace Hntea {

/*
 * 函数功能：MFCC 全局特征标准化
 * 参数说明：
 * 			input: 输入特征集
 * 			output: 输出特征集
 *
 * 返回值：标准化 MFCC 集全局特征标准化,即归一化均值为0，方差为1
 * */
static void mfccCMN(const std::vector<double> &input,std::vector<double> &output)
{
	double sum = std::accumulate(std::begin(input), std::end(input), 0.0);
	double mean = sum / input.size();

	sum = 0;

	for(int i=0;i<input.size();i++)
	{
		sum += (mean-input[i])*(mean-input[i]);
	}
	sum = sum/input.size();

	double xita = std::sqrt(sum);

	for(int i=0;i<input.size();i++)
	{
		output[i] = (input[i]-mean)/xita;
	}

}



FeatureSet::FeatureSet():Aquila::SignalSource() {
	// TODO Auto-generated constructor stub

}

FeatureSet::FeatureSet(const std::vector<int16_t>& source,double frequence)
:Aquila::SignalSource()
{

	m_data.resize(source.size());

	for(int i=0;i<source.size();i++)
	{
		m_data[i] = source[i];
	}

	m_sampleFrequency = frequence;
}

FeatureSet::~FeatureSet() {
	// TODO Auto-generated destructor stub
}

/*
 * 函数功能：均值统计
 * 参数说明：
 * 返回值：均值
 * */
float FeatureSet::mean()
{
	 double sum = std::accumulate(std::begin(m_data), std::end(m_data), 0.0);
	 return sum / m_data.size();
}


/*
 * 函数功能：统计过零率
 * 参数说明：
 * 返回值：过零率
 * */
float FeatureSet::zeroCrossing(){

	float prev = 0.0;
	float ret = 0.0;

	std::vector<double>::iterator iter = m_data.begin();

	for(int i=0;i<m_data.size();i++)
	{
		bool crossing = false;

		float sample = iter[i];

		if(sample >= 0.0)
		{
			if(prev <0.0)
				crossing = true;

		}else if(sample < 0.0)
		{
			if(prev>=0.0)
				crossing = true;
		}
		if(crossing == true)
		{
			ret++;
		}
		prev = sample;
	}

	return ret;
}

/*
 * 函数功能：统计对数能量
 * 参数说明：
 * 返回值：对数能量
 * */
float FeatureSet::logEnergy()
{

	float energy = 0.0;

	std::vector<double>::iterator iter = m_data.begin();

	for(int j=0;j<m_data.size();j++)
	{
		energy += ( ( iter[j]*iter[j] ));
	}
	return 10*log10(energy);

}

/*
 * 函数功能：获取MFCC特征集
 * 参数说明：
 * 			mfccValues: 特征集存放空间
 * 			numFeatures:特征序列个数
 * 返回值：MFCC集
 * */
void FeatureSet::mfccFeatures(std::vector<double> &mfccValues, std::size_t numFeatures)
{
	std::vector<double> tmp(numFeatures);
//	std::vector<double> CMN(numFeatures);

	mfccValues.resize(numFeatures);
	Aquila::Mfcc mfcc(m_data.size());
	Aquila::SignalSource signal(m_data,m_sampleFrequency);
	tmp = mfcc.calculate(signal,numFeatures);

	//均值归一 CMM -- 减弱声学信道扭曲带来的影响
	double sum = std::accumulate(std::begin(tmp), std::end(tmp), 0.0);
	double mean = sum / tmp.size();
	for(int i=0;i<numFeatures;i++)
	{
		tmp[i] = tmp[i] - mean;
	}

	mfccCMN(tmp,mfccValues);
}




} /* namespace Hntea */
