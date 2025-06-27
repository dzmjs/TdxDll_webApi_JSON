#include "stdafx.h"
#include "TCalcFuncSets.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "HttpUtil.h"

#define MAX_TICKERS 100


time_t double_yyyymmddHHmm_to_timestamp(int ymd, int time) {
	int date = ymd;  // 20190408
	int year = date / 10000;     // 2019
	int month = (date % 10000) / 100; // 04
	int day = date % 100;	//08
	int ms = time;

	struct tm t = { 0 };
	t.tm_year = year - 1900;  // tm_year从1900年起算
	t.tm_mon = month - 1;     // tm_mon从0开始计数
	t.tm_mday = day;
	t.tm_hour = (ms/100)-6;            // 00:00:00
	t.tm_min = (ms % 100);
	t.tm_sec = 0;

	return mktime(&t);        // 转成时间戳
}
time_t yyyymmdd_float_to_timestamp(int date_float) {
	int date = date_float;  // 20190408
	int year = date / 10000;     // 2019
	int month = (date % 10000) / 100; // 04
	int day = date % 100;	//08

	struct tm t = { 0 };
	t.tm_year = year - 1900;  // tm_year从1900年起算
	t.tm_mon = month - 1;     // tm_mon从0开始计数
	t.tm_mday = day;
	t.tm_hour = -6;            // 00:00:00
	t.tm_min = 0;
	t.tm_sec = 0;

	return mktime(&t);        // 转成时间戳
}
void write_log(const char* message) {
	FILE* fp = fopen("log_tdx.txt", "a");  // "a" 追加模式，写在文件末尾
	if (fp == NULL) {
		return;  // 文件打开失败，可扩展为写入错误处理
	}
	fprintf(fp, "%s\n", message);  // 写入一行
	fclose(fp);
}
void write_log_d(int message) {
	FILE* fp = fopen("log_tdx.txt", "a");  // "a" 追加模式，写在文件末尾
	if (fp == NULL) {
		return;  // 文件打开失败，可扩展为写入错误处理
	}
	fprintf(fp, "%d\n", message);  // 写入一行
	fclose(fp);
}
void write_log_f(float message) {
	FILE* fp = fopen("log_tdx.txt", "a");  // "a" 追加模式，写在文件末尾
	if (fp == NULL) {
		return;  // 文件打开失败，可扩展为写入错误处理
	}
	fprintf(fp, "%.0f\n", message);  // 写入一行
	fclose(fp);
}
//生成的dll及相关依赖dll请拷贝到通达信安装目录的T0002/dlls/下面,再在公式管理器进行绑定

void TestPlugin1(int DataLen,float* pfOUT,float* pfINa,float* pfINb,float* pfINc)
{
	BOOL up_down = false;
	for (int i = 0; i < DataLen; i++) {
		// 写入到txt文件进行测试
		//float d = pfINc[i];
		//char buffer[32];
		//sprintf(buffer, "%.2f", d);
		//write_log(buffer);

		pfOUT[i] = 0;
		if (i % 10 == 3) {
			up_down = !up_down;
			if (up_down) {
				pfOUT[i] = 1;
			}
			else {
				pfOUT[i] = -1;
			}
		}		
	}
}

char* get_period(char* p)
{
	char* cycles[] = {
		"1", "5", "15", "30", "60",
		"1D", "1W", "1M",
		"1D", "1D", "1D", "1Y",
		"1D", "1D"
	};
	int num = atoi(p);
	char* t = cycles[num];
	return t;
}

void parse_input(const char* input, char* ticker, char* period) {
	// 输入格式假设为 "1xxxxxxx"，总长度至少为 8
	if (strlen(input) < 8) {
		ticker[0] = '\0';
		period[0] = '\0';
		return;
	}

	// 从第2位开始取6位为 ticker
	strncpy(ticker, input + 1, 6);
	ticker[6] = '\0';  // 末尾补0

	// 取剩下的字符作为周期
	strcpy(period, input + 7);
}
void GetChanLunLine(int DataLen, float* pfOUT, float* ticker_period, float* fDate, float* fTime)
{
	int* date_o = new int[DataLen];
	// 写入到txt文件进行测试
	//float d = ticker_period[i];
	//char buffer[32];
	//sprintf(buffer, "%.2f", d);
	//write_log(buffer);
	char buffer[9];
	sprintf(buffer, "%.0f", ticker_period[0]);
	//write_log(buffer);
	char ticker[6];
	char period[2];

	parse_input(buffer, ticker, period);
	char* real_period=get_period(period);
	//write_log(ticker);
	//write_log(real_period);
	//周期从0到13,依次分别是1/5/15/30/60分钟,日/周/月,多分钟,多日/季/年,5秒线/多秒线
	char* uri = new char[100];
	strcpy(uri, "http://127.0.0.1:8001/chan?ct=30&symbol=");
	strcat(uri, ticker);
	strcat(uri, "&resolution=");
	strcat(uri, real_period);
	const char* url = uri;
	char* response = http_get(url);
	//write_log(url);
	for (int ii = 0; ii < DataLen; ii++) {
		date_o[ii] = (int)fDate[ii];
		//write_log_d(date_o[ii]);
	}
	TickerData tickers[MAX_TICKERS];
	int count = parse_json_array(response, tickers, MAX_TICKERS);
	int datePlus = 19000000;
	int point = DataLen-1;
	
	for (int i = 0; i < DataLen; i++) {
		pfOUT[i] = 0;
	}
	//write_log(real_period);
	if (real_period == "1D") {
		//周期为日
		for (int j = count-1; j >-1; j--) {
			TickerData td = tickers[j];
			int end = td.endDate;
			//char buffer2[32];
			//sprintf(buffer2, "%ld", end);
			//write_log_d(end);
			//write_log(buffer2);
			for (int d = point; d > -1; d--) {
				int date = date_o[d] + datePlus;
				int date_l=(int)yyyymmdd_float_to_timestamp(date);
				if (end == date_l) {
					if (strstr(td.class_type, "Third") != NULL) {
						pfOUT[d] = 3;
					}
					else if (strstr(td.class_type, "Second") != NULL) {
						pfOUT[d] = 2;
					}
					else if (strstr(td.class_type, "First") != NULL) {
						pfOUT[d] = 1;
					}
					if (strcmp(td.Line_Type , "down") == 0) {
						pfOUT[d] = -1 * (10 + pfOUT[d]);
					}
					else if (strcmp(td.Line_Type, "up") == 0) {
						pfOUT[d] = 1 * (10 + pfOUT[d]);
					}
					point = d;
					point--;
					break;
				}
			}
		}
	}
	else {
		//分钟级别
		for (int j = count - 1; j > -1; j--) {
			TickerData td = tickers[j];
			int end = td.endDate + 8*3600;
			//write_log("lineTime:");
			//write_log_d(end);
			//char buffer2[32];
			//sprintf(buffer2, "%ld", end);
			//write_log("line:");
			//write_log(buffer2);
			for (int d = point; d > -1; d--) {
				int date = date_o[d] + datePlus;
				int t = (int) roundf(fTime[d]);
				//write_log("kk:");
				//write_log_d(date);
				//write_log_f(t);
				//double lt = t+date * 10000;
				int date_l =(int) double_yyyymmddHHmm_to_timestamp(date, t);
				//write_log_f(date_l);
				if (labs(date_l - end) < 290) {
					if (strstr(td.class_type, "Third") != NULL) {
						pfOUT[d] = 3;
					}
					else if (strstr(td.class_type, "Second") != NULL) {
						pfOUT[d] = 2;
					}
					else if (strstr(td.class_type, "First") != NULL) {
						pfOUT[d] = 1;
					}
					if (strcmp(td.Line_Type, "down") == 0) {
						pfOUT[d] = -1 * (10 + pfOUT[d]);
					}
					else if (strcmp(td.Line_Type, "up") == 0) {
						pfOUT[d] = 1 * (10 + pfOUT[d]);
					}
					//write_log("same:");
					//write_log_f(date_l);
					point = d;
					point--;
					break;
				}
			}
		}
	}
}


void TestPlugin2(int DataLen,float* pfOUT,float* pfINa,float* pfINb,float* pfINc)
{
	for(int i=0;i<DataLen;i++)
	{
		pfOUT[i]=pfINa[i]+pfINb[i]+pfINc[i];
		pfOUT[i]=pfOUT[i]/3;
	}
}


//加载的函数
PluginTCalcFuncInfo g_CalcFuncSets[] = 
{
	{1,(pPluginFUNC)&TestPlugin1},
	{2,(pPluginFUNC)&TestPlugin2},
	{3,(pPluginFUNC)&GetChanLunLine},
	{0,NULL},
};

//导出给TCalc的注册函数
BOOL RegisterTdxFunc(PluginTCalcFuncInfo** pFun)
{
	if(*pFun==NULL)
	{
		(*pFun)=g_CalcFuncSets;
		return TRUE;
	}
	return FALSE;
}
