#include "pkgmeta.h"
#include <iostream>
#include <algorithm>

using namespace std;

template<typename T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
    os << "[" << endl;
    ::for_each(v.begin(), v.end(), [&](const T& t) { os << "\t" << t << endl; });
    os << "]" << endl;
    return os;
}

int main(int argc, char *argv[])
{
    PkgsMeta meta{"/home/sonald/packages.install"};
    vector<string> v{"core", "base", "desktop", "invalid"};
    for (auto& x: v) {
        cout << x << ":\n" << meta(x);
    }

    return 0;
}
