#include <iostream>

#include "shad/core/algorithm.h"
#include "shad/core/unordered_set.h"

constexpr static size_t kSize = 1024;
using set_t = shad::Set<int>;
using iterator = set_t::iterator;
using value_type = set_t::value_type;
using shad_inserter_t = shad::insert_iterator<shad::unordered_set<int>>;
using shad_buffered_inserter_t =
    shad::buffered_insert_iterator<shad::unordered_set<int>>;


namespace shad {

int main(int argc, char *argv[]) {
  // unordered_set
  shad::unordered_set<int> set_;

  // create set
  for (size_t i = 0; i < kSize; ++i) {
    set_.insert(2 * (i + 1));
  }


  shad::unordered_set<int> out;
  shad::transform(
      shad::distributed_parallel_tag{}, 
      set_.begin(), set_.end(), shad_buffered_inserter_t(out, out.begin()), 
      [](const value_type &i) { return i+1; });

  std::cout << "get output set \n";
  //for (auto v: out) std::cout << v << std::endl;

  return 0;
}

}  // namespace shad
