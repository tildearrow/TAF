#include "taf.h"

void preloadJob(PreloadPool* p) {
  p->runJob();
}

void PreloadPool::runJob() {
  sf::Image* created;
  while (!quit) {
    while (!pqueue.empty()) {
      poolLock.lock();
      for (PreloadImage& i: pool) {
        if (i.path==pqueue.front()) {
          poolLock.unlock();
          pqueue.pop();
          continue;
        }
      }
      created=new sf::Image;
      logD("preloading %s\n",pqueue.front().c_str());
      poolLock.unlock();
      if (created->loadFromFile(pqueue.front())) {
        poolLock.lock();
        pool.push_back(PreloadImage(pqueue.front()));
        pool.back().img=created;
        poolLock.unlock();
      } else {
        delete created;
      }
      pqueue.pop();
    }
    /*
    printf("pool contents:\n");
    for (PreloadImage& i: pool) {
      printf("- %s\n",i.path.c_str());
    }
    */
    usleep(50000);
  }
}

void PreloadPool::insert(string path) {
  pqueue.push(path);
}

sf::Image* PreloadPool::acquire(string path) {
  // check image pool
  poolLock.lock();
  for (PreloadImage& i: pool) {
    if (i.path==path) {
      logD("%s found in cache.\n",path.c_str());
      poolLock.unlock();
      return i.img;
    }
  }
  poolLock.unlock();
  // nothing? load image
  logD("%s not found in cache! loading.\n",path.c_str());
  sf::Image* loaded=new sf::Image;
  if (!loaded->loadFromFile(path)) {
    return NULL;
  }
  return loaded;
}

PreloadPool::PreloadPool(): quit(false) {
  thr=new sf::Thread(preloadJob,this);
  thr->launch();
}

PreloadPool::~PreloadPool() {
  quit=true;
  thr->wait();
}
