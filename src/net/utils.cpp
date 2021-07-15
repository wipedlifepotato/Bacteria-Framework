#include "utils.h"
namespace split {
// constexpr auto bufsize=2056;



std::vector<std::string> split(char *buf, const char schar, size_t bufsize) {
  size_t split_size;
  std::vector<std::string> rt;
  if (bufsize == 0)
    bufsize = strlen(buf);
  char **splitted = split_msg(buf, schar, &split_size, bufsize);
  for (size_t i = 0; i < split_size; i++) {
    rt.push_back(splitted[i]);
  }
  free_splitted(splitted, split_size);
  return rt;
}

}; // namespace split
