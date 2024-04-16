#include <iostream>

#include "UNtoU3.h"

int main()
{
   untou3::GRP_type grp;
   std::cin >> grp[0] >> grp[1] >> grp[2] >> grp[3] >> grp[4];

   std::cout << "Input: " << untou3::grp_to_string(grp) << std::endl;

   untou3::Diffs diffs;
   auto span = diffs.get_grp(grp);

   for (const auto& diff : span) {
      auto res = untou3::add_grp(grp, diff);
      std::cout << "   " << untou3::grp_to_string(res) << std::endl;
   }
}
