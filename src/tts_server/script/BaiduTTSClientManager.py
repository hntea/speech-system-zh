#!/usr/bin/env python
#-*- coding:utf-8 -*-
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
import sys
class BaiduTtsClientManager:

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
          
    def __defaultconfig(self):
          config = os.path.join(os.environ["HOME"],".SpeechSystem/config/config.json")
          return config
    
          
    def __readconfig(self,file):
        with open(file) as fd:
            jsout = js.load(fd)
            self.__workspace = jsout["speechServer"]["baidu"]["workspace"]
            self.__appid = jsout["speechServer"]["baidu"]["appid"]
            self.__apikey = jsout["speechServer"]["baidu"]["apikey"]
            self.__secretkey= jsout["speechServer"]["baidu"]["secretkey"] 
            self.__ttsserver =  jsout["speechServer"]["baidu"]["ttsserver"]
            
            self.__tex = jsout["speechServer"]["baidu"]["ttsParms"]["tex"]
            self.__lan = jsout["speechServer"]["baidu"]["ttsParms"]["lan"]
            self.__ctp = jsout["speechServer"]["baidu"]["ttsParms"]["ctp"]  
            self.__cuid = jsout["speechServer"]["baidu"]["ttsParms"]["cuid"]
            self.__spd = jsout["speechServer"]["baidu"]["ttsParms"]["spd"]
            self.__pit = jsout["speechServer"]["baidu"]["ttsParms"]["pit"]    
            self.__vol = jsout["speechServer"]["baidu"]["ttsParms"]["vol"]  
            self.__per = jsout["speechServer"]["baidu"]["ttsParms"]["per"]  
            
        self.__tts_dir = os.path.join(self.__workspace,"tts")    
        fd.close()
        
    def runTTS(self,text,file):
        url = self.__ttsserver+"?tex=" + text.encode("utf-8")+\
                   "&lan="+self.__lan+\
                   "&cuid="+self.__cuid+\
                   "&ctp="+self.__ctp+\
                   "&per="+self.__per+\
                   "&vol="+self.__vol+\
                   "&pit="+self.__pit+\
                   "&spd="+self.__spd+\
                   "&tok=" +unicode(self.__access_token,"utf-8")
                          
        f = urllib2.urlopen(url)
        with open(file,"wb") as fd:
            audio = f.read()
            fd.write(audio)

        fd.close()

    def __init__(self):
        self.__readconfig(self.__defaultconfig())
        self.__creatWorkspace(self.__tts_dir)
        dbmfile =  self.__creatDbm(self.__workspace,"token")
        self.__updateToken(dbmfile)

