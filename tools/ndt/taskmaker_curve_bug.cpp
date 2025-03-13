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

int main()
{       
        std::cout << "Begining" << endl;
        // int N = 20;
        // double tetr_size;

        // float dx = (1200.0 - xmin - xmax) / (N - 1);
        for (int height = 100; height >= 20; height -= 10)
        {       
                ifstream fi("../../tasks/objects/my_tasks/papper_i.xml", std::ios::in);

                //string tns = (tasknumber < 10 ? "0" : "") + to_string(tasknumber);
                ofstream fo("../../tasks/objects/my_tasks/papper_" + to_string(height) + ".xml");
                
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
                                        fo << height;

                                else if (codename == "TETR_SIZE")
                                        fo << static_cast<int>(height / 3);

                                else if (codename == "CUBE_H")
                                        fo << static_cast<int>(height / 2);
                                
                                else if (codename == "NUM_X" || codename == "NUM_Y")
                                        fo << static_cast<int>(1000. / (height / 2.));
                                
                                else if (codename == "MOVE_X" || codename == "MOVE_Y")
                                        fo << -500;

                                else if (codename == "MOVE_Z")
                                        fo << static_cast<int>(-1 * height);

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

