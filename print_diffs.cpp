#include <iostream>

#include "UNtoU3.h"

int main()
{
   untou3::Diffs diffs;

   for (int bin = 0; bin < 32; bin++)
   {
      auto grp = untou3::bin_to_grp(bin);
      std::cout << "Pattern: " << untou3::grp_to_string(grp) << ":" << std::endl;
      std::cout << "Diffs:" << std::endl;

      auto span = diffs.get_bin(bin);
      std::cout << "   count = " << span.size() << std::endl;

      for (const auto& e : span)
         std::cout << "   "
            << untou3::grp_to_string(e.first)
            << " (weight = " << e.second << ")"
            << std::endl;

      std::cout << std::endl;
   }
}
