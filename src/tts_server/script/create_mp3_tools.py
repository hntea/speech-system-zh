#!/usr/bin/env python
#-*- coding:utf-8 -*-

# create_mp3_tools.py
# Created on: 2017年1月4日
#    Author: hntea
#
# 说明：该脚本用于合成mp3文件  
#
import BaiduTTSClientManager as tts
import sys
import rospy
import os
from pypinyin import pinyin, lazy_pinyin

#设置默认编码格式-勿动
reload(sys)
sys.setdefaultencoding( "utf-8" )


def CreatWorkspace(dir):
    if not os.path.exists(dir):
        os.mkdir(dir)


#将字符串列表换成 unicode 列表
def StringToUincode(list):
    uargs = []
    #列表合成
    for item in list:
         uitem = unicode(item,"utf-8")
         uargs.append(uitem)
    return uargs

#将unicode转换成字符串
def UnicodeToString(list):
    sargs = []
    for item in list:
        argstr = item.encode("utf-8")
        n = str(argstr).strip()
        sargs.append(n)
    return sargs

#合成音频名称，返回列表
def nameForAudioFile(list,dir):
    namelist = [];
    for item in list:
        name  = ""
        pyin = lazy_pinyin(item)
        for n in pyin:
            name += n 
        
        name += ".mp3"
        file = os.path.join(dir,name)
        namelist.append(file)
    return namelist


def SelectNotExit(a,b):
#     tmp = [val for val in a if val in b]
#     print tmp
#     print list(set(a).intersection(set(b)))
    nlist = list(set(a).difference(set(b))) # a中有而b中没有的 
    return nlist


# #获取目录已有文件
# workspace = os.path.join(os.environ["HOME"],".SpeechSystem/play-resource/mp3")
# CreatWorkspace(workspace)
# exitlist = os.listdir(workspace)
# 
# 
# SelectNotExit(strname,exitlist)

if __name__ == '__main__':
    
    workspace = os.path.join(os.environ["HOME"],".SpeechSystem/play-resource/mp3")
    CreatWorkspace(workspace)
    
    #获取输入
    args = sys.argv
    del args[0]
    if len(args) == 0:
        print "请附带不少于一个参数，如：python create_map3.py 早上好"
        exit
    
    
    #将中文参数列表转换成拼音并合成文件名列表
    namelist  =  nameForAudioFile(StringToUincode(args),workspace)
    #再将 utf-8 转换成字符串，用于生成文件
    strname =  UnicodeToString(namelist)
    
    server = tts.BaiduTtsClientManager()
    index = 0; 
    for val in args:
        server.runTTS(val,strname[index])
        index +=1
