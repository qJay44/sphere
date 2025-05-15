#include "status.hpp"
#include "utils.hpp"

static const std::string statusPositive = clrp::format("Done"  , clrp::ATTRIBUTE::BOLD, clrp::FG::GREEN);
static const std::string statusNegative = clrp::format("Failed", clrp::ATTRIBUTE::BOLD, clrp::FG::RED);

static std::string title;
static std::string what;
static bool started;

void status::start(const std::string& titleArg, const std::string& whatArg) {
  if (started)
    error(std::format("An other status is already running ({}, [{}])", title, what));

  title = titleArg;
  what = whatArg;

  started = true;
  printf("%s [%s]...\r", title.c_str(), what.c_str());
}

void status::end(bool success) {
  if (!started)
    error("No status to end");

  if (success)
    printf("%s [%s] %s\n", title.c_str(), what.c_str(), statusPositive.c_str());
  else
    printf("%s [%s] %s\n", title.c_str(), what.c_str(), statusNegative.c_str());

  started = false;
}

