
#include<iostream>
#include<fstream>
#include<strstream>
#include "BossUpdateSet.h"

int main(int argc, char** argv)
{
  if (argc < 2) {
    argv[1] = "tbue.txt";
    //turn -1;
  }
  BossUpdateSet us(argv[1]);
  strstream str;
  us.dump(str);
  cout << str.str();
  return 0;
}
