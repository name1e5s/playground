#include <iostream>
#include <random>

using std::cin;
using std::cout;
using std::endl;

int main(int argc, const char **argv) {
  std::random_device seed;
  std::mt19937 rand(seed());
  std::uniform_int_distribution<int> dist(0, 1000);
  int result = dist(rand);

  int input;
  while (1) {
    cout << "Guess a number: ";
    cin >> input;
    if (input < 0 || input > 1000) {
      cout << "Illegal number." << endl;
    } else if (input == result) {
      cout << "You win the game!" << endl;
      break;
    } else if (input > result) {
      cout << "Input to big." << endl;
    } else {
      cout << "Input to little." << endl;
    }
  }
  return 0;
}
