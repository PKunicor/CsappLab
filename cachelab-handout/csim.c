#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "cachelab.h"

#define MaxSetbits 5
#define MaxLine 4
#define MaxBytebits 5

typedef struct Myline{
  int valid; //有效位
  unsigned long int tag;   //tag标记  十进制
  int lru_num;   // 最近使用标记 0 - (E - 1)
}Myline;
typedef struct Mycache{
  int S;            // set 大小
  int E;            // line 大小
  int B;            //偏移大小
  int size_Tag;     // tag bits = 64 - s- b
  Myline **set;    //set数组
}Mycache;



//get command line arguments
//p_s s的指针   p_E E的指针   p_b b的指针    fn 文件名
void Mygetopt(int argc, char *argv[], int *p_s, int *p_E, int *p_b, char *fn, int *flag_v);   
//16进制转换十进制
unsigned long int Hex2dec(char *hex);
//返回tag
unsigned long int GetDecTag(unsigned long int dec, int s, int b);
//返回组号
int GetDecSetNum(unsigned long int dec, int s, int b);
//返回这个dec地址下当前偏移量开始剩余的字节数
int GetDecRestByte(unsigned long int dec, int s, int b, int B);
//初始化set
void Initset(Mycache *mc);
//处理最近未使用
int GetminLRU(Myline *set, int E);
//是否set满了
int IsFullSet(Myline *set, int E);
//命中处理  
void HitDeal(Myline *set, int E, int Eindex);
//未命中 加载cache
// 有空余line加载
void MissLoadDeal(Myline *set, int E, int Eindex, unsigned long int dec_tag);
//line替换
void MissEvictionDeal(Myline *set, int E, int delindex, unsigned long int dec_tag);
//访问处理   返回1 hit   2  miss  3 miss eviction
int LoadDeal(Myline *set, int E, unsigned long int now_tag, int flag_v);
void StoreDeal(Myline *set, int E, unsigned long int now_tag, int flag_v);
void ModifyDeal(Myline *set, int E, unsigned long int now_tag, int flag_v);









int main(int argc, char *argv[]) {
  FILE *fp;
  int hit_count, miss_count, eviction_count;
  int s, E, b;
  int flag_v = 0;     //opt v 的标记
  char *file_name;
  char optcache = ' ';
  char ignorechar = ' ';
  char hex[65] = "";
  int size = 0;        //个数
  unsigned long int dec;


  hit_count = 0;
  miss_count = 0;
  eviction_count = 0;
  Mycache cache;
  file_name = (char *)malloc(sizeof(char) * 30);
  Mygetopt(argc, argv, &s, &E, &b, file_name, &flag_v);
  cache.S = pow(2, s);
  cache.E = E;
  cache.B = pow(2, b);
  cache.size_Tag = 64 - s - b;
  Initset(&cache);

  fp = fopen(file_name, "r");
  while(fscanf(fp, " %c %[^,]%c%d", &optcache, hex, &ignorechar, &size) != -1){
    if(optcache == 'I'){
      fscanf(fp, "%c", &ignorechar);
      continue;
    }
    if(flag_v == 1){
      printf("%c %s,%d", optcache, hex, size);
    }
    dec = Hex2dec(hex);
    //========处理内存访问
    int setnum = 0;  //组号
    int flag_kind = 0;   //cache 类型   hit miss  
    unsigned long int dectag = 0;
    //int restbyte = 0;  // 当前偏移量下剩余的字节数
    //int loadbyte = 0;  //已加载的字节数
    unsigned long int loaddec = dec;   //当前加载的地址
    setnum = GetDecSetNum(loaddec, s, b);
    dectag = GetDecTag(loaddec, s, b);
    //restbyte = GetDecRestByte(loaddec, s, b, cache.B);   //剩余加载的字节数
    flag_kind = LoadDeal(cache.set[setnum], E, dectag, flag_v);
    /*loadbyte += restbyte;
    loaddec += restbyte;   //当前地址加上剩余字节数  如果加载字节数不够  这就是下一个加载地址
    while(loadbyte < size){
      setnum = GetDecSetNum(loaddec, s, b);
      dectag = GetDecTag(loaddec, s, b);
      LoadDeal(cache.set[setnum], E, dectag, flag_v);
      loadbyte += cache.B;
      loaddec += cache.B;
    }*/
    if(flag_kind == 1){
      hit_count++;
      if(flag_v == 1){
        printf(" hit");
      }
    }
    else{
      if(flag_kind == 2){
        miss_count++;
        if(flag_v == 1){
          printf(" miss");
        }
      }
      else{
        miss_count++;
        eviction_count++;
        if(flag_v == 1){
          printf(" miss eviction");
        }
      }
    }
    if(optcache == 'M'){
      hit_count++;   //修改  store一定hit
      if(flag_v == 1){
        printf(" hit\n");
      }
    }
    else{
      if(flag_v == 1){
        printf("\n");
      }
    }


    //=====================
    fscanf(fp, "%c", &ignorechar);
  }
  

  printSummary(hit_count, miss_count, eviction_count);
  return 0;
}

void Mygetopt(int argc, char *argv[], int *p_s, int *p_E, int *p_b, char *fn, int *flag_v){
  int opt;
  while ((opt = getopt(argc, argv, "vh:s:E:b:t:")) != -1) {
    switch (opt) {
      case 's':
        *p_s = atoi(optarg);
        break;
      case 'E':
        *p_E = atoi(optarg);
        break;
      case 'b':
        *p_b = atoi(optarg);
        break;
      case 't':
        strcpy(fn, optarg);
        break;
      case 'v':
        *flag_v = 1;
        break;
    }
  }
}

unsigned long int Hex2dec(char *hex){
  int len = strlen(hex);
  unsigned long int dec = 0;
  int pow16 = 1;
  for(int i = len - 1; i >= 0; i--){
    if(isdigit(hex[i])){
      dec += (hex[i] - '0') * pow16;
    }
    else{
      dec += (hex[i] - 87) * pow16;
    }
    pow16 *= 16;
  }
  return dec;
}
unsigned long int GetDecTag(unsigned long int dec, int s, int b){
  return dec >> (s + b);
}
int GetDecSetNum(unsigned long int dec, int s, int b){
  int setbits = (1 << s);
  setbits = ~(~setbits + 1);
  dec = dec >> (b);
  return dec & setbits;   //返回set组号
}
int GetDecRestByte(unsigned long int dec, int s, int b, int B){
  int Bytebits = 1 << b;
  Bytebits = ~(~Bytebits + 1);
  dec = dec & Bytebits;   //剩余偏移量
  return B - dec;  //返回剩余的字节数
}
void Initset(Mycache *mc){
  int S = (*mc).S;
  int E = (*mc).E;
  (*mc).set = (Myline **)malloc(sizeof(Myline *) * S);
  
  for(int i = 0; i < S; i++){
    (*mc).set[i] = (Myline *)malloc(sizeof(Myline) * E);
  }
  for(int i = 0; i < (*mc).S; i++){
    for(int j = 0; j < (*mc).E; j++){
      (*mc).set[i][j].valid = 0;  //有效位为0
      (*mc).set[i][j].lru_num = 0;
    }
  }
}
int GetminLRU(Myline *set, int E){
  int min = 1000;
  int k = 0;
  for(int i = 0; i < E; i++){
    if(set[i].valid == 0){   // 如果存在一个无效位置 直接返回
      k = i;
      break;  
    }
    if(set[i].lru_num < min){  
      min = set[i].lru_num;
      k = i;
    }
  }
  return k;
}


int IsFullSet(Myline *set, int E){
  for(int i = 0; i < E; i++){
    if(set[i].valid == 0){
      return 0;
    }
  }
  return 1;
}
void HitDeal(Myline *set, int E, int Eindex){
  int nowLRU = set[Eindex].lru_num;
  //把比现在要访问的cache set中lrunum大的全部减一  
  for(int i = 0; i < E; i++){
    if(set[i].lru_num > nowLRU){
      set[i].lru_num--;
    }
  }
  set[Eindex].lru_num = E - 1;  //最新访问的cache LRU设置为最大
}
void MissLoadDeal(Myline *set, int E, int Eindex, unsigned long int dec_tag){

  //把比现在要访问的cache set中lrunum大的全部减一  
  for(int i = 0; i < E; i++){
    if(set[i].lru_num > 0){
      set[i].lru_num--;
    }
  }
  set[Eindex].tag = dec_tag;
  set[Eindex].valid = 1;  
  set[Eindex].lru_num = E - 1;  //最新访问的cache LRU设置为最大
}

void MissEvictionDeal(Myline *set, int E, int delindex, unsigned long int dec_tag){
  //将要替换的line命中  改为最近访问过,然后再替换掉tag值
  HitDeal(set, E, delindex);
  set[delindex].tag = dec_tag;
}
int LoadDeal(Myline *set, int E, unsigned long int now_tag, int flag_v){
  int i = 0;
  for(i = 0; i < E; i++){
    if(set[i].tag == now_tag && set[i].valid == 1){
      HitDeal(set, E, i);
      break;
    } 
  }
  if(i == E){
    int Lindex = GetminLRU(set, E);
    if(IsFullSet(set, E) == 1){
      //满的  替换
      MissEvictionDeal(set, E, Lindex, now_tag);
      return 3;
    }
    else{
      MissLoadDeal(set, E, Lindex, now_tag);
      return 2;
    }
  }
  else{
    return 1;
  }
}
void StoreDeal(Myline *set, int E, unsigned long int now_tag, int flag_v){
  //Store 相当于 Load

}
void ModifyDeal(Myline *set, int E, unsigned long int now_tag, int flag_v){
  //int kind = LoadDeal(set, E, now_tag,flag_v);
}


