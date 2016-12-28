#!/usr/bin/env python
#-*- coding:utf-8 -*-
#
#  BaiduVoiceClientAPI.py
#  Created on: 2016年12月22日
#  Author: hntea
# 
#  文件功能：提供百度语音请求API  
#
import urllib2
import json as js
import wave
import base64
import sys
import types
import os
import dbm
import datetime 
import uuid


class BaiduVoiceClientManager:

    '''This is  ASR class,it can read wav or pcm you give and post it to baidu voice server.
        If you want to use this class,please apply your own apikey and secretkey for baidu api.
        and then write ~/.SpeechSystem/config/config.json
     '''

    def __getMacAddress(self):
        mac = uuid.UUID(int=uuid.getnode()).hex[-12:]
        return ":".join([mac[e:e + 2] for e in range(0, 11, 2)])



    def __creatWorkspace(self,dir):
        if not os.path.exists(dir):
            os.mkdir(dir)
            

    def __creatDbm(self,dir,file):
        db_file = os.path.join(dir,file)
        db = dbm.open(db_file,'c')
        try:
            token = db['access_token']
        except:
            db['access_token'] = "0"
            db['time'] = '2012-09-20'
        db.close()
        return db_file
    

    def __writedbm(self,file,time,token):
          db = dbm.open(file,'w')
          db['access_token'] = token
          db['time'] = str(time)  
          db.close()   
   
    #token过期判断
    def __isTokenTimeout(self,file,nweeks):
          db = dbm.open(file,'w')
          history = db['time']  
          db.close()  
          today = datetime.datetime.today()
          ago = today - datetime.timedelta(weeks=nweeks)
          #将字符串转换成时间对象
          htime = datetime.datetime.strptime(history, '%Y-%m-%d')
          if(ago > htime):
              timeout = True
          else:
              timeout = False
                   
          return timeout
          

    def __isTokenNull(self,file):
        db = dbm.open(file,'w')
        token = db['access_token']  
        db.close()   
        if token == "0":
            isnull = True
        else:
            isnull = False
        return isnull
        
          
    def __defaultconfig(self):
          config = os.path.join(os.environ["HOME"],".SpeechSystem/config/config.json")
          return config
    
          
    def __readconfig(self,file):
        with open(file) as fd:
            jsout = js.load(fd)
            self.__workspace = jsout["speechServer"]["baidu"]["workspace"]
            self.__api = jsout["speechServer"]["baidu"]["appid"]
            self.__apikey = jsout["speechServer"]["baidu"]["apikey"]
            self.__secretkey= jsout["speechServer"]["baidu"]["secretkey"]
            self.__ttsserver =  jsout["speechServer"]["baidu"]["ttsserver"]
            self.__asrserver =  jsout["speechServer"]["baidu"]["asrserver"]
            self.__mac_address =  jsout["mac_address"]
        
        fd.close()
        
    
    #获取token
    def __getToken(self):
        baidu_url = "https://openapi.baidu.com/oauth/2.0/token?" \
                    "grant_type=client_credentials&" \
                    "client_id=" + self.__apikey + "&" \
                    "client_secret=" + self.__secretkey;

        respon = urllib2.urlopen(baidu_url)
        data = respon.read()
        self.__access_token = js.loads(data)['access_token']


    
    #更新token
    def __updateToken(self,file):
        if self.__isTokenNull(file) or self.__isTokenTimeout(file,2):
            self.__getToken()
            today = datetime.date.today() 
            self.__writedbm(file,today,self.__access_token)
        else:
            db = dbm.open(file,'w')
            self.__access_token = db['access_token']  
            db.close()   


    def asrprocess(self,file,formate,rate):
        with open(file,'rb') as f:
            data = f.read()
            filelen = len(data)
            f.close()

        base_data = base64.b64encode(data).decode('utf-8')
        dict = {
            'format' : formate,
            'rate' : rate,
            'channel' : 1,
            'cuid' : self.__getMacAddress(),
            'token' : self.__access_token,
            'lan' : 'zh',
            'len' : filelen,
            'speech' : base_data
        }

        json_data = js.dumps(dict).encode('utf-8')
        json_length = len(json_data)

        request = urllib2.Request(url=self.__asrserver)
        request.add_header("Content-Type", "application/json")
        request.add_header("Content-Length", json_length)
        respond = urllib2.urlopen(url=request, data=json_data)
        
        result_str = respond.read().decode('utf-8')
        json_resp = js.loads(result_str)
        if json_resp.has_key('result'):
            self.__asr_result=json_resp['result'][0]
        else:
            self.__asr_result="Null"
#         print "The result is：%s" %self.__asr_result.encode("utf-8")
        return self.__asr_result


    def __init__(self):
        self.__readconfig(self.__defaultconfig())
        self.__creatWorkspace(self.__workspace)
        dbmfile =  self.__creatDbm(self.__workspace,"token")
        self.__updateToken(dbmfile)
        




# if __name__ == '__main__':
#     man = BaiduVoiceClientManager()
#     print datetime.datetime.now()
#     print man.asrprocess("/home/hntea/.SpeechSystem/audio-file/8.wav","wav",16000)
#     print datetime.datetime.now()