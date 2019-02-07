#include "tst.h"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

void
example1()
{
  TST<> empty_tree{};

  try {
    empty_tree.value();
  } catch (const std::logic_error&) {
    std::cout << "logic_error: value()\n";
  }

  try {
    empty_tree.word();
  } catch (const std::logic_error&) {
    std::cout << "logic_error: word()\n";
  }

  try {
    empty_tree.left();
  } catch (const std::logic_error&) {
    std::cout << "logic_error: left()\n";
  }

  try {
    empty_tree.center();
  } catch (const std::logic_error&) {
    std::cout << "logic_error: center()\n";
  }

  try {
    empty_tree.right();
  } catch (const std::logic_error&) {
    std::cout << "logic_error: right()\n";
  }

  std::cout << "End of example 1.\n";
}

TST<>
make_empty_TST()
{
  return TST<>{};
}

void
example2()
{
  TST<> tst{};
  TST<> tst_copy{ tst };
  TST<> tst_move{ std::move(make_empty_TST()) };
  assert(tst.empty() && tst_copy.empty() && tst_move.empty());
  std::cout << "End of example 2.\n";
}

void
example3()
{
  TST<> tst{ std::string{ "catamorphism" } };
  assert(tst.exist(std::string{ "catamorphism" }));
  assert(!tst.exist(std::string{ "cat" }));
  auto tst2{ tst + std::string{ "cat" } };
  assert(tst2.exist(std::string{ "cat" }));
  std::cout << "End of example 3.\n";
}

void
example4()
{
  TST<> tst{ std::string{ "cat" } };
  assert(tst.value() == 'c');
  assert(tst.center().value() == 'a');
  assert(tst.center().center().value() == 't');
  assert(tst.word() == false);
  assert(tst.center().word() == false);
  assert(tst.center().center().word() == true);
  assert(tst.size() == 3);
  assert((tst + std::string{ "catalog" }).size() == 7);
  assert((tst + std::string{ "dog" }).size() == 6);
  std::cout << "End of example 4.\n";
}

void
example5()
{
  TST<> tst{ std::string{ "category" } };
  assert((tst + std::string{ "functor" } + std::string{ "theory" })
           .prefix(std::string{ "catamorphism" }) == std::string{ "cat" });
  std::cout << "End of example 5.\n";
}

int
main()
{
  example1();
  example2();
  example3();
  example4();
  example5();
}
