/* SPDX-License-Identifier: MIT */
/**
 * @file   ungrp.c
 * @author Marc-Alexandre Espiaut <ma.dev@espiaut.fr>
 * @date   2026-11-18
 * @version 0.1
 * @brief  Creates a GRP file from a list of files.
 *
 * @copyright Copyright (c) 2026 Marc-Alexandre Espiaut
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memorystream.h"
#include "types.h"

enum
{
  grp_name_len = 12
};

typedef struct
{
  char s[grp_name_len + 1];
} str12;

typedef struct file_s file_t;
struct file_s
{
  str12 name;
  u32 size;
};

MemoryStream g_grpfile = {0};
u32 g_grp_file_count = 0;
file_t* g_files = NULL;

static void
usage(void)
{
  fprintf(stderr, "usage: ungrp [-l] [DUKE3D.GRP]\n");
  exit(1);
}

static void
grp_read()
{
  u32 i = 0;
  memorystream_read(&g_grpfile, &g_grp_file_count, 4);

  g_files = calloc(g_grp_file_count, sizeof(*g_files));

  for (; i < g_grp_file_count; ++i)
  {
    memorystream_read(&g_grpfile, g_files[i].name.s, 12);
    memorystream_read(&g_grpfile, &g_files[i].size, 4);
  }
}

static void
grp_extract()
{
  u32 i = 0;
  for (; i < g_grp_file_count; ++i)
  {
    FILE* fd = NULL;

    u8* data = malloc(g_files[i].size);
    memorystream_read(&g_grpfile, data, g_files[i].size);

    fd = fopen(g_files[i].name.s, "w");
    fwrite(data, 1, g_files[i].size, fd);
    fclose(fd);
  }
}

int
main(int argc, char* argv[])
{
  if (argc < 2)
  {
    usage();
  }

  memorystream_init(&g_grpfile, argv[1]);

  str12 signature = {0};
  memorystream_read(&g_grpfile, signature.s, 12);
  if (strncmp(signature.s, "KenSilverman", 12) != 0)
  {
    fprintf(stderr, "ERROR: Not a valid Build archive file!\n");
    return EXIT_FAILURE;
  }

  grp_read();

  grp_extract();

  return EXIT_SUCCESS;
}
