# Convex Hull to Spherical Shell

This code provides some basic utilities constructing certain geometries in CGAL. In particular, it was written to do three primary things.

1. Given a set of points (>3), an inner radius, and an outer radius, this can produce a polyhedral partition of the spherical shell bounded by the two radii into the specified number of polyhedra.

2. Given a 3-map, this can triangulate every face in it.

3. It can refine a partition of the spherical shell such that all polyhedra have similar volumes by using a generalization of Lloyd's relaxation algorithm.

This was tested on CGAL version 5.3.1, but it should work on more recent versions. I will test this directly later.

## Table of Contents

1. [Generate Shell](#gen_shell)
2. [Triangulate All Faces](#tri)
3. [Lloyd Relaxation](#relax)

<a name="gen_shell"></a>
## Generate Shell

This code constructs a linear cellular partition of a spherical shell in 3D using combinatorial maps. The basic strategy can be broken down into a sequence of steps.

There is a notion of duality for the convex hull. Each face in the convex hull is dual to a vertex specified by the vertices on the face (I use the circumcenter). Each edge between two faces in the convex hull is dual to an edge connecting the vertices dual to those faces. Each vertex in the convex hull is dual to a face. The vertices of a dual polygon are those dual to the faces in the convex hull incident to the corresponding vertex.

The basic idea is to use extrusion on this dual to obtain the linear cellular partition of the spherical shell. However, this extrusion is done radially, from an inner radius to an outer one. This will stretch each dual polygon and create edges perpendicular to the sphere that are not parallel to each other. Therefore, this requires a modification of the existing approach. To use this, you need to provide a set of points, an inner radius r_in, and an outer radius r_out. I generate the points randomly, so this requires an int for the number of points num_pts. Using this is as simple as
```
std::vector<Point_3> points = random_spherical_points(num_pts);  
CGAL::Linear_cell_complex_for_combinatorial_map<3> shell = generate_shell(points, r_in, r_out);  
```

### Implementation Details
We need the following objects:  
PARAMETERS  
-num_pts: Positive integer corresponding to the number of volumes in the partition of the spherical shell.  
-r_in: Inner radius of the shell. Require r_in > 0.  
-r_out: Outer radius of the shell. Require r_out > r_in.  
COMBINATORIAL MAPS  
-shell: An empty 3-map that will contain the dual objects.  
-chull_map: The 2-map corresponding to the convex hull of the sampled points.  
CONTAINERS  
-inner_face: A vector/ circulator of handles to darts belonging to an inner face in clockwise order when viewed from r > r_in.   This will be cleared for each inner face.  
ASSOCIATIVE MAPS/ ARRAYS  
-chull_to_inner_vertex: A map from darts of chull_map to vertex handles of shell, particularly to inner vertices. The vertex is at the circumcenter of the corresponding face scaled to r_in.  
-chull_to_outer_vertex: A map from darts of chull_map to vertex handles of shell, particularly to outer vertices. The vertex is at the circumcenter of the corresponding face scaled to r_out.  
-3_sew_dict: A map from darts in chull_map to darts in shell informing how to 3-sew adjacent volumes.  
-inner_to_outer: A map from inner darts to their corresponding outer darts.  

1. Randomly sample num_pts points on a sphere with radius 1.

2. Compute the convex hull of the collection of points using CGAL. Use CGAL to convert the triangulation to a combinatorial 2-map chull_map.

3. Orient chull_map so that it is clockwise. To do so, pick any dart and compute the cross product, X <-- (d.Betas[1].org - d.org) x (d.Betas[0].org - d.org), and the barycenter, c <-- d.barycenter. If c.X < 0, we are done. If c.X > 0, invert the n-map by swapping Betas[0] and Betas[1] for all darts using reverse_orientation.

4. We want to avoid creating many copies of the same vertex. Therefore, we sweep over all faces and create the vertices first. We iterate getting one dart fh per face in the convex hull. We compute the inner and outer point p_in and p_out respectively. We add these to the vertex attributes once and get a pair of vertex handles vh_in and vh_out. We set dh <-- fh and store the pair (dh, vh_in) in chull_to_inner_vertex and (dh, vh_out) in chull_to_outer_vertex. We then set dh <-- dh.Betas[1] and repeat until we return back to fh. This step associates each dart in the convex hull with a reference to its corresponding inner and outer vertex.

5. Now we create all polygons. Obtain an iterator it with one dart per vertex in chull_map. Given a particular vertex, you can iterate over all darts incident to it by applying Betas[0] and then Betas[2]. So set dh <-- it, make two darts, one at the inner vertex and one at the outer vertex. Store these as you go around in a pair of vectors inner_face and outer_face respectively. Advance by setting dh <-- dh.Betas[0].Betas[2] until you have dh = it.

6. Loop over the size of inner_face. 1-sew (inner_face[i], inner_face[i+1]) and (outer_face[i], outer_face[i-1]), where addition and subtraction are computed modulo the size of the vector.

7. The last thing you have to do is store associations between the inner and outer faces. Since the outer dart radially outward from the inner one has origin related by scaling the destination of the inner dart, we store the pair (inner_face[i], outer_face[i+1]) in the inner_outer map. Return the inner_face vector of dart handles and the inner_outer map.

8. Now we create the lateral faces and 2-sew them to the inner and outer faces. For each d in inner_face, we create four isolated darts with origins: l1.org = d.Betas[1].org, l2.org = d.org, l3.org = inner_to_outer[d].Betas[1].org, and l4.org = inner_to_outer[d].org. You 2-sew (d, l1) and (inner_to_outer[d], l3), and then you 1-sew (l1,l2), (l2, l3), (l3, l4), and (l4,l1).

9. Now we glue the lateral faces for a given cell together. For each dart d in inner_face, 2-sew d.Betas[2].Betas[1] and d.Betas[0].Betas[2].Betas[0].

10. You have finished processing one vertex. Clear inner_face and move on to the next dart in the vertex iterator (step 5).

11. We can only 3-glue faces once we have created all cells. Once this is done, obtain an iterator of one dart e per edge in the convex hull. You simply 3-sew 3_sew_map(e).Betas[2] and 3_sew_map(e.Betas[2]).Betas[2].

<a name="tri"></a>
## Triangulate All Faces

This code is pretty self-explanatory. It inserts the circumcenter into each face and triangulates it. This is not possible with the default iterators in the naive way. One problem is that subdividing a face adds more faces, so you never reach the end. Using a for loop to store the values of the iterator, then looping over them also doesn't work.

Here, we have to get our hands a bit more dirty and use marks. Reserve a mark ma. Insertion always returns a handle for a dart incident to the new vertex. Loop over all darts until you reach lcc.darts().end(), which should be recomputed after insertion. Insert the circumcenter and store the handle dh_start. Create a new handle dh and set dh <-- dh_start. Apply Betas[1] and ma-mark all darts in this face, this is just dh, dh.Betas[0], and dh.Betas[1], since we've triangulated things. Go to the next face with dh <-- dh.Betas[2].Betas[1], and repeat until you reach a face that is ma-marked (the one corresponding to dh_start). Now we need to check if the face is 3-sewn. If it isn't, we are done. If it is, get another dart incident to the new vertex with dh <-- dh_start.Betas[3].Betas[1]. Repeat the above to mark all darts in this face. Move on to the next dart in the list of all darts and repeat the above if it is not ma-marked. Once you reach lcc.darts().end(), you are done. Un-ma-mark everything and free the mark, and you're done.

The function doesn't return anything, the linear cell complex lcc of dimension 3 is passed by reference, so you just use
```
CGAL::Linear_cell_complex_for_combinatorial_map<3> lcc = ...
triangulate_all_faces(lcc);
```

<a name="relax"></a>
## Lloyd Relaxation

This function employs a generalization of Lloyd's relaxation method. If we construct cells using randomly sampled points, they can have wildly unequal volumes. This occurs if some of the randomly sampled points are close together. We use the computed partition of the shell to refine it. You specify a number of iterations and the inner and outer radii  pass a 3-map lcc by reference. Applying this can be done with
```
CGAL::Linear_cell_complex_for_combinatorial_map<3> lcc = ...
lloyd_relaxation(lcc, num_iter, r_in, r_out);
```

### Mathematical Details

We compute the center of mass $`\vec{c}`$ (up to a constant factor) of a finite volume bounded by a triangulated surface. We have
```math
V\vec{c} = \int_V \vec{x}dV.
```
We can apply the inner product with any $\vec{\tau}$ and use the divergence theorem. The key is to notice that
```math
\nabla \cdot \langle \tau_1 x_1^2, \tau_2 x_2^2, \tau_3 x_3^2\rangle = 2 \vec{\tau} \cdot \vec{x}.
```
Introduce the matrix $`M(\vec{x}) = \text{diag}(x_i^2)`$, use the divergence theorem, the fact that $`M(\vec{x})`$ is symmetric to find, and finally the fact that $`\vec{\tau}`$ was arbitrary to find
```math
V\vec{c} = \frac{\vec{1}}{2} \oint_{\partial V} M(\vec{x})d\vec{S}.
```

Now we use the fact that our surface is triangulated. For each face, get one dart $`d`$ incident to it. Let $`Org`$ denote the function that gets the origin of a dart. We introduce
```math
\vec{v}_0 = Org(d), \qquad \vec{v}_1 = Org(\beta_1(d)), \qquad \vec{v}_{-1} = Org(\beta_0(d)).
```
Any point on the triangle can be written
```math
\vec{x} = (1-s-t) \vec{v}_0 + s\vec{v}_1 + t \vec{v}_{-1}
```
for $`s,t \geq 0`$ and $`s+t \leq 1`$. Given that our faces are counterclockwise when viewed from the (immediate) outside, the area element is
```math
d\vec{S} = \frac{1}{2} \frac{\partial \vec{x}}{\partial s} \times \frac{\partial \vec{x}}{\partial t} ds dt= \frac{1}{2} (\vec{v}_1-\vec{v}_0)\times(\vec{v}_{-1}-\vec{v}_0)ds dt.
```


### Implementation Details
