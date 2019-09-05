#include "taf.h"

void preloadJob(PreloadPool* p) {
  p->runJob();
}

void PreloadPool::runJob() {
  while (!quit) {
    printf("running.\n");
    usleep(200000);
  }
}

int PreloadPool::insert(string path) {
  return 0;
}

PreloadPool::PreloadPool(): quit(false) {
//  thr=new sf::Thread(preloadJob,this);
//  thr->launch();
}

PreloadPool::~PreloadPool() {
  quit=true;
//  thr->wait();
}
