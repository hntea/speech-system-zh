/*
 * AudioDeviceBace.cpp
 *
 *  Created on: 2016年11月8日
 *      Author: hntea
 */

#include "../include/AudioDeviceBace.h"
AudioDeviceBace::AudioDeviceBace(){}
AudioDeviceBace::AudioDeviceBace(std::string name,unsigned int  rate,char channle,
		char sample,enum _MODE mode) :
		_name(name),_rate(rate),_channle(channle),_sample(sample),_mode(mode),_handle(NULL){
}

AudioDeviceBace::~AudioDeviceBace() {
//	delete []_buf;
	// TODO Auto-generated destructor stub
}

void AudioDeviceBace::init(AudioDeviceBace& device)
{
	int i;
	int err;
	snd_pcm_hw_params_t *hw_params;

	switch(device._mode)
	{
	case 0:
		if ((err = snd_pcm_open (&device._handle, device._name.c_str(), SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
			fprintf (stderr, "cannot open audio device %s (%s)\n",device._name.c_str(),snd_strerror (err));
			exit (1);
		}
				break;
	case 1:
		if ((err = snd_pcm_open (&device._handle, device._name.c_str(), SND_PCM_STREAM_CAPTURE, 0)) < 0) {
			fprintf (stderr, "cannot open audio device %s (%s)\n",device._name.c_str(),snd_strerror (err));
			exit (1);
		}
				break;
	default:
		if ((err = snd_pcm_open (&device._handle, device._name.c_str(), SND_PCM_STREAM_CAPTURE, 0)) < 0) {
			fprintf (stderr, "cannot open audio device %s (%s)\n",device._name.c_str(),snd_strerror (err));
			exit (1);
		}
				break;

	}

	//申请硬件参数配置空间
	if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
		fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
		snd_strerror (err));
		exit (1);
	}

	//填充参数空间
	if ((err = snd_pcm_hw_params_any (device._handle, hw_params)) < 0) {
		fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
		snd_strerror (err));
		exit (1);
	}

	//配置处理模式
	if ((err = snd_pcm_hw_params_set_access (device._handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		fprintf (stderr, "cannot set access type (%s)\n",
		snd_strerror (err));
		exit (1);
	}

	//配置样本值,默认为16位
	switch(device._sample)
	{
		case 8:
			if ((err = snd_pcm_hw_params_set_format (device._handle, hw_params, SND_PCM_FORMAT_U8 )) < 0) {
				fprintf (stderr, "cannot set sample format (%s)\n",
				snd_strerror (err));
				exit (1);
			}
			break;
		case 16:
			if ((err = snd_pcm_hw_params_set_format (device._handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
				fprintf (stderr, "cannot set sample format (%s)\n",
				snd_strerror (err));
				exit (1);
			}
			break;
		default:
			if ((err = snd_pcm_hw_params_set_format (device._handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
				fprintf (stderr, "cannot set sample format (%s)\n",
				snd_strerror (err));
				exit (1);
			}
			break;
	}

	//配置采样频率
	if ((err = snd_pcm_hw_params_set_rate_near (device._handle, hw_params, &device._rate, 0)) < 0) {
		fprintf (stderr, "cannot set sample rate (%s)\n",
		snd_strerror (err));
		exit (1);
	}

	//配置通道数
	if ((err = snd_pcm_hw_params_set_channels (device._handle, hw_params, device._channle)) < 0) {
		fprintf (stderr, "cannot set channel count (%s)\n",
		snd_strerror (err));
		exit (1);
	}

	//写入参数
	if ((err = snd_pcm_hw_params (device._handle, hw_params)) < 0) {
		fprintf (stderr, "cannot set parameters (%s)\n",
		snd_strerror (err));
		exit (1);
	}

	//释放参数空间
	snd_pcm_hw_params_free (hw_params);

	if ((err = snd_pcm_prepare (device._handle)) < 0) {
		fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
		snd_strerror (err));
		exit (1);
	}

}
void AudioDeviceBace::deviceCfg(std::string name,char channle,char sample,unsigned int rate, enum _MODE mode){
	this->_channle = channle;
	this->_name = name;
	this->_rate = rate;
	this->_mode = mode;
	this->_sample = sample;
}
void AudioDeviceBace::init(){

	int i;
	int err;
	snd_pcm_hw_params_t *hw_params;

	switch(_mode)
	{
	case 0:
		if ((err = snd_pcm_open (&_handle, _name.c_str(), SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
			fprintf (stderr, "cannot open audio device %s (%s)\n",_name.c_str(),snd_strerror (err));
			exit (1);
		}
				break;
	case 1:
		if ((err = snd_pcm_open (&_handle,_name.c_str(), SND_PCM_STREAM_CAPTURE, 0)) < 0) {
			fprintf (stderr, "cannot open audio device %s (%s)\n",_name.c_str(),snd_strerror (err));
			exit (1);
		}
				break;
	default:
		if ((err = snd_pcm_open (&_handle, _name.c_str(), SND_PCM_STREAM_CAPTURE, 0)) < 0) {
			fprintf (stderr, "cannot open audio device %s (%s)\n",_name.c_str(),snd_strerror (err));
			exit (1);
		}
				break;

	}

	//申请硬件参数配置空间
	if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
		fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
		snd_strerror (err));
		exit (1);
	}

	//填充参数空间
	if ((err = snd_pcm_hw_params_any (_handle, hw_params)) < 0) {
		fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
		snd_strerror (err));
		exit (1);
	}

	//配置处理模式
	if ((err = snd_pcm_hw_params_set_access (_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		fprintf (stderr, "cannot set access type (%s)\n",
		snd_strerror (err));
		exit (1);
	}

	//配置样本值,默认为16位
	switch(_sample)
	{
		case 8:
			if ((err = snd_pcm_hw_params_set_format (_handle, hw_params, SND_PCM_FORMAT_U8 )) < 0) {
				fprintf (stderr, "cannot set sample format (%s)\n",
				snd_strerror (err));
				exit (1);
			}
			break;
		case 16:
			if ((err = snd_pcm_hw_params_set_format (_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
				fprintf (stderr, "cannot set sample format (%s)\n",
				snd_strerror (err));
				exit (1);
			}
			break;
		default:
			if ((err = snd_pcm_hw_params_set_format (_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
				fprintf (stderr, "cannot set sample format (%s)\n",
				snd_strerror (err));
				exit (1);
			}
			break;
	}

	//配置采样频率
	if ((err = snd_pcm_hw_params_set_rate_near (_handle, hw_params, &_rate, 0)) < 0) {
		fprintf (stderr, "cannot set sample rate (%s)\n",
		snd_strerror (err));
		exit (1);
	}

	//配置通道数
	if ((err = snd_pcm_hw_params_set_channels (_handle, hw_params, _channle)) < 0) {
		fprintf (stderr, "cannot set channel count (%s)\n",
		snd_strerror (err));
		exit (1);
	}

	//写入参数
	if ((err = snd_pcm_hw_params (_handle, hw_params)) < 0) {
		fprintf (stderr, "cannot set parameters (%s)\n",
		snd_strerror (err));
		exit (1);
	}

	//释放参数空间
	snd_pcm_hw_params_free (hw_params);

	if ((err = snd_pcm_prepare (_handle)) < 0) {
		fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
		snd_strerror (err));
		exit (1);
	}


}
char AudioDeviceBace::getChannle()
{
	return this->_channle;
}
char AudioDeviceBace::getSample()
{
	return this->_sample;
}
unsigned int AudioDeviceBace::getRate()
{
	return this->_rate;
}


