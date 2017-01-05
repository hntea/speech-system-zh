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
import BaiduASRClientManager as client

  
class BaiduASR:
    def __init__(self):
        
        subtopic = rospy.get_param("~subtopic","/asr_brige/pcm_file")
        
        self.man = client.BaiduVoiceClientManager()
        self.rate = rospy.get_param("/audio_capture/sample_rate",16000)
        
        rospy.init_node('baidu_asr', anonymous=True)
        rospy.Subscriber(subtopic,String,self.process)
        self.pub = rospy.Publisher("asr_server/baidu/online_f_res",String,queue_size=10)
        rospy.spin()


    def process(self,data):
            print data.data
            result = self.man.runASR(data.data,"pcm",self.rate)
            rospy.loginfo(result)
            if(self.isResultValid(result)):
                self.pub.publish(result)
            else:
                rospy.loginfo("[Baidu Asr] Oh no! Is noise!")

    def isResultValid(self,result):
        if result == "Null" or "，":
            valid = False
        else:
            valid = True
        return valid
            
        


if __name__ == '__main__':
    BaiduASR()       