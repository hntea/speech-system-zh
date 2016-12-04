# speech-system-zh

该功能包意在结合ROS框架开发出具有中文语音交互功能的语音交互系统；现在还在开发中..

##说明
该功能包意在结合ROS框架开发出具有中文语音交互功能的语音交互系统；还在开发中..

##计划完成的功能
1. 音频采集
2. 语音滤波预处理
3. 语音端点检测
4. 在线语音识别
5. 在线语义理解
6. 在线语音合成
7. 离线命令词识别
8. 离线意图推理
9. 本地与云端资源调度

##目前完成的功能有：

1. 音频信号采集
2. 科大讯飞在线听写
3. 科大讯飞离线命令词识别

##包依赖


[ALSA Install](http://blog.csdn.net/u013494117/article/details/52269463)
 
[c++ json Install](http://blog.csdn.net/u013494117/article/details/53213134)


##使用方法

1. 创建工作目录

	建议在 home 目录下创建 **~/.SpeechSystem/** 并将 speech_server_bace 中的文件复制到此处。

2. 配置启动文件
    
	使用 `arecord list` 查看音频设备并正确配置 `audio_capture/launch/capture.launch` 文件
	使用 `speech_asr_online` 包下的启动文件配置 **config.json** 文件所在地址。
	

3. 配置服务文件

	配置 工作目录中的 **config/config.json** 文件

-- hntea-hong
