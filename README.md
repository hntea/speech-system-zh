# speech-system-zh


##说明
该功能包意在结合ROS框架开发出具有中文语音交互功能的语音交互系统；还在开发中..

##计划完成的功能
1. 音频采集
2. 语音信号预加重
3. 特征提取
3. 语音端点检测
4. 在线语音识别
5. 在线语义理解
6. 在线语音合成
7. 离线命令词识别
8. 离线意图推理
9. 本地与云端资源调度

##目前完成的功能有：

1. 音频信号采集
2. 音频信号预加重
3. 短时过零、对数能量特征提取
3. 科大讯飞在线听写
4. 科大讯飞离线命令词识别
5. rqt_plot 辅助包打印音频信号

##包依赖


[ALSA Install](http://blog.csdn.net/u013494117/article/details/52269463)
 
[c++ json Install](http://blog.csdn.net/u013494117/article/details/53213134)

[Aquila C++ DSP library](http://aquila-dsp.org/articles/examples/)


##使用方法

1. 创建工作目录

	建议在 home 目录下创建 **~/.SpeechSystem/** 并将 speech_server_bace 中的文件复制到此处；科大讯飞的库需要改成自己申请的，把 libmic.so 代替，并且把本地识别库也一并替换。

2. 配置启动文件
    
	使用 
	```
	arecord list 
	```
	或者 
	```
	cat /proc/asound/pcm
	```
	查看音频设备并正确配置 **`audio_capture/launch/capture.launch`** 文件
	
	例如：博主使用的机子上有 kinect2.0 和 罗技的usb摄像头：
	```
	hntea@HnteaPC:~/gitSoft$ cat /proc/asound/pcm 
	00-03: HDMI 0 : HDMI 0 : playback 1
	01-00: ALC887-VD Analog : ALC887-VD Analog : playback 1 : capture 1
	01-02: ALC887-VD Alt Analog : ALC887-VD Alt Analog : capture 1
	02-00: USB Audio : USB Audio : capture 1
	03-00: USB Audio : USB Audio : capture 1

	hntea@HnteaPC:~/gitSoft$ arecord -l
	**** CAPTURE 硬體裝置清單 ****
	card 1: PCH [HDA Intel PCH], device 0: ALC887-VD Analog [ALC887-VD Analog]
	  子设备: 1/1
	  子设备 #0: subdevice #0
	card 1: PCH [HDA Intel PCH], device 2: ALC887-VD Alt Analog [ALC887-VD Alt Analog]
	  子设备: 1/1
	  子设备 #0: subdevice #0
	card 2: U0x46d0x825 [USB Device 0x46d:0x825], device 0: USB Audio [USB Audio]
	  子设备: 1/1
	  子设备 #0: subdevice #0
	card 3: Sensor [Xbox NUI Sensor], device 0: USB Audio [USB Audio]
	  子设备: 1/1
	  子设备 #0: subdevice #0

	```
	如果想使用使用 kinect(Xbox NUI);则将launch文件中的 name 改成 **plughw:3,0**
	
	使用 `speech_asr_online` 包下的启动文件配置 **config.json** 文件所在地址。
	
3. 配置服务文件

	配置 工作目录中的 **config/config.json** 文件;配置文件中描述讯飞与其它平台的各种服务参数。可针对修改。

-- hntea-hong
