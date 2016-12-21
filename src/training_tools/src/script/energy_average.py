#!/usr/bin/env python
#-*-coding:utf-8-*-
import rospy
import audio_msgs.msg
from std_msgs.msg import String
import time
import datetime
import os
import dbm
import json
#文件锁
import fcntl  
#双端队列包
from collections import deque
from rospy.timer import sleep

#节点功能说明：统计历史能量噪声平均
class AvergyLevel:
    '''This class aim at  calculating  energy level from audio_energy and then write to dictionary'''

    def __init__(self):
       
        self.state = False;
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
        
        #创建永久词典,两个属性共用一个词典
        self.device = rospy.get_param("/audio_capture/name","default")
        self.db_name = self.device+"-energyAndzero-avg.dbm"
        self.db_file = os.path.join(self.wkdir,self.db_name)
        self.createDbm()
        
        #初始化历史数据
        self.initHistoryData()    
              
        #初始化缓存
        #能量
        self.eg_circle_buffer = deque(range(10))
        #过零
        self.zc_circle_buffer = deque(range(10))
        

        
        rospy.init_node('energy_average', anonymous=True)
        rospy.Subscriber("audio_zero_crossing",audio_msgs.msg.AudioFeature, self.zeroCallback)
        rospy.Subscriber("audio_energy",audio_msgs.msg.AudioFeature, self.energyCallback)
        rospy.Timer(rospy.Duration(5), self.timer_callback)
        self.pub = rospy.Publisher('static_eg_zc_average', String, queue_size=5)
        rospy.spin()
      
      
    #初始化历史数据
    def initHistoryData(self):
         self.db = dbm.open(self.db_file,'w')
         self.eg_total_sum = float(self.db["eg_total_sum"])
         self.eg_min_sum = float(self.db["eg_min_sum"])
         self.eg_max_sum = float(self.db["eg_max_sum"])
         self.eg_total_avg = float(self.db["eg_total_avg"])
         self.eg_min_avg  = float(self.db["eg_min_avg"])
         self.eg_max_avg = float(self.db["eg_max_avg"])
         self.eg_numbers = float(self.db["eg_numbers"])
         
         #注意数据溢出问题
         self.zc_total_sum = float(self.db["zc_total_sum"])
         self.zc_min_sum = float(self.db["zc_min_sum"])
         self.zc_max_sum = float(self.db["zc_max_sum"])
         self.zc_total_avg = float(self.db["zc_total_avg"])
         self.zc_min_avg  = float(self.db["zc_min_avg"])
         self.zc_max_avg = float(self.db["zc_max_avg"])
         self.zc_numbers = float(self.db["zc_numbers"])

         self.timer = self.db['timer']
         self.db.close()
    

        
    #说明：创建永久词典
    #注意：如果设备不同，将以插件名区分永久词典
    def createDbm(self):
        #没有则创建
        self.db = dbm.open(self.db_file,'c')
        #添加项目，空则添加项目
        try:
            hw = self.db['device']
        except:
            self.db['eg_total_sum'] = '0'
            self.db['eg_min_sum'] = '0'
            self.db['eg_max_sum'] = '0'
            self.db['eg_total_avg'] = '0'
            self.db['eg_min_avg'] = "0"
            self.db['eg_max_avg'] = '0' 
            self.db['eg_numbers'] = '0'      
            
            self.db['zc_total_sum'] = '0'
            self.db['zc_min_sum'] = '0'
            self.db['zc_max_sum'] = '0'
            self.db['zc_total_avg'] = '0'
            self.db['zc_min_avg'] = "0"
            self.db['zc_max_avg'] = '0' 
            self.db['zc_numbers'] = '0'           
                 
            self.db['device'] = self.device 
            self.db['timer'] = ' '
        self.db.close()
    
    #写永久词典
    def writeTodbm(self):
        self.db = dbm.open(self.db_file,'w')
        self.timer = datetime.datetime.fromtimestamp(time.time()).strftime('%Y-%m-%d %H:%M:%S')
        self.db['timer'] = self.timer 
        
        #写能量数据
        self.db['eg_total_sum'] = str(self.eg_total_sum)
        self.db['eg_min_sum'] = str(self.eg_min_sum)
        self.db['eg_max_sum'] = str(self.eg_max_sum)

        #写过零数据
        self.db['zc_total_sum'] = str(self.zc_total_sum)
        self.db['zc_min_sum'] = str(self.zc_min_sum)
        self.db['zc_max_sum'] = str(self.zc_max_sum)
        
        #如果误差范围很小，则可以休眠一段时间，并将消息写入json文件
        eg_err = float(self.db['eg_min_avg']) - (self.eg_min_sum/self.eg_numbers * 10) 
        zc_err = float(self.db['zc_min_avg']) - (self.zc_min_sum/self.zc_numbers * 10) 
              
        self.db['eg_total_avg'] = str(self.eg_total_sum/self.eg_numbers)                            
        self.db['eg_min_avg'] = str(self.eg_min_sum/self.eg_numbers * 10)
        self.db['eg_max_avg'] = str(self.eg_max_sum/self.eg_numbers * 10)
        self.db['eg_numbers'] = str(self.eg_numbers)
            
        self.db['zc_total_avg'] = str(self.zc_total_sum/self.zc_numbers)                            
        self.db['zc_min_avg'] = str(self.zc_min_sum/self.zc_numbers * 10)
        self.db['zc_max_avg'] = str(self.zc_max_sum/self.zc_numbers * 10)
        self.db['zc_numbers'] = str(self.zc_numbers)    
        
        #若同时收敛，则写如json 文件并且发布消息
        if((-0.01<eg_err and eg_err<0.01) and (-1<zc_err and zc_err<1)):
            with open(os.path.join(self.wkdir,"eg_zc_average.json"),'w') as fd:
                 self.ezdir["eg_total_avg"] = self.db['eg_total_avg']    
                 self.ezdir['eg_min_avg'] = self.db['eg_min_avg']
                 self.ezdir['eg_max_avg'] = self.db['eg_max_avg']
                 self.ezdir["zc_total_avg"] = self.db['zc_total_avg']    
                 self.ezdir['zc_min_avg'] = self.db['zc_min_avg']
                 self.ezdir['zc_max_avg'] = self.db['zc_max_avg']
                 fd.write(json.dumps(self.ezdir))       
                 fd.close()        
            self.state = True
        
        if self.state:
            rospy.loginfo("Publish msg and goto sleep about 5 min")
            self.pub.publish("update")
            self.state = False
            sleep(300)
        
        self.db.close()
    
    #词典数据显示
    def dbmShow(self):
        print"=====================History data======================"
        print"Time: ",self.timer
        print "-------------------------energy-----------------------"
        print"eg_total_sum = ",self.eg_total_sum
        print"eg_total_avg = ",self.eg_total_avg
        print"eg_min_sum = ",self.eg_min_sum
        print"eg_min_avg = ",self.eg_min_avg
        print "eg_max_sum = ",self.eg_max_sum
        print"eg_max_avg = ",self.eg_max_avg
        print"eg_numbers = ",self.eg_numbers
        
        print "-------------------zero crossing--------------------"
        print"zc_total_sum = ",self.zc_total_sum
        print"zc_total_avg = ",self.zc_total_avg
        print"zc_min_sum = ",self.zc_min_sum
        print"zc_min_avg = ",self.zc_min_avg
        print"zc_max_sum = ",self.zc_max_sum
        print"zc_max_avg = ",self.zc_max_avg
        print"zc_numbers = ",self.zc_numbers


    #初始化工作空间
    def initWorkSpace(self):
        #创建工作目录
        if not os.path.exists(self.wkdir):
            os.mkdir(self.wkdir)
        #创建配置文件
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
                
    
    def falshBuffer(self,feature,cbuf):
        cbuf.popleft()
        cbuf.append(feature)
    
    
    def timer_callback(self ,event):
        #self.dbmShow()
        self.writeTodbm()
    
    def energyCallback(self ,data):
        self.falshBuffer(data.feature,self.eg_circle_buffer)
        self.eg_numbers +=1
                  
        if(self.eg_numbers%10 == 0):
            self.eg_total_sum += sum(self.eg_circle_buffer)
            self.eg_max_sum += max(self.eg_circle_buffer)
            self.eg_min_sum +=  min(self.eg_circle_buffer)
#             print "min_sum = ",self.min_sum
#             print "max_sum = ",self.max_sum
#             print "total_sum = ",self.total_sum

    def zeroCallback(self ,data):
        self.falshBuffer(data.feature,self.zc_circle_buffer)
        self.zc_numbers +=1
                  
        if(self.zc_numbers%10 == 0):
            self.zc_total_sum += sum(self.zc_circle_buffer)
            self.zc_max_sum += max(self.zc_circle_buffer)
            self.zc_min_sum +=  min(self.zc_circle_buffer)


        


                

if __name__ == '__main__':
    avg = AvergyLevel()