#pragma once

#include "PageViewModel.h"

#include <string>

class HtmlPageRenderer {
public:
    [[nodiscard]] static std::string render(const PageViewModel& vm);
};
