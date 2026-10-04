// Topics: file I/O - text, error handling, copy utility, character I/O & seeking, binary I/O
#include "check.hpp"
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace fs = std::filesystem;

struct Record {  // trivially copyable => safe to write as raw bytes
    int id;
    double score;
    char tag[8];
};

// Copy utility: stream the file in chunks.
bool CopyFile(const fs::path& from, const fs::path& to) {
    std::ifstream in{from, std::ios::binary};
    std::ofstream out{to, std::ios::binary};
    if (!in || !out) return false;
    char buf[4096];
    while (in.read(buf, sizeof buf) || in.gcount() > 0) out.write(buf, in.gcount());
    return out.good();
}

int main() {
    fs::path dir = fs::temp_directory_path() / "blair_page_io_io";
    fs::create_directories(dir);
    auto text = dir / "text.txt";

    SECTION("text write / read");
    {
        std::ofstream out{text};
        out << "line one\nline two\n42 3.5\n";
    }  // RAII closes the file
    std::ifstream in{text};
    CHECK(in.is_open());
    std::string l1, l2; int i; double d;
    std::getline(in, l1);
    std::getline(in, l2);
    in >> i >> d;
    CHECK(l1 == "line one" && l2 == "line two" && i == 42 && d == 3.5);

    SECTION("error handling");
    std::ifstream missing{dir / "nope.txt"};
    CHECK(!missing && !missing.is_open());
    std::ifstream bad{text};
    int notANumber = 0;
    bad >> notANumber;  // "line" is not an int
    CHECK(bad.fail() && !bad.bad());
    bad.clear();        // reset the error state before reusing the stream
    CHECK(bad.good());
    std::ifstream thrower;
    thrower.exceptions(std::ifstream::failbit);
    bool threw = false;
    try { thrower.open(dir / "nope.txt"); } catch (const std::ios_base::failure&) { threw = true; }
    CHECK(threw);

    SECTION("character I/O & seeking");
    std::ifstream chars{text};
    char c;
    chars.get(c);
    CHECK(c == 'l');
    chars.seekg(5, std::ios::beg);
    CHECK(chars.tellg() == 5);
    chars.get(c);
    CHECK(c == 'o');
    chars.seekg(-3, std::ios::end);
    CHECK(static_cast<char>(chars.peek()) == '5' || chars.peek() == '.');

    SECTION("binary I/O");
    auto bin = dir / "data.bin";
    std::vector<Record> recs{{1, 9.5, "a"}, {2, 7.25, "b"}, {3, 1.0, "c"}};
    {
        std::ofstream out{bin, std::ios::binary};
        out.write(reinterpret_cast<const char*>(recs.data()), recs.size() * sizeof(Record));
    }
    CHECK(fs::file_size(bin) == recs.size() * sizeof(Record));
    std::ifstream bin_in{bin, std::ios::binary};
    bin_in.seekg(sizeof(Record));  // random access: jump to record #2
    Record r{};
    bin_in.read(reinterpret_cast<char*>(&r), sizeof r);
    CHECK(r.id == 2 && r.score == 7.25 && r.tag[0] == 'b');

    SECTION("copy utility");
    auto copy = dir / "copy.bin";
    CHECK(CopyFile(bin, copy));
    CHECK(fs::file_size(copy) == fs::file_size(bin));
    CHECK(!CopyFile(dir / "nope", dir / "x"));

    fs::remove_all(dir);
    std::cout << "OK\n";
}
