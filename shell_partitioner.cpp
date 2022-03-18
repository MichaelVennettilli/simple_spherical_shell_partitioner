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
//#include <algorithm>

/* // If you want to use exact constructions.
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
typedef CGAL::Linear_cell_complex<3,3,
  CGAL::Linear_cell_complex_traits<3, CGAL::Exact_predicates_exact_constructions_kernel> > LCC_CH;
*/
typedef CGAL::Exact_predicates_inexact_constructions_kernel  K;
typedef CGAL::Polyhedron_3<K>                     Polyhedron_3;
typedef K::Point_3                                Poly_Point_3;

typedef CGAL::Linear_cell_complex_for_combinatorial_map<2,3> LCC_CH;
typedef LCC_CH::Dart_handle           Dart_handle_CH;
// typedef LCC_CH::Point                 Point_CH;
typedef CGAL::Linear_cell_complex_for_combinatorial_map<3> LCC_3;
typedef LCC_3::Dart_handle           Dart_handle_3;
// typedef LCC_3::Point                 Point_3;

int main()
{
  double radius = 1.0;
  int num_pts = 4;
  std::vector<Poly_Point_3> points = random_spherical_points(num_pts);
  LCC_CH chull = make_chull(points);

  //print_vertices(chull);
  //LCC_CH::Vertex_attribute_range::iterator it=lcc.vertex_attributes().begin();
  //std::cout<<"point: "<<lcc.point_of_vertex_attribute(it)<<std::endl;
  //std::cout<<"point: "<<lcc.point(lcc.dart_of_attribute<0>(it))<<std::endl;
  //CGAL::draw(chull);
  LCC_3 shell;

  return EXIT_SUCCESS;
}
