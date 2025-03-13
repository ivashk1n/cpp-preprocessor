#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using filesystem::path;

path operator""_p(const char* data, std::size_t sz) {
    return path(data, data + sz);
}

// напишите эту функцию
bool PreprocessImpl(const path& in_file, ifstream& in, ofstream& outs, const vector<path>& include_directories) {

    int line_number = 0;
    string text;

    do {

        if (!getline(in, text)) { //остановка цикла при конце файла
            return true;
        }

        ++line_number;

        static regex num_reg1(R"/(\s*#\s*include\s*"([^"]*)"\s*)/");
        static regex num_reg2(R"/(\s*#\s*include\s*<([^>]*)>\s*)/");
        smatch m;

        if (regex_match(text, m, num_reg1)) {
            path include_path = string(m[1]); //путь текущего файла
            path file_directory = in_file.parent_path() / include_path; // полный путь к файлу

            if (!filesystem::exists(file_directory)) {  //существует ли файл в той же папке, что и in_file
                bool found = false;
                for (const auto& include_directory : include_directories) {
                    file_directory = include_directory / include_path;

                    if (filesystem::exists(file_directory)) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    cout << "unknown include file " << include_path.filename().string() << " at file " << in_file.string() << " at line " << line_number << endl;
                    return false;
                }

            }

            ifstream in_directory(file_directory);
            if (!in) {
                return false;
            }

            if (!PreprocessImpl(file_directory, in_directory, outs, include_directories)) {
                return false;
            }
        }



        else if (regex_match(text, m, num_reg2)) {
            path include_path = string(m[1]);
            path file_directory = in_file.parent_path() / include_path;
            bool found = false;

            for (const auto& include_directory : include_directories) {
                file_directory = include_directory / include_path;

                if (filesystem::exists(file_directory)) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                cout << "unknown include file " << include_path.filename().string() << " at file " << in_file.string() << " at line " << line_number << endl;
                return false;
            }

            ifstream in_directory(file_directory);
            if (!in) {
                return false;
            }

            if (!PreprocessImpl(file_directory, in_directory, outs, include_directories)) {
                return false;
            }
        }
        else {
            outs << text << endl;
        }



    } while (in);

    return true;
}


bool Preprocess(const path& in_file, const path& out_file, const vector<path>& include_directories) {

    ifstream in(in_file);
    if (!in) {
        return false;
    }

    ofstream outs(out_file);
    if (!outs) {
        return false;
    }

    return PreprocessImpl(in_file, in, outs, include_directories);
}

string GetFileContents(string file) {
    ifstream stream(file);

    // конструируем string по двум итераторам
    return { (istreambuf_iterator<char>(stream)), istreambuf_iterator<char>() };
}

void Test() {
    error_code err;
    filesystem::remove_all("sources"_p, err);
    filesystem::create_directories("sources"_p / "include2"_p / "lib"_p, err);
    filesystem::create_directories("sources"_p / "include1"_p, err);
    filesystem::create_directories("sources"_p / "dir1"_p / "subdir"_p, err);

    {
        ofstream file("sources/a.cpp");
        file << "// this comment before include\n"
            "#include \"dir1/b.h\"\n"
            "// text between b.h and c.h\n"
            "#include \"dir1/d.h\"\n"
            "\n"
            "int SayHello() {\n"
            "    cout << \"hello, world!\" << endl;\n"
            "#   include<dummy.txt>\n"
            "}\n"s;
    }
    {
        ofstream file("sources/dir1/b.h");
        file << "// text from b.h before include\n"
            "#include \"subdir/c.h\"\n"
            "// text from b.h after include"s;
    }
    {
        ofstream file("sources/dir1/subdir/c.h");
        file << "// text from c.h before include\n"
            "#include <std1.h>\n"
            "// text from c.h after include\n"s;
    }
    {
        ofstream file("sources/dir1/d.h");
        file << "// text from d.h before include\n"
            "#include \"lib/std2.h\"\n"
            "// text from d.h after include\n"s;
    }
    {
        ofstream file("sources/include1/std1.h");
        file << "// std1\n"s;
    }
    {
        ofstream file("sources/include2/lib/std2.h");
        file << "// std2\n"s;
    }

    assert((!Preprocess("sources"_p / "a.cpp"_p, "sources"_p / "a.in"_p,
        { "sources"_p / "include1"_p,"sources"_p / "include2"_p })));

    ostringstream test_out;
    test_out << "// this comment before include\n"
        "// text from b.h before include\n"
        "// text from c.h before include\n"
        "// std1\n"
        "// text from c.h after include\n"
        "// text from b.h after include\n"
        "// text between b.h and c.h\n"
        "// text from d.h before include\n"
        "// std2\n"
        "// text from d.h after include\n"
        "\n"
        "int SayHello() {\n"
        "    cout << \"hello, world!\" << endl;\n"s;

    assert(GetFileContents("sources/a.in"s) == test_out.str());
}

int main() {
    Test();
}