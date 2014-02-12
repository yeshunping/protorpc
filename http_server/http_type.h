
#include <string>

namespace protorpc {

//  Try to guess a good content-type for 'path', return "application/misc" as default type
std::string GuessContentType(const std::string& path);

}
