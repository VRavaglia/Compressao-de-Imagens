//
// Created by Victor on 03/06/2022.
//

#ifndef WAVELETS_ENCODERWRAPPER_H
#define WAVELETS_ENCODERWRAPPER_H

#include <string>
#include "VQ.h"
extern "C"
{
#include "subdefs2.h"
}

using namespace std;

class EncoderWrapper {
private:
    static void write_header(FILE *fout, const vector<vector<performance>> &performances, unsigned bestCodebooks[NBANDS]);
public:
    static void encode(const string& in, const string& out);
};


#endif //WAVELETS_ENCODERWRAPPER_H
