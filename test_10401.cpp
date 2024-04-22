// test_10401.cpp - a test driver for untou3::UNtoU3 class.
// 
// License: BSD 2-Clause (https://opensource.org/licenses/BSD-2-Clause)
//
// Copyright (c) 2024, Daniel Langr
// All rights reserved.
//
// Program implements the U(N) to U(3) reduction where n=2 (N=6) for the input irrep [4,2,2,2,2,0],
// which is represented by R = [1,0,4,0,1]. After the reduction, the program iterates over
// generated U(3) weights and print level dimensionalities of those that represent U(3) irreps.

#include <iostream>
#include "UNtoU3.h"

int main() {
   untou3::UNtoU3 gen; 
   // n=2 - a given HO level, N = (n+1)*(n+2)/2 = 6
   gen.generateXYZ(2);
   // generation of U(3) irreps in the input U(6) irrep
   gen.generateU3Weights({ 1, 0, 4, 0, 1 }, 6);
   // iteration over generated U(3) weights
   for (const auto & pair : gen.multMap()) {
      // get U(3) weight lables
      const auto & weight = pair.first;
      // get its level dimensionality if its nonzero and the U(3) weight is a U(3) irrep 
      if (auto D_l = gen.getLevelDimensionality(weight))
         std::cout << "[" << weight[0] << "," << weight[1] << "," << weight[2] << "] : " << D_l << "\n";
   }
}
