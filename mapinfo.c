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

#include "errorcodes.h"
#include "memorystream.h"
#include "types.h"

#define MAPINFO_VERSION "1.0"

typedef struct player_s player_t;
struct player_s
{
  vec3_i32_t position;
  i16 angle;
};

typedef struct ceilling_floor_s ceilling_floor_t;
struct ceilling_floor_s
{
  i8 shade;
  i32 height;
  i16 pic;
  i16 slope;
  i16 stat;
  u8 palette;
  vec2_u8_t panning;
};

typedef struct sector_s sector_t;
struct sector_s
{
  i16 wall_ptr;
  i16 wall_count;
  ceilling_floor_t ceilling;
  ceilling_floor_t floor;
  u8 visibility;
  u8 filler;
  u16 lotag;
  i16 hitag;
  i16 extra;
};

typedef struct wall_s wall_t;
struct wall_s
{
  vec2_i32_t position;
  i16 wall_next_right;
  i16 wall_next_left;
  i16 sector_next;
  i16 stat;
  i16 pic;
  i16 pic_over;
  signed char shade;
  u8 pal;
  vec2_u8_t repeat;
  vec2_u8_t panning;
  i16 lotag;
  i16 hitag;
  i16 extra;
};

typedef struct sprite_s sprite_t;
struct sprite_s
{
  vec3_i32_t position;
  i16 stat;
  i16 pic;
  signed char shade;
  u8 pal;
  u8 clipping_distance;
  u8 filler;
  vec2_u8_t repeat;
  vec2_i8_t offset;
  i16 sector;
  i16 status;
  i16 angle;
  i16 owner;
  vec3_i16_t vel;
  u16 lotag;
  u16 hitag;
  i16 extra;
};

typedef struct map_s map_t;
struct map_s
{
  i32 version;
  player_t player;
  i16 sector_start;
  u16 sector_count;
  sector_t* sector;
  u16 wall_count;
  wall_t* wall;
  u16 sprite_count;
  sprite_t* sprite;
};

static void
usage(char* prgname, char* prgver)
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
sector_parse(MemoryStream* ms, sector_t* sector)
{
  memorystream_read(ms, &sector->wall_ptr, sizeof(sector->wall_ptr));
  memorystream_read(ms, &sector->wall_count, sizeof(sector->wall_count));

  memorystream_read(ms, &sector->ceilling.height, sizeof(sector->ceilling.height));
  memorystream_read(ms, &sector->floor.height, sizeof(sector->floor.height));

  memorystream_read(ms, &sector->ceilling.stat, sizeof(sector->ceilling.stat));
  memorystream_read(ms, &sector->floor.stat, sizeof(sector->floor.stat));

  memorystream_read(ms, &sector->ceilling.pic, sizeof(sector->ceilling.pic));
  memorystream_read(ms, &sector->ceilling.slope, sizeof(sector->ceilling.slope));
  memorystream_read(ms, &sector->ceilling.shade, sizeof(sector->ceilling.shade));
  memorystream_read(ms, &sector->ceilling.palette, sizeof(sector->ceilling.palette));
  memorystream_read(ms, &sector->ceilling.panning.x, sizeof(sector->ceilling.panning.x));
  memorystream_read(ms, &sector->ceilling.panning.y, sizeof(sector->ceilling.panning.y));

  memorystream_read(ms, &sector->floor.pic, sizeof(sector->floor.pic));
  memorystream_read(ms, &sector->floor.slope, sizeof(sector->floor.slope));
  memorystream_read(ms, &sector->floor.shade, sizeof(sector->floor.shade));
  memorystream_read(ms, &sector->floor.palette, sizeof(sector->floor.palette));
  memorystream_read(ms, &sector->floor.panning.x, sizeof(sector->floor.panning.x));
  memorystream_read(ms, &sector->floor.panning.y, sizeof(sector->floor.panning.y));

  memorystream_read(ms, &sector->visibility, sizeof(sector->visibility));

  memorystream_read(ms, &sector->filler, sizeof(sector->filler));

  memorystream_read(ms, &sector->lotag, sizeof(sector->lotag));
  memorystream_read(ms, &sector->hitag, sizeof(sector->hitag));
  memorystream_read(ms, &sector->extra, sizeof(sector->extra));
}

static void
wall_parse(MemoryStream* ms, wall_t* wall)
{
  memorystream_read(ms, &wall->position.x, sizeof(wall->position.x));
  memorystream_read(ms, &wall->position.y, sizeof(wall->position.y));

  memorystream_read(ms, &wall->wall_next_right, sizeof(wall->wall_next_right));
  memorystream_read(ms, &wall->wall_next_left, sizeof(wall->wall_next_left));

  memorystream_read(ms, &wall->sector_next, sizeof(wall->sector_next));

  memorystream_read(ms, &wall->stat, sizeof(wall->stat));

  memorystream_read(ms, &wall->pic, sizeof(wall->pic));
  memorystream_read(ms, &wall->pic_over, sizeof(wall->pic_over));

  memorystream_read(ms, &wall->shade, sizeof(wall->shade));
  memorystream_read(ms, &wall->pal, sizeof(wall->pal));

  memorystream_read(ms, &wall->repeat.x, sizeof(wall->repeat.x));
  memorystream_read(ms, &wall->repeat.y, sizeof(wall->repeat.y));

  memorystream_read(ms, &wall->panning.x, sizeof(wall->panning.x));
  memorystream_read(ms, &wall->panning.y, sizeof(wall->panning.y));

  memorystream_read(ms, &wall->lotag, sizeof(wall->lotag));
  memorystream_read(ms, &wall->hitag, sizeof(wall->hitag));
  memorystream_read(ms, &wall->extra, sizeof(wall->extra));
}

static void
sprite_parse(MemoryStream* ms, sprite_t* sprite)
{
  memorystream_read(ms, &sprite->position.x, sizeof(sprite->position.x));
  memorystream_read(ms, &sprite->position.y, sizeof(sprite->position.y));
  memorystream_read(ms, &sprite->position.z, sizeof(sprite->position.z));

  memorystream_read(ms, &sprite->stat, sizeof(sprite->stat));

  memorystream_read(ms, &sprite->pic, sizeof(sprite->pic));
  memorystream_read(ms, &sprite->shade, sizeof(sprite->shade));
  memorystream_read(ms, &sprite->pal, sizeof(sprite->pal));

  memorystream_read(ms, &sprite->clipping_distance, sizeof(sprite->clipping_distance));

  memorystream_read(ms, &sprite->filler, sizeof(sprite->filler));

  memorystream_read(ms, &sprite->repeat.x, sizeof(sprite->repeat.x));
  memorystream_read(ms, &sprite->repeat.y, sizeof(sprite->repeat.y));

  memorystream_read(ms, &sprite->offset.x, sizeof(sprite->offset.x));
  memorystream_read(ms, &sprite->offset.y, sizeof(sprite->offset.y));

  memorystream_read(ms, &sprite->sector, sizeof(sprite->sector));
  memorystream_read(ms, &sprite->status, sizeof(sprite->status));

  memorystream_read(ms, &sprite->angle, sizeof(sprite->angle));

  memorystream_read(ms, &sprite->owner, sizeof(sprite->owner));

  memorystream_read(ms, &sprite->vel.x, sizeof(sprite->vel.x));
  memorystream_read(ms, &sprite->vel.y, sizeof(sprite->vel.y));
  memorystream_read(ms, &sprite->vel.z, sizeof(sprite->vel.z));

  memorystream_read(ms, &sprite->lotag, sizeof(sprite->lotag));
  memorystream_read(ms, &sprite->hitag, sizeof(sprite->hitag));
  memorystream_read(ms, &sprite->extra, sizeof(sprite->extra));
}

static void
map_parse(MemoryStream* ms, map_t* map)
{
  memorystream_read(ms, &map->version, sizeof(map->version));

  memorystream_read(ms, &map->player.position.x, sizeof(map->player.position.x));
  memorystream_read(ms, &map->player.position.y, sizeof(map->player.position.y));
  memorystream_read(ms, &map->player.position.z, sizeof(map->player.position.z));
  memorystream_read(ms, &map->player.angle, sizeof(map->player.angle));

  memorystream_read(ms, &map->sector_start, sizeof(map->sector_start));

  memorystream_read(ms, &map->sector_count, sizeof(map->sector_count));

  map->sector = malloc(sizeof(*map->sector) * map->sector_count);
  if (map->sector)
  {
    size_t i = 0;
    for (; i < map->sector_count; ++i)
    {
      sector_parse(ms, &map->sector[i]);
    }
  }
  else
  {
    perror("malloc");
    exit(e_error_malloc);
  }

  memorystream_read(ms, &map->wall_count, sizeof(map->wall_count));
  map->wall = malloc(sizeof(*map->wall) * map->wall_count);
  if (map->wall)
  {
    size_t i = 0;
    for (; i < map->wall_count; ++i)
    {
      wall_parse(ms, &map->wall[i]);
    }
  }
  else
  {
    perror("malloc");
    exit(e_error_malloc);
  }

  memorystream_read(ms, &map->sprite_count, sizeof(map->sprite_count));
  map->sprite = malloc(sizeof(*map->sprite) * map->sprite_count);
  if (map->sprite)
  {
    size_t i = 0;
    for (; i < map->sprite_count; ++i)
    {
      sprite_parse(ms, &map->sprite[i]);
    }
  }
  else
  {
    perror("malloc");
    exit(e_error_malloc);
  }
}

/*
static void
sector_print(sector_t s)
{
  printf("[sector] wall_ptr: %d, wall_count: %d, ceilling (shade: %d, height: %d, pic: %d, slope: %d, stat: %d, palette: %d, panning: (%d, %d)), floor (shade: %d, height: %d, pic: %d, slope: %d, stat: %d, palette: %d, panning: (%d, %d)), visibility: %d, filler: %d, lotag: %d, hitag: %d, extra: %d\n", s.wall_ptr, s.wall_count, s.ceilling.shade, s.ceilling.height, s.ceilling.pic, s.ceilling.slope, s.ceilling.stat, s.ceilling.palette, s.ceilling.panning.x, s.ceilling.panning.y, s.floor.shade, s.floor.height, s.floor.pic, s.floor.slope, s.floor.stat, s.floor.palette, s.floor.panning.x, s.floor.panning.y, s.visibility, s.filler, s.lotag, s.hitag, s.extra);
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
*/

static char*
is_single_player(map_t* map)
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

  for (i = 0; i < map->sector_count; ++i)
  {
    if (map->sector[i].lotag == 32767)
    {
      return "Yes (\?\?)";
    }
    if (map->sector[i].lotag == 65534)
    {
      return "Yes (\"We're gonna fry your ass, Nukem!\")";
    }
    if (map->sector[i].lotag == 65535)
    {
      return "Yes (Normal nuke button)";
    }
  }

  /*
  i = 0;
  for (; i < map->sector_count; ++i)
  {
    sector_print(map->sector[i]);
  }
  */
  return "No";
}

static char*
is_dukematch(map_t* map, char* buffer)
{
  i32 result = 0;

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
is_coop(map_t* map, char* buffer)
{
  i32 result = 0;

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

static char*
is_vanilla_compatible(map_t* map)
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
map_print(map_t* map, char* path)
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
    i32 i;
    MemoryStream map_file = {0};

    for (i = 1; i < argc; ++i)
    {
      map_t map = {0};
      memorystream_init(&map_file, argv[i]);
      map_parse(&map_file, &map);
      memorystream_free(&map_file);
      map_print(&map, argv[i]);
      map_free(&map);
    }
  }

  return EXIT_SUCCESS;
}
