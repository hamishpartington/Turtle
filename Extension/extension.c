#include "extension.h"

int main(void)
{

    lines *start = init_lines();
    SDL_display_turtle(start);
    lines_free(start);

    return EXIT_SUCCESS;
}

int SDL_display_turtle(lines* start)
{
    //copy pointer to move
    lines *l = start;
    // initialise SDL modules
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    // creates window
    SDL_Window *window = SDL_CreateWindow("Turtle",
                                          // x position
                                          SDL_WINDOWPOS_CENTERED,
                                          // y position
                                          SDL_WINDOWPOS_CENTERED,
                                          // width
                                          WIDTH,
                                          // height
                                          HEIGHT,
                                          SDL_WINDOW_OPENGL);

    // creates texture
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *display = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                   SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);
    //initiate turtle
    SDL_Rect turtle;
    turtle.w = SIDELENGTH;
    turtle.h = SIDELENGTH;
    turtle.x = START_X;
    turtle.y = START_Y;
 
    // SDL_Point points[MAX_POINTS];
    // points[0].x = START_X + ADJUST;
    // points[0].y = START_Y;
    // int pn = 0;

    bool quit = false;

    while (!quit)
    {
        SDL_Event e;

        if (SDL_PollEvent(&e))
        {   
            if(e.type == SDL_QUIT){
                quit = true;
            }
            if(e.type == SDL_KEYDOWN){
                turtle = move_turtle(&l, e.key.keysym.sym, turtle);
            }
        SDL_SetRenderDrawColor(renderer, DARK_GREEN, OPAQUE);
        SDL_RenderFillRect(renderer, &turtle);
        render_line(start, renderer);
        SDL_SetRenderTarget(renderer, NULL);
        SDL_RenderCopy(renderer, display, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_SetRenderTarget(renderer, display);
        
        SDL_SetRenderDrawColor(renderer, WHITE, OPAQUE);
        SDL_RenderClear(renderer);
        
        }

    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

lines* init_lines(void)
{
    lines* l = (lines*)neill_calloc(1, sizeof(lines));
    l->x = START_X;
    l->y = START_Y;
    l->colour = 'W';
    l->facing = PI/2;

    return l;
}

SDL_Rect move_turtle(lines **l, int key_inp, SDL_Rect turtle)
{   
    if(key_inp == SDLK_UP){
        advance_line(l);
        set_prev_values(l);
        (*l)->x = new_x(*l, -MOVE_DIST);
        (*l)->y = new_y(*l, -MOVE_DIST);
        turtle.y = (*l)->y;
        turtle.x = (*l)->x;
    }else if (key_inp == SDLK_DOWN){
        advance_line(l);
        set_prev_values(l);
        (*l)->x = new_x(*l, MOVE_DIST);
        (*l)->y = new_y(*l, MOVE_DIST);
        turtle.y = (*l)->y;
        turtle.x = (*l)->x;
    }else if (key_inp== SDLK_LEFT){
        advance_line(l);
        set_prev_values(l);
        (*l)->facing = facing_adjust(((*l)->previous->facing + RAD));
    }else if (key_inp == SDLK_RIGHT){
        advance_line(l);
        set_prev_values(l);
        (*l)->facing = facing_adjust(((*l)->previous->facing - RAD));
    }else if (key_inp == SDLK_w){
        advance_line(l);
        set_prev_values(l);
        (*l)->colour = 'W';
    }else if (key_inp == SDLK_c){
        advance_line(l);
        set_prev_values(l);
        (*l)->colour = 'C';
    }else if (key_inp == SDLK_k){
        advance_line(l);
        set_prev_values(l);
        (*l)->colour = 'K';
    }else if (key_inp == SDLK_b){
        advance_line(l);
        set_prev_values(l);
        (*l)->colour = 'B';
    }else if (key_inp == SDLK_r){
        advance_line(l);
        set_prev_values(l);
        (*l)->colour = 'R';
    }else if (key_inp == SDLK_m){
        advance_line(l);
        set_prev_values(l);
        (*l)->colour = 'M';
    }else if (key_inp == SDLK_g){
        advance_line(l);
        set_prev_values(l);
        (*l)->colour = 'G';
    }else if (key_inp == SDLK_y){
        advance_line(l);
        set_prev_values(l);
        (*l)->colour = 'Y';
    }
    return turtle;
}

bool lines_free(lines* start)
{
    if(!start){
        return false;
    }

    lines *temp;

    while(start->next){
        temp = start->next;
        free(start);
        start = temp;
    }

    free(start);

    return true;
}


double new_y(lines* l, double n)
{
    double new_y;
    if(l->facing <= PI/2){
        new_y = l->previous->y + opposite(n, l->facing);
    }else if(l->facing <= PI){
        new_y = l->previous->y + adjacent(n, (l->facing - PI/2));
    }else if(l->facing <= 1.5 * PI){
        new_y = l->previous->y - opposite(n, (l->facing - PI));
    }else{
        new_y = l->previous->y - adjacent(n, (l->facing - 1.5*PI));
    }
    if(new_y > HEIGHT){
        return (new_y - HEIGHT);
    }
    if(new_y < 0){
        return (new_y + HEIGHT);
    }
    return new_y;
}

double new_x(lines* l, double n)
{
    double new_x;
    if(l->facing <= PI/2){
        new_x = l->previous->x - adjacent(n, l->facing);
    }else if(l->facing <= PI){
        new_x = l->previous->x + opposite(n, (l->facing - PI/2));
    }else if(l->facing <= 1.5 * PI){
        new_x = l->previous->x + adjacent(n, (l->facing - PI));
    }else{
        new_x = l->previous->x - opposite(n, (l->facing - 1.5*PI));
    }

    if(new_x > WIDTH){
        return (new_x - WIDTH);
    }
    if(new_x < 0){
        return (new_x + WIDTH);
    }

    return new_x;
}


double opposite(double hypotenuse, double theta)
{
    double opposite;

    opposite = sin(theta) * hypotenuse;

    return opposite;
}

double adjacent(double hypotenuse, double theta)
{
    double adjacent;

    adjacent = cos(theta) * hypotenuse;

    return adjacent;
}

double facing_adjust(double facing)
{
    if(facing > (2*PI)){
        facing = facing - (2*PI);
    }
    if(facing < 0 ){
        facing = facing + (2*PI);
    }

    return facing;
}

void* neill_calloc(int n, size_t size)
{
   void* v = calloc(n, size);
   if(v==NULL){
    fprintf(stderr, "Cannot calloc() space\n");
    exit(EXIT_FAILURE);
   }
   return v;
}

void advance_line(lines** t)
{
    lines* prev = *t;
    (*t)->next = (lines*)neill_calloc(1, sizeof(lines));
    (*t) = (*t)->next;
    (*t)->previous = prev;
}

void set_prev_values(lines** t)
{
    (*t)->colour = (*t)->previous->colour;
    (*t)->facing = (*t)->previous->facing;
    (*t)->x = (*t)->previous->x;
    (*t)->y = (*t)->previous->y;

}

void render_line(lines* start, SDL_Renderer *renderer)
{
    double x1, x2, y1, y2;
    while(start->next != NULL){
        x1 = start->x;
        x2 = start->next->x;
        y1 = start->y;
        y2 = start->next->y;
        set_colour(renderer, start->colour);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        start = start->next;

    }
}

void set_colour(SDL_Renderer *renderer, char colour)
{

 switch (colour)
    {
    case 'W':
        SDL_SetRenderDrawColor(renderer, GREY, OPAQUE);
        break;

    case 'B':
        SDL_SetRenderDrawColor(renderer, BLUE, OPAQUE);
        break;

    case 'K':
        SDL_SetRenderDrawColor(renderer, BLACK, OPAQUE);
        break;

    case 'C':
        SDL_SetRenderDrawColor(renderer, CYAN, OPAQUE);
        break;

    case 'Y':
        SDL_SetRenderDrawColor(renderer, YELLOW, OPAQUE);
        break;

    case 'G':
        SDL_SetRenderDrawColor(renderer, GREEN, OPAQUE);
        break;

    case 'R':
        SDL_SetRenderDrawColor(renderer, RED, OPAQUE);
        break;

    case 'M':
        SDL_SetRenderDrawColor(renderer, MAGENTA, OPAQUE);
        break;
    }
}
// void SDL_make_board(SDL_Renderer* renderer)
// {
//   SDL_Rect square = {0, 0, CHECKER_SIZE, CHECKER_SIZE};
//   SDL_Rect queen  = {QUEEN_OFFSET, QUEEN_OFFSET, QUEEN_SIZE, QUEEN_SIZE};
//   bool white = true;
//   int qy;

//   for(int i = 0; i < n; i++){
//     for(int j = 0; j < n; j++){
//       if(!white){
//         SDL_SetRenderDrawColor(renderer, BLACK, OPAQUE);
//       }else{
//         SDL_SetRenderDrawColor(renderer, WHITE, OPAQUE);
//       }
//       SDL_RenderFillRect(renderer, &square);

//       if((sol[i] - 1 == j)){
//       //red queen
//         SDL_SetRenderDrawColor(renderer, RED, OPAQUE);
//         qy = j*CHECKER_SIZE + QUEEN_OFFSET;
//         queen.y = qy;
//         SDL_RenderFillRect(renderer, &queen);
//       }
//       square.y += CHECKER_SIZE;
//       /*alternate black white squares except for when starting new column on
//         boards where n is even*/
//       if(!(j == (n-ADJUST) && n % EVEN == 0)){
//         white = !white;
//       }
//     }
//     square.y = 0;
//     square.x += CHECKER_SIZE;
//     queen.x += CHECKER_SIZE;
//   }
// }
