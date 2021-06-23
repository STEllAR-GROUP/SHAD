#include <iostream>
#include <cstring>
#include <assert.h> 

#include "shad/runtime/handle.h"
#include "shad/runtime/locality.h"
#include "shad/runtime/runtime.h"


namespace shad {

struct exData {
  size_t counter;
  shad::rt::Locality locality;
  size_t extra;
  void reset() {
    counter = 0;
    locality = shad::rt::Locality();
  }
};

static exData globalData = {0, shad::rt::Locality(), 0ul};
static const unsigned kNumIters = 1;
static const size_t kValue = 3;

namespace detail{
  static void async_2(shad::rt::Handle & /*unused*/, const exData &data) {
    __sync_fetch_and_add(&globalData.counter, data.counter);
    globalData.locality = data.locality;
  };

} //namespace detail

static void check(const uint8_t * /*unused*/, const uint32_t /*unused*/) {
  //std::cout << globalData.locality << "counter: " << globalData.counter
  //          << std::endl;
  assert(globalData.locality == shad::rt::thisLocality());
  assert(globalData.counter ==
    (kValue + static_cast<uint32_t>(globalData.locality)) * kNumIters);
};

static void async_1(shad::rt::Handle & handle, const exData &data) {
  auto loc = shad::rt::thisLocality();
  shad::rt::asyncExecuteAt(handle, loc, detail::async_2, data);
};



int main(int argc, char *argv[]) {

  shad::rt::Handle handle;
  auto loc = shad::rt::thisLocality();
  size_t value = kValue + static_cast<uint32_t>(loc);
  exData data = {value, loc};

  for (size_t i = 0; i < kNumIters; i++) {
    shad::rt::asyncExecuteAt(handle, loc, async_1, data);
  }
  

  shad::rt::waitForCompletion(handle);
  // check
  shad::rt::executeAt(loc, check, nullptr, 0);


  return 0;
}

} // namespace shad

