#include "asteroids.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define NUM_SECTIONS 20
#define ASTEROID_THRESHOLD 25
#define SAFE_DISTANCE 3

struct ship_state
{
  int row;
};


bool
is_section_dead_end (int field[][FIELD_WIDTH], int section_num)
{
  int section_size = FIELD_HEIGHT / NUM_SECTIONS;
  int beginning_point = section_num * section_size;
  int ending_point = beginning_point + section_size;
  int asteroid_count = 0;

  // checking to see how many asteroids are in the current row 
  //to see if moving would be beneificial
  for (int row = beginning_point; row < ending_point; row++)
    {
      for (int col = 0; col < FIELD_WIDTH; col++)
	{
	  if (field[row][col])
	    {
	      asteroid_count++;
	    }
	}
    }

  return asteroid_count > ASTEROID_THRESHOLD;
}

bool
will_collide (int field[][FIELD_WIDTH], int row, int move)
{
  int next_row = row + move;
  if (next_row < 0 || next_row >= FIELD_HEIGHT)
    {
      //this is checking to say that moving outside the
      //field parameters will end in collision
      return true;
    }
  for (int i = 0; i < SHIP_WIDTH; i++)
    {
      if (field[next_row][i])
	{
	  // Checking for collision with the front of the ship
	  return true;
	}
    }
  for (int i = SHIP_WIDTH; i < 2 * SHIP_WIDTH; i++)
    {
      if (field[next_row][i])
	{
	  // Checking for collision with the middle of the ship
	  return true;
	}
    }
  for (int i = 2 * SHIP_WIDTH; i < 3 * SHIP_WIDTH; i++)
    {
      if (field[next_row][i])
	{
	    // Checking for collision with the back of the ship
	  return true;
	}
    }
  // Check if the ship is about to collide with an asteroid that is directly under it or
  // 1 or 2 spaces behind it.
  if (move == MOVE_UP && next_row - row <= 2)
    {
      for (int i = 0; i < SHIP_WIDTH; i++)
	{
	  if (field[row][i] && field[row - 1][i])
	    {
	      // Collision detected
	      return true;
	    }
	}
    }
  else if (move == MOVE_DOWN && row - next_row <= 2)
    {
      for (int i = 0; i < SHIP_WIDTH; i++)
	{
	  if (field[row][i] && field[row + 1][i])
	    {
	      // Collision detected
	      return true;
	    }
	}
    }
  // No collision detected
  return false;
}


struct ship_action
move_ship (int field[][FIELD_WIDTH], void *ship_state)
{
  struct ship_state *state = (struct ship_state *) ship_state;

  // If ship state is NULL, initialize it with the starting position which will always set the 
  //ships starting position to the middle of the feild
  if (state == NULL)
    {
      state = (struct ship_state *) malloc (sizeof (struct ship_state));
      state->row = FIELD_HEIGHT / 2;
    }

  int current_row = state->row;
  int move = MOVE_NO;



  // Check the nearest asteroids on the row above and below the ship
  int nearest_above = FIELD_WIDTH, nearest_below = FIELD_WIDTH;
  for (int i = 0; i < FIELD_WIDTH; i++)
    {
      if (field[current_row - 1][i])
	{
	  nearest_above = i;
	  break;
	}
    }
  for (int i = 0; i < FIELD_WIDTH; i++)
    {
      if (field[current_row + 1][i])
	{
	  nearest_below = i;
	  break;
	}
    }


  // If the ship is too close to the top or bottom edge of the field, move away from the edge
  if (current_row <= SHIP_WIDTH || current_row >= FIELD_HEIGHT - SHIP_WIDTH)
    {
      if (nearest_above > nearest_below)
	{
	  move = MOVE_UP;
	}
      else
	{
	  move = MOVE_DOWN;
	}
    }
    
  else if (nearest_above >= SAFE_DISTANCE
	   && !will_collide (field, current_row, MOVE_UP)
	   && !is_section_dead_end (field,
				    current_row / FIELD_HEIGHT *
				    NUM_SECTIONS))
    {
      move = MOVE_UP;
    }
  else if (nearest_below >= SAFE_DISTANCE
	   && !will_collide (field, current_row, MOVE_DOWN)
	   && !is_section_dead_end (field,
				    (current_row +
				     SHIP_WIDTH) / FIELD_HEIGHT *
				    NUM_SECTIONS))
    {
      move = MOVE_DOWN;
    }


  // If there is no asteroid nearby, move randomly up or down
  else if (nearest_above >= SAFE_DISTANCE && nearest_below >= SAFE_DISTANCE)
    {
      int dice = random () % 100;
      if (dice < 50)
	move = MOVE_UP;
      else if (dice < 100)
	move = MOVE_DOWN;
    }
  // If moving up will not cause a collision, move up
  else if (nearest_above >= SAFE_DISTANCE
	   && !will_collide (field, current_row, MOVE_UP))
    {
      move = MOVE_UP;
    }
  // If moving down will not cause a collision, move down
  else if (nearest_below >= SAFE_DISTANCE
	   && !will_collide (field, current_row, MOVE_DOWN))
    {
      move = MOVE_DOWN;
    }
  // If moving up will not cause a collision with an asteroid that is directly under the ship or
  // 1 or 2 spaces behind it, move up.
  else if (nearest_above >= SAFE_DISTANCE
	   && !will_collide (field, current_row, MOVE_UP)
	   && !will_collide (field, current_row, MOVE_UP))
    {
      move = MOVE_UP;
    }
  // If moving down will not cause a collision with an asteroid that is directly under the ship or
  // 1 or 2 spaces behind it, move down.
  else if (nearest_below >= SAFE_DISTANCE
	   && !will_collide (field, current_row, MOVE_DOWN)
	   && !will_collide (field, current_row, MOVE_DOWN))
    {
      move = MOVE_DOWN;
    }

  // If the move will cause a collision, don't move
  if (move != MOVE_NO && will_collide (field, current_row, move))
    {
      move = MOVE_NO;
    }

  // Update the ship state
  state->row += move;

  struct ship_action action = { move, state };
  return action;
}
