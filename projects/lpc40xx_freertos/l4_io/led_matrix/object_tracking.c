#include "object_tracking.h"
#include "mp3.h"
#include "shapes.h"

static uint8_t old_i = 50;
static uint8_t first_moving_object = 1, first_enemy_object = 2;
time_t t;
struct object_details onscreen_objects_struct[number_of_objects];

void initialize_object_details() {
  int random;
  life = 50;
  enemy_score = 0;
  // void_function_t draw_enemy_pointer = &draw_enemy;
  for (int i = 0; i < number_of_objects; i++) {
    random = rand() % 63;
    onscreen_objects_struct[i].row = random;
    random = rand() % 63;
    onscreen_objects_struct[i].column = random;

    if (i == 0)
      onscreen_objects_struct[i].obj_nature = FRIEND_OBJECT;
    else if (i == 1)
      onscreen_objects_struct[i].obj_nature = LIFE_OBJECT;
    else
      onscreen_objects_struct[i].obj_nature = ENEMY_OBJECT;

    // onscreen_objects_struct[i].obj_nature = rand() % 2;

    onscreen_objects_struct[i].status = true;
  }
}

void randomizer_objects() {
  int random;
  for (int i = first_moving_object; i < number_of_objects; i++) {
    random = rand() % 3;
    random = random - 1;
    onscreen_objects_struct[i].row += random;
    if ((onscreen_objects_struct[i].row < -8) || (onscreen_objects_struct[i].row > 71))
      onscreen_objects_struct[i].row = rand() % 63;
    random = rand() % 3;
    random = random - 1;
    onscreen_objects_struct[i].column += random;
    if ((onscreen_objects_struct[i].column < -8) || (onscreen_objects_struct[i].column > 71))
      onscreen_objects_struct[i].column = rand() % 63;
    // printf("%d %d %d\n", onscreen_objects_struct[i].row, onscreen_objects_struct[i].column, i);
  }
}

void randomizer_objects_level_1() {

  uint8_t upper = 10, lower = 50;

  for (int i = first_moving_object; i < number_of_objects; i++) {

    if ((onscreen_objects_struct[i].row < upper) || (onscreen_objects_struct[i].row > lower))
      onscreen_objects_struct[i].row = upper + (rand() % (lower - upper));

    onscreen_objects_struct[i].column--;
    if ((onscreen_objects_struct[i].column < -8) || (onscreen_objects_struct[i].column > 71))
      onscreen_objects_struct[i].column = 63;
    // printf("%d %d %d\n", onscreen_objects_struct[i].row, onscreen_objects_struct[i].column, i);
  }
}

void randomizer_objects_level_2() {

  int random;
  for (int i = first_moving_object; i < number_of_objects; i++) {
    random = rand() % 3;
    random = random - 1;
    onscreen_objects_struct[i].row += random;
    if ((onscreen_objects_struct[i].row < -8) || (onscreen_objects_struct[i].row > 71))
      onscreen_objects_struct[i].row = rand() % 63;

    onscreen_objects_struct[i].column--;
    if ((onscreen_objects_struct[i].column < -8) || (onscreen_objects_struct[i].column > 71))
      onscreen_objects_struct[i].column = 63;
    // printf("%d %d %d\n", onscreen_objects_struct[i].row, onscreen_objects_struct[i].column, i);
  }
}

void draw_from_structure() {

  for (uint8_t i = 0; i < number_of_objects; i++) {
    if (onscreen_objects_struct[i].status) {
      switch (onscreen_objects_struct[i].obj_nature) {

      case FRIEND_OBJECT:
        draw_friend(onscreen_objects_struct[i].row, onscreen_objects_struct[i].column);
        break;

      case ENEMY_OBJECT:
        draw_enemy(onscreen_objects_struct[i].row, onscreen_objects_struct[i].column);
        break;

      case LIFE_OBJECT:
        draw_life(onscreen_objects_struct[i].row, onscreen_objects_struct[i].column);
        break;

      default:
        break;
      }
    }
  }
}

void detect_click(uint8_t p, uint8_t q, uint8_t hit) {

  if (hit) {
    uint8_t x = p, y = q;

    uint64_t temp;

    temp = frame_buffer[x][ENEMY_PLANE];
    temp = temp >> (63 - y);
    temp = temp << 63;
    led_matrix__set_pixel(x, 63 - y, RED);

    if (temp != 0) {
      for (uint8_t i = 0; i < number_of_objects; i++) {

        if (((onscreen_objects_struct[i].row) <= x) && ((onscreen_objects_struct[i].row) + 7 >= x) &&
            ((onscreen_objects_struct[i].column) <= y) && ((onscreen_objects_struct[i].column) + 7 >= y)) {
          onscreen_objects_struct[i].status = false;
          enemy_score++;
          // mp3__send_command(C_PLAY_FOLD_FILE, 0x0301);
          // printf("Friendly kill left %d Enemy Killed %d\n", life, enemy_score);
        }
      }
    }

    temp = frame_buffer[p][FRIEND_PLANE];
    temp = temp >> (63 - q);
    temp = temp << 63;
    // led_matrix__set_pixel(p, 63 - q, YELLOW);

    if (temp != 0) {

      for (uint8_t i = 0; i < number_of_objects; i++) {

        if (((onscreen_objects_struct[i].row) <= p) && ((onscreen_objects_struct[i].row) + 7 >= p) &&
            ((onscreen_objects_struct[i].column) <= q) && ((onscreen_objects_struct[i].column) + 7 >= q)) {
          onscreen_objects_struct[i].status = false;
          life--;

          // mp3__send_command(C_PLAY_W_VOL, 0x1e01);
          // printf("Friendly kill left %d Enemy Killed %d\n", life, enemy_score);
          // print_score(enemy_score, 0, 32);
        }
      }
    }
  }
}

void collision_detection() {
  uint8_t x, y;
  uint64_t temp, a, b;

  for (uint8_t j = 0; j < 64; j++) {
    // temp = frame_buffer[j][FRIEND_PLANE] & frame_buffer[j][ENEMY_PLANE];
    a = frame_buffer[j][FRIEND_PLANE];
    b = frame_buffer[j][ENEMY_PLANE];

    temp = a & b;
    if (temp) {
      x = j;
      y = set_bit_position(temp);
      // fprintf(stderr, "%d %d\n", x, y);

      // for (uint8_t i = 0; i < number_of_objects; i++) {
      // uint32_t temp1 = (uint32_t)(temp & (0xFFFFFFFF));
      // temp = temp >> 32;
      // uint32_t temp2 = (uint32_t)(temp & (0xFFFFFFFF));
      // fprintf(stderr, "%lu  %lu \n", temp1, temp2);

      for (int i = first_enemy_object; i <= (number_of_objects - 1); i++) {
        if (((onscreen_objects_struct[i].row) <= x) && ((onscreen_objects_struct[i].row) + 7 >= x) &&
            ((onscreen_objects_struct[i].column) <= y) && ((onscreen_objects_struct[i].column) + 7 >= y) &&
            ((onscreen_objects_struct[i].obj_nature) == ENEMY_OBJECT)) {

          if (old_i != i) {
            // fprintf(stderr, "old_i %d i %d", old_i, i);
            life--;
            // update_mp3_details(GUNSHOT, gunshot_duration);
            old_i = i;
          }
        }
      }

      if (((onscreen_objects_struct[0].row) <= x) && ((onscreen_objects_struct[0].row) + 7 >= x) &&
          ((onscreen_objects_struct[0].column) <= y) && ((onscreen_objects_struct[0].column) + 7 >= y) &&
          ((onscreen_objects_struct[0].obj_nature) == FRIEND_OBJECT)) {

        if (life == 0) {
          // life--;
          onscreen_objects_struct[0].status = false;
        }
      }
    }
  }
}

void collision_detection_for_life() {
  uint64_t temp, a, b;

  for (uint8_t j = 0; j < 64; j++) {

    a = frame_buffer[j][FRIEND_PLANE];
    b = frame_buffer[j][LIFE_PLANE];

    temp = a & b;

    if (temp) {
      life++;
      onscreen_objects_struct[1].status = false;
      break;
    }
  }
}

uint8_t set_bit_position(uint64_t temp) {

  uint32_t buffer, a;
  uint8_t column = 0;

  uint32_t temp1 = (uint32_t)(temp & (0xFFFFFFFF));
  temp = temp >> 32;
  uint32_t temp2 = (uint32_t)(temp & (0xFFFFFFFF));
  // fprintf(stderr, "%lu  %lu \n", temp2, temp1);

  for (int8_t i = 31; i >= 0; i--) {
    a = (1 << i);
    buffer = (temp1 & a);

    if (buffer) {
      // fprintf(stderr, "lsb %d\n", (i));
      column = 63 - i;
      return (column);
    }
  }

  for (int8_t i = 31; i >= 0; i--) {
    a = (1 << i);
    buffer = (temp2 & a);

    if (buffer) {
      // fprintf(stderr, "msb %d\n", (i));
      column = 31 - i;
      return (column);
    }
  }
  return (column);
}

void update_friend_location() {

  onscreen_objects_struct[0].row = zigbee_joystick_message[X_coord];
  onscreen_objects_struct[0].column = zigbee_joystick_message[Y_coord];

  // onscreen_objects_struct[0].row = 45;
  // onscreen_objects_struct[0].column = 13;
}