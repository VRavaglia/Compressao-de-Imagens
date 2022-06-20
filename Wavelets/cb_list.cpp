//
// Created by Victor on 16/06/2022.
//

#include "cb_list.h"
#include "VQ.h"


intMatrix bsize_list(int band){
    const intMatrix bsize_list_0{{1,2},{2,2},{2,4},{4,4}, {2,8}, {4,8}, {8,8}};
    const intMatrix bsize_list_1{{1,2},{1,4},{2,4},{2,8},{4,8}};
    const intMatrix bsize_list_2{{1,2},{2,2},{2,4},{4,4}, {8,8}};
    const intMatrix bsize_list_3{{2,1},{4,1},{4,2},{8,2},{8,4}};
    const intMatrix bsize_list_4{{1,2},{1,4},{2,4},{2,8},{4,8}};
    const intMatrix bsize_list_5{{1,2},{2,2},{4,4},{4, 8}, {8,8}};
    const intMatrix bsize_list_6{{2,1},{4,1},{4,2},{8,2},};
    const intMatrix bsize_list_7{{1,2},{1,4},{2,4},{2,8},{4,8}};
    const intMatrix bsize_list_8{{1,2},{2,2},{4,4},{4, 8}, {8,8}};
    const intMatrix bsize_list_9{{2,1},{4,1},{4,2},{8,2},{8,4}};

    const vector<intMatrix> bsize_listr{bsize_list_0, bsize_list_1, bsize_list_2, bsize_list_3, bsize_list_4, bsize_list_5, bsize_list_6, bsize_list_7, bsize_list_8, bsize_list_9};

    return bsize_listr[band];
}

vector<int> csize_list(int band){
    const vector<int> csize_list_0{64, 128, 256, 512, 1024, 2048, 4096, 4096*2};
    const vector<int> csize_list_1{8,16, 64, 128, 256, 512, 1024, 2048, 4096, 4096*2};
    const vector<int> csize_list_2{8,16, 64, 128, 256, 512, 1024, 2048, 4096, 4096*2};
    const vector<int> csize_list_3{8,16, 64, 128, 256, 512, 1024, 2048, 4096, 4096*2};
    const vector<int> csize_list_4{8,16, 64, 128, 256, 512, 1024, 2048, 4096, 4096*2};
    const vector<int> csize_list_5{8,16, 64, 128, 256, 512, 1024, 2048, 4096, 4096*2};
    const vector<int> csize_list_6{8,16, 64, 128, 256, 512, 1024, 2048, 4096, 4096*2};
    const vector<int> csize_list_7{8,16, 64, 128, 256, 512, 1024, 2048, 4096, 4096*2};
    const vector<int> csize_list_8{8,16, 64, 128, 256, 512, 1024, 2048, 4096, 4096*2};
    const vector<int> csize_list_9{8,16, 64, 128, 256, 512, 1024, 2048, 4096, 4096*2};

    const intMatrix csize_listr{csize_list_0, csize_list_1, csize_list_2, csize_list_3, csize_list_4, csize_list_5, csize_list_6, csize_list_7, csize_list_8, csize_list_9};

    return csize_listr[band];
}





