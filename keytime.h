#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "conio.h"

struct eachkey{
    int key_num; //キーの位置番号
    double keytimes; //各キー入力時間保存用
};

struct eachkey keys[30];
int keyweight[30]; //各キーの重み付け保存用
char keyplace[30] = {'q','w','e','r','t','y','u','i','o','p','a','s','d','f','g','h','j','k','l',';','z','x','c','v','b','n','m',',','.','/'};

double getETime(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (double)tv.tv_usec*1e-6;
}

/*指定されたキーが押される時間を計測*/
double keytime(char c){
    double start, end, time;
    char getkey;
    int ec;
    printf("手をホームポジションにおいて,スペースキーを押してください\n");
    while(1){
        int space = getch();
        if(space == ' ') break;
    }
    start = getETime();
    printf("「%c」を入力してください>> ", c);
    while(1){
        ec = getch();
        if(ec == c) break;
    }
    printf("%c\n", ec);
    end = getETime();
    time = end - start;
    printf("キー入力時間：%lf秒\n", time);
    return time;
}

int GetRandom(int min,int max){
	return min + (int)(rand()*(max-min+1.0)/(1.0+RAND_MAX));
}

/*ランダムにキー入力を受付＋時間計測して重み順位付け*/
int keyweightcal(){
    int i, n, k, h;
    struct eachkey tmp;
    for(i=0;i<30;i++) keys[i].key_num = i; //キーの位置番号設定
    for(i=0;i<30;i++) keys[i].keytimes = -1; //keytimes初期化
    for(i=0;i<30;i++){
        n = GetRandom(0,29);
        while(keys[n].keytimes!=-1){ //記録されていないキーを探すｓ
            n = GetRandom(0,29);
        }
        keys[n].keytimes = keytime(keyplace[n]);
    }
    /*ソート*/
    for(k=0;k<30;k++){
        for(h=k+1;h<30;h++){
            if(keys[k].keytimes > keys[h].keytimes){
                tmp =  keys[k];
                keys[k] = keys[h];
                keys[h] = tmp;
            }
        }
    }
    /*重み付け*/
    for(i=0;i<30;i++){
        keyweight[keys[i].key_num] = i;
    }
    /*結果確認用*/
    for(i=0;i<30;i++){
        printf("キー番号(%d)の重み:%d\n",i,keyweight[i]);
    }
}