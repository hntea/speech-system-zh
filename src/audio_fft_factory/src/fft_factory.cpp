/*
 * fft_factory.cpp
 *
 *  Created on: 2016年12月9日
 *      Author: hntea
 */


#include <ros/ros.h>
#include "audio_msgs/AudioData.h"
#include "audio_msgs/TimeFeature.h"
#include "audio_msgs/FreqData.h"
#include <vector>
#include <aquila/global.h>
#include "aquila/transform/AquilaFft.h"
#include "aquila/transform/FftFactory.h"
#include "aquila/transform/OouraFft.h"
namespace Hntea{
/*
 * 需要定义一个循环缓冲区，每次只变换512字节。
 * */
class RosFftFactory{
public:
	RosFftFactory(std::size_t length){
		_sample = new double[512]{0};
		_sb = _nh.subscribe("hamming_window",50,chapterCallback);
		_pub = _nh.advertise<audio_msgs::FreqData>("fft_result",1000);
		ros::spin();
	}
	virtual ~RosFftFactory(){
		delete []_sample;
		std::cout<<"~RosFftFactory()"<<std::endl;
	}

	static void  chapterCallback(const audio_msgs::AudioData &msgs){
		std::vector<int16_t> src_vec(msgs.data);
		std::vector<int16_t>::iterator src_iter = src_vec.begin();

		//注意傅立叶变换-基2算法的点数必须是2的幂级数形式！
		Aquila::AquilaFft factory(512);
		msgToarray(src_vec,_sample);
		Aquila::SpectrumType spectrum = factory.fft(_sample);
		std::vector<Aquila::ComplexType>::iterator iter = spectrum.begin();
		audio_msgs::FreqData data;
		data.real.resize(msgs.data_size);
		data.image.resize(msgs.data_size);
		//变换后的数据是对称的，只需要取一半。
		for(int i=0;i<spectrum.size()/2;i++){
			data.image[i] = std::imag(iter[i]);
			data.real[i] = std::real(iter[i]);
		}
		_pub.publish(data);
		ROS_INFO_THROTTLE(60,"Audio FFT Factory is OK");
	}
	static void msgToarray(std::vector<int16_t> &src,Aquila::SampleType* des){
		std::vector<int16_t>::iterator src_iter = src.begin();
		for(int i=0;i<src.size();i++){
			des[i] = src_iter[i];
		}
	}

private:
	ros::NodeHandle _nh;
    ros::Subscriber _sb;
    static ros::Publisher _pub;
    static Aquila::SampleType *_sample;
};
ros::Publisher RosFftFactory::_pub;
Aquila::SampleType *RosFftFactory::_sample;
}
int main (int argc, char **argv)
{
  int winlen;
  ROS_INFO ("Ros Node Name : audio_fft_factory");
  ROS_INFO ("Ros Node Subscribe : audio_pre_emphasis");
  ROS_INFO ("Ros Node Publish Topic : fft_result");
  ros::init(argc, argv, "audio_fft_factory");
  bool yes = ros::param::get("/audio_capture/block_size",winlen);
  if(yes){
	  std::cout<<"Set FFT block size from [/audio_capture/block_size],is ["<<winlen<<"]"<<std::endl;
  }else{
	  std::cout<<"Set FFT block size from [launch file],default size [320]"<<std::endl;
	  ros::param::param<int>("~winlen",winlen,512);
  }
  Hntea::RosFftFactory factory(winlen);
}
