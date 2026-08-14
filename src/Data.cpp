#include <boost/lexical_cast.hpp>
#include <fstream>
#include <iostream>

#include "Data.hpp"

void Data::Grid::gengrid(unsigned int y_n, unsigned int x_n)
{
	// Runs in linear time with respect to the grid size
	m_vertices.reserve(3 * x_n * y_n);
	for(unsigned int y = 0; y < y_n; y++)
	{
		for(unsigned int x = 0; x < x_n; x++)
		{
			m_vertices.push_back(x);
			m_vertices.push_back(y);
			m_vertices.push_back(0);
		}
	}

	for(unsigned int i = 0; i < m_vertices.size() / 3 - x_n; i++)
	{
		if((i + 1) % x_n != 0 && i % x_n != 0)
		{
			// The two triangles above
			m_faces.insert(m_faces.end(), {i, x_n + i, x_n + i - 1});
			m_faces.insert(m_faces.end(), {i, i + 1, x_n + i});
		} else if((i + 1) % x_n == 0)
			m_faces.insert(m_faces.end(), {i, x_n + i, x_n + i - 1});
		else
			m_faces.insert(m_faces.end(), {i, i + 1, x_n + i});

	}
}

Data::Grid::Grid(unsigned int y_n, unsigned int x_n) : m_yn(y_n), m_xn(x_n)
{
	gengrid(y_n, x_n);
}

void Data::Grid::m_scale(const Data::Bounds2D& b)
{
	// Regenerate Grid if Needed
	// Hopefully, this doesn't happen too often
	if(b.steps_x != m_xn || b.steps_y != m_yn)
	{
		m_xn = b.steps_x;
		m_yn = b.steps_y;
		m_vertices = {};
		m_faces = {};
		gengrid(b.steps_x, b.steps_y);
	}
	// Translate (0,0) to min 
	for(int i = 0; i < m_vertices.size(); i++)
	{
		switch (i % 3)
		{
			case 0: m_vertices[i] *= b.step_size_x; m_vertices[i] += b.min[0]; break;
			case 1: m_vertices[i] *= b.step_size_y; m_vertices[i] += b.min[1]; break;
		}

	}
	
}

Data::OffMeshData::OffMeshData(std::vector<float> in_vertices,
                               std::vector<unsigned int> in_faces)
    : vertices(in_vertices), faces(in_faces) {
  num_of_vertices = vertices.size() / 3;
  num_of_faces = faces.size() / 3;
  ComputeProperties();
}

Data::OffMeshData::OffMeshData(std::string filename) {
  ReadOffFileAttrib(num_of_vertices, num_of_faces, num_of_edges, filename);
  ReadOffData(vertices, faces, filename);
  ComputeProperties();
}

Data::OffMeshData::OffMeshData(std::shared_ptr<Data::Bounds2D> b,
                               std::function<float(double, double)> f) {
  unsigned int size = b->steps_x * b->steps_y;
  std::vector<std::future<float>> tasks;
  vertices = std::vector<float>(size * 3);
  {
    int index_x = 0, index_y = 0, index_z = 0;
    float x = b->min[0];
    float y = b->min[1];
    for (int i = 0; i < b->steps_x; i++, x += b->step_size_x) {
      y = b->min[1];
      for (int j = 0; j < b->steps_y; j++, y += b->step_size_y) {
        index_x = (i * b->steps_x + j) * 3 + 0;
        index_y = (i * b->steps_x + j) * 3 + 1;
        index_z = (i * b->steps_x + j) * 3 + 2;
        vertices[index_x] = x;
        vertices[index_y] = y;
        tasks.push_back(std::async(f, x, y));
      }
    }
    for (int i = 0, counter = 0; i < b->steps_x; i++) {
      for (int j = 0; j < b->steps_y; j++) {
        index_x = (i * b->steps_x + j) * 3 + 0;
        index_y = (i * b->steps_x + j) * 3 + 1;
        index_z = (i * b->steps_x + j) * 3 + 2;
        vertices[index_z] = -tasks[counter].get();
        points.push_back(
            Point_3(vertices[index_x], vertices[index_y], -vertices[index_z]));
        counter++;
      }
    }
  }
  Meshify();
  ComputeProperties();
}

void Data::OffMeshData::Meshify(double per, double radius_ratio_bound) {
  Perimeter perimeter(per);
  CGAL::advancing_front_surface_reconstruction(points.begin(), points.end(),
                                               std::back_inserter(facets),
                                               perimeter, radius_ratio_bound);
  for (auto facet : facets)
    for (int i = 0; i < 3; i++)
      faces.push_back(facet[i]);

  num_of_faces = facets.size();
}

std::ostream &std::operator<<(std::ostream &os,
                              const std::array<std::size_t, 3> &f) {
  os << "3 " << f[0] << " " << f[1] << " " << f[2];
  return os;
}

void Data::OffMeshData::ExportToOff(std::string file_name) {
  std::ofstream out(file_name + ".off");
  out << "OFF\n" << points.size() << " " << facets.size() << " 0\n";
  std::copy(points.begin(), points.end(),
            std::ostream_iterator<Point_3>(out, "\n"));
  std::copy(facets.begin(), facets.end(),
            std::ostream_iterator<std::array<std::size_t, 3>>(out, "\n"));
  out.close();
}

void Data::OffMeshData::ReadOffFileAttrib(unsigned int &vertices,
                                          unsigned int &facets,
                                          unsigned int &edges,
                                          std::string file_name) {
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

void Data::OffMeshData::ReadOffData(std::vector<float> &vertices_arr,
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

void Data::OffMeshData::ComputeProperties() {
  for (int i = 0; i < vertices.size(); i++) {
    switch (i % 3) {
    case 0:
      if (max[0] < vertices[i])
        max[0] = vertices[i];
      if (min[0] > vertices[i])
        min[0] = vertices[i];
      break;
    case 1:
      if (max[1] < vertices[i])
        max[1] = vertices[i];
      if (min[1] > vertices[i])
        min[1] = vertices[i];
      break;
    case 2:
      if (max[2] < vertices[i])
        max[2] = vertices[i];
      if (min[2] > vertices[i])
        min[2] = vertices[i];
      break;
    }
  }
  centre = glm::vec3((max[0] - min[0]) / 2, (max[1] - min[1]) / 2,
                     (max[2] - min[2]) / 2);
}

void Data::OffMeshData::ScaleZ(double z_scaling) {
  for (int i = 0; i < vertices.size(); i++)
    if (i % 3 == 2)
      vertices[i] *= z_scaling;
}
