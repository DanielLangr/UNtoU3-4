// UNtoU3.h - a header file that implements the algorithm for reduction of an U(N) irreducible representation (irrep) into U(3) irreps.
// 
// License: BSD 2-Clause (https://opensource.org/licenses/BSD-2-Clause)
//
// Copyright (c) 2024, Daniel Langr
// All rights reserved.

#ifndef UNTOU3_H
#define UNTOU3_H

#ifndef MAXIMUM_UN_LABEL
#define MAXIMUM_UN_LABEL 4

#include <array>
#include <cstdint>
#include <span>
#include <vector>

namespace untou3
{
   inline const int L = MAXIMUM_UN_LABEL;  // maximum value of U(N) label

   using GRP_type = std::array<int16_t, L + 1>;  // type for representation of Gelfand pattern

   class Diffs {
      public:
         void generate()
         {
            uint64_t max = 1UL << (L + 1);
         }

         std::span<GRP_Type> get(const GRP_Type& in)
         {
            auto index = grp_to_bin(in);

            auto first = indexes_[index];
            auto count = counts_[index];

            if (count == 0)
               return {};
            else
               return { differences_.data() + first, count };
         }

      private:
         uint64_t grp_to_bin(const GRP_Type& in)
         {
            uint64_t res = 0;

            for (int k = 0; k <= L; k++)
               if (in[k] != 0)
                  res |= 1UL << (L - k);

            return res;
         }

         std::vector<GRP_type> differences_;  // differences data

         std::vector<size_t> indexes_;  // index of first difference 
         std::vector<size_t> counts_;  // count of differences
   };

} 

#endif  
