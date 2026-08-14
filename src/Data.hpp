#ifndef FDVS_SRC_DATA_HPP_
#define FDVS_SRC_DATA_HPP_

// glm
#include <glm/glm.hpp>

// CGAL
#include <CGAL/Advancing_front_surface_reconstruction.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/tuple.h>
#include <boost/lexical_cast.hpp>

// CPP Libraries
#include <cmath>
#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <string>
#include <vector>
#include <concepts>

namespace std {
std::ostream &operator<<(std::ostream &os, const std::array<std::size_t, 3> &f);
} // namespace std

namespace Data {

typedef std::array<double, 2> Point2D;

struct INVALID_BOUNDS_EXCEPTION {};
struct Bounds2D {
  Bounds2D(Point2D in_min, unsigned int in_steps_x, unsigned int in_steps_y,
double in_step_size_x, double in_step_size_y)
      : min(in_min), steps_x(in_steps_x), steps_y(in_steps_y),
        step_size_x(in_step_size_x), step_size_y(in_step_size_y) {}
  Point2D min;
  unsigned int steps_x;
  unsigned int steps_y;
  double step_size_x;
  double step_size_y;
};

typedef CGAL::Simple_cartesian<double> K;
typedef K::Point_3 Point_3;

struct Perimeter {
  double bound;

  Perimeter(double bound) : bound(bound) {}

  template <typename AdvancingFront, typename Cell_handle>
  double operator()(const AdvancingFront &adv, Cell_handle &c,
                    const int &index) const {
    // bound == 0 is better than bound < infinity
    // as it avoids the distance computations
    if (bound == 0) {
      return adv.smallest_radius_delaunay_sphere(c, index);
    }

    // If perimeter > bound, return infinity so that facet is not used
    double d = 0;
    d = sqrt(squared_distance(c->vertex((index + 1) % 4)->point(),
                              c->vertex((index + 2) % 4)->point()));
    if (d > bound)
      return adv.infinity();
    d += sqrt(squared_distance(c->vertex((index + 2) % 4)->point(),
                               c->vertex((index + 3) % 4)->point()));
    if (d > bound)
      return adv.infinity();
    d += sqrt(squared_distance(c->vertex((index + 1) % 4)->point(),
                               c->vertex((index + 3) % 4)->point()));
    if (d > bound)
      return adv.infinity();

    // Otherwise, return usual priority value: smallest radius of delaunay
    // sphere
    return adv.smallest_radius_delaunay_sphere(c, index);
  }
};

struct Grid {
	Grid(unsigned int y_n = 100, unsigned int x_n = 100);
	void m_scale(const Data::Bounds2D& b);
	void gengrid(unsigned int y_n = 100, unsigned int x_n = 100);
	std::vector<float> m_vertices;
	std::vector<unsigned int> m_faces;
	unsigned int m_yn, m_xn;
};

// Store Data Retrieved from Off Files
struct OffMeshData {
  OffMeshData(std::vector<float> in_vertices,
              std::vector<unsigned int> in_faces);
  OffMeshData(std::string filename);
  OffMeshData(std::shared_ptr<Data::Bounds2D> b,
              std::function<float(double, double)> f);

  template<typename F>
	  requires std::invocable<F, double, double>
	  	&& std::same_as<std::invoke_result_t<F, double, double>, float>
  OffMeshData(const Grid& input_grid, F&& f)
  {
	  vertices = input_grid.m_vertices;
	  for(int i = 2; i < input_grid.m_vertices.size(); i += 3)
		  vertices[i] = std::invoke(f, static_cast<double>(vertices[i-2]), static_cast<double>(vertices[i-1]));
	  faces = input_grid.m_faces;
	  num_of_vertices = input_grid.m_vertices.size() / 3;
	  num_of_faces = input_grid.m_faces.size() / 3;
	  ComputeProperties();
  }

  // Properties
  unsigned int num_of_vertices = 0;
  unsigned int num_of_faces = 0;
  unsigned int num_of_edges = 0;
  glm::vec3 centre = glm::vec3(1.0f);
  glm::vec3 max = glm::vec3(1.0f);
  glm::vec3 min = glm::vec3(1.0f);

  // Points
  std::vector<float> vertices;
  std::vector<unsigned int> faces;
  void Meshify(double per = 20.0f, double radius_ratio_bound = 5.0f);
  void ExportToOff(std::string file_name);
  void ScaleZ(double z_scaling);

private:
  std::vector<Point_3> points;
  std::vector<std::array<std::size_t, 3>> facets;
  void ComputeProperties();
  void ReadOffFileAttrib(unsigned int &vertices, unsigned int &facets,
                         unsigned int &edges, std::string file_name);
  void ReadOffData(std::vector<float> &vertices_arr,
                   std::vector<unsigned int> &faces_arr, std::string file_name);
};

} // namespace Data

#endif // !FDVS_SRC_DATA_H_
