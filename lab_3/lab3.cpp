#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <tuple>

template <typename Ch, typename Tr, typename... Args>
std::ostream& operator<<(std::basic_ostream<Ch, Tr>& os,
                         std::tuple<Args...> const& t) {
  std::apply(
      [&os](auto&&... args) { ((os << args << " "), ...); },
      t);  // https://stackoverflow.com/questions/6245735/pretty-print-stdtuple
  return os;
};

template <typename... Args>
class CSVParser {
 public:
  using value_type = std::tuple<Args...>;
  using reference = const value_type&;
  explicit CSVParser(std::ifstream& ifs, size_t line_offset = 0,
                     char column_delim = ' ')
      : file_(ifs), line_offset_(line_offset), column_delim_(column_delim) {
    if (!file_.is_open()) throw std::ifstream::failure("File is not open");
  }

  class Iterator {
   public:
    Iterator() : file_(nullptr){};
    Iterator(std::istream& parent, int offset, char column_delim)
        : file_(&parent), line_number_(offset), column_delim_(column_delim) {
      for (size_t i = 0; i < offset; ++i) {
        try {
          if (!getline(*file_, str_)) {
            file_ = nullptr;
            throw std::invalid_argument("Get end of file");
          }
        } catch (std::invalid_argument) {
          std::cout << "Get end of file after offset" << std::endl;
          exit(0);
        }
      }
      ++*this;
    };
    bool operator!=(const Iterator& y) { return !(*this == y); }
    bool operator==(const Iterator& y) { return (*this).file_ == y.file_; }

    reference operator*() const { return output_; }

    const Iterator operator++() {
      if (!getline(*file_, str_)) file_ = nullptr;
      ++line_number_;
      std::istringstream line(str_);
      std::string element;
      size_t index = 0;
      try {
        for_each(output_, [&](auto&& x) {
          if (getline(line, element, column_delim_)) {
            auto buf = x;
            decltype(buf) tmpValue;
            std::istringstream tmpStream(element);
            tmpStream >> tmpValue;
            x = tmpValue;
            std::ostringstream checkType;
            checkType << tmpValue;
            ++index;

            if (checkType.str() != element) {
              throw index;
            }
          }
        });
      } catch (size_t idx) {
        std::cout << "Invalid argument in " << idx << " column of "
                  << line_number_ << " line " << std::endl;
      }
      return *this;
    }
    template <typename TupleT,
              typename Fn>  // apply method can help us to apply  chos_en
                            // method on chosen data
    void for_each(TupleT&& tp, Fn&& fn) {
      std::apply(
          [&fn]<typename... T>(T&&... args) {
            (fn(std::forward<T>(args)), ...);
          },
          std::forward<TupleT>(
              tp));  // https://www.cppstories.com/2022/tuple-iteration-apply/
    }

   private:
    size_t line_number_;
    char column_delim_;
    std::string str_;
    value_type output_;
    std::istream* file_;
  };
  Iterator begin() { return Iterator(file_, line_offset_, column_delim_); }
  Iterator end() { return Iterator(); }

 private:
  std::ifstream& file_;
  size_t line_offset_;
  char column_delim_;
};

int main(int argc, const char* argv[]) {
  std::ifstream file("tableEr.csv");
  int offset = 2;
  char delim = ' ';
  CSVParser<std::string, int, double, std::string> parser(file, offset, delim);
  for (std::tuple<std::string, int, double, std::string> rs : parser) {
    std::cout << rs << std::endl;
  }

  // std::ifstream file("table2.csv");
  // int offset = 1;
  // char delim = ',';
  // CSVParser<int, std::string, int, std::string> parser(file, offset, delim);
  // for (std::tuple<int, std::string, int, std::string> rs : parser) {
  //   std::cout << rs << std::endl;
  // }
  file.close();
  return 0;
}
