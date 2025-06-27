# TdxDll_webApi_JSON

[English Version](./README.md) | [中文版](./README_CN.md)

本项目基于通达信客户端，调用dll，发送get请求，获取json并解析json。 
## 介绍
基于其他软件分析得到的缠论成果数据库表，与python开发的web server，集成到通达信，使用dll拓展的方式，发送http请求，获得json格式数据，然后解析，再在通达信页面k线图上绘制出缠论线段与买卖点。 
## 流程
- 通达信集成该项目编译的dll，使用公式调用dll内部的方法； 
- dll发送http请求，得到json个数数据； 
- 解析json数据，变成结构体数组； 
- 日期/时间匹配，返回输出格式（上涨下跌的起点，买卖点类型）； 
- 通达信公式处理返回的结果，绘制线段并标注买卖点。 