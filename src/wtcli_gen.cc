#include "wtcli_gen.hh"
#include "utils/option_parser.hh"
#include "types.hh"
#include "utils/csv.hh"
#include <iostream>
#include <fstream>

using namespace wt;

int wtcli_generate_signal(Opt::Parser &parser) {
  bool to_stdout = false;
  std::ofstream outfile;
  if ("-" == parser.get_values("outfile").back()) { to_stdout = true; }
  else {
    outfile.open(parser.get_values("outfile").back());
    if (! outfile.is_open()) {
      std::cerr << "Can not open file '" << parser.get_values("outfile").back()
                << "' for output." << std::endl;
      return -1;
    }
  }

  Eigen::VectorXd x = Eigen::VectorXd::Zero(std::stoi(parser.get_values("samples").back()));
  std::list<size_t> deltas;
  std::list<std::string>::const_iterator delta = parser.get_option("delta").begin();
  for(; delta != parser.get_option("delta").end(); delta++) { deltas.push_back(std::stoi(*delta)); }
  std::list<double> sins;
  std::list<std::string>::const_iterator sin = parser.get_option("sin").begin();
  for(; sin != parser.get_option("sin").end(); sin++) { sins.push_back(std::stod(*sin)); }

  for (int i=0; i<x.rows(); i++) {
    // apply detlas
    if (parser.has_option("delta")) {
      std::list<size_t>::iterator item = deltas.begin();
      for(; item != deltas.end(); item++) {
        if (i == int(*item)) { x[i] += 1; }
      }
    }
    // apply sin
    if (parser.has_option("sin")) {
      std::list<double>::iterator item = sins.begin();
      for(; item != sins.end(); item++) {
        x[i] += std::sin((2*M_PI*i)/(*item));
      }
    }
  }

  if (to_stdout) {
    CSV::write(x, std::cout);
  } else {
    CSV::write(x, outfile);
    outfile.close();
  }

  return 0;
}
