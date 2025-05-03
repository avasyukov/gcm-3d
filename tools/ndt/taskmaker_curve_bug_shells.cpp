#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

using std::cout;
using std::ifstream;
using std::ofstream;
using std::to_string;
using std::string;
using std::endl;

string round3(double a, char q) {
        return to_string(static_cast<int>(std::trunc(a))) + q + 
        to_string(static_cast<int>(std::trunc((a - (double) (int) a) * 10))) + 
        to_string(static_cast<int>(std::trunc((((a - (double) (int) a) * 10) - (double) (int) ((a - (double) (int) a) * 10)) * 10)));
}

int main()
{       
        std::cout << "Begining" << endl;

        string inp;
        std::cout << "Введите с каким типом таска работаете\n";
        std::cin >> inp;

        // int N = 20;
        // double tetr_size;

        // float dx = (1200.0 - xmin - xmax) / (N - 1);
        for (double height = 1; height > -0.0001; height -= 0.1)
        {       
                // lazy potato
                if (fabs(height - 0) < 0.0001) {
                        height = 0.04;
                }
                ifstream fi("../../tasks/objects/shells/" + inp + "/papper_i.xml", std::ios::in);

                //string tns = (tasknumber < 10 ? "0" : "") + to_string(tasknumber);
                ofstream fo("../../tasks/objects/shells/" + inp + "/papper_" + round3(height, '_') + ".xml");
                
                char c = fi.get();
                while (!fi.eof()) {
                        
                        if (c == '$')
                        {
                                string codename;
                                c = fi.get();
                                while (c != '$') {
                                        codename += c;
                                        c = fi.get();
                                }

                                if (codename == "HEIGHT")
                                        fo << round3(height, '.');
                                
                                else if (codename == "HEIGHT_")
                                        fo << round3(height, '_');

                                else if (codename == "TETR_SIZE") {

                                        if (height == 0.04)
                                                fo << 0.004;
                                        else {
                                                // 0.6-9, 1
                                                if (height > 0.55)
                                                        fo << round3(0.6 / 10., '.');
                                                else
                                                        fo << round3(height / 10., '.');
                                        }

                                }

                                else if (codename == "TAU") {
                                        // 0.6-9, 1
                                        if (height > 0.55)
                                                fo << 0.000001;
                                        // 0.3-5
                                        else if (height > 0.25)
                                                fo << 0.0000005;
                                        // 0.1-2
                                        else if (height > 0.09)
                                                fo << 0.0000002;
                                        // 0.04
                                        else
                                                fo << 0.00000005;
                                }

                                else if (codename == "PER_SNAP") {
                                        // 0.6-9, 1
                                        if (height > 0.55)
                                                fo << 2;
                                        // 0.3-5
                                        else if (height > 0.25)
                                                fo << 4;
                                        // 0.1-2
                                        else if (height > 0.09)
                                                fo << 10;
                                        // 0.04
                                        else
                                                fo << 40;
                                }

                                else if (codename == "SZ")
                                        fo << round3(height / 2., '.');
                                
                                else if (codename == "NUM_X" || codename == "NUM_Y")
                                        fo << static_cast<int>(10. / (height / 2.));
                                
                                else if (codename == "MOVE_X" || codename == "MOVE_Y")
                                        fo << -5;

                        }
                        else
                                fo << c;
                        c = fi.get();
                }

                std::cout << "Maked xml with height " << height << ";\n";
                fi.close();
                fo.close();
        }

        // for (int height = 20; height > 0; height -= 5)
        // {
        //         ifstream fi("../../tasks/objects/my_tasks/papper_i.xml", std::ios::in);
        //         //string tns = (tasknumber < 10 ? "0" : "") + to_string(tasknumber);
        //         ofstream fo("../../tasks/objects/my_tasks/papper_" + to_string(height) + ".xml");
                
        //         char c = fi.get();
        //         while (!fi.eof())
        //         {
        //                 if (c == '$')
        //                 {
        //                         string codename;
        //                         c = fi.get();
        //                         while (c != '$')
        //                         {
        //                                 codename += c;
        //                                 c = fi.get();
        //                         }
        //                         if (codename == "HEIGHT")
        //                                 fo << height;
        //                         else if (codename == "TETR_SIZE")
        //                                 fo << to_string(height / 10) + '.' + 
        //                                         to_string(static_cast<int>(std::trunc(((double) height / 10 - height / 10) * 10)));
                                                        
        //                         // else if (codename == "XMAX")
        //                         //         fo << xmax + tasknumber * dx;
        //                 }
        //                 else
        //                         fo << c;
        //                 c = fi.get();
        //         }

        //         std::cout << "Maked xml with height " << height << ";\n";
        //         fi.close();
        //         fo.close();
        // }

        cout << "Created succesfully!" << endl;
        return 0;
}

