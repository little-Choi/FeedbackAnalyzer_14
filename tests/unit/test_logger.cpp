#include <catch2/catch_test_macros.hpp>

#include <sstream>

#include "Logger.h"

namespace {

class StreamCapture {
public:
    explicit StreamCapture(std::ostream& stream) : stream_(stream), previous_(stream.rdbuf(buffer_.rdbuf())) {}
    ~StreamCapture() { stream_.rdbuf(previous_); }
    std::string str() const { return buffer_.str(); }

private:
    std::ostream& stream_;
    std::ostringstream buffer_;
    std::streambuf* previous_;
};

}  // namespace

TEST_CASE("LG-01 logInfo writes INFO to stdout", "[p2][logger]") {
    StreamCapture cap(std::cout);
    Logger::logInfo("smoke info");
    REQUIRE(cap.str().find("INFO:") != std::string::npos);
    REQUIRE(cap.str().find("smoke info") != std::string::npos);
}

TEST_CASE("LG-02 logWarning writes WARNING to stdout", "[p2][logger]") {
    StreamCapture cap(std::cout);
    Logger::logWarning("smoke warning");
    REQUIRE(cap.str().find("WARNING:") != std::string::npos);
}

TEST_CASE("LG-03 logError writes ERROR to stderr", "[p2][logger]") {
    StreamCapture cap(std::cerr);
    Logger::logError("smoke error");
    REQUIRE(cap.str().find("ERROR:") != std::string::npos);
}

TEST_CASE("LG-04 logDebug respects debug mode", "[p2][logger]") {
    Logger::setDebugMode(false);
    StreamCapture capOff(std::cout);
    Logger::logDebug("hidden");
    REQUIRE(capOff.str().empty());

    Logger::setDebugMode(true);
    StreamCapture capOn(std::cout);
    Logger::logDebug("visible");
    REQUIRE(capOn.str().find("DEBUG:") != std::string::npos);
    Logger::setDebugMode(true);
}

TEST_CASE("LG-05 setDebugMode toggles flag", "[p3][logger]") {
    Logger::setDebugMode(false);
    REQUIRE_FALSE(Logger::isDebugMode());
    Logger::setDebugMode(true);
    REQUIRE(Logger::isDebugMode());
}
