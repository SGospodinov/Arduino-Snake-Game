#include "LedControl.h"

#define CS_PIN 10
#define CLK_PIN 11
#define DATA_PIN 12
#define MAT_WIDTH 8
#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3

LedControl lc = LedControl(DATA_PIN, CLK_PIN, CS_PIN, 1);
boolean mat[MAT_WIDTH][MAT_WIDTH];
boolean food = false;
boolean alive = true;
int direction = RIGHT;
int snake_length = 2;
int snake[63][2] = {{3,4},{3,3}};
int button_pins[] = {3,4,5,6};
int food_x, food_y;

byte row_to_byte(boolean* row){
  byte result = 0;
  for(int i = 0; i<MAT_WIDTH; i++){
    result = (result << 1) | row[i];
  }
  return result;
}

void display_mat(){
  lc.clearDisplay(0);
  for(int i = 0; i<MAT_WIDTH; i++){
    lc.setRow(0, i, row_to_byte(mat[i]));    
  }
}

void generate_food(){
  if(!food){
    do{
       food_x = random(MAT_WIDTH);
       food_y = random(MAT_WIDTH);
    }while(mat[food_x][food_y]);
    mat[food_x][food_y] = true;
  }
  food = true;
}

void normalize_snake(){
  for(int i = 0; i < snake_length; i++){
    for(int j = 0; j < 2; j++){ 
      if(snake[i][j] > 7){
        snake[i][j] = snake[i][j] - 8;
      }else if(snake[i][j] < 0){
        snake[i][j] = 8 + snake[i][j];
      }
    }
  }
}

void move_snake(){
  //delete old snake state
  for(int i = 0; i < snake_length; i++){ 
    mat[snake[i][0]][snake[i][1]] = false;
  }
  int new_snake[63][2];
  //find new head of the snake
  switch(direction){
    case RIGHT:
      new_snake[0][0] = snake[0][0];
      new_snake[0][1] = snake[0][1]+1;
      break;
    case LEFT:
      new_snake[0][0] = snake[0][0];
      new_snake[0][1] = snake[0][1]-1;
      break;
    case UP:
      new_snake[0][0] = snake[0][0]-1;
      new_snake[0][1] = snake[0][1];
      break;
    case DOWN:
      new_snake[0][0] = snake[0][0]+1;
      new_snake[0][1] = snake[0][1];
      break;      
  }
  for(int i = 1; i < snake_length; i++){
    new_snake[i][0] = snake[i-1][0];
    new_snake[i][1] = snake[i-1][1];
  }
  for(int i = 0; i < snake_length; i++){
    snake[i][0] = new_snake[i][0];
    snake[i][1] = new_snake[i][1];
  }
  normalize_snake();
}

void add_snake(){
  for(int i = 0; i < snake_length; i++){
    mat[snake[i][0]][snake[i][1]] = true;
  }
}

void read_buttons(){
  if(digitalRead(button_pins[LEFT]) == HIGH && direction != RIGHT)
    direction = LEFT;
  else if(digitalRead(button_pins[RIGHT]) == HIGH && direction != LEFT)
    direction = RIGHT;
  else if(digitalRead(button_pins[UP]) == HIGH && direction != DOWN)
    direction = UP;
  else if(digitalRead(button_pins[DOWN]) == HIGH && direction != UP)
    direction = DOWN;
}

void check_for_hit(){
  if(snake[0][0] == food_x && snake[0][1] == food_y){
    eat_food();
  }
  for(int i = snake_length-1; i > 4; i--){
    if(snake[0][0] == snake[i][0] && snake[0][1] == snake[i][1]){
      alive = false;
    }
  }
}

void eat_food(){
  food = false;
  snake_length++;
  for(int i = snake_length; i > 1; i--){
    snake[i][0] = snake[i-1][0];
    snake[i][1] = snake[i-1][1];
  }
  snake[1][0] = food_x;
  snake[1][1] = food_y;
  switch(direction){
    case RIGHT:
      snake[0][0] = snake[0][0];
      snake[0][1] = snake[0][1]+1;
      break;
    case LEFT:
      snake[0][0] = snake[0][0];
      snake[0][1] = snake[0][1]-1;
      break;
    case UP:
      snake[0][0] = snake[0][0]-1;
      snake[0][1] = snake[0][1];
      break;
    case DOWN:
      snake[0][0] = snake[0][0]+1;
      snake[0][1] = snake[0][1];
      break;      
  }
}

void setup() {
  lc.shutdown(0,false);
  lc.setIntensity(0,8);
  lc.clearDisplay(0);
  Serial.begin(9600);
  randomSeed(analogRead(0));
  for(int i = 0; i < 4; i++)
    pinMode(button_pins[i], OUTPUT);
}

void loop() {
  while(alive && snake_length < 63){
    read_buttons();
    move_snake();
    check_for_hit();
    add_snake();
    generate_food();
    display_mat();
    delay(350);
  }
  lc.clearDisplay(0);
}
