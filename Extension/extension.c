#include "extension.h"

int main (int argc, char** argv)
{

    if(argc > ARGS){
        fprintf(stderr, "Usage: %s (<output_file>)\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    lines *start = init_lines();
    SDL_display_turtle(start);
    output_ttl(start);
    lines_free(start);

    if(argc == ARGS){
        char system_command[MAX_COMMAND];
        strcpy(system_command, "./interp Extension/SDL_output.ttl ");
        strcat(system_command, argv[1]);
            if(system(system_command) == -1){
                fprintf(stderr, "system() call failed\n");
                return EXIT_FAILURE;
            }
    }

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

    bool quit = false;

    while (!quit)
    {
        SDL_Event e;

        if (SDL_PollEvent(&e))
        {   
            if(e.type == SDL_QUIT){
                quit = true;
            }
            if(e.type == SDL_KEYDOWN || e.type == SDL_KEYUP){
                turtle = move_turtle(&l, e.key.keysym.sym, turtle);
            }
        SDL_SetRenderDrawColor(renderer, RGB_DARK_GREEN, OPAQUE);
        SDL_RenderFillRect(renderer, &turtle);
        render_line(start, renderer);
        SDL_SetRenderTarget(renderer, NULL);
        SDL_RenderCopy(renderer, display, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_SetRenderTarget(renderer, display);
        
        SDL_SetRenderDrawColor(renderer, RGB_WHITE, OPAQUE);
        SDL_RenderClear(renderer);
        
        }

    }
    //add END to program
    advance_line(&l);
    set_prev_values(&l);
    strcpy(l->instruct, "END");
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
    l->colour = WHITE;
    l->facing = HALF_PI;
    strcpy(l->instruct, "START");
    l->val = 0;

    return l;
}

SDL_Rect move_turtle(lines **l, int key_inp, SDL_Rect turtle)
{   
    double x, y;
    if(key_inp == SDLK_UP){
        //to make sure line advance if start
        if(strsame((*l)->instruct, "START")){
            advance_line(l);
            set_prev_values(l);
            strcpy((*l)->instruct, "FORWARD");
            (*l)->val = 0;
        }
        x = new_x(*l, -MOVE_DIST);
        y = new_y(*l, -MOVE_DIST);
        // so turtle does not move out of bounds
        if(!(x > WIDTH || x < 0 || y > HEIGHT || y < 0)){
            advance_line(l);
            set_prev_values(l);
            (*l)->x = x;
            (*l)->y = y;
            turtle.y = (*l)->y;
            turtle.x = (*l)->x;
            strcpy((*l)->instruct, "FORWARD");
            (*l)->val = 1;
        }
    }else if (key_inp == SDLK_DOWN){
        if(strsame((*l)->instruct, "START")){
            advance_line(l);
            set_prev_values(l);
            strcpy((*l)->instruct, "FORWARD");
            (*l)->val = 0;
        }
        x = new_x(*l, MOVE_DIST);
        y = new_y(*l, MOVE_DIST);
        // so turtle does not move out of bounds
        if(!(x > WIDTH || x < 0 || y > HEIGHT || y < 0)){
            advance_line(l);
            set_prev_values(l);
            (*l)->x = x;
            (*l)->y = y;
            turtle.y = (*l)->y;
            turtle.x = (*l)->x;
            strcpy((*l)->instruct, "FORWARD");
            (*l)->val = -1;
        }
    }else if (key_inp == SDLK_LEFT){
        advance_line(l);
        set_prev_values(l);
        (*l)->facing = facing_adjust(((*l)->previous->facing + RAD));
        strcpy((*l)->instruct, "RIGHT");
        (*l)->val = -1;
    }else if (key_inp == SDLK_RIGHT){
        advance_line(l);
        set_prev_values(l);
        (*l)->facing = facing_adjust(((*l)->previous->facing - RAD));
        strcpy((*l)->instruct, "RIGHT");
        (*l)->val = 1;
    }else if (key_inp == SDLK_w){
        advance_line(l);
        set_prev_values(l);
        (*l)->colour = WHITE;
        strcpy((*l)->instruct, "COLOUR \"WHITE\"");
    }else if (key_inp == SDLK_c){
        advance_line(l);
        set_prev_values(l);
        (*l)->colour = CYAN;
        strcpy((*l)->instruct, "COLOUR \"CYAN\"");
    }else if (key_inp == SDLK_k){
        advance_line(l);
        set_prev_values(l);
        (*l)->colour = BLACK;
        strcpy((*l)->instruct, "COLOUR \"BLACK\"");
    }else if (key_inp == SDLK_b){
        advance_line(l);
        set_prev_values(l);
        (*l)->colour = BLUE;
        strcpy((*l)->instruct, "COLOUR \"BLUE\"");
    }else if (key_inp == SDLK_r){
        advance_line(l);
        set_prev_values(l);
        (*l)->colour = RED;
        strcpy((*l)->instruct, "COLOUR \"RED\"");
    }else if (key_inp == SDLK_m){
        advance_line(l);
        set_prev_values(l);
        (*l)->colour = MAGENTA;
        strcpy((*l)->instruct, "COLOUR \"MAGENTA\"");
    }else if (key_inp == SDLK_g){
        advance_line(l);
        set_prev_values(l);
        (*l)->colour = GREEN;
        strcpy((*l)->instruct, "COLOUR \"GREEN\"");
    }else if (key_inp == SDLK_y){
        advance_line(l);
        set_prev_values(l);
        (*l)->colour = YELLOW;
        strcpy((*l)->instruct, "COLOUR \"YELLOW\"");
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
    if(l->facing <= HALF_PI){
        new_y = l->previous->y + opposite(n, l->facing);
    }else if(l->facing <= PI){
        new_y = l->previous->y + adjacent(n, (l->facing - HALF_PI));
    }else if(l->facing <= THREE_HALF_PI){
        new_y = l->previous->y - opposite(n, (l->facing - PI));
    }else{
        new_y = l->previous->y - adjacent(n, (l->facing - THREE_HALF_PI));
    }
    return new_y;
}

double new_x(lines* l, double n)
{
    double new_x;
    if(l->facing <= HALF_PI){
        new_x = l->previous->x - adjacent(n, l->facing);
    }else if(l->facing <= PI){
        new_x = l->previous->x + opposite(n, (l->facing - HALF_PI));
    }else if(l->facing <= THREE_HALF_PI){
        new_x = l->previous->x + adjacent(n, (l->facing - PI));
    }else{
        new_x = l->previous->x - opposite(n, (l->facing - THREE_HALF_PI));
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
    if(facing > (TWO_PI)){
        facing = facing - (TWO_PI);
    }
    if(facing < 0 ){
        facing = facing + (TWO_PI);
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
    case WHITE:
        SDL_SetRenderDrawColor(renderer, RGB_GREY, OPAQUE);
        break;

    case BLUE:
        SDL_SetRenderDrawColor(renderer, RGB_BLUE, OPAQUE);
        break;

    case BLACK:
        SDL_SetRenderDrawColor(renderer, RGB_BLACK, OPAQUE);
        break;

    case CYAN:
        SDL_SetRenderDrawColor(renderer, RGB_CYAN, OPAQUE);
        break;

    case YELLOW:
        SDL_SetRenderDrawColor(renderer, RGB_YELLOW, OPAQUE);
        break;

    case GREEN:
        SDL_SetRenderDrawColor(renderer, RGB_GREEN, OPAQUE);
        break;

    case RED:
        SDL_SetRenderDrawColor(renderer, RGB_RED, OPAQUE);
        break;

    case MAGENTA:
        SDL_SetRenderDrawColor(renderer, RGB_MAGENTA, OPAQUE);
        break;
    }
}

void output_ttl(lines* start)
{
    FILE* output = fopen("Extension/SDL_output.ttl", "w+");
    lines* l = start;
    fprintf(output, "%s\n", start->instruct);
    //to optimise
    while(l->next){
        l = l->next;
        if(strsame(l->instruct, l->previous->instruct)){
            l->val += l->previous->val;
        }
        if(!strsame(l->instruct, l->next->instruct)){
           if(l->instruct[0] == COLOUR){
                fprintf(output, "%s\n", l->instruct);
           }else{
                fprintf(output, "%s %f\n",l->instruct, l->val);
           }
        }
        if(strsame("END", l->next->instruct)){
            l = l->next;
            fprintf(output, "%s\n", l->instruct);
        }
    }
    fclose(output);
}
