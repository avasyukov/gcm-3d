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
        // int N = 20;
        double tetr_size;
        // float dx = (1200.0 - xmin - xmax) / (N - 1);
        for (double height = 1; height >= 0.2; height -= 0.1)
        {       
                ifstream fi("../../models/shells/papper_z_i.geo", std::ios::in);

                //string tns = (tasknumber < 10 ? "0" : "") + to_string(tasknumber);
                ofstream fo("../../models/shells/papper_z_" + round3(height, '_') + ".geo");
                
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
                                else if (codename == "TETR_SIZE")
                                        fo << round3(height / 10., '.');
                                // else if (codename == "XMAX")
                                //         fo << xmax + tasknumber * dx;
                        }
                        else
                                fo << c;
                        c = fi.get();
                }

                std::cout << "Maked geo with height " << height << ";\n";
                fi.close();
                fo.close();
        }

        for (double height = 0.1; height > 0.03; height -= 0.0599999999)
        {       
                ifstream fi("../../models/shells/papper_z_i.geo", std::ios::in);

                //string tns = (tasknumber < 10 ? "0" : "") + to_string(tasknumber);
                ofstream fo("../../models/shells/papper_z_" + round3(height, '_') + ".geo");
                
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
                                else if (codename == "TETR_SIZE")
                                        fo << round3(height / 10., '.');
                                // else if (codename == "XMAX")
                                //         fo << xmax + tasknumber * dx;
                        }
                        else
                                fo << c;
                        c = fi.get();
                }

                std::cout << "Maked geo with height " << height << ";\n";
                fi.close();
                fo.close();
        }

        // for (int height = 20; height > 0; height -= 5)
        // {
        //         ifstream fi("../../models/my_models/papper_z_i.geo", std::ios::in);

        //         //string tns = (tasknumber < 10 ? "0" : "") + to_string(tasknumber);
        //         ofstream fo("../../models/my_models/papper_z_" + to_string(height) + ".geo");
                
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

        //         std::cout << "Maked geo with height " << height << ";\n";
        //         fi.close();
        //         fo.close();
        // }

        cout << "Created succesfully!" << endl;
        return 0;
}

