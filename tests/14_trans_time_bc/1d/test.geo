ChL=0.02;
// Default 2d algo (1) snd MeshAdapt (4) produce nonsymetric mesh
// Delaunay (5) produce symmetric mesh
//Mesh.Algorithm=1;

Point(1) = {0,0,0,ChL};
Point(2) = {1,0,0,ChL};

Line(3) = {1,2};

Physical Line(100) = {3};

