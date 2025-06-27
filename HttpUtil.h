#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include "ticker_data.h"

char* http_get(const char* url);
void print_ticker(const TickerData* t);
int parse_json_array(const char* json, TickerData* arr, int max_count);
#ifdef __cplusplus
}
#endif