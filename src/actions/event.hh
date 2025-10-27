#pragma once

#include "types.hh"

#include <G4Event.hh>

#include <functional>

struct EventCounter {
   EventCounter() : current_(0), print_mod_(1) {}
  ~EventCounter() {}

  void next() {
    current_++;
    if (current_ == 10 * print_mod_) {
      print_mod_ *= 10;
    }
    if (current_ % print_mod_ == 0) {
      std::cout << "Event #" << std::setw(10) << current_ << std::endl;
    }
  }
private:
  u32 current_;
  u32 print_mod_;
};

std::function<void(const G4Event*)> store_event();
