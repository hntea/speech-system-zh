/*
 * capture_nchannle.cpp
 *
 *  Created on: 2017年1月12日
 *      Author: hntea
 */



#include "../../lib/audio_bace/include/AudioDeviceBace.h"
#include <ros/ros.h>
#include "audio_msgs/AudioData.h"

namespace Hntea{

/*
 * 函数功能：分配二维数组
 * 参数说明：
 * 			row: 行数
 * 			line: 列数
 * */
template<typename T>
T** malloc2D(int row,int line){
	T **array2D;
	array2D = new T *[row];
	for( int i=0; i<row; i++ )
	{
	    array2D[i] = new T [line]  ;
	}

	return array2D;
}

/*
 * 函数功能：释放二维数组
 * 参数说明：
 * 			row: 行数
 * 			array2D：数组空间
 * */
template<typename T>
void free2D(T **array2D,int row){
	for( int i=0; i<row; i++ )
	{
		delete [] array2D[i];
	}
	delete array2D;
}

class Kinect2AudioCapture{
public:
	Kinect2AudioCapture(){

		//使用参数解析器，初始化块大小 200ms
        ros::param::param<int>("~block_size", _size, 512);
        ros::param::param<int>("~channels", _channle, 4);
        ros::param::param<int>("~depth", _sample, 16);
        ros::param::param<int>("~sample_rate", _rate, 16000);
        ros::param::param<std::string>("~name", _name, "plughw:2,0");


		 _data = malloc2D<short>(_channle,_size);

        _dev.config(_name,_channle,_sample,_rate,MODE_RECPRDER,SND_PCM_ACCESS_RW_NONINTERLEAVED);


        _pub = _nh.advertise<audio_msgs::AudioData>("audio_data", 50, true);
        audio_msgs::AudioData msg;
        //重新消息定义数组大小
       msg.data.resize(_size);
	   msg.data_size = _size;
	   while(ros::ok())
	   {
		   _dev.readn(_data,_size);
		   for(int i=0;i<_channle;i++)
		   {
			   for(int j=0;j<_size;j++)
			   {
				   msg.data[j] = _data[i][j];
			   }
				_pub.publish(msg);
		   }

		ros::spinOnce();
		}
	}
	~Kinect2AudioCapture(){
		free2D(_data,_channle);
	}





private:
    ros::NodeHandle _nh;
    ros::Publisher _pub;
    AudioDeviceBace _dev;
    short** _data;

    int  _size;
    std::string _name;
    int  _channle,_sample, _rate;
};
}

int main (int argc, char **argv)
{
  ros::init(argc, argv, "audio_capture");
  Hntea::Kinect2AudioCapture server;
}

