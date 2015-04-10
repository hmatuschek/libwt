#include "wtcli_trafo.hh"
#include "wt.hh"
#include "csv.hh"
#include "option_parser.hh"

#include <iostream>
#include <fstream>

using namespace wt;


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
  for (size_t i=0; i<nScales; i++, scale+=ds) { scales[i] = ds; }

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
  WaveletTransform wt(Morlet(2), scales);
  CMatrix res(x.size(), scales.size());
  wt(x, res);

  // output result
  if (to_stdout) {
    CSV::write(res.array().abs(), std::cout);
  } else {
    CSV::write(res.array().abs(), outfile);
    outfile.close();
  }

  return 0;
}
