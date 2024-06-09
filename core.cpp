#include <SDL2/SDL.h>
#include <emscripten.h>
#include <cstdlib>
#include <vector>
#include <cstdlib>

const int UP = 1;
const int DOWN = 2;
const int LEFT = 3;
const int RIGHT = 4;

const int SCREEN_WIDTH = 700;
const int SCREEN_HEIGHT = 700;

const int NUM_ROWS = 30;
const int NUM_COLUMNS = 30    ;

const float X_START = 0;
const float Y_START = 0;
constexpr float TILES_SEPARATION = SCREEN_WIDTH/NUM_ROWS;

#include <iostream>

/*
Our board is made up of tiles, the sanke moves from one tile to the next.

Each tile has a row, a column, x y coordinates an an id
*/
class Tile{
    public:
    double x;
    double y;
    int id;

    int row;
    int column;

    Tile(int row_a, int column_a,int id_a);
    Tile();
};

//this constructor is just so the compiler doesn't whine. it should never be used
Tile::Tile(){

    id = 0;
    column = 0;
    row = 0;

}

Tile::Tile(int row_a, int column_a,int id_a){

    id = id_a;
    column = column_a;
    row = row_a;

}
/*
The board is a list of tiles, coordinates of the food nad id of the food. 

id_food indicates the tile where the food is located

We also have a snake that is a list of tile ids.
*/
class Board{

    public:
    
    std::vector<Tile> tiles_list;
    Board();

    Tile& getTileId(int id);

    Tile& getTileRowsCol(int row, int col);

    Tile& getNeighbor(int id, int dir);

    void move_snake(int head, int direction);
    int get_random_number();


    std::vector<int> snake;

    double food_x;
    double food_y;

    int id_food;
    
    bool food_exist;


};

int Board::get_random_number(){
    
	// Providing a seed value
	std::srand((unsigned) std::time(NULL));

	// Get a random number
	int random = std::rand();

	return random % tiles_list.size();
}

void Board::move_snake(int head, int direction){
    Tile& new_tile = this->getNeighbor(head, direction);

    if (this->id_food == new_tile.id){
        this->snake.insert(this->snake.begin(), new_tile.id);

        this->id_food = get_random_number();

        this->food_x = tiles_list[id_food].x;
        this->food_y = tiles_list[id_food].y;

    }else{
        this->snake.insert(this->snake.begin(), new_tile.id);
        this->snake.pop_back();
    }
}


Board::Board(){

    int id_count = 0;//gives a unique id to each tile
    for (int i =0; i< NUM_ROWS; i++){
        for (int j =0 ;j< NUM_COLUMNS; j++){
            Tile my_tile = Tile(i, j, id_count);

            my_tile.x = X_START + (i * TILES_SEPARATION);
			my_tile.y = Y_START + (j * TILES_SEPARATION);

            tiles_list.push_back(my_tile);

            id_count++;
        }
    }
    id_food = get_random_number();

    food_x = tiles_list[id_food].x;
    food_y = tiles_list[id_food].y;
}

Tile& Board::getTileId(int id){
    for ( auto& tile : this->tiles_list){
        if (tile.id == id)
            return tile;
    }
}
/*
Get a tile using its row and column
*/
Tile& Board::getTileRowsCol(int row, int col){
    for ( auto& tile : this->tiles_list){
        if ((tile.row == row) &&( tile.column == col))
            return tile;
    }
}
/*
Obtain the neighboring tile given an id and a direction
*/
Tile& Board::getNeighbor(int id, int dir){
    Tile& myTile = this->getTileId(id);
    int neighbor_column, neighbor_row = 0;

    switch( dir){
        case UP:
         neighbor_row = ( myTile.row );
         neighbor_column = (myTile.column-1)% NUM_COLUMNS;
        break;

        case DOWN:
         neighbor_row = ( myTile.row );
         neighbor_column = (myTile.column+1)% NUM_COLUMNS;
        break;

        case LEFT:
         neighbor_row = ( myTile.row -1) % NUM_ROWS;
         neighbor_column = (myTile.column);
        break;

        case RIGHT:
         neighbor_row = ( myTile.row +1 ) % NUM_ROWS;
         neighbor_column = (myTile.column);
        break;
    }
    return getTileRowsCol(neighbor_row, neighbor_column);
}






struct context
{
    SDL_Renderer *renderer;
    int iteration;
    Board myBoard;
    int tileId;
    int direction;

    
};



// start_timer(): call JS to set an async timer for 500ms
EM_JS(void, start_timer, (), {
  Module.timer = false;
  setTimeout(function() {
    Module.timer = true;
  }, 500);
});


void mainloop(void *arg)
{
    
    
    context *ctx = static_cast<context*>(arg);
    SDL_Renderer *renderer = ctx->renderer;

    std::cout << "direction--:"<< std::to_string(ctx->direction) << std::endl;
    
    // example: draw a moving rectangle
    
    // red background
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);

    
    
 	SDL_PumpEvents();  
	
    const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );
        if( currentKeyStates[ SDL_SCANCODE_UP ] ){
            std::cout << "up" << std::endl;
            ctx->direction = UP;
        } else if( currentKeyStates[ SDL_SCANCODE_DOWN ] ){
            std::cout << "down" << std::endl;
            ctx->direction = DOWN;
        } else if( currentKeyStates[ SDL_SCANCODE_LEFT ] ){
            std::cout << "right" << std::endl;
            ctx->direction = LEFT;
        } else if( currentKeyStates[ SDL_SCANCODE_RIGHT ] ) {
            std::cout << "right" << std::endl;
           ctx-> direction = RIGHT;
        } else{
            std::cout << "no cambio de dir" << std::endl;
        }
    

    Tile& new_tile = ctx->myBoard.getNeighbor(ctx->tileId, ctx->direction);
    ctx->myBoard.move_snake(ctx->tileId, ctx->direction);
    
    // moving blue rectangle
    SDL_Rect r;
    r.x = new_tile.x;
    r.y = new_tile.y;
    //r.x = ctx->iteration;
    //r.y =20;
    r.w = TILES_SEPARATION-1;
    r.h = TILES_SEPARATION-1;
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255 );
    SDL_RenderFillRect(renderer, &r );

    //draw food
    SDL_Rect food_rect;
    food_rect.x = ctx->myBoard.food_x;
    food_rect.y = ctx->myBoard.food_y;
    //r.x = ctx->iteration;
    //r.y =20;
    food_rect.w = TILES_SEPARATION-3;
    food_rect.h = TILES_SEPARATION-3;
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255 );
    SDL_RenderFillRect(renderer, &food_rect );

    //draw snake
    for (const auto& tile_id: ctx->myBoard.snake){
        Tile& borrowed = ctx->myBoard.tiles_list[tile_id];
        SDL_Rect s;
        s.x = borrowed.x;
        s.y = borrowed.y;
        s.w = TILES_SEPARATION-2;
        s.h = TILES_SEPARATION-2;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255 );
        SDL_RenderFillRect(renderer, &s );
        }
        

    SDL_RenderPresent(renderer);

    ctx->iteration++;
    ctx->tileId=new_tile.id;

    emscripten_sleep(100);
}

int main()
{
    start_timer();
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);

    Board myBoard = Board();

    Tile& my_tile=myBoard.getTileRowsCol(3,3);

    context ctx;
    ctx.renderer = renderer;
    ctx.iteration = 0;
    ctx.myBoard = myBoard;
    ctx.tileId = my_tile.id;
    ctx.direction= RIGHT;

    const int simulate_infinite_loop = 1; // call the function repeatedly
    const int fps = -1; // call the function as fast as the browser wants to render (typically 60fps)

        
    emscripten_set_main_loop_arg(mainloop, &ctx, fps, simulate_infinite_loop);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
