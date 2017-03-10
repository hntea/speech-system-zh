/*
 * feature_set.cpp
 *
 *  Created on: 2017年2月27日
 *      Author: hntea
 *
 * 函数功能：多特征采集
 */

#include <iostream>
#include <vector>
#include "audio_msgs/AudioData.h"
#include "audio_msgs/AudioFeatureSet.h"
#include "ros/ros.h"
#include "FeatureSet.h"


ros::Publisher _pub;
float _frq;

void callback(const audio_msgs::AudioData &msgs)
{
	std::vector<int16_t> src(msgs.data);
	Hntea::FeatureSet features(src,_frq);
	audio_msgs::AudioFeatureSet features_msgs;
	features_msgs.logenergy = features.logEnergy();
	features_msgs.mean = features.mean();
	features_msgs.zerocrossing = features.zeroCrossing();

	std::vector<double> mfccset;
	features.mfccFeatures(mfccset,12);
	features_msgs.mfcc.resize(mfccset.size());
	for(int i=0;i<mfccset.size();i++)
	{
		features_msgs.mfcc[i] = mfccset[i];
	}

	_pub.publish(features_msgs);
}

int main (int argc, char **argv)
{
	ros::init(argc, argv, "feature_set");
	if(!ros::param::get("/audio_capture/sample_rate",_frq))
	{
		_frq = 16000;
	}
	ros::NodeHandle n;
	ros::Subscriber sb = n.subscribe("hamming_window",50,callback);
	_pub = n.advertise<audio_msgs::AudioFeatureSet>("audio_feature/feature_set",1000);
	ros::spin();
	return 0;
}


