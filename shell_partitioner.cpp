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

void make_chull(LCC_CH &lcc, double r_in, int num_pts){
  Polyhedron_3 poly;
  std::mt19937 mt(time(NULL));
  double azi_angle, polar_angle;
  const double pi = 3.14159265;
  std::uniform_real_distribution<double> azimuthal_dist(-1.0,1.0);
  std::uniform_real_distribution<double> polar_dist(0.0,2*pi);
  std::vector<Poly_Point_3> points;
  for(int i=0; i<num_pts; i++){
	  // Generate the angles
	  azi_angle = acos(azimuthal_dist(mt));
	  polar_angle = polar_dist(mt);
	  points.push_back(Poly_Point_3(r_in*sin(azi_angle)*cos(polar_angle),
			  r_in*sin(azi_angle)*sin(polar_angle), r_in*cos(azi_angle)));
  }
  CGAL::convex_hull_3(points.begin(), points.end(), poly);
  Dart_handle_CH dh=CGAL::import_from_polyhedron_3<LCC_CH,Polyhedron_3>
    (lcc, poly);
  // Check the orientation
  LCC_CH::Vector v1 = lcc.point(lcc.beta(dh,1))-lcc.point(dh);
  LCC_CH::Vector v2 = lcc.point(lcc.beta(dh,0))-lcc.point(dh);
  LCC_CH::Vector barycenter = lcc.barycenter<2>(dh) - CGAL::ORIGIN;
  double signature = CGAL::cross_product(v1,v2)*barycenter;
  if(signature>0.0) lcc.reverse_orientation();
  return;
}

void print_vertices(LCC_CH &lcc){
  for (LCC_CH::Vertex_attribute_range::iterator
         it=lcc.vertex_attributes().begin(),
         itend=lcc.vertex_attributes().end();
       it!=itend; ++it)
  {
    std::cout<<"point: "<<lcc.point_of_vertex_attribute(it)<<std::endl;
  }
}

int main()
{
  LCC_CH lcc;
  double radius = 1.0;
  int num_pts = 50;
  make_chull(lcc, radius, num_pts);

  print_vertices(lcc);
  //LCC_CH::Vertex_attribute_range::iterator it=lcc.vertex_attributes().begin();
  //std::cout<<"point: "<<lcc.point_of_vertex_attribute(it)<<std::endl;
  //std::cout<<"point: "<<lcc.point(lcc.dart_of_attribute<0>(it))<<std::endl;
  CGAL::draw(lcc);


  return EXIT_SUCCESS;
}
