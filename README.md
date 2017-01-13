# speech-system-zh


#说明
该功能包意在结合ROS框架开发出具有中文语音交互功能的语音交互系统；还在开发中..欢迎有兴趣的伙伴加入！

##系统版本 Ubuntu14.04 / Ros-indigo



##目前完成的功能有：

1. **音频信号采集**: 支持ALSA挂载设备，Kinect2/logiUSB 摄像头等
2. **音频信号滤波**：预备加重、加窗
3. **特征提取**：短时过零、对数能量、短时平均幅度差（AMDF）、功率普
4. **语音端点检测算法**：时域双特征双阈值
3. **科大讯飞在线听写**
4. **科大讯飞离线命令词识别**
5. **百度在线语音识别**
6. **百度、讯飞在线语音合成**
7. **图灵在线语义理解**
8. **语音播放**：VLC播放引擎，支持大多音频格式
5. **画图辅助**： 打印音频信号


##待开发功能：

8. 离线意图推理
9. 本地与云端资源调度

##依赖库

###C/C++ 第三方库安装说明
####ALSA : [ALSA Install](http://blog.csdn.net/u013494117/article/details/52269463)
####C\++ Json : [C++ json Install](http://blog.csdn.net/u013494117/article/details/53213134)
####Aquila : [Aquila C++ DSP library](http://aquila-dsp.org/download/)
**注意**
Aquila包安装时需要注意：安装后检查 /usr/local/lib/ 目录下是否存在 libOoura_fft.a;如果没有，则需要手动将：/Aquila/build/lib/libOoura_fft.a 复制到上述目录中。

####Eigen:[Eigen C++ 矩阵分析库](http://eigen.tuxfamily.org/index.php?title=Main_Page)
Eigen 安装方法：
1. 下载最新版本
1. 解压库文件
1. 进入库文件目录
```
cd ~/eigen-eigen-f562a193118d
mkdir build
cmake .
sudo make install
```

####VLC: [VLC 多媒体驱动引擎](https://wiki.videolan.org/Documentation:Documentation/)
VLC 安装方法:
1. 在Ubuntu软件中心安装 VLC 播放器
1. 安装开发库
 ```
 apt-get install libvlccore-dev libvlc-dev
 ```



###Python 第三方库安装说明
####Python-pinyin : [python-pinyin](https://github.com/mozillazg/python-pinyin)

#各包功能说明
1. **audio_capture ：**
   提供音频捕获功能，只要是通过 ALSA 驱动挂载的音频设备，均可使用该包获取音频信号，默认采样频率16k,单通道、样本长度16位、捕获块大小512字节（建议修改保持256的倍数），各参数可通过启动文件调整
   
2. **audio_msgs：**
   该消息包提供自定义音频消息，系统所有的音频消息在该包下定义，也是整个系统的基础

2. **audio_filter**
   该包功能用于对音频信号进行滤波处理，目前的功能有 音频预加重、音频加窗

3. **audio_fectures**
   该包提供音频特征提取功能，目前有：短时能量、短时过零、短时平均幅度、短时功率普

4. **audio_fft_factory**
   该包目的在于提供傅立叶变换：目前只有正变换、逆变换还没处理

5. **speech_vad**
   该包主要是用于语音端点检测，目的是包含多种算法，目前只有双阈值检测算法，其它算法有待添加

6. **training_tools**
   该包主要功能是对相关数据进行训练、提取有用的特征信号，辅助其它功能包

7. **asr_brige**
   该包主要用于 生成音频文件、缓存器（语音端点前后的数据）；用于桥接百度服务与讯飞等服务器或本地处理
8. **asr_server**
   该功能包使用第三方提供的语音服务，分在线语音识别与离线命令词识别，目前在线支持百度和讯飞，离线只有讯飞	
8. **system_launch**
   用于一键启动配置过的功能包
   
9. **tts_server**
   该功能包提供语音合成功能，支持百度与讯飞
   
10. **nlu_server**
   该功能包提供在线语义理解、离线意图推理
11. **audio_play**
   音频播放-播放使用默认声卡，VLC播放引擎支持多种音频格式

12.   **lib**
   该包包括第三方库、自己开发的类库

9. **plot_data**
   该包主要目的是用于配合 rqt_plot 工具，方便分析音频信号
   
#使用方法

1. 创建工作目录

	建议在 home 目录下创建 **~/.SpeechSystem/** ，并将 **resource/** 目录下的子目录复制到此处；下载科大讯飞 **Linux SDK**，并把 libmic.so 复制到**/usr/local/lib**目录下 ，同时把讯飞 SDK/bin/msc/rec/common.jet 复制到下面的 **xf-source/asr-local-model/asr/**目录下 。详细目录结构如下所示：
```
hntea@HnteaPC:~$ tree ./.SpeechSystem/ -d
./.SpeechSystem/
├── audio-file		//存放 asr_brage 中 filemanager 节点产生的音频文件
├── baidu			//存放token
├── config			//存放第三方语音服务配置参数
├── statistic-features	//存放统计特征参数
└── xf-source			//存放科大讯飞资源
    ├── asr-local-model
    │   └── asr
    │       └── GrmBuilld
    │           └── temp
    └── msc				
        └── 22624bb0d1c739e1b32af5966240821b
11 directories
```
**注意：凡是用到第三方的语音服务平台，一律使用 config/config.json 文件配置，使用时请配置好自己的资源参数！**

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

	配置 工作目录中的 **config/config.json** 文件;配置文件中描述讯飞与其它平台的各种服务参数。可针对修改。其中：
	- 	**???:**  帐号信息
	- 	**xxx:**  PC登录用户名
	
1. 编译说明：

   在安装好必要的库后，创建catkin工作空间，首先生效工作空间并编译消息包,最后再全部编译：
   ````
   source devel/setup.bash
   catkin_make --pkg audio_msgs
   catkin_make .
   ````
2.  当编译通过并配置好相应的文件，先让系统启动一段时间，之后查看** ～/.SpeechSystem/statistic-features/eg_zc_average.json** 文件，查看相应的值，根据该值为**端点检测配置阈值**

3. 使用示例
```
roslaunch system_launch system.launch
```
之后对着麦克风说话，如果没有检测到端点信号，请查看打印消息

	```
		========== History Noise Leve =========
	_znoise.up = 269.080780    _znoise.low = 212.797470
	_enoise.up = 14.152426     _enoise.low = 12.870467

	Recommend you seting as follow:
					[energyUpthreshold] = [18.152426]
					[zeroUpthreshhold] = [134.540390]
	
	```
**设置 speech_vad/launch 文件中的参数，过零调高与能量调低以增加灵敏度。**
	


##使用示例
###**Author:Hntea-hong   From: Findlab404**