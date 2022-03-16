#define CGAL_USE_BASIC_VIEWER

#include <CGAL/Linear_cell_complex_for_combinatorial_map.h>
#include <CGAL/Polyhedron_3_to_lcc.h>
#include <CGAL/draw_linear_cell_complex.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/convex_hull_3.h>

#include <vector>
#include <random>
#include <math.h>

#define PI 3.14159265


/* // If you want to use exact constructions.
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
typedef CGAL::Linear_cell_complex<3,3,
  CGAL::Linear_cell_complex_traits<3, CGAL::Exact_predicates_exact_constructions_kernel> > LCC_3;
*/
typedef CGAL::Exact_predicates_inexact_constructions_kernel  K;
typedef CGAL::Polyhedron_3<K>                     Polyhedron_3;
typedef K::Point_3                                Point_3;
typedef CGAL::Surface_mesh<Point_3>               Surface_mesh;

typedef CGAL::Linear_cell_complex_for_combinatorial_map<2,3> LCC_3;
typedef LCC_3::Dart_handle           Dart_handle;
typedef LCC_3::Point                 Point;

int main()
{
  // 1) Compute the convex hull.
  Polyhedron_3 poly;
  std::mt19937 mt(time(NULL));
  double azi_angle, polar_angle;
  double radius = 1.0;
  std::uniform_real_distribution<double> azimuthal_dist(-1.0,1.0);
  std::uniform_real_distribution<double> polar_dist(0.0,2*PI);
  int num_pts = 50;
  std::vector<Point_3> points;
  for(int i=0; i<num_pts; i++){
	  // Generate the azimuthal angle
	  azi_angle = acos(azimuthal_dist(mt));
	  polar_angle = polar_dist(mt);
	  points.push_back(Point(radius*sin(azi_angle)*cos(polar_angle),
			  radius*sin(azi_angle)*sin(polar_angle), radius*cos(azi_angle)));
  }
  CGAL::convex_hull_3(points.begin(), points.end(), poly);

  // 2) Convert the triangulation into a 3D lcc.
  LCC_3 lcc;
  Dart_handle dh=CGAL::import_from_polyhedron_3<LCC_3,Polyhedron_3>
    (lcc, poly);

  CGAL::draw(lcc);

  return EXIT_SUCCESS;
}
