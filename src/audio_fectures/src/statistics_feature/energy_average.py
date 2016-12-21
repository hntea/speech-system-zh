#!/usr/bin/env python
#-*-coding:utf-8-*-
import rospy
import audio_msgs.msg
import time
import datetime
import os
import dbm
import json
#文件锁
import fcntl  
#双端队列包
from collections import deque

#节点功能说明：统计历史能量噪声平均
class EnergyAvergy:
    '''This class aim at  calculating  energy level from audio_energy and then write to dictionary'''

    def __init__(self):
       
        self.ezdir = {
                'zc_total_avg' : 0,
                'zc_min_avg' : 0,
                'zc_max_avg' : 0,
                'eg_total_avg' : 0,
                'eg_min_avg' : 0,
                'eg_max_avg' : 0
        }
        #创建工作空间
        self.wkdir = os.path.join(os.environ["HOME"],".SpeechSystem/statistic-features")
        self.wkdir = rospy.get_param("~wkdir",self.wkdir)
        self.initWorkSpace()
        #创建永久词典
        self.device = rospy.get_param("/audio_capture/name","default")
        self.filename = self.device+"-eg_avg.dbm"
        self.dbmfile = os.path.join(self.wkdir,self.filename)
        self.createDbm()
        
        #初始化历史数据
        self.initHistoryData()    
              
        #初始化缓存
        self.circle_buffer = deque(range(10))
        

        
        rospy.init_node('energy_average', anonymous=True)
        rospy.Subscriber("audio_energy",audio_msgs.msg.AudioFeature, self.callback)
        rospy.Timer(rospy.Duration(5), self.timer_callback)
        rospy.spin()
      
      
    #初始化历史数据
    def initHistoryData(self):
         self.db = dbm.open(self.dbmfile,'w')
         self.total_sum = float(self.db["total_sum"])
         self.min_sum = float(self.db["min_sum"])
         self.max_sum = float(self.db["max_sum"])
         self.total_avg = float(self.db["total_avg"])
         self.min_avg  = float(self.db["min_avg"])
         self.max_avg = float(self.db["max_avg"])
         self.numbers = float(self.db["numbers"])
         self.timer = self.db['timer']
         self.db.close()
    

        
    #说明：创建永久词典
    #注意：如果设备不同，将以插件名区分永久词典
    def createDbm(self):
        #没有则创建
        self.db = dbm.open(self.dbmfile,'c')
        #添加项目，空则添加项目
        try:
            hw = self.db['device']
        except:
            self.db['total_sum'] = '0'
            self.db['min_sum'] = '0'
            self.db['max_sum'] = '0'
            self.db['total_avg'] = '0'
            self.db['min_avg'] = "0"
            self.db['max_avg'] = '0' 
            self.db['numbers'] = '0'           
            self.db['device'] = self.device 
            self.db['timer'] = ' '
        self.db.close()
    
    #写永久词典
    def writeTodbm(self):
        self.db = dbm.open(self.dbmfile,'w')
        self.timer = datetime.datetime.fromtimestamp(time.time()).strftime('%Y-%m-%d %H:%M:%S')
        self.db['timer'] = self.timer 
        self.db['total_sum'] = str(self.total_sum)
        self.db['min_sum'] = str(self.min_sum)
        self.db['max_sum'] = str(self.max_sum)

        
        #如果误差范围很小，则可以休眠一段时间，并将消息写入json文件
        err = float(self.db['min_avg']) - (self.min_sum/self.numbers * 10) 

        if (-0.001 <err and err < 0.001):
            print "err = ",err
            with open(os.path.join(self.wkdir,"eg_zc_average.json"),'w') as fd:
                #修改文件时上排他锁,若其它进程正在使用该文件，则阻塞
                fcntl.flock(fd.fileno(), fcntl.LOCK_EX) 
                self.ezdir["eg_total_avg"] = self.db['total_avg']
                self.ezdir['eg_min_avg'] = self.db['min_avg']
                self.ezdir['eg_max_avg'] = self.db['max_avg']
                fd.write(json.dumps(self.ezdir))
                print "OK---Write in json"
                fd.close()
       
       
        self.db['total_avg'] = str(self.total_sum/self.numbers)                            
        self.db['min_avg'] = str(self.min_sum/self.numbers * 10)
        self.db['max_avg'] = str(self.max_sum/self.numbers * 10)
        self.db['numbers'] = str(self.numbers)    

        self.db.close()
    
    #词典数据显示
    def dbmShow(self):
        print"=====================History data======================"
        print"Time: ",self.timer
        print"total_sum = ",self.total_sum
        print"total_avg = ",self.total_avg
        print"min_sum = ",self.min_sum
        print"min_avg = ",self.min_avg
        print "max_sum = ",self.max_sum
        print"max_avg = ",self.max_avg
        print"count = ",self.numbers
        print"======================Ending===========================\n"

    #初始化工作空间
    def initWorkSpace(self):
        if not os.path.exists(self.wkdir):
            os.mkdir(self.wkdir)
        if not os.path.isfile(os.path.join(self.wkdir,"eg_zc_average.json")):
            print "No "+os.path.join(self.wkdir,"eg_zc_average.json")+"file,Creating...."
#             ezdir = {
#                 'zc_total_avg' : 0,
#                 'zc_min_avg' : 0,
#                 'zc_max_avg' : 0,
#                 'eg_total_avg' : 0,
#                 'eg_min_avg' : 0,
#                 'eg_max_avg' : 0
#             }
            with open(os.path.join(self.wkdir,"eg_zc_average.json"),'w') as fd:
                fd.write(json.dumps(self.ezdir))
                fd.close()
                
    
    def falshBuffer(self,energy):
        self.circle_buffer.popleft()
        self.circle_buffer.append(energy)
    
    
    def timer_callback(self ,event):
        self.dbmShow()
        self.writeTodbm()
    
    def callback(self ,data):
        self.falshBuffer(data.feature)
        self.numbers +=1
                  
        if(self.numbers%10 == 0):
            self.total_sum += sum(self.circle_buffer)
            self.max_sum += max(self.circle_buffer)
            self.min_sum +=  min(self.circle_buffer)
#             print "min_sum = ",self.min_sum
#             print "max_sum = ",self.max_sum
#             print "total_sum = ",self.total_sum

     


                

if __name__ == '__main__':
    eg = EnergyAvergy()