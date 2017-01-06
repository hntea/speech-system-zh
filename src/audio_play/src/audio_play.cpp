/*
 * audio_play.cpp
 *
 *  Created on: 2017年1月6日
 *      Author: hntea
 */

#include <ros/ros.h>
#include <std_msgs/String.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vlc/vlc.h>
#include <vlc/libvlc.h>
#include <vlc/libvlc_events.h>
#include <vlc/libvlc_media_discoverer.h>
#include <vlc/libvlc_media_player.h>

namespace Hntea{

class AuidoPlayer{
public:
	AuidoPlayer(){

		ros::param::param<std::string>("~subtopic",_subtopic,"/tts_server/baidu_tts");
		_sb = _nh.subscribe(_subtopic,10,tts_callback);
		_pub = _nh.advertise<std_msgs::String>("audio_play/play_state",1000);
		ros::spin();
	}
	~AuidoPlayer(){}

	static void  tts_callback(const std_msgs::String &msgs){
		play(msgs.data);
	}

	static void libvlc_callback( const struct libvlc_event_t * event, void *userdata ){
		_end = true;
	}


	static void play(std::string file){
		libvlc_media_t *m;
		_engine = libvlc_new(0, NULL);
	    // create a new item
	    m = libvlc_media_new_path(_engine, file.c_str());

	    // create a media play playing environment
	    _mp = libvlc_media_player_new_from_media(m);
	    libvlc_media_release(m);

	    //play
	    libvlc_media_player_play(_mp);

	    //add listen
	    libvlc_event_manager_t *vlcEventManager = libvlc_media_player_event_manager(_mp);
	    int ret = libvlc_event_attach(vlcEventManager,libvlc_MediaPlayerEndReached,libvlc_callback,NULL);

	    while(!_end)
	    	usleep(1000);

	    stop();
	}

	static void stop(){
	    // stop playing
	    libvlc_media_player_stop(_mp);
	    // free the media_player
	    libvlc_media_player_release(_mp);
	    libvlc_release(_engine);
	    _end = false;
	}
private:
    ros::NodeHandle _nh;
    ros::Subscriber _sb;
    std::string _subtopic;

    static bool _end;
	static libvlc_instance_t* _engine;
	static libvlc_media_player_t* _mp;
    static ros::Publisher _pub;
};
bool AuidoPlayer::_end = false;
libvlc_instance_t* AuidoPlayer::_engine;
libvlc_media_player_t* AuidoPlayer::_mp;
ros::Publisher AuidoPlayer::_pub;
}
int main (int argc, char **argv)
{
  ROS_INFO ("Ros Node Name : audio_play");
  ros::init(argc, argv, "audio_play");
  Hntea::AuidoPlayer player;
}


