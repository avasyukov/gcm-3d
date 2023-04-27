meshPointDist = 0.2;

Point(1) = {5, 5, -5, meshPointDist};
Point(2) = {5, 5, 0, meshPointDist};
Point(3) = {-5, 5, 0, meshPointDist};
Point(4) = {-5, 5, -5, meshPointDist};
Point(5) = {5, -5, -5, meshPointDist};
Point(6) = {5, -5, 0, meshPointDist};
Point(7) = {-5, -5, 0, meshPointDist};
Point(8) = {-5, -5, -5, meshPointDist};
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


