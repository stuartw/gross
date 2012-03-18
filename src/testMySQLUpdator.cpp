
#include<iostream>
#include "BossRealTimeMySQLUpdator.h"

int main(int argc, char** argv)
{
  if (argc < 5) {
    return -2;
  }
  int id = atol(argv[1]);
  BossRealTimeUpdator* upd = new BossRealTimeMySQLUpdator(id);
  return 1-upd->updateJobParameter(id,argv[2],argv[3],argv[4]);
}
