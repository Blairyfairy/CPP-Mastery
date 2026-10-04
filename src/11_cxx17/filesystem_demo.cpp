// Topics (C++17): std::filesystem - path, directory_entry, directory functions, permissions
#include "check.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

int main() {
    SECTION("path manipulation");
    fs::path p = "/home/user/docs/report.final.txt";
    CHECK(p.filename() == "report.final.txt");
    CHECK(p.stem() == "report.final");
    CHECK(p.extension() == ".txt");
    CHECK(p.parent_path() == "/home/user/docs");
    CHECK(p.is_absolute());
    CHECK((fs::path{"a"} / "b" / "c.txt").generic_string() == "a/b/c.txt");
    fs::path q = p;
    q.replace_extension(".md");
    CHECK(q.extension() == ".md");

    SECTION("directory functions");
    fs::path root = fs::temp_directory_path() / "blair_page_io_fs";
    fs::remove_all(root);
    CHECK(fs::create_directories(root / "sub" / "deep"));
    for (auto name : {"a.txt", "b.log"}) std::ofstream{root / name} << "data";
    std::ofstream{root / "sub" / "c.txt"} << "more";
    CHECK(fs::exists(root / "a.txt") && fs::is_regular_file(root / "a.txt"));
    CHECK(fs::is_directory(root / "sub"));
    CHECK(fs::file_size(root / "a.txt") == 4);

    SECTION("directory_entry & iteration");
    std::vector<std::string> top;
    for (const fs::directory_entry& e : fs::directory_iterator(root))
        top.push_back(e.path().filename().string());
    std::sort(top.begin(), top.end());
    CHECK((top == std::vector<std::string>{"a.txt", "b.log", "sub"}));
    int txtCount = 0;
    for (const auto& e : fs::recursive_directory_iterator(root))
        if (e.is_regular_file() && e.path().extension() == ".txt") ++txtCount;
    CHECK(txtCount == 2);

    SECTION("copy / rename / remove");
    fs::copy_file(root / "a.txt", root / "a_copy.txt");
    fs::rename(root / "a_copy.txt", root / "renamed.txt");
    CHECK(fs::exists(root / "renamed.txt") && !fs::exists(root / "a_copy.txt"));
    CHECK(fs::remove(root / "renamed.txt"));

    SECTION("permissions");
    auto f = root / "a.txt";
    fs::permissions(f, fs::perms::owner_read | fs::perms::owner_write);
    auto perms = fs::status(f).permissions();
    CHECK((perms & fs::perms::owner_read) != fs::perms::none);
    CHECK((perms & fs::perms::others_write) == fs::perms::none);

    SECTION("error handling via error_code overloads (no exceptions)");
    std::error_code ec;
    auto sz = fs::file_size(root / "missing", ec);
    CHECK(ec && sz == static_cast<std::uintmax_t>(-1));

    fs::remove_all(root);
    std::cout << "OK\n";
}
