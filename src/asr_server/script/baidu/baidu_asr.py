#!/usr/bin/env python
#-*- coding:utf-8 -*-
#
#  baidu_asr.py
#  Created on: 2016年12月22日
#  Author: hntea
# 
#  文件功能：百度语音识别

import rospy
from std_msgs.msg import String
import BaiduVoiceClientManager as client

  
class BaiduASR:
    def __init__(self):
        
        self.man = client.BaiduVoiceClientManager()
        self.rate = rospy.get_param("/audio_capture/sample_rate",16000)
        
        rospy.init_node('baidu_asr', anonymous=True)
        rospy.Subscriber("/asr_brige/pcm_file",String,self.process)
        self.pub = rospy.Publisher("asr_server/baidu_asr",String,queue_size=10)
        rospy.spin()


    def process(self,data):
            print data.data
            result = self.man.asrprocess(data.data,"pcm",self.rate)
            rospy.loginfo(result)
            if(self.isResultValid(result)):
                self.pub.publish(result)
            else:
                rospy.loginfo("Oh no! Is noise!")

    def isResultValid(self,result):
        if result == "Null" or "，":
            valid = False
        else:
            valid = True
        return valid
            
        


if __name__ == '__main__':
    BaiduASR()       