#include "wtcli_trafo.hh"
#include "wavelettransform.hh"
#include "utils/csv.hh"
#include "utils/option_parser.hh"

#include <iostream>
#include <fstream>

using namespace wt;

wt::Scalar angle(const wt::CScalar &a) {
  return std::atan2(a.real(),a.imag());
}


int
wtcli_transform(Opt::Parser &parser)
{
  size_t nScales = 16;
  std::string scaledef = parser.get_option("scales").back();
  size_t offset = 0;
  double minScale = std::stod(scaledef, &offset);
  if (offset >= scaledef.size()) {
    std::cerr << "Illegal scale definition: requires MINSCALE:MAXSCALE[:#SCALES]" << std::endl;
    return -1;
  }
  scaledef = scaledef.substr(offset+1);
  double maxScale = std::stod(scaledef, &offset);
  if (offset < scaledef.size()) {
    scaledef = scaledef.substr(offset+1);
    nScales = std::stoi(scaledef, &offset);
  }

  RVector scales(nScales);
  double scale = minScale, ds = (maxScale-minScale)/nScales;
  for (size_t i=0; i<nScales; i++, scale+=ds) { scales[i] = scale; }

  // Read input
  RMatrix input;
  if ("-" == parser.get_values("infile").back()) {
    CSV::read(input, std::cin);
  } else {
    std::ifstream infile(parser.get_values("infile").back());
    if (! infile.is_open()) {
      std::cerr << "Can not open file '" << parser.get_values("infile").back()
                << "' for input." << std::endl;
      return -1;
    }
    CSV::read(input, infile);
  }

  size_t input_column = 0;
  if (parser.has_option("column")) {
    input_column = std::stoi(parser.get_option("column").back());
  }
  CVector x = input.col(input_column).cast<CScalar>();

  // Determine output
  bool to_stdout = false;
  std::ofstream outfile;
  if ("-" == parser.get_values("outfile").back()) {
    to_stdout = true;
  } else {
    outfile.open(parser.get_values("outfile").back());
    if (! outfile.is_open()) {
      std::cerr << "Can not open file '" << parser.get_values("outfile").back()
                << "' for output." << std::endl;
      return -1;
    }
  }

  // Create and perform transform
  WaveletTransform wt(Morlet(2), scales, parser.has_flag("subsample"));
  CMatrix res(x.size(), scales.size());
  wt(x, res);

  // output result
  if (to_stdout) {
    if (parser.has_flag("abs")) {
      CSV::write(res.array().abs(), std::cout);
    } else if (parser.has_flag("angle")) {
      CSV::write(res.unaryExpr(std::ptr_fun(angle)), std::cout);
    } else if (parser.has_flag("real")) {
      CSV::write(res.real(), std::cout);
    } else if (parser.has_flag("imag")) {
      CSV::write(res.imag(), std::cout);
    } else {
      CSV::write(res, std::cout);
    }
  } else {
    if (parser.has_flag("abs")) {
      CSV::write(res.array().abs(), outfile);
    } else if (parser.has_flag("angle")) {
      CSV::write(res.unaryExpr(std::ptr_fun(angle)), outfile);
    } else if (parser.has_flag("real")) {
      CSV::write(res.real(), outfile);
    } else if (parser.has_flag("angle")) {
      CSV::write(res.imag(), outfile);
    } else {
      CSV::write(res, outfile);
    }
    outfile.close();
  }

  return 0;
}
