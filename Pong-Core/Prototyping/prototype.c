/*
 * Snake2Pong -- Making pong from Snake since 2022
 * seand - code from Dr. Carroll
 * This utilizes a pong_board struct of size BOARD_SIZE
 * 
*/


#include <stdio.h>

#define BOARD_SIZE 8
//These defines being global may not be JPL compliant, but more compliant than magic numbers, you have been warned!!
#define PADDLE_WIDTH 1
#define PADDLE_HEIGHT 3

//Command to BUILD 
//gcc prototype.c -o Prototype.o -Wall

enum move{STAY, UP, DOWN};
enum side{LEFT, RIGHT, UP, DOWN};							//UP and DOWN are placements for DEBUG purposes
enum pong_ball_dirs{N, NE, E, SE, S, SW, W, NW};			//Cardinal Directions (N,E,S,W) are for DEBUG puropses

typedef struct {
    char board[BOARD_SIZE][BOARD_SIZE];
} pong_board;

typedef struct{
    char paddle_object[PADDLE_WIDTH][PADDLE_HEIGHT];
    int loc[2];                 //NOTE MAGIC NUMBER-- This array is of size two merely because the array for the pong board is 2D, and therefore only has two dimensions for locations
} pong_paddle;

typedef struct{
    char ball_object;
    int loc[2];                  //Same NOTE as pong_paddle.loc
    enum pong_ball_dirs dir;
} pong_sphere;

void pong_board_init(pong_board* pb){
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

    // pb->board[BOARD_SIZE][BOARD_SIZE] = filled_board;

    for(int i=0; i<BOARD_SIZE; i++){
        for(int j=0; j<BOARD_SIZE; j++){
            pb->board[i][j] = filled_board[i][j];
        }
    }
}

void pong_board_display(pong_board* pb){
    for(int i=0; i<BOARD_SIZE; i++){
        for(int j=0; j<BOARD_SIZE; j++){
            printf("%c  ", pb->board[i][j]);
        }
        printf("\n");
    }
}

void paddle_init(pong_paddle* pd, enum side side){
    char paddle_char = 'X';

    //Condition for if the paddle is to be placed on the left side FIXED MAGIC NUMBER WITH ENUM
    if(side == LEFT){
        pd->loc[0] = 0;
        pd->loc[1] = 3;
    }
    //Condition for if the paddle is to be placed on the right side FIXED MAGIC NUMBER WITH ENUM
    if(side == RIGHT){
        pd->loc[0] = 8;
        pd->loc[1] = 3;
    }
    
    for(int i=0; i<PADDLE_HEIGHT; i++){
        for(int j=0; j<PADDLE_WIDTH; j++){
            pd->paddle_object[i][j] = paddle_char;
        }
    }
}

void paddle_movement(pong_paddle* pd, pong_board* pb, enum move move){
    //Guard clause for checking y movement validity
    if(pd->loc[1] == 0 || pd->loc[1] == 6){
        return;
    }

    //This function checks whether the input for the paddle to move UP is high, also may not JPL compliant, you have been warned!!
    if(move == UP){
        pd->loc[1]++;
    }

    //This function checks whether the input for the paddle to move DOWN is high, also may not be JPL compliant, you have been warned!!
    if(move == DOWN){
        pd->loc[1]--;
    }
}

void sphere_init(pong_sphere* ps){
    ps->ball_object = 'O';
    ps->loc[0] = 4;
    ps->loc[1] = 4;
    ps->dir = NE;
}

void sphere_direction_collision(pong_sphere* ps, pong_board* pb){
    //Write some stuff in here that will calculate:
    //  Whether the pong ball has hit a paddle or the ceiling
    //  Choose an opposite direction if it has hit a paddle or celing
    //  Reset the game if it hits the back wall.
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

    return 0;
}
