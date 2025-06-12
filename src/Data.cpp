#include <boost/lexical_cast.hpp>
#include <fstream>
#include <iostream>

#include "Data.hpp"

void Data::ReadOffFileAttrib(unsigned int &vertices, unsigned int &facets,
                             unsigned int &edges, std::string file_name) {
  std::ifstream in(file_name);
  std::string buffer;
  in >> buffer;
  if (buffer == "OFF") {
    in >> buffer;
    vertices = boost::lexical_cast<unsigned int>(buffer);
    in >> buffer;
    facets = boost::lexical_cast<unsigned int>(buffer);
    in >> buffer;
    edges = boost::lexical_cast<unsigned int>(buffer);
  }
  in.close();
}

void Data::ReadOffData(std::vector<float> &vertices_arr,
                       std::vector<unsigned int> &faces_arr,
                       std::string file_name) {

  unsigned int vertices = 0, facets = 0, edges = 0;
  std::ifstream in(file_name);
  std::string buffer;
  float buffer_float;
  unsigned int buffer_uint = 0;

  in >> buffer;
  if (buffer == "OFF") {
    in >> buffer;
    vertices = boost::lexical_cast<unsigned int>(buffer);
    in >> buffer;
    facets = boost::lexical_cast<unsigned int>(buffer);
    in >> buffer;
    edges = boost::lexical_cast<unsigned int>(buffer);

    // Read Vertices
    unsigned int it = 0;
    unsigned int counter_v = 1;
    while (it <= 3 * vertices) {
      in >> buffer;
      buffer_float = boost::lexical_cast<float>(buffer);
      if ((counter_v % 3) == 0) {
        vertices_arr.push_back(-buffer_float);
      } else
        vertices_arr.push_back(buffer_float);
      it++;
      counter_v++;
    }

    // Read Faces
    unsigned int counter = 1; // I don't know why but one makes it work
    while (it <= (3 * vertices + 4 * facets)) {
      in >> buffer;
      buffer_uint = boost::lexical_cast<unsigned int>(buffer);
      if ((counter % 4) != 0) {
        faces_arr.push_back(buffer_uint);
        if (counter < 50)
          std::cerr << buffer_uint << " ";
      }
      it++;
      counter++;
    }
  }
  in.close();
}
