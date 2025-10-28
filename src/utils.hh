#pragma once

#define BREAKPOINT(LABEL) {                               \
  std::cout << "========================================" \
            << LABEL                                      \
            << "========================================" \
            << std::endl;                                 \
  }
