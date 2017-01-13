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

	virtual ~AudioDeviceBace();

	/*
	 * 函数功能：配置声卡
	 * 参数说明：
	 * 			name:设备名
	 * 			channel: 通道数
	 * 			sample： 样本长度
	 * 			rate ： 采样频率
	 * 			mode： 读/写 模式
	 * 			access：交错/非交错 模式
	 * */
	void config(std::string name,char channel,char sample,unsigned int rate, enum _MODE mode,_snd_pcm_access access);





	//获取硬件配置
	char getChannle();
	char getSample();
	unsigned int getRate();


	/*
	* 函数功能：读取声卡数据 单通道
	* 参数说明：
	* 					buf:存储PCM数据
	* 					size:帧数
	*
	* */
	template <typename T>
	void read( T* buf,int size);

	/*
	* 函数功能：读取声卡数据 多通道
	* 参数说明：
	* 					buf:存储PCM数据
	* 					size:帧数
	*
	* */
	template <typename T>
	void  readn( T** buf,int size);

	/*
	 * 函数功能：写数据到声卡
	 * 参数说明：
	 * 			src：单通道数据
	 * 			size: 帧数
	 * */
	template <typename T>
	void write(T* src,int size);

	/*
	 * 函数功能：写数据到声卡
	 * 参数说明：
	 * 			src：多通道数据
	 * 			size: 帧数
	 * */
	template <typename T>
	void writen(T** src,int size);


private:
	std::string _name;
	enum _MODE _mode;				//录音或播放 0为播放，1为录音
	_snd_pcm_access _access;			//数据存放方式
	_snd_pcm* _handle;				//回话语句柄
	unsigned int  _rate;			//采样频率
	char _channle;					//通道数
	char _sample;					//样本精度


};


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
void  AudioDeviceBace::readn( T** buf,int size)
{
	int err = 0;
	if ((err = snd_pcm_readn(_handle,(void**)buf,size)) != size)
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

template <typename T>
void AudioDeviceBace::writen(T** src,int size)
{
	int err = 0;
	if ((err = snd_pcm_writei (_handle,(void**)src,size)) != size)
	{
		fprintf (stderr, "write to audio interface failed (%s)\n",
		snd_strerror (err));
		exit (1);
	}
}

#endif /* SRC_KINECT_SPEECH_SRC_AUDIODEVICEBACE_H_ */
