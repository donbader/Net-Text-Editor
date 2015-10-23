# Net-Text-Editor
OS homework


寫超久我的媽Orz

PROG1: SOCKET PROGRAMMING
網路編輯器(基本分)
一個client,一個server

當client 連上server 時, 輸入指定command 可以做到以下功能

C ) 在client 端輸入檔名後, 在server 端建立一空的檔案

E ) 在client 端輸入欲編輯的檔名,確認server端有此檔案後,將client 端輸入的文字存入此檔案內(離開編輯時儲存檔案,方式自訂)

R ) 在client 端輸入欲刪除的檔名, 將 server 端檔案刪除

L ) 在client 端顯示目前server 上所有的檔案名稱

D ) 在client 端輸入指定檔案名稱, 並從server下載此檔案到client 資料夾


額外擴充功能 檔案加解密

批次下載

目錄

自己想出的功能

Due date: 2015/10/22 18:00

￼
Socket


A socket is defined as an endpoint for communication

Concatenation of IP address and port

The socket 161.25.19.8:1625 refers to port 1625 on host 161.25.19.8

Communication consists between a pair of sockets

Refer to Section 5.5, Advanced Linux Programming 

