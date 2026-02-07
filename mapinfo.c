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
  unsigned short lotag;
  unsigned short hitag;
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
sector_print(sector_t s)
{
  printf("[sector] wall_ptr: %d, wall_count: %d, ceilling (shade: %d, height: %d, pic: %d, slope: %d, stat: %d, palette: %d, panning: (%d, %d)), floor (shade: %d, height: %d, pic: %d, slope: %d, stat: %d, palette: %d, panning: (%d, %d)), filler: %d, lotag: %d, hitag: %d, extra: %d\n", s.wall_ptr, s.wall_count, s.ceilling.shade, s.ceilling.height, s.ceilling.pic, s.ceilling.slope, s.ceilling.stat, s.ceilling.palette, s.ceilling.panning.x, s.ceilling.panning.y, s.floor.shade, s.floor.height, s.floor.pic, s.floor.slope, s.floor.stat, s.floor.palette, s.floor.panning.x, s.floor.panning.y, s.visibility, s.filler, s.lotag, s.hitag, s.extra);
}

static void
sprite_print(sprite_t s)
{
  printf("pic: %d, (%d, %d, %d), %d, stat:%d, shade: %d, pal: %d, clipping_distance: %d, filler: %d, repeat: (%d, %d), offset: (%d, %d), sector: %d, status: %d, owner: %d, vel: (%d, %d, %d), lotag: %d, hitag: %d, extra: %d\n", s.pic, s.position.x, s.position.y, s.position.z, s.angle, s.stat, s.shade, s.pal, s.clipping_distance, s.filler, s.repeat.x, s.repeat.y, s.offset.x, s.offset.y, s.sector, s.status, s.owner, s.vel.x, s.vel.y, s.vel.z, s.lotag, s.hitag, s.extra);
}

static void
wall_print(wall_t w)
{
  printf("[wall] position: (%d, %d), wall_next_right: %d, wall_next_left: %d, sector_next: %d, stat: %d, pic: %d, pic_over: %d, shade: %d, pal: %d, repeat: (%d, %d), panning: (%d, %d), lotag: %d, hitag: %d, extra: %d\n", w.position.x, w.position.y, w.wall_next_right, w.wall_next_left, w.sector_next, w.stat, w.pic, w.pic_over, w.shade, w.pal, w.repeat.x, w.repeat.y, w.panning.x, w.panning.y, w.lotag, w.hitag, w.extra);
}

static char*
is_single_player(const map_t* const map)
{
  size_t i = 0;

  for (; i < map->sprite_count; ++i)
  {
    if (map->sprite[i].pic == 142)
    {
      /*sprite_print(&map->sprite[i]);*/

      if (map->sprite[i].lotag == 32767)
      {
        return "Yes (\?\?)";
      }
      if (map->sprite[i].lotag == 65534)
      {
        return "Yes (\"We're gonna fry your ass, Nukem!\")";
      }
      if (map->sprite[i].lotag == 65535)
      {
        return "Yes (Normal nuke button)";
      }
      if (map->sprite[i].pal == 14) /* NUKEBUTTON */
      {
        return "Yes (Secret level exit)";
      }
    }
  }
  i = 0;
  for (; i < map->sector_count; ++i)
  {
    sector_print(map->sector[i]);
  }
  return "No";
}

static char*
is_dukematch(const map_t* const map, char* buffer)
{
  int result = 0;

  {
    size_t i = 0;
    for (; i < map->sprite_count; ++i)
    {
      if ((map->sprite[i].pic == 1405) /* APLAYER */ && (map->sprite[i].lotag == 0))
      {
        ++result;
      }
    }
  }

  if (result)
  {
    snprintf(buffer, 256, "Yes (%d players)", result + 1);
    return buffer;
  }
  else
  {
    return "No";
  }
}

static char*
is_coop(const map_t* const map, char* buffer)
{
  int result = 0;

  {
    size_t i = 0;
    for (; i < map->sprite_count; ++i)
    {
      if ((map->sprite[i].pic == 1405) /* APLAYER */ && (map->sprite[i].lotag == 1))
      {
        ++result;
      }
    }
  }

  if (result)
  {
    snprintf(buffer, 256, "Yes (%d players)", result + 1);
    return buffer;
  }
  else
  {
    return "No";
  }
}

static const char*
is_vanilla_compatible(const map_t* const map)
{
  if ((map->sector_count <= 1024) && (map->wall_count <= 8192) && (map->sprite_count <= 4096))
  {
    return "Yes";
  }
  else
  {
    return "No";
  }
}

static void
map_print(map_t* map, const char* path)
{
  char buffer[256] = {'\0'};
  printf("Filename: %s\n", path);
  printf("MAP version: %d\n", map->version);
  printf("Single Player: %s\n", is_single_player(map)); /* Yes/No */
  printf("Cooperative 2-8 Player: %s\n", is_coop(map, buffer)); /* Yes (x players) */
  printf("DukeMatch 2-8 Player: %s\n", is_dukematch(map, buffer)); /* Yes (x players) */
  printf("Atomic Edition Required: \n"); /* Yes/No */
  printf("New Art: \n"); /* Yes/No */
  printf("Vanilla DUKE3D.EXE compatible: %s (%d sectors, %d walls, %d sprites)\n\n", is_vanilla_compatible(map), map->sector_count, map->wall_count, map->sprite_count); /* Yes (x sectors, x walls, x sprites)*/
}

static void
map_free(const map_t* map)
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
