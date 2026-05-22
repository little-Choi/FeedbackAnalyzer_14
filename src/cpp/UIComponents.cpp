#include "UIComponents.h"
#include "Constants.h"

const std::vector<std::string>& UIComponents::getCategories() {
    static const std::vector<std::string> categories = Constants::categoryNames();
    return categories;
}
