/* SPDX-License-Identifier: MIT */
/**
 * @file   mapinfo.c
 * @author Marc-Alexandre Espiaut <ma.dev@espiaut.fr>
 * @date   2026-02-01
 * @version 1.0
 * @brief  Displays information about a list of MAP files.
 *
 * @copyright Copyright (c) 2026 Marc-Alexandre Espiaut
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAPINFO_VERSION "1.0"

enum
{
  error_fclose = 2,
  error_fopen,
  error_fseek,
  error_ftell,
  error_fwrite,
  error_name_upper
};

typedef struct vec2_i32_s vec2_i32_t;
struct vec2_i32_s
{
  int x;
  int y;
};

typedef struct vec2_i8_s vec2_i8_t;
struct vec2_i8_s
{
  signed char x;
  signed char y;
};

typedef struct vec2_u8_s vec2_u8_t;
struct vec2_u8_s
{
  unsigned char x;
  unsigned char y;
};

typedef struct vec3_i16_s vec3_i16_t;
struct vec3_i16_s
{
  short x;
  short y;
  short z;
};

typedef struct vec3_i32_s vec3_i32_t;
struct vec3_i32_s
{
  int x;
  int y;
  int z;
};

typedef struct player_s player_t;
struct player_s
{
  vec3_i32_t position;
  short angle;
};

typedef struct ceilling_floor_s ceilling_floor_t;
struct ceilling_floor_s
{
  signed char shade;
  int height;
  short pic;
  short slope;
  short stat;
  unsigned char palette;
  vec2_u8_t panning;
};

typedef struct sector_s sector_t;
struct sector_s
{
  short wall_ptr;
  short wall_count;
  ceilling_floor_t ceilling;
  ceilling_floor_t floor;
  unsigned char visibility;
  unsigned char filler;
  short lotag;
  short hitag;
  short extra;
};

typedef struct wall_s wall_t;
struct wall_s
{
  vec2_i32_t position;
  short wall_next_right;
  short wall_next_left;
  short sector_next;
  short stat;
  short pic;
  short pic_over;
  signed char shade;
  unsigned char pal;
  vec2_u8_t repeat;
  vec2_u8_t panning;
  short lotag;
  short hitag;
  short extra;
};

typedef struct sprite_s sprite_t;
struct sprite_s
{
  vec3_i32_t position;
  short stat;
  short pic;
  signed char shade;
  unsigned char pal;
  unsigned char clipping_distance;
  unsigned char filler;
  vec2_u8_t repeat;
  vec2_i8_t offset;
  short sector;
  short status;
  short angle;
  short owner;
  vec3_i16_t vel;
  short lotag;
  short hitag;
  short extra;
};

typedef struct map_s map_t;
struct map_s
{
  int version;
  player_t player;
  short sector_start;
  unsigned short sector_count;
  sector_t* sector;
  unsigned short wall_count;
  wall_t* wall;
  unsigned short sprite_count;
  sprite_t* sprite;
};

static long
safe_ftell(FILE* stream)
{
  long result = 0;

  if (!stream)
  {
    exit(error_ftell);
  }

  result = ftell(stream);
  if (result == -1)
  {
    perror("ftell");
    exit(error_ftell);
  }
  else
  {
    return result;
  }
}

static int
safe_fseek(FILE* stream, long offset, int whence)
{
  int result = 0;

  if (!stream)
  {
    exit(error_fseek);
  }

  result = fseek(stream, offset, whence);
  if (result == -1)
  {
    perror("fseek");
    exit(error_fseek);
  }
  else
  {
    return result;
  }
}

static void
safe_fclose(FILE* stream)
{
  if (!stream)
  {
    exit(error_fclose);
  }

  if (fclose(stream) == EOF)
  {
    perror("fclose");
    exit(error_fclose);
  }
}

static FILE*
safe_fopen(const char* path, const char* mode)
{
  FILE* fp = NULL;

  if (!path || !mode)
  {
    exit(error_fopen);
  }

  fp = fopen(path, mode);
  if (!fp)
  {
    perror("fopen");
    exit(error_fopen);
  }
  return fp;
}

static void
usage(const char* prgname, const char* prgver)
{
  fprintf(stderr, "%s %s : Copyright (c) 2026 Marc-Alexandre Espiaut\n", prgname, prgver);
  fprintf(stderr, "\n");
  fprintf(stderr, "%s is a tool for displaying informations about Build games maps (.map).\n", prgname);
  fprintf(stderr, "\n");
  fprintf(stderr, "Usage: %s [files]\n", prgname);
  fprintf(stderr, "\n");
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "  -h, --help             Show this help message and exit\n");
  /*
    fprintf(stderr, "  -v, --verbose          Enable verbose output\n");
    fprintf(stderr, "  -l, --list LIST.TXT    Add files as listed in LIST.TXT\n");
  */
  fprintf(stderr, "\n");
  fprintf(stderr, "Examples:\n");
  fprintf(stderr, "  %s e1l1.map myhouse.map\n", prgname);
  exit(EXIT_FAILURE);
}

static void
sector_read(sector_t* sector, FILE* fp)
{
  fread(&sector->wall_ptr, 1, sizeof(sector->wall_ptr), fp);
  fread(&sector->wall_count, 1, sizeof(sector->wall_count), fp);

  fread(&sector->ceilling.height, 1, sizeof(sector->ceilling.height), fp);
  fread(&sector->floor.height, 1, sizeof(sector->floor.height), fp);

  fread(&sector->ceilling.stat, 1, sizeof(sector->ceilling.stat), fp);
  fread(&sector->floor.stat, 1, sizeof(sector->floor.stat), fp);

  fread(&sector->ceilling.pic, 1, sizeof(sector->ceilling.pic), fp);
  fread(&sector->ceilling.slope, 1, sizeof(sector->ceilling.slope), fp);
  fread(&sector->ceilling.shade, 1, sizeof(sector->ceilling.shade), fp);
  fread(&sector->ceilling.palette, 1, sizeof(sector->ceilling.palette), fp);
  fread(&sector->ceilling.panning.x, 1, sizeof(sector->ceilling.panning.x), fp);
  fread(&sector->ceilling.panning.y, 1, sizeof(sector->ceilling.panning.y), fp);

  fread(&sector->floor.pic, 1, sizeof(sector->floor.pic), fp);
  fread(&sector->floor.slope, 1, sizeof(sector->floor.slope), fp);
  fread(&sector->floor.shade, 1, sizeof(sector->floor.shade), fp);
  fread(&sector->floor.palette, 1, sizeof(sector->floor.palette), fp);
  fread(&sector->floor.panning.x, 1, sizeof(sector->floor.panning.x), fp);
  fread(&sector->floor.panning.y, 1, sizeof(sector->floor.panning.y), fp);

  fread(&sector->visibility, 1, sizeof(sector->visibility), fp);

  fread(&sector->filler, 1, sizeof(sector->filler), fp);

  fread(&sector->lotag, 1, sizeof(sector->lotag), fp);
  fread(&sector->hitag, 1, sizeof(sector->hitag), fp);
  fread(&sector->extra, 1, sizeof(sector->extra), fp);
}

static void
wall_read(wall_t* wall, FILE* fp)
{
  fread(&wall->position.x, 1, sizeof(wall->position.x), fp);
  fread(&wall->position.y, 1, sizeof(wall->position.y), fp);

  fread(&wall->wall_next_right, 1, sizeof(wall->wall_next_right), fp);
  fread(&wall->wall_next_left, 1, sizeof(wall->wall_next_left), fp);

  fread(&wall->sector_next, 1, sizeof(wall->sector_next), fp);

  fread(&wall->stat, 1, sizeof(wall->stat), fp);

  fread(&wall->pic, 1, sizeof(wall->pic), fp);
  fread(&wall->pic_over, 1, sizeof(wall->pic_over), fp);

  fread(&wall->shade, 1, sizeof(wall->shade), fp);
  fread(&wall->pal, 1, sizeof(wall->pal), fp);

  fread(&wall->repeat.x, 1, sizeof(wall->repeat.x), fp);
  fread(&wall->repeat.y, 1, sizeof(wall->repeat.y), fp);

  fread(&wall->panning.x, 1, sizeof(wall->panning.x), fp);
  fread(&wall->panning.y, 1, sizeof(wall->panning.y), fp);

  fread(&wall->lotag, 1, sizeof(wall->lotag), fp);
  fread(&wall->hitag, 1, sizeof(wall->hitag), fp);
  fread(&wall->extra, 1, sizeof(wall->extra), fp);
}

static void
sprite_read(sprite_t* sprite, FILE* fp)
{
  fread(&sprite->position.x, 1, sizeof(sprite->position.x), fp);
  fread(&sprite->position.y, 1, sizeof(sprite->position.y), fp);
  fread(&sprite->position.z, 1, sizeof(sprite->position.z), fp);

  fread(&sprite->stat, 1, sizeof(sprite->stat), fp);

  fread(&sprite->pic, 1, sizeof(sprite->pic), fp);
  fread(&sprite->shade, 1, sizeof(sprite->shade), fp);
  fread(&sprite->pal, 1, sizeof(sprite->pal), fp);

  fread(&sprite->clipping_distance, 1, sizeof(sprite->clipping_distance), fp);

  fread(&sprite->filler, 1, sizeof(sprite->filler), fp);

  fread(&sprite->repeat.x, 1, sizeof(sprite->repeat.x), fp);
  fread(&sprite->repeat.y, 1, sizeof(sprite->repeat.y), fp);

  fread(&sprite->offset.x, 1, sizeof(sprite->offset.x), fp);
  fread(&sprite->offset.y, 1, sizeof(sprite->offset.y), fp);

  fread(&sprite->sector, 1, sizeof(sprite->sector), fp);
  fread(&sprite->status, 1, sizeof(sprite->status), fp);

  fread(&sprite->angle, 1, sizeof(sprite->angle), fp);

  fread(&sprite->owner, 1, sizeof(sprite->owner), fp);

  fread(&sprite->vel.x, 1, sizeof(sprite->vel.x), fp);
  fread(&sprite->vel.y, 1, sizeof(sprite->vel.y), fp);
  fread(&sprite->vel.z, 1, sizeof(sprite->vel.z), fp);

  fread(&sprite->lotag, 1, sizeof(sprite->lotag), fp);
  fread(&sprite->hitag, 1, sizeof(sprite->hitag), fp);
  fread(&sprite->extra, 1, sizeof(sprite->extra), fp);
}

static void
map_read(map_t* map, const char* path)
{
  FILE* fp = NULL;

  fp = safe_fopen(path, "rb");

  fread(&map->version, 1, sizeof(map->version), fp);

  fread(&map->player.position.x, 1, sizeof(map->player.position.x), fp);
  fread(&map->player.position.y, 1, sizeof(map->player.position.y), fp);
  fread(&map->player.position.z, 1, sizeof(map->player.position.z), fp);
  fread(&map->player.angle, 1, sizeof(map->player.angle), fp);

  fread(&map->sector_start, 1, sizeof(map->sector_start), fp);

  fread(&map->sector_count, 1, sizeof(map->sector_count), fp);

  map->sector = malloc(sizeof(*map->sector) * map->sector_count);
  if (map->sector)
  {
    size_t i = 0;
    for (; i < map->sector_count; ++i)
    {
      sector_read(&map->sector[i], fp);
    }
  }

  fread(&map->wall_count, 1, sizeof(map->wall_count), fp);
  map->wall = malloc(sizeof(*map->wall) * map->wall_count);
  if (map->wall)
  {
    size_t i = 0;
    for (; i < map->wall_count; ++i)
    {
      wall_read(&map->wall[i], fp);
    }
  }

  fread(&map->sprite_count, 1, sizeof(map->sprite_count), fp);
  map->sprite = malloc(sizeof(*map->sprite) * map->sprite_count);
  if (map->sprite)
  {
    size_t i = 0;
    for (; i < map->sprite_count; ++i)
    {
      sprite_read(&map->sprite[i], fp);
    }
  }

  safe_fclose(fp);
}

static void
map_print(map_t* map, const char* path)
{
  printf("%s is a MAP format %d, with player start position (%d, %d, %d) and angle %d in sector %d. The map has %d sectors, %d walls, and %d sprites.\n", path, map->version, map->player.position.x, map->player.position.y, map->player.position.z, map->player.angle, map->sector_start, map->sector_count, map->wall_count, map->sprite_count);
}

static void
map_free(map_t* map)
{
  free(map->sprite);
  free(map->wall);
  free(map->sector);
}

int
main(int argc, char* argv[])
{
  if (argc < 2)
  {
    usage(argv[0], MAPINFO_VERSION);
  }
  else
  {
    int i;
    for (i = 1; i < argc; ++i)
    {
      map_t map = {0};
      map_read(&map, argv[i]);
      map_print(&map, argv[i]);
      map_free(&map);
    }
  }

  return EXIT_SUCCESS;
}
