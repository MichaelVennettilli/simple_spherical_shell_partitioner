# Convex Hull to Spherical Shell

This code provides some basic utilities constructing certain geometries in CGAL. In particular, it was written to do three primary things.

1. Given a positive int (>3), an inner radius, and an outer radius, this can produce a polyhedral partition of the spherical shell bounded by the two radii into the specified number of polyhedra.

2. Given a 3-map, this can triangulate every face in it.

3. It can refine a partition of the spherical shell such that all polyhedra have similar volumes by using a generalization of Lloyd's relaxation algorithm.

## Generate Shell

This code constructs a linear cellular partition of a spherical shell in 3D using combinatorial maps. The basic strategy can be broken down into a sequence of steps.

There is a notion of duality for the convex hull. Each face in the convex hull is dual to a vertex specified by the vertices on the face (I use the circumcenter or the barycenter). Each edge between two faces in the convex hull is dual to an edge connecting the vertices dual to those faces. Each vertex in the convex hull is dual to a face. The vertices of a dual polygon are those dual to the faces in the convex hull incident to the corresponding vertex.

The basic idea is to use extrusion on this dual to obtain the linear cellular partition of the spherical shell. However, this extrusion is done radially, from r_in to r_out. This will stretch each dual polygon and create edges perpendicular to the sphere that are not parallel to each other. Therefore, this requires a modification of the existing approach.

We need the following objects:  
PARAMETERS  
-num_vols: Positive integer corresponding to the number of volumes in the partition of the spherical shell.  
-r_in: Inner radius of the shell. Require r_in > 0.  
-r_out: Outer radius of the shell. Require r_out > r_in.  
COMBINATORIAL MAPS  
-shell: An empty 3-map that will contain the dual objects.  
-chull_map: The 2-map corresponding to the convex hull of the sampled points.  
CONTAINERS  
-shell_vertices: A container of the coordinate tuples/ point objects for vertices.  
-inner_face: A vector/ circulator of handles to darts belonging to an inner face in clockwise order when viewed from r > r_in.   This will be cleared for each inner face.  
ASSOCIATIVE MAPS/ ARRAYS  
-chull_to_inner_vertex: A map from darts of chull_map to shell_vertices, particularly to inner vertices. The vertex is at the circumcenter or barycenter of the corresponding face scaled to r_in.  
-chull_to_outer_vertex: A map from darts of chull_map to shell_vertices, particularly to outer vertices. The vertex is at the circumcenter or barycenter of the corresponding face scaled to r_out.  
-3_sew_dict: A map from darts in chull_map to darts in shell informing how to 3-sew adjacent volumes.  
-inner_to_outer: A map from inner darts to their corresponding outer darts.  
-chull_to_inner_dart: A map from darts in chull_map to their corresponding inner darts in shell.  

1. Randomly sample num_vols points on a sphere with radius r_in.

2. Compute the convex hull of the collection of points using CGAL. Use CGAL to convert the triangulation to a combinatorial 2-map chull_map.

3. Orient chull_map so that it is clockwise. To do so, pick any dart and compute the cross product, X <-- (d.Betas[1].org - d.org) x (d.Betas[0].org - d.org), and the barycenter, c <-- 1/3*(d.org + d.Betas[1].org + d.Betas[0].org). If c.X < 0, we are done. If c.X > 0, invert the n-map by swapping Betas[0] and Betas[1] for all darts.

4. Obtain an iterator of one dart per vertex in chull_map.

5. Given one dart for a vertex v, this dart belongs to a face incident to v and has v as its origin. Reserve two marks: made_vertex  and inner_ordered.
-If made_vertex is false, compute the dual vertices using the circumcenter or the barycenter, append them to shell_vertices. One will lie at r_in, the other at r_out. Mark made_vertex for all darts in the facet, and add key-value pairs (dart, center_in) to chull_to_inner_vertex and (dart, center_out) to chull_to_outer_vertex for all darts in the facet. Mark made_vertex true for all of the darts in the facet.
-If made_vertex is true, simply obtain the inner vertex using chull_to_inner_vertex.
Return back to the initial dart d with v as its origin. You can obtain one dart per face incident to v with v as its origin by setting e <-- d and repeatedly applying e <-- e.Betas[0].Betas[2]. You want to do a couple of things. Create a dart d' in shell for each e with origin given by the associative array (chull_to_inner_vertex[e]) described earlier in this step. Store handles for each dart in a vector or circulator inner_face. Write the pair (e.Betas[0], d') to the associative array 3_sew_map. Add the pair (e, d') to chull_to_inner_dart. Mark e with inner_ordered and repeat until inner_ordered is true.

6. For each element d_i in inner_face, 1-sew d_i to d_{i+1} so that d_{i}.Betas[1] = d_{i+1}. We have 1-sewn the faces on the inner face of the cells.

7. Now we assemble the outer face.  Since we created the inner and outer vertices simultaneously, we don't need to worry about creating anything. For each d in inner_face, create an isolated dart d' with d'.org <-- chull_to_outer_vertex[chull_to_inner_dart[d.Betas[0]]]. Add the pairs (d,d') to the map inner_to_outer. For d in inner_face, 1-sew inner_to_outer[d] to inner_to_outer[d.Betas[0]].

8. Now we create the lateral faces and 2-sew them to the inner and outer faces. For each d in inner_face, we create four isolated darts with origins: l1.org = d.Betas[1].org, l2.org = d.org, l3.org = inner_to_outer[d].Betas[1].org, and l4.org = inner_to_outer[d].org. You 2-sew (d, l1) and (inner_to_outer[d], l3), and then you 1-sew (l1,l2), (l2, l3), (l3, l4), and (l4,l1).

9. Now we glue the lateral faces for a given cell together. For each dart d in inner_face, 2-sew d.Betas[2].Betas[1] and d.Betas[0].Betas[2].Betas[0].

10. You have finished processing one vertex. Clear inner_face and move on to the next dart in the vertex iterator (step 5).

11. We can only 3-glue faces once we have created all cells. Once this is done, obtain an iterator of one dart e per edge in the convex hull. You simply 3-sew 3_sew_map(e).Betas[2] and 3_sew_map(e.Betas[2]).Betas[2].

12. Unmark everything and free the marks that you have used.

## Triangulate All Faces

## Lloyd Relaxation
