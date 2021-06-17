#include <iostream>

#include "shad/core/algorithm.h"
#include "shad/core/unordered_set.h"
#include "shad/util/measure.h"


namespace shad {

int main(int argc, char *argv[]) {
  using iterator = shad::Set<int>::iterator;
  using value_type = shad::Set<int>::value_type;
  using shad_inserter_t = shad::insert_iterator<shad::unordered_set<int>>;
  using shad_buffered_inserter_t =
      shad::buffered_insert_iterator<shad::unordered_set<int>>;
  /***
  Question:
  1. create an unorder_set containing int value 2, 3, 4, 5
  2. use shad::transform to double each value in the unorder_set 
     and store in another unorder_set
***/

  // unordered_set
  shad::unordered_set<int> set_;




  // create set
  shad_buffered_inserter_t ins(set_, set_.begin());
  for (size_t i = 0; i < 4; ++i) {
    ins = i + 2;
  }
  ins.wait();
  ins.flush();

  for (auto v: set_) std::cout << v << std::endl;

  // shad transform algorithm
  shad::unordered_set<int> out;
  shad::unordered_set<int> out_2;
  using shad_inserter_t = shad::insert_iterator<shad::unordered_set<int>>;
  using shad_buffered_inserter_t =
    shad::buffered_insert_iterator<shad::unordered_set<int>>;

  shad::transform(
      shad::distributed_parallel_tag{}, set_.begin(), set_.end(),
      shad_inserter_t(out, out.begin()), [](const value_type &i) { return i * 2; });
  for (auto v: out) std::cout << v << std::endl;

  shad::transform(
     shad::distributed_parallel_tag{}, set_.begin(), set_.end(),
     shad_buffered_inserter_t(out_2, out_2.begin()), [](const value_type &i) { return i * 2; });
  for (auto v: out_2) std::cout << v << std::endl;
  


  return 0;
}

}  // namespace shad