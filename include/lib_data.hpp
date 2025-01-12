#include <CGAL/Advancing_front_surface_reconstruction.h>
#include <boost/lexical_cast.hpp>

typedef Point_3D std::array<float, 3>;

class data {

public:
  enum class DATA_TYPE { 2D, 3D, 4D };
  data();
  data(std::vector<Point_3D> &vertices, DATA_TYPE dimension);

  bool read_data_from_file(std::string file_path, std::string file_type);

  std::vector<unsigned int>
  cgal_xyz_to_off(std::vector<Point_3D> &vertices); // returns faces
};
