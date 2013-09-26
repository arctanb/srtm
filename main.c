#include <math.h>
#include <stdint.h>
#include <stdio.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define ABS(a)     (((a) < 0) ? -(a) : (a))

#define WAYPOINT_GRANULARITY 2

/******************
 * SRTM functions *
 ******************/

static inline uint16_t read_nth_number(FILE *file, uint32_t n) {
  uint8_t read_data[2];
  fseek(file, n * 2, SEEK_SET);
  fread(&read_data[0], 2, 1, file);
  return read_data[1] | (read_data[0] << 8);
}

/**
 * east and north are in arcseconds, and relative to the bottom left corner
 * assuming SRTM data is from outside the US, i.e. 3 arcsecond resolution
 */
static inline uint16_t read_rel_lat_long(FILE *file, uint32_t rel_east,
    uint32_t rel_north) {
  uint32_t i = 1201 - rel_north / 3;
  uint32_t j = rel_east / 3;
  return read_nth_number(file, (i - 1) * 1201 + (j - 1));
}

static inline int32_t next_degree_down(int32_t arcsecond) {
  return (arcsecond + 180*60*60) / (60*60) - 180;
}

static inline void get_file_params(int32_t east, int32_t north, char *fname,
    uint32_t *rel_east, uint32_t *rel_north) {
  int32_t east_deg = next_degree_down(east);
  int32_t north_deg = next_degree_down(north);
  *rel_east = east - east_deg * 60 * 60;
  *rel_north = north - north_deg * 60 * 60;
  sprintf(fname, "data/%c%02d%c%03d.hgt",
      north_deg >= 0 ? 'N' : 'S',
      ABS(north_deg),
      east_deg >= 0 ? 'E' : 'W',
      ABS(east_deg));
}

static inline uint16_t read_lat_long(int32_t east, int32_t north) {
  char fname[20];
  uint32_t rel_east, rel_north;
  get_file_params(east, north, fname, &rel_east, &rel_north);
  FILE *file = fopen(fname, "r");
  if (!file) {
    fprintf(stderr, "could not open file %s\n", fname);
  }
  uint16_t ret = read_rel_lat_long(file, rel_east, rel_north);
  fclose(file);
  return ret;
}

/************
 * Waypoint *
 ************/

void straight_line(int32_t s_east, int32_t s_north, int32_t e_east,
    int32_t e_north) {
  double delta_east = e_east - s_east;
  double delta_north = e_north - s_north;
  double delta = sqrt(delta_east * delta_east + delta_north + delta_north);
  uint32_t num_steps = delta / WAYPOINT_GRANULARITY;
  for (uint32_t i = 0; i < num_steps; ++i) {
    int32_t east = s_east + (delta_east * i / num_steps);
    int32_t north = s_north + (delta_north * i / num_steps);
    uint16_t result = read_lat_long(east, north);
    printf("%d\n", result);
  }
}

void read_waypoint(FILE *file) {
  uint32_t num_points;
  fscanf(file, "%d\n", &num_points);
  int32_t prev_east, prev_north, cur_east, cur_north;
  for (int i = 0; i < num_points; ++i) {
    char cs[2];
    uint32_t ns[6];
    if (fscanf(file, "%c %d %d %d %c %d %d %d\n", &cs[0], &ns[0], &ns[1],
        &ns[2], &cs[1], &ns[3], &ns[4], &ns[5]) <= 0) {
      fprintf(stderr, "file cannot be read\n");
    }
    prev_east = cur_east;
    prev_north = cur_north;
    cur_north =  ns[0] * 60*60 + ns[1] * 60 + ns[2];
    cur_east = ns[3] * 60*60 + ns[4] * 60 + ns[5];
    if (cs[0] == 'S') {
      cur_north = -cur_north;
    }
    if (cs[1] == 'W') {
      cur_east = -cur_east;
    }
    if (i > 0) {
      straight_line(prev_east, prev_north, cur_east, cur_north);
    }
  }
}

/***********************
 * Program Entry Point *
 ***********************/

int main(int argc, const char *argv[]) {
  //straight_line(-99*60*60+1020, 19*60*60+1450, -99*60*60+1080, 19*60*60+1550);
  //printf("\n");
  FILE *file = fopen("waypoints", "r");
  read_waypoint(file);
  fclose(file);
}

