#include "pkgmeta.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <cctype>

template<typename T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
    os << "[" << endl;
    ::for_each(v.begin(), v.end(), [&](const T& t) { os << "\t" << t << endl; });
    os << "]" << endl;
    return os;
}

PkgsMeta::PkgsMeta(const string& config)
{
    ::ifstream is(config);
    if (!is) {
        cerr << config << " can not be opened";
        return;
    }

    string buf, line;
    while (getline(is, line)) {
        if (line.empty() || line.at(0) == '#') 
            continue;
        
        buf += line;
    }

    line.clear();
    for (int i = 0, sz = buf.length(); i < sz; i++) {
        switch(buf[i]) {
            case '=': 
            case '(':
            case ')':
                if (line.size()) {
                    _tokens.push_back(line);
                    line.clear();
                }
                _tokens.push_back(string{buf[i]});
                break;

            default:
                if (::isspace(buf[i])) {
                    if (line.size()) {
                        _tokens.push_back(line);
                        line.clear();
                    }

                    while (::isspace(buf[i++])) ;
                    i--;
                }

                line += buf[i];
                break;
        }
    }

    if (!line.empty()) _tokens.push_back(line);
    cerr << _tokens;
    parse();
}

vector<string> PkgsMeta::operator()(const string& groupName)
{
    if (_meta.find(groupName) == _meta.end()) return {};
    return _meta[groupName];
}

void PkgsMeta::parse()
{
    while (!eot())
        parseGroup();

}

void PkgsMeta::parseGroup()
{
    string grp = next();
    cerr << "parsing group " << grp << endl;

    _meta[grp] = {};
    next(); // eat =
    next(); // eat (
    while (peek() != ")") {
        _meta[grp].push_back(next());
    }
    next(); // eat )
}

bool PkgsMeta::eot() const
{
    return _cursor == _tokens.size();
}

string PkgsMeta::next() const
{
    return _tokens[_cursor++];
}

string PkgsMeta::peek() const
{
    return _tokens[_cursor];
}
