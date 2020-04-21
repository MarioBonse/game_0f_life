#include <iostream>
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include <chrono>
#include <thread>

//#define PRINT False


// USED TO REPRESENT BOARD CHUNKS
typedef struct {
    int start;
    int end;
} RANGE;

int square__init(){
    return (std::rand())%2;
}

void update_cell(int j, int N, int M, std::vector<int> &board, std::vector<int> &previus_board){
    int row = j/M;
    int col = j%M;
    int up = (row == 0) ? 0: previus_board[(row - 1)*M + col];
    int up_left = (row == 0 || col == 0) ? 0: previus_board[(row - 1)*M + col - 1]; 
    int up_right = (row == 0 || col == M - 1) ? 0: previus_board[(row - 1)*M + col + 1]; 
    int down = (row == N - 1) ? 0: previus_board[(row + 1)*M + col];
    int left = (col == 0) ? 0: previus_board[(row)*M + col - 1];
    int right = (col == M - 1) ? 0: previus_board[(row)*M + col + 1];
    int down_left = ((row == N - 1) || col == 0) ? 0: previus_board[(row + 1)*M + col - 1];
    int down_right = ((row == N - 1) || col == M - 1) ? 0: previus_board[(row + 1)*M + col + 1];
    int sum = up + right + left + down + down_left + down_right + up_left + up_right;

    board[row*M + col] = previus_board[row*M + col]; // copy previus value and maybe change it
    if((previus_board[row*M + col] == 0) && (sum == 3))//dead -> live
    {
        board[row*M + col] = 1;
    }
    if ( (previus_board[row*M + col] == 1) && ((sum < 2) || (sum > 3))){// alived -> die
        board[row*M + col]  = 0;
    }

}

void print_board(std::vector<int> previus_board, int N, int M){
    for (int i=0; i<M + 2; ++i)
        std::cout<<"\r";
    for (int i=0; i<M; ++i)
        std::cout<<"-";
    std::cout<<"\n";
    for (int i=0; i<N; ++i){
        for (int j=0; j<M; ++j){
            if((previus_board[j + M*i] != 1) && (previus_board[j + M*i] != 0))
                std::cout<<"ERRORRR";
            if(previus_board[j + M*i] ){
                std::cout<<"x";
            }else{
                std::cout<<" ";
            }
        }
        std::cout<<"\n";
    }
    for (int i=0; i<M; ++i)
        std::cout<<"-";
    std::cout<<"\n";
}



int main(int argc, char * argv[]) {

    if(argc != 6) {
        std::cout << "Usage is: " << argv[0] << " N M number_step seed nw" << std::endl;
        return(-1);
    }

    int N = atoi(argv[1]);
    int M = atoi(argv[2]);
    int num_steps = atoi(argv[3]);
    std::srand(atoi(argv[4]));
    int nw = atoi(argv[5]);


    int index = 0; 
    std::vector<std::vector<int>>  boards(2, std::vector<int> (N*M) );  
    std::generate(boards[index].begin(), boards[index].end(), square__init); 

    #ifdef PRINT
        print_board(boards[index], N, M);
    #endif



    auto start = std::chrono::high_resolution_clock::now();
    for (int i=0; i< num_steps; i++) {
        #pragma omp parallel for num_threads(nw) 
        for(int j = 0; j<N*M; j++){
            update_cell(j, N, M, boards[!index], boards[index]);

        }
        #ifdef PRINT
        print_board(boards[!index], N, M);
        sleep(1);
        #endif // DEBUG

        // I can do also this in parallel
        index = !index;

    }
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    auto usec    = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    std::cout << usec << std::endl;
    return(0);
}