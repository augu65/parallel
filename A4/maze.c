/*
By: Jonah Stegman
CIS*3090
Prints out a randomized maze based on a size and seeds the randomization
*/
#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <unistd.h>
#include <omp.h>

struct stack_maze
{
    int count;
    int array[10000][3];
    int top;
};

//global grid variable
char ** grid;

// adds an element to the stack
void push(int x, int y, int d, struct stack_maze * stack){
    stack->top ++;
    stack->array[stack->top][0] = x;
    stack->array[stack->top][1] = y;
    stack->array[stack->top][2] = d;
    stack->count ++;
}

//deletes the top element from the stack
void pop(struct stack_maze * stack){
    if (stack->count !=0) {
        stack->array[stack->top][0] = -1;
        stack->array[stack->top][1] = -1;
        stack->array[stack->top][2] = -1;
        stack->top --;
        stack->count --;
    }
}
void pMaze(int num) {
    struct stack_maze *stack = malloc(sizeof(struct stack_maze) * 1);
    stack->count = 0;
    stack->top = 0;
    
}

void maze(int size) {
    struct stack_maze * stack = malloc(sizeof(struct stack_maze)*1);
    stack->count = 0;
    stack->top = 0;
    int top[3];
    grid[1][1] = '0';
    //randomize the start
    int num = rand()%2;
    if(num == 1){
        push(3, 1, 1, stack);
        push(1, 3, 2, stack);
    }else{
        push(1, 3, 2, stack);
        push(3, 1, 1, stack);
    }
    while(stack->count !=0){
        top[0] = stack->array[stack->top][0];
        top[1] = stack->array[stack->top][1];
        top[2] = stack->array[stack->top][2];
        pop(stack);
        if (grid[top[0]][top[1]] == '.'){
            grid[top[0]][top[1]] = '0';
            //prints the number between two filled in dots
            if (top[2] == 1){
                grid[top[0] - 1][top[1] ] = '0';
            }else if(top[2] == 3){
                grid[top[0] + 1][top[1]] = '0';
            }else if(top[2] == 2){
                grid[top[0]][top[1] - 1] = '0';
            }else if(top[2] == 4){
                grid[top[0]][top[1] + 1] = '0';
            }
            int arr[4];
            int ctr = 0;
            while(ctr < 4){
                int flag = 0;
                //gets random numbers
                do{
                    flag = 0;
                    num = rand()%4;
                    for(int i=0; i<ctr; i++){
                        if(arr[i] == num){
                            flag = 1;
                            break;
                        }
                    }
                }while(flag == 1);
                arr[ctr] = num;
                ctr++;
                //pushes on all 4 sizes if possible
                if(top[0] - 2 > 0  && num == 0){
                    if(grid[top[0] - 2][top[1]] == '.'){
                        push(top[0] - 2,top[1], 3, stack);
                    }
                }
                if(top[0] + 2 < size && num == 1){
                    if( grid[top[0] + 2][top[1]] == '.'){
                        push(top[0] + 2,top[1], 1, stack);
                    }
                }
                if(top[1] + 2 < size && num == 2){
                    if (grid[top[0]][top[1] + 2] == '.'){
                        push(top[0], top[1] + 2, 2, stack);
                    }
                }
                if(top[1] - 2 > 0 && num == 3){
                    if(grid[top[0]][top[1] - 2] == '.'){
                        push(top[0],top[1] - 2, 4, stack);
                    }
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int thread = 4;
    int argPtr;
    int size = 11;
    int seed = 1;
    // read command line arguments for number of iterations 
    if (argc > 1) {
        argPtr = 1;
        while(argPtr < argc) {
            if (strcmp(argv[argPtr], "-n") == 0) {
                sscanf(argv[argPtr+1], "%d", &size);
                argPtr += 2;
                if(size < 5){
                    printf("please enter a size 5 or larger.\ngoodbye.\n");
                    return 0;
                }
            } else  if (strcmp(argv[argPtr], "-s") == 0) {
                sscanf(argv[argPtr+1], "%d", &seed);
                argPtr += 2;
                // seed the randomization
                srand(seed);
            }
        }
    }
    //create the grid of the correct size
    grid = malloc(sizeof(char*)*size);
    for (int i = 0; i< size; i++) {
        grid[i] = malloc(sizeof(char)*size);
        for (int j = 0; j < size; j++) {
            grid[i][j] = '.';
        }
    }
    #ifdef PARALLEL
        #pragma omp parallel num_threads(thread)
        pMaze(size);
    #else
        maze(size);
    #endif

    //prints out the grid
     for (int i = 0; i< size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
    return 0;
}