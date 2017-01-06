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
from pypinyin import lazy_pinyin
import os
#设置默认编码格式-勿动
reload(sys)
sys.setdefaultencoding( "utf-8" )
  
  
class BaiduTTS:
    

    
    def __init__(self):
        self.__creatWorkspace("/tmp/baidu_tts")
        
        subtopic = rospy.get_param("~subtopic","/asr_server/xf/online_s_res")
        self.tts_man = tts.BaiduTtsClientManager()
        rospy.init_node('baidu_tts', anonymous=True)
        rospy.Subscriber(subtopic,String,self.process)
        self.pub = rospy.Publisher("tts_server/baidu_tts",String,queue_size=10)
        rospy.spin()

    def __creatWorkspace(self,dir):
        if not os.path.exists(dir):
            os.mkdir(dir)
      
      
    def nameForAudioFile(self,dir,data):
        name  = ""
        udata = unicode(data,"utf-8")
        pyin = lazy_pinyin(udata)
        for n in pyin:
            name += n       
        name += ".mp3"
        filename = os.path.join(dir,name)
        filename = filename.replace('。','')
        return filename 
            
    def process(self,data):
        if(data.data != "Null"):
            rospy.loginfo("正在合成："+data.data)
            file =  self.nameForAudioFile("/tmp/baidu_tts",data.data)
            self.tts_man.runTTS(data.data,file)
            self.pub.publish(file)
            
if __name__ == '__main__':
    BaiduTTS()