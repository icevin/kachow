#pragma once

#include <set>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

class FileSystem {
  public:
    virtual bool exists(const fs::path& p) = 0;
    virtual bool create_directory(const fs::path& p) = 0;
    virtual bool is_regular_file(const fs::path& p) = 0;
    virtual bool remove(const fs::path& p) = 0;
    virtual bool write_file(const fs::path& p, const std::string& body) = 0;
    virtual std::string read_file(const fs::path& p) = 0;
};

class RealFileSsystem : public FileSystem{
  public:
    bool exists(const fs::path& p);
    bool create_directory(const fs::path& p);
    bool is_regular_file(const fs::path& p);
    bool remove(const fs::path& p);
    bool write_file(const fs::path& p, const std::string& body);
    std::string read_file(const fs::path& p);
};

class FakeFileSsystem : public FileSystem{
  public:
    bool exists(const fs::path& p);
    bool create_directory(const fs::path& p);
    bool is_regular_file(const fs::path& p);
    bool remove(const fs::path& p);
    bool write_file(const fs::path& p, const std::string& body);
    std::string read_file(const fs::path& p);
  private:
    std::set<std::string> fakeFS;
};