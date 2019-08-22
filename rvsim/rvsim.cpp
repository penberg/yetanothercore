/* RISC-V RVI32 Verilator-based simulator */

#include "Vrv_core.h"
#include "verilated.h"

#include <fcntl.h>
#include <libgen.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstddef>
#include <iostream>
#include <memory>

class Image
{
  int _fd;
  void* _addr;
  size_t _len;

public:
  static auto load(const std::string& filename) -> Image;
  Image(int fd, void* addr, size_t len);
  ~Image();
  uint32_t read32(size_t addr) const;
};

auto
Image::load(const std::string& filename) -> Image
{
  auto fd = ::open(filename.c_str(), O_RDONLY);
  if (fd < 0) {
    throw std::system_error(errno, std::system_category(), "open");
  }
  struct ::stat st;
  if (::fstat(fd, &st) < 0) {
    throw std::system_error(errno, std::system_category(), "fstat");
  }
  size_t len = st.st_size;
  void* addr = ::mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);
  if (addr == MAP_FAILED) {
    throw std::system_error(errno, std::system_category(), "mmap");
  }
  return Image{fd, addr, len};
}

Image::Image(int fd, void* addr, size_t len)
  : _fd{fd}
  , _addr{addr}
  , _len{len}
{}

Image::~Image()
{
  ::munmap(_addr, _len);
  ::close(_fd);
}

uint32_t
Image::read32(size_t addr) const
{
  assert(addr < _len);
  uint32_t* data = reinterpret_cast<uint32_t*>(_addr);
  return data[addr / sizeof(uint32_t)];
}

int
main(int argc, char* argv[])
{
  /* Parse command line arguments: */
  Verilated::commandArgs(argc, argv);
  if (argc != 2) {
    std::cout << "usage: " << ::basename(argv[0]) << " [image]" << std::endl;
    std::exit(1);
  }
  std::string image_filename = argv[1];

  /* Load image: */
  std::cout << "Image: " << image_filename << std::endl;
  auto image = Image::load(image_filename);

  /* Reset the core: */
  auto core = std::make_unique<Vrv_core>();
  core->rst_i = 1;
  core->eval();
  core->rst_i = 0;

  /* Start simulation: */
  while (!Verilated::gotFinish()) {
    /* Rising clock edge: */
    core->insn_data_i = image.read32(core->insn_addr_o);
    core->clk_i = 1;
    core->eval();

    /* Falling clock edge: */
    core->clk_i = 0;
    core->eval();
  }
}
