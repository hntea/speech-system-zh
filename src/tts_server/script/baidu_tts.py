#!/usr/bin/env python
#-*- coding:utf-8 -*-
#
# baidu_tts.py
# Created on: 2017年1月4日
#    Author: hntea
#

import BaiduTTSClientManager as tts
import sys
import rospy
from pypinyin import pinyin, lazy_pinyin
from std_msgs.msg import String

#设置默认编码格式-勿动
reload(sys)
sys.setdefaultencoding( "utf-8" )
  
  
class BaiduTTS:
    def __init__(self):
        subtopic = rospy.get_param("~subtopic","/asr_server/xf/online_s_res")
        self.tts_man = tts.BaiduTtsClientManager()
        rospy.init_node('baidu_tts', anonymous=True)
        rospy.Subscriber(subtopic,String,self.process)
        self.pub = rospy.Publisher("asr_tts/baidu_tts",String,queue_size=10)
        rospy.spin()


    def process(self,data):
        if(data.data != "Null"):
            try:
                print "Change ",data.data," to speak"
                self.tts_man.runTTS(data.data,"audio.mp3")
            except:
                rospy.loginfo("[baidu_tts] Time out!")    
            
if __name__ == '__main__':
    BaiduTTS()