#include <stdio.h>

#define BOARD_SIZE 8

//Command to BUILD 
//gcc prototype.c -o Prototype.o

struct pong_board{
    char board[BOARD_SIZE][BOARD_SIZE];
}pong_board_struct;

void pong_board_init(struct pong_board* pb){
    // char filled_board[BOARD_SIZE][BOARD_SIZE];
    char filled_board[BOARD_SIZE][BOARD_SIZE] = {
        {"........"},
        {"........"},
        {"........"},
        {"........"},
        {"........"},
        {"........"},
        {"........"},
        {"........"}};
    //OR for a functional approach
    // for(int i=0; i<BOARD_SIZE; i++){
    //     for(int j=0; j<BOARD_SIZE; j++){
    //         filled_board[i][j]=".";
    //     }
    // }

    pb->board[BOARD_SIZE][BOARD_SIZE] = filled_board;
}

void pong_board_display(struct pong_board* pb){
    for(int i=0; i<BOARD_SIZE; i++){
        for(int j=0; j<BOARD_SIZE; j++){
            printf("%d", pb->board[i][j]);
        }
    }
}

void print_XO_board(){
    //Prints out an 8x8 matrix of Xs
    for(int n=0; n<BOARD_SIZE/2; n++){
        printf("X  O  X  O  X  O  X  O\n");
        printf("O  X  O  X  O  X  O  X\n");
    }
}

void print_dot_board(){
    //Prints out a board of only dots "."
    for(int n=0; n<BOARD_SIZE; n++){
        printf(".  .  .  .  .  .  .  .\n");
    }
}

int main(){
    // struct pong_board pong_game_board;
    // struct pong_board* pong_game_board_ptr;

    // pong_game_board_ptr = &pong_game_board;

    // pong_board_init(pong_game_board_ptr);
    // pong_board_display(pong_game_board_ptr);

    int testx[8][8];
    int testy = {
        {1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1}
    };

    testx = testy;

    return 0;
}
