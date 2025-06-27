#pragma once
// ticker_data.h
#ifndef TICKER_DATA_H
#define TICKER_DATA_H

typedef struct {
    char Ticker[20];
    char Line_Type[10];
    int startDate;
    int endDate;
    double startPrice;
    double endPrice;
    char class_type[50]; // null ��Ϊ ""
    double close_end;
    double close_begin;
    int rate;
    char third_buy_closeness[50]; // null ��Ϊ ""
} TickerData;

#endif // TICKER_DATA_H
