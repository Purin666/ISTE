#include <assert.h>
#include <sstream>

#define ASSERT_WITH_CONSOLEMSG(cond, msg) {if(!(cond)) {std::ostringstream str; str << msg; std::cerr << str.str(); std::abort();}} while(0)

#define ASSERT_WITH_MSG(cond, msg) (void)((!!(cond)) ||  (_wassert(msg, _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0))
