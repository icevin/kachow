#include <iostream>
#include <sstream>

#include "filesystem.hh"
#include <boost/log/trivial.hpp>

namespace fs = boost::filesystem;

bool RealFileSsystem::exists(const fs::path& p)
{
    BOOST_LOG_TRIVIAL(info) << "Exist: " << p;
    return fs::exists(p);
}

bool RealFileSsystem::create_directory(const fs::path& p)
{
    BOOST_LOG_TRIVIAL(info) << "create: " << p;
    return fs::create_directory(p);
}

bool RealFileSsystem::is_regular_file(const fs::path& p)
{
    BOOST_LOG_TRIVIAL(info) << "regular: " << p;
    return fs::is_regular_file(p);
}

bool RealFileSsystem::remove(const fs::path& p)
{
    BOOST_LOG_TRIVIAL(info) << "remove: " << p;
    return fs::remove(p);
}

bool RealFileSsystem::write_file(const fs::path& p, const std::string& body)
{
    fs::ofstream entity(p);
    entity << body;
    entity.close();
    return true;
}

std::string RealFileSsystem::read_file(const fs::path& p)
{
    std::ostringstream buf;
    std::ifstream input(p.c_str());
    buf << input.rdbuf();
    return buf.str();
}

bool FakeFileSsystem::exists(const fs::path& p)
{
    std::set<std::string>::iterator it = fakeFS.find(p.string());
    return it != fakeFS.end();
}

bool FakeFileSsystem::create_directory(const fs::path& p)
{
    fakeFS.insert(p.string());
    return true;
}

bool FakeFileSsystem::is_regular_file(const fs::path& p)
{
    return true;
}

bool FakeFileSsystem::remove(const fs::path& p)
{
    fakeFS.erase(p.string());
    return true;
}

bool FakeFileSsystem::write_file(const fs::path& p, const std::string& body)
{
    fakeFS.insert(p.string());
    return true;
}

std::string FakeFileSsystem::read_file(const fs::path& p)
{
    if (exists(p))
        return "Read file";
    else
        return "No such file";
}