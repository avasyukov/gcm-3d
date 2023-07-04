#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main()
{
        int N = 40;
        float xmin = 100;
        float xmax = 420;
        float dx = (1200.0 - xmin - xmax) / (N - 1);
        for (int tasknumber = 0; tasknumber < N; tasknumber++)
        {
                ifstream fi("tasks/tests/aberration_000.xml", std::ios::in);
                string tns = (tasknumber < 10 ? "0" : "") + to_string(tasknumber);
                ofstream fo("tasks/tests/aberration_000_" + tns + ".xml");
                char c = fi.get();
                while (!fi.eof())
                {
                        if (c == '$')
                        {
                                string codename;
                                c = fi.get();
                                while (c != '$')
                                {
                                        codename += c;
                                        c = fi.get();
                                }
                                if (codename == "TASKNUMBER")
                                        fo << tns;
                                else if (codename == "XMIN")
                                        fo << xmin + tasknumber * dx;
                                else if (codename == "XMAX")
                                        fo << xmax + tasknumber * dx;
                        }
                        else
                                fo << c;
                        c = fi.get();
                }
        }
        cout << endl;
        return 0;
}

