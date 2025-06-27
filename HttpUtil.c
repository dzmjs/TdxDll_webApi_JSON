#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <wininet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ticker_data.h"

#pragma comment(lib, "wininet.lib")
#define MAX_TICKERS 100
int main2() {
    printf("Hello, C in Visual Studio!\n");
    return 0;
}


#pragma comment(lib, "wininet.lib")

// ʹ�� WinINet ���� HTTP GET ����
char* http_get(const char* url) {
    HINTERNET hInternet = InternetOpen("SimpleHTTP", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) {
        printf("InternetOpen failed.\n");
        return NULL;
    }

    HINTERNET hUrl = InternetOpenUrlA(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hUrl) {
        printf("InternetOpenUrl failed.\n");
        InternetCloseHandle(hInternet);
        return NULL;
    }

    char* result = (char*)malloc(1024 * 100); // ���� 100KB ������
    if (!result) return NULL;

    DWORD bytesRead = 0;
    DWORD totalRead = 0;
    char buffer[4096];

    while (InternetReadFile(hUrl, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        memcpy(result + totalRead, buffer, bytesRead);
        totalRead += bytesRead;
    }

    result[totalRead] = '\0'; // ��ӽ�����

    InternetCloseHandle(hUrl);
    InternetCloseHandle(hInternet);

    return result;
}

// �򵥲�������
int main3() {
    const char* url = "http://127.0.0.1:8001/chan?symbol=SH.600000&resolution=1D";
    char* response = http_get(url);

    if (response) {
        printf("Response:\n%s\n", response);
        free(response);
    }
    else {
        printf("Request failed.\n");
    }

    return 0;
}

// �����ո�Ͷ���
void skip_space(const char** p) {
    while (**p == ' ' || **p == '\n' || **p == '\r' || **p == ',') (*p)++;
}

// ����һ���ֶ�ֵ��֧���ַ��������֣�
void parse_string_field(const char* field, const char** p, char* dest) {
    const char* pos = strstr(*p, field);
    if (!pos) {
        dest[0] = '\0';
        return;
    }

    pos += strlen(field);

    // �����ո��ð��
    while (*pos == ' ' || *pos == ':') pos++;

    if (*pos == '\"') {
        pos++;  // ������ʼ����
        const char* end = strchr(pos, '\"');
        if (end) {
            size_t len = end - pos;
            strncpy(dest, pos, len);
            dest[len] = '\0';
            *p = end + 1;  // �ƽ� *p ָ��
        }
        else {
            dest[0] = '\0'; // ���Ų���ԣ��쳣
        }
    }
    else if (strncmp(pos, "null", 4) == 0) {
        dest[0] = '\0';
        *p = pos + 4;
    }
    else {
        dest[0] = '\0'; // ��ʽ����
    }
}

// ���� long long �����ֶ�
long long parse_long_field(const char* field, const char** p) {
    const char* pos = strstr(*p, field);
    if (!pos) return 0;
    pos += strlen(field);
    skip_space(&pos);
    long long val = 0;
    sscanf(pos, "%lld", &val);
    return val;
}

// ���� double �����ֶ�
double parse_double_field(const char* field, const char** p) {
    const char* pos = strstr(*p, field);
    if (!pos) return 0;
    pos += strlen(field);
    skip_space(&pos);
    double val = 0;
    sscanf(pos, "%lf", &val);
    return val;
}

// ���� int �����ֶ�
int parse_int_field(const char* field, const char** p) {
    const char* pos = strstr(*p, field);
    if (!pos) return 0;
    pos += strlen(field);
    skip_space(&pos);
    int val = 0;
    sscanf(pos, "%d", &val);
    return val;
}

// ����һ����¼
int parse_one_object(const char* json, TickerData* data) {
    parse_string_field("\"Ticker\":", &json, data->Ticker);
    parse_string_field("\"Line_Type\":", &json, data->Line_Type);
    data->startDate = parse_int_field("\"startDate\":", &json);
    data->endDate = parse_int_field("\"endDate\":", &json);
    data->startPrice = parse_double_field("\"startPrice\":", &json);
    data->endPrice = parse_double_field("\"endPrice\":", &json);
    parse_string_field("\"class_type\":", &json, data->class_type);
    data->close_end = parse_double_field("\"close_end\":", &json);
    data->close_begin = parse_double_field("\"close_begin\":", &json);
    data->rate = parse_int_field("\"rate\":", &json);
    parse_string_field("\"third_buy_closeness\":", &json, data->third_buy_closeness);
    return 0;
}

// ������ JSON �����н����������
int parse_json_array(const char* json, TickerData* arr, int max_count) {
    const char* p = json;
    int count = 0;

    while ((p = strchr(p, '{')) && count < max_count) {
        const char* end = strchr(p, '}');
        if (!end) break;

        char object[2048] = { 0 };
        strncpy(object, p, end - p + 1);
        object[end - p + 1] = '\0';

        parse_one_object(object, &arr[count]);
        count++;
        p = end + 1;
    }

    return count;
}

// ��ӡ���
void print_ticker(const TickerData* t) {
    printf("Ticker: %s, Line_Type: %s, startDate: %lld, endDate: %lld\n", t->Ticker, t->Line_Type, t->startDate, t->endDate);
    printf("startPrice: %.4f, endPrice: %.4f\n", t->startPrice, t->endPrice);
    printf("class_type: %s, close_end: %.4f, close_begin: %.4f, rate: %d\n", t->class_type, t->close_end, t->close_begin, t->rate);
    printf("third_buy_closeness: %s\n", t->third_buy_closeness);
    printf("---------------------------------\n");
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
// ʾ���÷�
int main() {
    char ticker[6];
    char period[2]="5";

    char* real_period=get_period(period);
    const char* url = "http://127.0.0.1:8001/chan?symbol=SH.000001&resolution=1D";
    char* response = http_get(url);

    if (response) {
        //printf("Response:\n%s\n", response);
        //free(response);
    }
    else {
        printf("Request failed.\n");
    }

    

    TickerData tickers[MAX_TICKERS];
    int count = parse_json_array(response, tickers, MAX_TICKERS);

    for (int i = 0; i < count; i++) {
        print_ticker(&tickers[i]);
    }

    return 0;
}
