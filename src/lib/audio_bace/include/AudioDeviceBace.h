/*
 * AudioDeviceBace.h
 *
 *  Created on: 2016年11月8日
 *      Author: hntea
 */

#ifndef SRC_KINECT_SPEECH_SRC_AUDIODEVICEBACE_H_
#define SRC_KINECT_SPEECH_SRC_AUDIODEVICEBACE_H_

#include <alsa/asoundlib.h>
#include <iostream>
enum _MODE{
	MODE_PLAYER = 0,
	MODE_RECPRDER,
};


class AudioDeviceBace {
public:
	AudioDeviceBace();
	AudioDeviceBace(std::string name,unsigned int  rate,char channle,
			char sample,enum _MODE mode);
	virtual ~AudioDeviceBace();

	//配置
	void init(AudioDeviceBace& device);
	void init();
	void deviceCfg(std::string name,char channle,char sample,unsigned int rate, enum _MODE mode);
	//获取硬件配置
	char getChannle();
	char getSample();
	unsigned int getRate();


	//录音
	/*
	* 函数功能：读取声卡数据
	* 参数说明：
	* 					buf:存储PCM数据
	* 					size:读取字节数
	*
	* */
	template <typename T>
	void read( T* buf,int size);

	//播放
	template <typename T>
	void write(T* src,int size);



private:
	std::string _name;
	enum _MODE _mode;				//录音或播放 0为播放，1为录音
	_snd_pcm* _handle;				//回话语句柄
	unsigned int  _rate;			//采样频率
	char _channle;					//通道数
	char _sample;					//样本精度


};


//模板类/成员函数必须在同一个文件中实现
template <typename T>
void  AudioDeviceBace::read( T* buf,int size)
{
	int err = 0;
	if ((err = snd_pcm_readi (_handle,buf,size))
			!= size)
	{
		fprintf (stderr, "read from audio interface failed (%s)\n",
		snd_strerror (err));
		exit (1);
	}
}


template <typename T>
void AudioDeviceBace::write(T* src,int size)
{
	int err = 0;
	if ((err = snd_pcm_writei (_handle,src,size)) != size)
	{
		fprintf (stderr, "write to audio interface failed (%s)\n",
		snd_strerror (err));
		exit (1);
	}
}


#endif /* SRC_KINECT_SPEECH_SRC_AUDIODEVICEBACE_H_ */
