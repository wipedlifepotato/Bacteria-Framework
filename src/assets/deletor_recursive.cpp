#include <cstring>
#include <dirent.h>
#include <getopt.h>
#include <iostream>
#include <map>
#include <sys/types.h>
#include <unistd.h>
#ifdef __WIN32
constexpr auto defDirDelimeter = '\\';
#else
constexpr auto defDirDelimeter = '/';
#endif

namespace FileSystem {
using sfPair_t = std::pair<std::string, double long>;
namespace util {
namespace size {
sfPair_t isOf(double long bytes) { // change name
  constexpr const char *m[] = {"bytes", "kbytes", "mBytes", "gBytes", "tBytes"};
  unsigned char i = 0;
  while (bytes / 1024 >= 1.0 && i < sizeof(m)) {
    bytes /= 1024;
    i++;
  }
  return {m[i], bytes};
}
} // namespace size

} // namespace util
class dir {
private:
  std::string m_namedir;
  DIR *m_dir;

public:
  ~dir(void) { closedir(m_dir); }
  dir(std::string dir) : m_namedir(dir), m_dir(opendir(dir.c_str())) {
    if (m_dir == nullptr)
      throw(("Can't open dir " + dir));
    // std::cout << "Opened dir: " << dir << std::endl;
  }
  using filepair_t = std::map<std::string, size_t>;
  filepair_t filepair;
  filepair_t get(std::string path, DIR *t) {
    filepair_t rt;
    struct dirent *d;
    while ((d = readdir(t))) {
      std::string fullpath = (path + defDirDelimeter + d->d_name);
      if (d->d_type == DT_DIR) {
        if (std::string(d->d_name) == "." || (strcmp(d->d_name, "..") == 0))
          continue;
        //         std::cout << "open directory: " << fullpath << std::endl;
        auto anotherdir = get(fullpath);
        for (auto f : anotherdir) {
          rt[f.first] = f.second;
        }
        continue;
        // DT_REG
      }
      //       std::cout << "Type: " << int(d->d_type) << std::endl;
      FILE *f = fopen(fullpath.c_str(), "rb");
      if (f == nullptr)
        // fprintf
        std::cerr << "Warning: can't open " << fullpath << std::endl;
      else {
        size_t sf;
        fseek(f, 0, SEEK_END);
        sf = ftell(f);
        // rewind(f); // fseek(f, 0, SEEK_SET);
        fclose(f);
        //             std::cout << " Size of file: " << sf << std::endl;
        rt[fullpath] = sf; // 1024; // in kBytes
      }
    } // while
    return rt;
  }
  filepair_t get(void) { return get(m_namedir, m_dir); }
  filepair_t get(std::string path) {
    if (path == "." || path == "..")
      return {};
    DIR *tmp = opendir(path.c_str());
    if (tmp == nullptr)
      throw("Can't open dir " + path);
    return get(path, tmp);
    closedir(tmp);
  }
};
} // namespace FileSystem
/*std::pair<std::string, double long>
isSizeOf(double long bytes){
        constexpr const char * m[]=
        {"bytes", "kbytes", "mBytes", "gBytes", "tBytes"};
        unsigned char i = 0;
        while( bytes/1024 >= 1.0 && i < sizeof(m) ){
                bytes/=1024;
                i++;
        }
        return {m[i], bytes};
}*/
namespace Shred {
void doit(FileSystem::sfPair_t pair, size_t count) {
  FILE *f = fopen(pair.first.c_str(), "wb");
  if (f == nullptr) {
    std::cerr << "Warning: can't open - " << pair.first << std::endl;
    return;
  }
  char *buf = new char[128];
  fseek(f, SEEK_END, 0);
  auto size_file = ftell(f);
  fseek(f, SEEK_SET, 0);
  auto wrand = [&buf, f](size_t c) {
    srand(time(NULL));
    for (size_t i = 0; i < c; i++) {
      buf[i] = static_cast<char>(rand());
    }
    fwrite(buf, c, 1, f);
  };
  for (unsigned long i = 0; i < size_file; i += 128) {
    for (count; count--;)
      wrand(128);
  }
  delete buf;
  fclose(f);
}
void shred(std::string fullpath) {
  char command[256];
  bzero(command, sizeof(command));
  sprintf(command, "shred -u \"%s\"\n", fullpath.c_str());
  system(command);
}
} // namespace Shred
using namespace FileSystem;

void usage(void) {
  puts(__DATE__ " deletor dada; usage: ./dada [options] path to dir\n"
                "-s 3030 --maxsize=3030 size in bytes\n"
                "-z --system_shred -> use system shred\n"
                "--help\n");
}
int main(int argc, char const *const *argv) // arg count, arg values
{
  // GetOpt
  char c;
  int option_index;

  static size_t maxSize = 0;
  static bool useSystemShred = false;
  static struct option long_options[] = {{"maxsize", required_argument, 0, 's'},
                                         {"help", required_argument, 0, 'h'},
                                         {"system_shred", no_argument, 0, 'z'},
                                         {0, 0, 0, 0}};

  while ((c = getopt_long(argc, const_cast<char **>(argv), "s:hz", long_options,
                          &option_index)) > 0) {
    switch (c) {
    case 's':
      maxSize = atoll(optarg);
      break;

    case 'z':
      useSystemShred = true;
      break;
    case 'h':
    default:
      usage();
      return 0;
      break;
    }
  }
  // geopt...
  if (maxSize == 0)
    return fprintf(stderr, "[%s -h]\n", argv[0]);
  std::string dirpath = argv[argc - 1];
  try {
    auto myDir = dir(dirpath);
    auto files = myDir.get();
    std::cout << "Foreach myDir map: " << std::endl;
    for (auto file : files) {
      std::cout << "FUllpath: " << file.first << std::endl;
      auto sOf = FileSystem::util::size::isOf(file.second);
      std::cout << "Size in bytes: " << file.second << std::endl;
      std::cout << "Size of file: " << sOf.second << " " << sOf.first
                << std::endl;
      if (file.second > maxSize) {
        printf("%d > %d\n", file.second, maxSize);
        puts("Shred it!");
        if (useSystemShred)
          Shred::shred(file.first);
        else
          Shred::doit(file, 33);
        puts("Continue");
      }
      for (unsigned int i = 60; i--;)
        printf("%c", '~');
      puts("");
    }
  } catch (std::string e) {
    std::cerr << e << std::endl;
  };

  return 0;
}
