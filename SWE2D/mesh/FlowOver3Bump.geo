Point(1) = {0, -15, 0, 1.5};
Point(2) = {16,-15, 0, 1.5};
Point(3) = {16, 15, 0, 1.5};
Point(4) = {0, 15, 0,  1.5};
Point(5) = {75, 15, 0, 1.5};
Point(6) = {75, -15, 0, 1.5};
Line(1) = {1, 2};
Line(2) = {2, 3};
Line(3) = {3, 4};
Line(4) = {4, 1};
Line(5) = {2, 6};
Line(6) = {6, 5};
Line(7) = {5, 3};
Line Loop(1) = {1, 2, 3, 4};
Plane Surface(1) = {1};
Line Loop(2) = {-2, 5, 6, 7};
Plane Surface(2) = {2};
Physical Line(1) = {1, 3, 4, 5, 6, 7};
Physical Surface(1) = {1};
Physical Surface(2) = {2};
Coherence;
