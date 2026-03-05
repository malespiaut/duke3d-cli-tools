/* SPDX-License-Identifier: MIT */
/**
 * @file   memorystream.h
 * @author Marc-Alexandre Espiaut <ma.dev@espiaut.fr>
 * @date   2026-03-05
 * @brief  Loadind and manipulating files in memory.
 *
 * @copyright Copyright (c) 2026 Marc-Alexandre Espiaut
 */

#ifndef MEMORYSTREAM_H
#define MEMORYSTREAM_H

#include <string.h>

#include "errorcodes.h"
#include "types.h"

enum
{
  e_ms_seek_set,
  e_ms_seek_cur,
  e_ms_seek_end
};

typedef struct MemoryStream MemoryStream;
struct MemoryStream
{
  u8* data;
  i64 size;
  i64 position;
};

static void
memorystream_init(MemoryStream* ms, char* path)
{
  FILE* fp = fopen(path, "rb");

  if (!fp)
  {
    perror("fopen");
    exit(e_error_fopen);
  }

  if (fseek(fp, 0, SEEK_END) == -1)
  {
    perror("fseek");
    exit(e_error_fseek);
  }

  ms->size = ftell(fp);

  if (fseek(fp, 0, SEEK_SET) == -1)
  {
    perror("fseek");
    exit(e_error_fseek);
  }

  ms->data = malloc((size_t)ms->size);
  if (!ms->data)
  {
    perror("malloc");
    exit(e_error_malloc);
  }

  if ((i64)fread(ms->data, 1, (size_t)ms->size, fp) != ms->size)
  {
    perror("fread");
    exit(e_error_fread);
  }

  if (fclose(fp) == EOF)
  {
    perror("fclose");
    exit(e_error_fclose);
  }

  ms->position = 0;
}

static i64
memorystream_read(MemoryStream* ms, void* dst, i64 n)
{
  i64 remaining = ms->size - ms->position;

  if (n > remaining)
  {
    n = remaining;
  }

  memcpy(dst, ms->data + ms->position, (size_t)n);

  ms->position += n;

  return n;
}

static void
memorystream_free(MemoryStream* ms)
{
  free(ms->data);
  ms->size = 0;
  ms->position = 0;
}

#endif /* MEMORYSTREAM_H */
