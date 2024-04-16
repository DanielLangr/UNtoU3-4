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
#endif

#include <array>
#include <cstdint>
#include <span>
#include <string>
#include <sstream>
#include <vector>

namespace untou3
{
   inline const int L = MAXIMUM_UN_LABEL;  // maximum value of U(N) label

   using GRP_type = std::array<int16_t, L + 1>;  // type for representation of Gelfand pattern

   GRP_type bin_to_grp(uint64_t n)
   {
      GRP_type res;

      for (int k = 0; k <= L; k++)
         res[L - k] = (n >> k) & 0x01;

      return res;
   }

   uint64_t grp_to_bin(const GRP_type& grp)
   {
      uint64_t res = 0;

      for (int k = 0; k <= L; k++)
         if (grp[L - k] != 0)
            res |= 1UL << k;

      return res;
   }

   std::string grp_to_string(const GRP_type& grp)
   {
      std::stringstream s;

      s << "[";
      for (int i = 0; i < L; i++)
         s << grp[i] << ", ";
      s << grp[L] << "]";

      return s.str();
   }

   class Diffs {
      public:
         Diffs()
         {
            generate();
         }

         std::span<GRP_type> get_bin(uint64_t bin)
         {
            auto first = indexes_[bin];
            auto count = counts_[bin];

            if (count == 0)
               return {};
            else
               return { differences_.data() + first, count };
         }

         std::span<GRP_type> get_grp(const GRP_type& grp)
         {
            auto bin = grp_to_bin(grp);
            return get_bin(bin);
         }

      private:
         void generate_rules_recursive(const uint64_t bin, const GRP_type& grp, GRP_type diff, int first)
         {
            // find second nonzero:
            int second = first + 1;
            while ((second < L + 1) && (grp[second] == 0))
               second++;

            // no second nonzero:
            if (second == L + 1)
            {
               counts_[bin]++;
               differences_.push_back(diff);

               return;
            }

            for (int i = first; i <= second; i++)
            {
               diff[i]++;
               generate_rules_recursive(bin, grp, diff, second);
               diff[i]--;
            }
         }

         void generate_rules(const uint64_t bin)
         {
            auto grp = bin_to_grp(bin);

            GRP_type diff;
            for (int k = 0; k < L + 1; k++)
               diff[k] = grp[k] ? -1 : 0;

            // find first nonzero:
            int first = 0;
            while ((first < L + 1) && (grp[first] == 0))
               first++;

            // no nonzeros:
            if (first == L + 1)
            {
               indexes_[bin] = differences_.size();
               counts_[bin] = 0;

               return;
            }

            // find second nonzero:
            int second = first + 1;
            while ((second < L + 1) && (grp[second] == 0))
               second++;

            // no second nonzero:
            if (second == L + 1)
            {
               indexes_[bin] = differences_.size();
               counts_[bin] = 1;
               differences_.push_back(diff);

               return;
            }

            indexes_[bin] = differences_.size();

            for (int i = first; i <= second; i++)
            {
               diff[i]++;
               generate_rules_recursive(bin, grp, diff, second);
               diff[i]--;
            }
         }

         void generate()
         {
            uint64_t max = 1UL << (L + 1);
            
            indexes_.resize(max, 0);
            counts_.resize(max, 0);

            for (uint64_t bin = 0; bin < max; bin++)
               generate_rules(bin);
         }

         std::vector<GRP_type> differences_;  // differences data

         std::vector<size_t> indexes_;  // index of first difference 
         std::vector<size_t> counts_;  // count of differences
   };
} 

#endif  
