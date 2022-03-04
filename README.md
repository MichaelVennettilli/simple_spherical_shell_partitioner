This code constructs a linear cellular partition of a spherical shell in 3D using combinatorial maps. The basic strategy can be broken down into a sequence of steps.

1. Randomly sample some points on a sphere with radius r_in.

2. Compute the convex hull of the collection of points using CGAL. Use CGAL to convert the triangulation to a combinatorial 2-map chull_map.

3. There is a notion of duality for the convex hull. Each face in chull_map is dual to a vertex at the center of the circumcircle specified by the vertices on the face. Each edge between two faces in chull_map is dual to an edge connecting the vertices dual to those faces. Each vertex in chull_map is dual to a polygon. The vertices of the dual polygon are those dual to the faces in chull_map incident to the vertex in chull_map.

4. The basic idea is to use extrusion on this dual to obtain the linear cellular partition of the spherical shell. However, this extrusion is done radially, from r_in to r_out. This will stretch each dual polygon and create edges perpendicular to the sphere that are not parallel. Therefore, this requires a modification of the existing approach.

5. This is found by using Ohori et al.'s approach to incrementally build linear cell complexes. The strategy is to create one dart per dual vertex. We then specify which vertex darts should appear in each of the faces. While constructing the faces we create lists corresponding to each volume that indicates which faces are on the boundary of the volume. Then we create the volumes by sewing the faces together.
