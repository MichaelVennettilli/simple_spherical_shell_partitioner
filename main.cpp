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

typedef CGAL::Exact_predicates_inexact_constructions_kernel  K;
typedef CGAL::Polyhedron_3<K>                     Polyhedron_3;
typedef K::Point_3                                Point_3;

typedef CGAL::Linear_cell_complex_for_combinatorial_map<2,3> LCC_CH;
typedef LCC_CH::Dart_handle           Dart_handle_CH;

typedef CGAL::Linear_cell_complex_for_combinatorial_map<3> LCC_3;
typedef LCC_3::Dart_handle           Dart_handle_3;

int main(int argc, char* argv[])
{
  double r_in = 1.0;
  double r_out = 2.0;
  int num_pts = 50;
  if (argc>1){
    num_pts = std::atoi(argv[1]);
  }
  if (argc>2){
		r_in = std::atof(argv[2]);
	}
	if (argc>3){
		r_out = std::atof(argv[3]);
	}

  std::vector<Point_3> points = random_spherical_points(num_pts);
  LCC_CH chull = make_chull(points);
  LCC_3 shell = generate_shell(points, r_in, r_out);
  LCC_3 tri = shell;
  triangulate_all_faces(tri);
  LCC_3 relaxed = shell;
  lloyd_relaxation(relaxed, 10, r_in, r_out);
  std::cout<<"Convex hull :"<<std::endl<<"  ";
  chull.display_characteristics(std::cout) << ", valid="
                                         << chull.is_valid() << std::endl;
  std::cout<<"Shell :"<<std::endl<<"  ";
  shell.display_characteristics(std::cout) << ", valid="
                                         << shell.is_valid() << std::endl;
  std::cout<<"Relaxed :"<<std::endl<<"  ";
  relaxed.display_characteristics(std::cout) << ", valid="
                                         << relaxed.is_valid() << std::endl;
  CGAL::draw(chull);
  CGAL::draw(shell);
  CGAL::draw(tri);
  CGAL::draw(relaxed);
  return EXIT_SUCCESS;
}
