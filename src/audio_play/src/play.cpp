/*
 * play_mp3.cpp
 *
 *  Created on: 2017年1月5日
 *      Author: hntea
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vlc/vlc.h>
#include <vlc/libvlc.h>
#include <vlc/libvlc_events.h>
#include <vlc/libvlc_media_discoverer.h>
#include <vlc/libvlc_media_player.h>




bool end = false;
void libvlc_callback( const struct libvlc_event_t * event, void *userdata ){
	end = true;
}


int main(int argc, char **argv)
{
	libvlc_instance_t *inst;
	libvlc_media_t *m;
	libvlc_media_player_t *mp;
    // load the vlc engine
    inst = libvlc_new(0, NULL);

    if(argc < 2){
    	printf("Please input: rosrun audio_paly play [file]\n");
    	return -1;
    }

    // create a new item
    m = libvlc_media_new_path(inst, argv[1]);

    // create a media play playing environment
    mp = libvlc_media_player_new_from_media(m);

    // no need to keep the media now
    libvlc_media_release(m);


    // play the media_player
    libvlc_media_player_play(mp);


    libvlc_event_manager_t *vlcEventManager = libvlc_media_player_event_manager(mp);

    // second manager
//    libvlc_event_manager_t *vlcEventManager = libvlc_media_list_player_event_manager(lp);

    int ret = libvlc_event_attach(vlcEventManager,libvlc_MediaPlayerEndReached,libvlc_callback,NULL);


    while(!end)
    	usleep(1000);

    // stop playing
    libvlc_media_player_stop(mp);

    // free the media_player
    libvlc_media_player_release(mp);

    libvlc_release(inst);


    return 0;
}

