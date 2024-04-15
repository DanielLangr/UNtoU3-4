#include <array>
#include <cstdint>
#include <iostream>

static const int N = 4;  // highest label

using T = std::array<int, N + 1>;  // labels go from 0 to N

T neg(const T& a)
{
   T res;

   for (int i = 0; i < N + 1; i++)
      res[i] = a[i] ? -1 : 0;

   return res;
}

void print(const T& a)
{
   std::cout << "[";
   for (int i = 0; i < N; i++)
      std::cout << a[i] << ", ";
   std::cout << a[N] << "]" << std::endl;
}
 
void generate_rules_recursive(const T& in, T diff, int first)
{
   // find second nonzero:
   int second = first + 1;
   while ((second < N + 1) && (in[second] == 0))
      second++;

   // no second nonzero:
   if (second == N + 1)
   {
      print(diff);
      return;
   }

   for (int i = first; i <= second; i++)
   {
      T temp = diff;
      temp[i]++;
      generate_rules_recursive(in, temp, second);
   }
}

void generate_rules(const T& in)
{
   auto diff = neg(in);

   // find first nonzero:
   int first = 0;
   while ((first < N + 1) && (in[first] == 0))
      first++;

   // no nonzeros:
   if (first == N + 1)
      return;

   // find second nonzero:
   int second = first + 1;
   while ((second < N + 1) && (in[second] == 0))
      second++;

   // no second nonzero:
   if (second == N + 1)
   {
      print(diff);
      return;
   }

   for (int i = first; i <= second; i++)
   {
      T temp = diff;
      temp[i]++;
      generate_rules_recursive(in, temp, second);
   }
}

int main()
{
   static const uint64_t n = 1UL << N + 1;
   for (uint64_t k = 0; k < n; k++)
   {
      T in;

      for (int l = N; l >= 0; l--)
         in[l] = ((k >> l) & 0x01);

      std::cout << "Input: " << std::endl;
      print(in);

      std::cout << "Output: " << std::endl;
      generate_rules(in);

      std::cout << std::endl;
   }
}
