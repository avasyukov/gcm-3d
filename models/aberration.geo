meshPointDist = 20.0;
X = 1200;
Y = 1200;
Z = 1200;
x = 0;
y = 0;
z = 0;

Point(1) = {x, y, z, meshPointDist};
Point(2) = {x, y, Z, meshPointDist};
Point(3) = {X, y, Z, meshPointDist};
Point(4) = {X, y, z, meshPointDist};
Point(5) = {x, Y, z, meshPointDist};
Point(6) = {x, Y, Z, meshPointDist};
Point(7) = {X, Y, Z, meshPointDist};
Point(8) = {X, Y, z, meshPointDist};
Line(9) = {1, 2};
Line(10) = {2, 3};
Line(11) = {3, 4};
Line(12) = {4, 1};
Line(13) = {5, 6};
Line(14) = {6, 7};
Line(15) = {7, 8};
Line(16) = {8, 5};
Line(17) = {1, 5};
Line(18) = {2, 6};
Line(19) = {3, 7};
Line(20) = {4, 8};
Line Loop(21) = {-9, -12, -11, -10};
Ruled Surface(21) = {21};
Line Loop(22) = {13, 14, 15, 16};
Ruled Surface(22) = {22};
Line Loop(23) = {10, 19, -14, -18};
Ruled Surface(23) = {23};
Line Loop(24) = {17, -16, -20, 12};
Ruled Surface(24) = {24};
Line Loop(25) = {11, 20, -15, -19};
Ruled Surface(25) = {25};
Line Loop(26) = {9, 18, -13, -17};
Ruled Surface(26) = {26};
Surface Loop(27) = {21, 22, 23, 24, 25, 26};
Volume(36) = {27};

