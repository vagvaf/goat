// cppimport

/*PGR-GNU*****************************************************************
File: isochrone.cpp
Copyright (c) 2021 Majk Shkurti, <majk.shkurti@plan4better.de>
Copyright (c) 2020 Vjeran Crnjak, <vjeran@crnjak.xyz>
------
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 ********************************************************************PGR-GNU*/
#include "types.h"
#include "isochrone.h"

#ifdef DEBUG
std::vector<std::string> split(const std::string &input, const std::string &regex = " ")
{
  std::regex re(regex);
  std::sregex_token_iterator first{input.begin(), input.end(), re, -1}, last; // the '-1' is what makes the regex split (-1 := what was not matched)
  std::vector<std::string> tokens{first, last};
  return tokens;
}

std::vector<Edge>
read_file(std::string name_file)
{
  std::vector<Edge> data_edges;
  Edge edge;
  std::ifstream myfile;
  std::string line;
  myfile.open(name_file, std::ios::in);

  int lineCount = 0;
  std::getline(myfile, line); // ignore header line
  while (std::getline(myfile, line) && !line.empty())
  {
    std::vector<std::string> segmented = split(line, "(\\,\\[\\[)");
    std::vector<std::string> props = split(segmented[0], "(\\,)");
    edge.id = std::stoll(props[0]);
    edge.source = std::stoll(props[1]);
    edge.target = std::stoll(props[2]);
    edge.cost = std::stod(props[3]);
    edge.reverse_cost = std::stod(props[4]);
    edge.length = std::stod(props[5]);
    // Remove last brackets ]] and split geometry coordinate pairs.
    segmented[1].erase(segmented[1].length() - 2);
    std::vector<std::string> coords = split(segmented[1], "(\\]\\,\\[)");

    // Loop through coords
    std::vector<std::array<double, 2>> geometry;
    for (auto coord : coords)
    {
      std::array<double, 2> xy;
      std::vector<std::string> xy_str = split(coord, "(\\,)");
      xy[0] = std::stod(xy_str[0]);
      xy[1] = std::stod(xy_str[1]);
      geometry.push_back(xy);
    }
    edge.geometry = geometry;
    data_edges.push_back(edge);
    lineCount++;
  }

  myfile.close();
  return data_edges;
}

int main()
{
  std::cout << "Main function...(Testing)!";
  std::vector<std::array<double, 2>> points_ = {{0, 0}, {0.25, 0.15}, {1, 0}, {1, 1}};
  auto convex_hull = convexhull(points_);
  auto concave_points = concaveman<double, 16>({{0, 0}, {0.25, 0.15}, {1, 0}, {1, 1}}, {0, 2, 3}, 2, 0);

  // demo network file location
  std::string network_file = "../data/test_network.csv";
  std::vector<Edge> data_edges_vector = read_file(network_file);
  static const int64_t total_edges = data_edges_vector.size();
  Edge *data_edges = new Edge[total_edges];
  for (int64_t i = 0; i < total_edges; ++i)
  {
    data_edges[i] = data_edges_vector[i];
  }
  data_edges_vector.clear();
  std::vector<double> distance_limits = {60};
  std::vector<int64_t> start_vertices{2147483647};
  bool only_minimum_cover = false;
  auto results = compute_isochrone(data_edges, total_edges, start_vertices,
                                   distance_limits, only_minimum_cover);

  return 0;
}
#endif

// ---------------------------------------------------------------------------------------------------------------------
// pybind11 bindings
// ---------------------------------------------------------------------------------------------------------------------
#ifndef DEBUG

struct Isochrone
{
  // Calculate isochrone for a given set of start vertices.
  Result calculate(
      py::array_t<int64_t> &edge_ids_, py::array_t<int64_t> &sources_,
      py::array_t<int64_t> &targets_, py::array_t<double> &costs_,
      py::array_t<double> &reverse_costs_, py::array_t<double> &length_, std::vector<std::vector<std::array<double, 2>>> &geometry,
      py::array_t<int64_t> start_vertices_,
      py::array_t<double> distance_limits_,
      bool only_minimum_cover_)
  {
    auto total_edges = edge_ids_.shape(0);

    auto edge_ids_c = edge_ids_.unchecked<1>();
    auto sources_c = sources_.unchecked<1>();
    auto targets_c = targets_.unchecked<1>();
    auto costs_c = costs_.unchecked<1>();
    auto reverse_costs_c = reverse_costs_.unchecked<1>();
    auto length_c = length_.unchecked<1>();

    auto start_vertices_c = start_vertices_.unchecked<1>();
    auto total_start_vertices = start_vertices_.shape(0);

    auto distance_limits_c = distance_limits_.unchecked<1>();
    auto total_distance_limits = distance_limits_.shape(0);

    bool only_minimum_cover = only_minimum_cover_;
    Edge *data_edges = new Edge[total_edges];

    for (int64_t i = 0; i < total_edges; ++i)
    {
      data_edges[i].id = edge_ids_c(i);
      data_edges[i].source = sources_c(i);
      data_edges[i].target = targets_c(i);
      data_edges[i].cost = costs_c(i);
      data_edges[i].reverse_cost = reverse_costs_c(i);
      data_edges[i].length = length_c(i);
      data_edges[i].geometry = geometry[i];
    }

    std::vector<int64_t> start_vertices(total_start_vertices);
    for (int64_t i = 0; i < total_start_vertices; ++i)
    {
      start_vertices[i] = start_vertices_c[i];
    }
    std::vector<double> distance_limits(total_distance_limits);
    for (int64_t i = 0; i < total_distance_limits; ++i)
    {
      distance_limits[i] = distance_limits_c[i];
    }
    auto isochrone_points = compute_isochrone(data_edges, total_edges, start_vertices,
                                              distance_limits, only_minimum_cover);
    return isochrone_points;
  }
};

PYBIND11_MODULE(isochrone, m)
{
  m.doc() = "Isochrone Calculation";
  // m.def("isochrone", &isochrone, "Isochrone Calculation");
  py::class_<IsochroneStartPoint>(m, "IsochroneShape")
      .def_readwrite("start_id", &IsochroneStartPoint::start_id)
      .def_readwrite("shape", &IsochroneStartPoint::shape);

  py::class_<IsochroneNetworkEdge>(m, "IsochroneNetworkEdge")
      .def_readwrite("start_id", &IsochroneNetworkEdge::start_id)
      .def_readwrite("edge", &IsochroneNetworkEdge::edge)
      .def_readwrite("start_perc", &IsochroneNetworkEdge::start_perc)
      .def_readwrite("end_perc", &IsochroneNetworkEdge::end_perc)
      .def_readwrite("start_cost", &IsochroneNetworkEdge::start_cost)
      .def_readwrite("end_cost", &IsochroneNetworkEdge::end_cost)
      .def_readwrite("shape", &IsochroneNetworkEdge::geometry);

  py::class_<Result>(m, "Result")
      .def_readwrite("isochrone", &Result::isochrone)
      .def_readwrite("network", &Result::network);

  // bindings to Isochrone class
  py::class_<Isochrone>(m, "Isochrone")
      .def(py::init<>())
      .def("calculate", &Isochrone::calculate);
}

/*
<%
setup_pybind11(cfg)
%>
*/
#endif
