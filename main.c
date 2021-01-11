#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include "keytime.h"
#include<string.h>

#define MAX_GEN 5        //最大世代交代
#define POP_SIZE 100       //集団のサイズ
#define LEN_KEYS 30      //遺伝子の長さ
#define GEN_GAP 0.2       //世代交代の割合
#define P_MUTATION 0.1    //突然変異の確率
#define RANDOM_MAX 32767 
#define BEFORE 0
#define AFTER 1

char name[256];
int keyboards[POP_SIZE][LEN_KEYS];  //染色体(キーボード配列)
int fitness[POP_SIZE];               //適合度
int max,min,sumfitness;              //適合度の,max,min,sum
int n_min;                           //適合度のminの添字
int n_max;                           //適合度のmaxの添字

void PrintKeyboardFitness();
void Statistics();
void Crossover(int parent1,int parent2,int *child1, int *child2);
void Mutation(int child);
int ObjFunc(int i);
int Select();
void filewrite(int keyboard[],char* phase);

#define EMPTY -2
#define Used -1
#define A 0
#define B 1
#define C 2
#define D 3
#define E 4
#define F 5
#define G 6
#define H 7
#define I 8
#define J 9
#define K 10
#define L 11
#define M 12
#define N 13
#define O 14
#define P 15
#define Q 16
#define R 17
#define S 18
#define T 19
#define U 20
#define V 21
#define W 22
#define X 23
#define Y 24
#define Z 25
#define Others1 26
#define Others2 27
#define Others3 28
#define Others4 29

int key_options[LEN_KEYS];     //配置可能なキー
int STRINGS = 0;
char str[256][256] = {};   //str[STRINGS] = {"WATASHIHA","HOSHIIDESU"};   //日本語文字列
char alphabet[30] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',':','<','>','?'};

//擬似乱数
static unsigned long int next = 1;

int Rand(void){
  next = next*1103515245 + 12345;
  return (unsigned int)(next/65536)%(RANDOM_MAX+1);
}

void Srand(unsigned int seed){
  next = seed;
}

void init_key_options(){
  int i;
  for(i=0;i<LEN_KEYS;i++){
    key_options[i] = i;
  }
}

void be_empty(int i){
  int j;
  for(j=0;j<LEN_KEYS;j++)
    keyboards[i][j] = EMPTY;
}

//初期データ設定
void Initialize(){
  int i,j,n;
  
  for(i=0;i<POP_SIZE;i++){
    init_key_options();
    for(j=0;j<LEN_KEYS;j++){
      n = Rand()%LEN_KEYS;
      while(key_options[n]==Used){n++;if(n>LEN_KEYS-1)n=A;} //まだ使われていないキーの検索
      keyboards[i][j] = key_options[n];
      key_options[n] = Used;
    }
    fitness[i] = ObjFunc(i);
  }

  printf("First Position\n");
  PrintKeyboardFitness();
  printf("----------------\n");
}

//データ表示関数
void PrintEachKeyboardFitness(int i){
  int j;
  printf("[%d] ",i);
  for(j=0;j<LEN_KEYS;j++){
    printf("%d,",keyboards[i][j]);
  }
  printf(":%d\n",fitness[i]);
}

void PrintKeyboardFitness(){
  int i;
  for(i=0;i<POP_SIZE;i++)PrintEachKeyboardFitness(i);
}

void PrintStatistics(int gen){
  printf("[gen=%2d] max=%d min=%d sumfitness=%d ave=%f\n",
	 gen,max,min,sumfitness,(double)sumfitness/(double)POP_SIZE);
}

void PrintCrossover(int flag,int parent1,int parent2,int child1,int child2,int n_cross){
  switch(flag){
  case BEFORE:
    printf("parent1 |");PrintEachKeyboardFitness(parent1);
    printf("parent2 |");PrintEachKeyboardFitness(parent2);
    printf("delete1 |");PrintEachKeyboardFitness(child1);
    printf("delete2 |");PrintEachKeyboardFitness(child2);
    printf("n_cross=%d\n",n_cross);
    break;
  case AFTER:
    printf("child1 |");PrintEachKeyboardFitness(child1);
    printf("child2 |");PrintEachKeyboardFitness(child2);
    printf("------------------\n");
    break;
  }
}

void PrintMutation(int flag,int child,int n_mutate1,int n_mutate2){
  switch(flag){
  case BEFORE:
    printf("child(OLD)|");PrintEachKeyboardFitness(child);
    printf("n_mutate1=%d <--> n_mutate2=%d\n",n_mutate1,n_mutate2);
    break;
  case AFTER:
    printf("child(NEW)|");PrintEachKeyboardFitness(child);
    printf("------------------\n");
    break;
  }
}
    
//世代の処理
void Generation(int gen){
  int parent1,parent2;
  int child1,child2;
  int n_gen;
  int i;

  //集団の表示
  Statistics();
  PrintStatistics(gen);

  //世代交代
  n_gen=(int)((double)POP_SIZE*GEN_GAP/2.0);
  for(i=0;i<n_gen;i++){
    Statistics();
    parent1 = Select(-1);
    parent2 = Select(parent1);
    Crossover(parent1,parent2,&child1,&child2);
    Mutation(child1);
    Mutation(child2);
  }
}

//目的関数(各文字列sを打つときに指が移動した回数/文字数　が少ない方が優れている(指ごとに重み付け？))
//考える
//現状：ホームポジションにあるキーが入力されたらcount++(簡単だったから)
int ObjFunc(int i){

  int j,k = 0;
  int count = 0; //指が移動してしまった回数
  int point = 0; //返す評価値
  for(j=0;j<STRINGS;j++){
    int n = 0; //文字列の添字
    while(str[j][n]!='\0'){
      if(!(n!=0 && str[j][n]==str[j][n-1])){ //１つ前の文字と同じ時はカウントしない
	for(k=0;k<=29;k++){
          if(alphabet[keyboards[i][k]]==str[j][n]) break;
        }
	  if(k!=30){
      count += keyweight[k];
    }
      }
      n++;
    }
    point += n*10000; //文字数*10000ポイント加算←全部10秒かかった設定
  }
  point -= count;
  printf("count:%d",count);
  return point; //（全文字数*10000-カウント数）が最終ポイント
}

//fitnessの合計値の計算
void Statistics(){
  int i;

  max = 0;
  min = RANDOM_MAX;    //後で適切な値に変更
  sumfitness = 0;

  for(i=0;i<POP_SIZE;i++){
    if(fitness[i]>max){
      max = fitness[i];
      n_max = i;
    }
    if(fitness[i]<min){
      min = fitness[i];
      n_min = i;
    }
    sumfitness += fitness[i];
  }
}

//選択
//ルーレット
int Select(int not_n){
  int i,n=0;
  double rand;
  double fit_rate_loading[POP_SIZE] = {};
  fit_rate_loading[0] = (double)fitness[0]/(double)sumfitness;

  for(i=1;i<POP_SIZE;i++){
    fit_rate_loading[i] = fit_rate_loading[i-1] + (double)fitness[i]/(double)sumfitness;
  }

  rand = (double)Rand()/((double)(RANDOM_MAX+1));    //0<=num<1とする
  while(fit_rate_loading[n]<rand){
    n++;
  }
  if(n!=not_n){return n;}
  else{return Select(not_n);}
}

//交叉
void Crossover(int parent1,int parent2,int *child1, int *child2){
  int min2;
  int n_cross;
  int i,j,n;

  //1番小さい値を子供としてセット
  *child1 = n_min;
  //2番目に小さい値を見つける
  min2 = POP_SIZE;
  for(i=0;i<POP_SIZE;i++){
    if(i!=*child1){
      if(min<=fitness[i]&&fitness[i]<min2){
	min2 = fitness[i]; *child2  = i;
      }
    }
  }
  //交叉方法考える
  //現状:交叉位置で親1と親2のキーを使用キーが被らないように配置(説明むずい）
  
  //交叉位置
  n_cross = Rand()%(LEN_KEYS-1)+1; //n_cross = 1,...,29
  //交叉
  PrintCrossover(BEFORE,parent1,parent2,*child1,*child2,n_cross);
  init_key_options();
  be_empty(*child1);
  for(j=0;j<n_cross;j++){
    keyboards[*child1][j] = keyboards[parent1][j];
    key_options[keyboards[parent1][j]] = Used;
  }
  for(j=n_cross;j<LEN_KEYS;j++){
    if(key_options[keyboards[parent2][j]] != Used){
      keyboards[*child1][j] = keyboards[parent2][j];
      key_options[keyboards[parent2][j]] = Used;
    }
  }
  for(j=0;j<LEN_KEYS;j++){
    if(keyboards[*child1][j]==EMPTY){
      n = Rand()%LEN_KEYS;
      while(key_options[n]==Used){n++;if(n>LEN_KEYS-1)n=A;} //まだ使われていないキーの検索
      keyboards[*child1][j] = key_options[n];
      key_options[n] = Used;
    }
  }
      
  init_key_options();
  be_empty(*child2);
  for(j=0;j<n_cross;j++){
    keyboards[*child2][j] = keyboards[parent2][j];
    key_options[keyboards[parent2][j]] = Used;
  }
  for(j=n_cross;j<LEN_KEYS;j++){
    if(key_options[keyboards[parent1][j]] != Used){
      keyboards[*child2][j] = keyboards[parent1][j];
      key_options[keyboards[parent1][j]] = Used;
    }
  }
  for(j=0;j<LEN_KEYS;j++){
    if(keyboards[*child2][j]==EMPTY){
      n = Rand()%LEN_KEYS;
      while(key_options[n]==Used){n++;if(n>LEN_KEYS-1)n=A;} //まだ使われていないキーの検索
      keyboards[*child2][j] = key_options[n];
      key_options[n] = Used;
    }
  }
  
  fitness[*child1] = ObjFunc(*child1);
  fitness[*child2] = ObjFunc(*child2);
  PrintCrossover(AFTER,parent1,parent2,*child1,*child2,n_cross);
}


//突然変異方法考える
//現状：一定確率でキー入れ替わり
void Mutation(int child){

  int n_mutate1;
  int n_mutate2;
  int x;
  double rand;

  rand = (double)Rand()/((double)(RANDOM_MAX+1));    //0<=num<1とする
  if(rand<P_MUTATION){
    //突然変異位置
    n_mutate1 = Rand()%LEN_KEYS;  //n_mutate1=0,...,29
    n_mutate2 = Rand()%LEN_KEYS;  //n_mutate2=0,...,29
    //突然変異
    PrintMutation(BEFORE,child,n_mutate1,n_mutate2);
    x = keyboards[child][n_mutate1];
    keyboards[child][n_mutate1] = keyboards[child][n_mutate2];
    keyboards[child][n_mutate2] = x;
    fitness[child] = ObjFunc(child);
    PrintMutation(AFTER,child,n_mutate1,n_mutate2);
  }
}

void fileread(){
  FILE *fp;
  int i=0;
	char fname[] = "learning.txt";
	char text[256];
 
	fp = fopen(fname, "r");
	if(fp == NULL) {
	  exit(1);
	}
	
	for (i = 0; fgets(text, 256, fp) != NULL; i++){
		strcpy(str[i], text);
	}
	STRINGS = i;
	
	fclose(fp);
}

void filewrite(int keyboard[],char* phase){
  int i;
  char filename[256];
  strcpy(filename,name);
  FILE* f = fopen(strcat(strcat(filename,phase),"_result.txt"), "w");

  for(i=0;i<LEN_KEYS;i++){
    fprintf(f, "%c\n", alphabet[keyboard[i]]);
  }

  fclose(f);
}
  

//メイン関数
int main(int argc,char **argv){
  int gen,i;
  
  Srand((unsigned) time(NULL)); //seed値変更

  printf("名前を入力してください -> ");
  scanf("%s",name);
  fileread();

  keyweightcal(); //指のクセ診断（キーの重み付け）
  Initialize();
  
  for(gen=1;gen<=MAX_GEN;gen++){
    Generation(gen);
    if(gen==1)
      filewrite(keyboards[n_max],"_first");
    if(gen==MAX_GEN/2)
      filewrite(keyboards[n_max],"_intermediate");
    if(gen==MAX_GEN)
      filewrite(keyboards[n_max],"_final");
  }
  
}
