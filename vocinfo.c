/* SPDX-License-Identifier: MIT */
/**
 * @file   vocinfo.c
 * @author Marc-Alexandre Espiaut <ma.dev@espiaut.fr>
 * @date   2026-01-31
 * @version 1.1
 * @brief  Displays information about a list of VOC files.
 *
 * @copyright Copyright (c) 2026 Marc-Alexandre Espiaut
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VOCINFO_VERSION "1.1"

enum
{
  error_fclose = 128,
  error_fopen,
  error_fseek,
  error_ftell,
  error_fwrite,
  error_name_upper
};

typedef struct voc_header_s voc_header_t;
struct voc_header_s
{
  unsigned short size;
  unsigned short version;
  unsigned short checksum;
};

typedef struct block_header_s block_header_t;
struct block_header_s
{
  unsigned char type;
  unsigned int length;
};

typedef struct data_type1_s data_type1_t;
struct data_type1_s
{
  unsigned char frequency_divisor;
  unsigned char codec;
  unsigned char* data;
};

typedef struct data_type2_s data_type2_t;
struct data_type2_s
{
  unsigned char* data;
};

typedef struct data_type3_s data_type3_t;
struct data_type3_s
{
  unsigned short length;
  unsigned char frequency_divisor;
};

typedef struct data_type4_s data_type4_t;
struct data_type4_s
{
  unsigned short value;
};

typedef struct data_type5_s data_type5_t;
struct data_type5_s
{
  char* text;
};

typedef struct data_type6_s data_type6_t;
struct data_type6_s
{
  unsigned short count;
};

typedef struct data_type8_s data_type8_t;
struct data_type8_s
{
  unsigned short frequency_divisor;
  unsigned char codec;
  unsigned char channels_num;
};

typedef struct data_type9_s data_type9_t;
struct data_type9_s
{
  unsigned int rate;
  unsigned char bits;
  unsigned char channels_num;
  unsigned short codec;
  unsigned int reserved;
  unsigned char* data;
};

typedef union block_data_u block_data_t;
union block_data_u
{
  data_type1_t type1;
  data_type2_t type2;
  data_type3_t type3;
  data_type4_t type4;
  data_type5_t type5;
  data_type6_t type6;
  data_type8_t type8;
  data_type9_t type9;
};

typedef struct block_s block_t;
struct block_s
{
  block_header_t header;
  block_data_t data;
};

typedef struct voc_s voc_t;
struct voc_s
{
  voc_header_t header;
  block_t* block;
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
  fprintf(stderr, "%s is a tool for displaying informations about Creative Voice Files (.voc).\n", prgname);
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
  fprintf(stderr, "  %s sound1.voc sound2.voc sound3.voc\n", prgname);
  exit(EXIT_FAILURE);
}

static const char*
version_name_get(int version)
{
  switch (version)
  {
    case 266:
      return "1.10";
      break;
    case 276:
      return "1.20";
      break;
    default:
      return "UNKNOWN";
      break;
  }
}

static const char*
codec_name_get(int codec)
{
  switch (codec)
  {
    case 0x00:
      return "8 bits unsigned PCM";
      break;
    case 0x01:
      return "4 bits to 8 bits Creative ADPCM";
      break;
    case 0x02:
      return "3 bits to 8 bits Creative ADPCM (AKA 2.6 bits)";
      break;
    case 0x03:
      return "2 bits to 8 bits Creative ADPCM";
      break;
    case 0x04:
      return "16 bits signed PCM";
      break;
    case 0x06:
      return "alaw";
      break;
    case 0x07:
      return "ulaw";
      break;
    case 0x0200:
      return "4 bits to 16 bits Creative ADPCM. Only valid in block type 9";
      break;
    default:
      return "UNKNOWN";
      break;
  }
}

static size_t
blocks_count(FILE* fp)
{
  size_t count = 0;
  size_t i = 0;
  block_header_t bh = {0};

  if (!fp)
  {
    fprintf(stderr, "blocks_count(): File pointer is NULL!\n");
    exit(EXIT_FAILURE);
  }

  while ((i = fread(&bh.type, 1, 1, fp) > 0))
  {
    if (fread(&bh.length, 1, 3, fp) <= 0)
    {
      bh.length = 0;
    }

    switch (bh.type)
    {
      case 0:
        fprintf(stdout, "0x%lx: block type 0 (%d bytes): Terminator\n", safe_ftell(fp), bh.length);
        break;
      case 1:
        {
          block_data_t data = {0};
          long cursor_backup = safe_ftell(fp);

          fread(&data, 1, 12, fp);
          safe_fseek(fp, cursor_backup, SEEK_SET);

          fprintf(stdout, "0x%lx: block type 1 (%d bytes): Sound data (sample rate:%d, codec:%s)\n", safe_ftell(fp), bh.length, 1000000 / (256 - data.type1.frequency_divisor), codec_name_get(data.type1.codec));
        }
        break;
      case 2:
        fprintf(stdout, "0x%lx: block type 2 (%d bytes): Sound data without type\n", safe_ftell(fp), bh.length);
        break;
      case 3:
        fprintf(stdout, "0x%lx: block type 3 (%d bytes): Silence\n", safe_ftell(fp), bh.length);
        break;
      case 4:
        fprintf(stdout, "0x%lx: block type 4 (%d bytes): Marker\n", safe_ftell(fp), bh.length);
        break;
      case 5:
        {
          char* buf = NULL;
          long cursor_backup = safe_ftell(fp);

          buf = malloc(bh.length + 1);
          memset(buf, 0, bh.length + 1);

          fread(buf, 1, bh.length, fp);
          safe_fseek(fp, cursor_backup, SEEK_SET);

          fprintf(stdout, "0x%lx: block type 5 (%d bytes): Text: %s\n", safe_ftell(fp), bh.length, buf);

          free(buf);
        }
        break;
      case 6:
        fprintf(stdout, "0x%lx: block type 6 (%d bytes): Repeat start\n", safe_ftell(fp), bh.length);
        break;
      case 7:
        fprintf(stdout, "0x%lx: block type 7 (%d bytes): Repeat end\n", safe_ftell(fp), bh.length);
        break;
      case 8:
        fprintf(stdout, "0x%lx: block type 8 (%d bytes): Extra information\n", safe_ftell(fp), bh.length);
        break;
      case 9:
        {
          block_data_t data = {0};
          long cursor_backup = safe_ftell(fp);

          fread(&data, 1, 12, fp);
          safe_fseek(fp, cursor_backup, SEEK_SET);

          fprintf(stdout, "0x%lx: block type 9 (%d bytes): Sound data (sample rate:%d, bits:%d, channels:%d, codec:%s, reserved:%d)\n", safe_ftell(fp), bh.length, data.type9.rate, data.type9.bits, data.type9.channels_num, codec_name_get(data.type9.codec), data.type9.reserved);
        }
        break;
      default:
        fprintf(stderr, "WARN: Unknown block type %d\n", bh.type);
        break;
    }

    safe_fseek(fp, bh.length, SEEK_CUR);
    ++count;
  }

  return count;
}

static void
voc_info(const char* path)
{
  FILE* fp = NULL;
  voc_t voc = {0};

  fp = safe_fopen(path, "rb");

  {
    char signature[20] = {0};
    fread(&signature, 1, sizeof(signature), fp);

    if (strncmp("Creative Voice File\x1a", signature, sizeof(signature)))
    {
      fprintf(stderr, "%s isn't a Creative Voice FIle!\n", path);
      return;
    }
  }

  {
    fread(&voc.header.size, 1, sizeof(voc.header.size), fp);
    fread(&voc.header.version, 1, sizeof(voc.header.version), fp);
    fread(&voc.header.checksum, 1, sizeof(voc.header.checksum), fp);

    fprintf(stdout, "=== %s (header size:%d, version:%s, checksum:0x%x) ===\n", path, voc.header.size, version_name_get(voc.header.version), voc.header.checksum);
  }

  {
    size_t count = blocks_count(fp);
    fprintf(stdout, "%ld blocks found!\n", count);
  }

  safe_fclose(fp);
}

int
main(int argc, char* argv[])
{
  if (argc < 2)
  {
    usage(argv[0], VOCINFO_VERSION);
  }
  else
  {
    int i;
    for (i = 1; i < argc; ++i)
    {
      voc_info(argv[i]);
    }
  }

  return EXIT_SUCCESS;
}
