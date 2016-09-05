#ifndef __WT_CSV_HH__
#define __WT_CSV_HH__

#include <ostream>
#include <istream>
#include <list>
#include "types.hh"

namespace wt {

class CSV
{
public:
  template <class ArrayClass>
  static void read(ArrayClass &result, std::istream &stream,
                   const std::string &del="\t", size_t skipRows=0)
  {
    typedef typename ArrayClass::Scalar Scalar;

    std::string buffer;
    std::list< std::list<Scalar> > values;
    size_t columns = 0;
    // Skip rows
    for (size_t i=0; i<skipRows; i++) { std::getline(stream, buffer); }
    // Read content
    while ((!stream.eof()) && (!stream.bad()) && (!stream.fail())) {
      // Read line
      std::getline(stream, buffer);
      if (buffer.empty()) { continue; }
      // split the line
      size_t offset = 0;
      values.push_back(std::list<Scalar>());
      while (offset != std::string::npos) {
        // Read a number
        size_t n;
        values.back().push_back(std::stod(buffer.substr(offset), &n));
        // adjust offset
        offset += n;
        // find del.
        size_t idx = buffer.find(del, offset);
        // If found -> adjust offset, other wise end
        if (idx != std::string::npos) { offset = idx+1; }
        else { offset = idx; }
      }
      // Update max column
      columns = std::max(columns, values.back().size());
    }
    // Allocate output
    result.resize(values.size(), columns);
    typename std::list< std::list<Scalar> >::iterator row = values.begin();
    for (size_t i=0; row != values.end(); row++, i++) {
      typename std::list<Scalar>::iterator value = row->begin();
      for (size_t j=0; value != row->end(); value++, j++) {
        result(i,j) = *value;
      }
    }
    // done.
  }

  template <class Derived>
  static void write(const Eigen::DenseBase<Derived> &data, std::ostream &stream,
                    const std::string &del="\t")
  {
    if ((0 >= data.rows()) || (0 >= data.cols())) { return; }
    for (int i=0; i<data.rows(); i++) {
      for (int j=0; j<data.cols(); j++) {
        stream << data(i,j);
        if (j < (data.cols()-1)) { stream << del; }
      }
      if (i < (data.rows()-1)) { stream << std::endl; }
    }
  }
};

}

#endif // __WT_CSV_HH__
