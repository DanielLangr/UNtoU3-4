// UNtoU3.h - a header file that implements the algorithm for the reduction
// of an U(N) irreducible representation (irrep) with highest label L=4
// into U(3) irreps.
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
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <numeric>
#include <span>
#include <string>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <vector>

namespace untou3
{
   inline const int L = MAXIMUM_UN_LABEL;  // maximum value of U(N) label

   using GRP_type = std::array<int16_t, L + 1>;  // type for representation of Gelfand pattern

   using Element_type = std::pair<GRP_type, uint32_t>;  // differenece + its weight vector element

   GRP_type add_grp(const GRP_type& a, const GRP_type& b)
   {
      GRP_type res;

      for (int k = 0; k <= L; k++)
         res[k] = a[k] + b[k];

      return res;
   }

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

         std::span<Element_type> get_bin(uint64_t bin)
         {
            auto first = indexes_[bin];
            auto count = counts_[bin];

            if (count == 0)
               return {};
            else
               return { differences_.data() + first, count };
         }

         std::span<Element_type> get_grp(const GRP_type& grp)
         {
            auto bin = grp_to_bin(grp);
            return get_bin(bin);
         }

      private:
         int64_t get_weight(const GRP_type& diff)
         {
            int64_t res = 0;

            for (int k = 0; k <= L; k++)
               res += diff[k] * (L - k) * (-1);

            assert(res >= 0);
            assert(res <= L);

            return res;
         }

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
               differences_.emplace_back(diff, get_weight(diff));

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
               differences_.emplace_back(diff, get_weight(diff));

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

         std::vector<Element_type> differences_;  // differences data

         std::vector<size_t> indexes_;  // index of first difference 
         std::vector<size_t> counts_;   // count of differences
   };

   class UNtoU3
   {
      public:
         using U3Weight = std::array<uint32_t, 3>;  // representation of U(3) weight

      private:
         // helper haseher class; need to be here:
         struct U3Weight_hasher
         {
            // Boost-motivated:
            template <typename T>
            static void hash_combine(std::size_t& seed, const T& v) 
            {
               seed ^= std::hash<T>{}(v) + 0x9E3779B9 + (seed << 6) + (seed >> 2);
            }

            size_t operator()(const U3Weight& key) const
            {
               size_t res = 0;

               for (auto w : key)
                  hash_combine(res, w);

               return res;
            }
         };

      public:
         // type for data structure for storing U(3) weights and their multiplicities:
         using U3MultMap = std::unordered_map<U3Weight, uint32_t, U3Weight_hasher>;

         enum { NZ, NX, NY };  // definition of the order of axis for weight vectors

         // Generates HO quanta vectors for given nth HO level.
         // Need to be used befor generateU3Weights() member function is called.
         void generateXYZ(int n)
         {
            for (auto & v : xyz_)
               v.clear();

            for (int k = 0; k <= n; k++) {
               uint32_t nz = n - k;
               for (int nx = k; nx >= 0; nx--) {
                  xyz_[NZ].push_back(nz);
                  xyz_[NX].push_back(nx);
                  xyz_[NY].push_back(k - nx);
               }
            }
         }

         // Generates U(3) weights and their multiplicities for an input U(N) irrep [f].
         // [f] is specified as a top row of Gelfand pattern [n_L,...,n_1,n_0]
         // N=n_L+...+n_0 must be equal to (n+1)*(n+2)/2, where n was used as an argument of generateXYZ.
         void generateU3Weights(const GRP_type& grp, uint64_t N, U3Weight weight = {0, 0, 0})
         {
#ifndef NDEBUG
            auto N_ = std::accumulate(std::begin(grp), std::end(grp), 0);
            assert(N_ == N);
#endif

            auto span = diffs_.get_grp(grp);

            if (span.size() == 0)
            {
               mult_[weight]++;
               return;
            }

            for (const auto& e : span)
            {
               auto lower_grp = add_grp(grp, e.first);
               generateU3Weights(lower_grp, N - 1, {
                     weight[0] + e.second * xyz_[0][N - 1],
                     weight[1] + e.second * xyz_[1][N - 1],
                     weight[2] + e.second * xyz_[2][N - 1] });
            }
         }

         // Provides an access to the table of U(3) weights and their multiplicities generated by generateU3Weights.
         // Returns a constant reference to the computer representation of this table of type U3MultMap.
         const U3MultMap& multMap() const
         {
            return mult_;
         }

         // Get level dimensionality for a given U(3) weight passed as an argument.
         uint64_t getLevelDimensionality(const U3Weight& weight) const
         {
            uint32_t f1 = weight[0], f2 = weight[1], f3 = weight[2];
            if ((f1 < f2) || (f2 < f3)) return 0;

            auto u3_mult = mult_.find({f1, f2, f3});
            assert(u3_mult != mult_.end());
            auto mult = u3_mult->second;

            u3_mult = mult_.find({f1 + 1, f2 + 1, f3 - 2});
            mult += (u3_mult == mult_.end()) ? 0 : u3_mult->second;

            u3_mult = mult_.find({f1 + 2, f2 - 1, f3 - 1});
            mult += (u3_mult == mult_.end()) ? 0 : u3_mult->second;

            u3_mult = mult_.find({f1 + 2, f2, f3 - 2});
            mult -= (u3_mult == mult_.end()) ? 0 : u3_mult->second;

            u3_mult = mult_.find({f1 + 1, f2 - 1, f3});
            mult -= (u3_mult == mult_.end()) ? 0 : u3_mult->second;

            u3_mult = mult_.find({f1, f2 + 1, f3 - 1});
            mult -= (u3_mult == mult_.end()) ? 0 : u3_mult->second;

            return mult;
         }

      private:
         Diffs diffs_;  // differences for generation of lower Gelfand pattern rows

         std::array<std::vector<uint32_t>, 3> xyz_;  // HO quanta vectors generated by generateXYZ

         U3MultMap mult_;  // table of resulting U(3) irreps and their multiplicities
   };
} 

#endif  
