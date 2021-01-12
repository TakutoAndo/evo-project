#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "conio.h"



int keyweight[30]; //各キーの重み付け保存用

/*ランダムにキー入力を受付＋時間計測*/
void keyweightcal(){
    int keykey[30] = {306,650,989,885,818,750,589,753,708,767,846,673,774,943,1137,584,1135,713,870,808,700,889,565,900,860,681,670,720,806,859};
    for(int n=0;n<30;n++){
        keyweight[n] = keykey[n];
    }
    
    /*結果確認用*/
    for(int i=0;i<30;i++){
        printf("キー番号(%d)の重み:%d\n",i,keyweight[i]);
    }
}