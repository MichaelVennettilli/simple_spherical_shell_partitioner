#define CGAL_USE_BASIC_VIEWER

#include <CGAL/Linear_cell_complex_for_combinatorial_map.h>
#include <CGAL/Polyhedron_3_to_lcc.h>
#include <CGAL/draw_linear_cell_complex.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/convex_hull_3.h>
#include "shell_partitioner.h"

#include <vector>
#include <random>
#include <math.h>
#include <cassert>
//#include <algorithm>

/* // If you want to use exact constructions.
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
typedef CGAL::Linear_cell_complex<3,3,
  CGAL::Linear_cell_complex_traits<3, CGAL::Exact_predicates_exact_constructions_kernel> > LCC_CH;
*/
typedef CGAL::Exact_predicates_inexact_constructions_kernel  K;
typedef CGAL::Polyhedron_3<K>                     Polyhedron_3;
typedef K::Point_3                                Point_3;

typedef CGAL::Linear_cell_complex_for_combinatorial_map<2,3> LCC_CH;
typedef LCC_CH::Dart_handle           Dart_handle_CH;
// typedef LCC_CH::Point                 Point_CH;
typedef CGAL::Linear_cell_complex_for_combinatorial_map<3> LCC_3;
typedef LCC_3::Dart_handle           Dart_handle_3;
// typedef LCC_3::Point                 Point_3;

int main()
{
  double r_in = 1.0;
  double r_out = 2.0;
  int num_pts = 50;
  std::vector<Point_3> points = random_spherical_points(num_pts);
  LCC_CH chull = make_chull(points);
  LCC_3 shell = generate_shell(points, r_in, r_out);
  assert(chull.is_valid());
  assert(shell.is_valid());
  //print_vertices(chull);
  //LCC_CH::Vertex_attribute_range::iterator it=lcc.vertex_attributes().begin();
  chull.display_characteristics(std::cout) << ", valid="
                                         << chull.is_valid() << std::endl;
  shell.display_characteristics(std::cout) << ", valid="
                                         << shell.is_valid() << std::endl;

  return EXIT_SUCCESS;
}
