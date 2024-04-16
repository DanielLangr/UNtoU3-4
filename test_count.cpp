#include <iostream>

#include "UNtoU3.h"

untou3::Diffs diffs;

unsigned long counter = 0;

void process_grp(const untou3::GRP_type& grp)
{
   auto span = diffs.get_grp(grp);

   if (span.size() == 0)
   {
      counter++;
      return;
   }

   for (const auto& diff : span) {
      auto res = untou3::add_grp(grp, diff);
      process_grp(res);
   }
}

int main()
{
   untou3::GRP_type grp;
   std::cin >> grp[0] >> grp[1] >> grp[2] >> grp[3] >> grp[4];

   std::cout << "Input: " << untou3::grp_to_string(grp) << std::endl;
   process_grp(grp);

   std::cout << "Count: " << counter << std::endl;
}
