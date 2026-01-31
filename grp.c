/* SPDX-License-Identifier: MIT */
/**
 * @file   grp.c
 * @author Marc-Alexandre Espiaut <ma.dev@espiaut.fr>
 * @date   2026-01-30
 * @version 1.1
 * @brief  Creates a GRP file from a list of files.
 *
 * @copyright Copyright (c) 2026 Marc-Alexandre Espiaut
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GRP_VERSION "1.1"

enum
{
  grp_name_len = 12
};

typedef struct
{
  char s[grp_name_len];
} str12;

enum
{
  error_fclose = 128,
  error_fopen,
  error_fseek,
  error_ftell,
  error_fwrite,
  error_name_upper
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

static void
safe_fwrite(const void* ptr, size_t size, size_t n, FILE* stream)
{
  if (!ptr || !stream)
  {
    exit(error_fwrite);
  }

  if (fwrite(ptr, size, n, stream) != n)
  {
    perror("fwrite");
    safe_fclose(stream);
    exit(error_fwrite);
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
abort_if_exists(const char* path)
{
  FILE* fp = fopen(path, "rb");
  if (fp)
  {
    safe_fclose(fp);
    fprintf(stderr, "ERROR: %s already exists! Quitting!\n", path);
    exit(EXIT_FAILURE);
  }
}

static void
name_upper(str12* name)
{
  size_t i;

  if (!name)
  {
    exit(error_name_upper);
  }

  for (i = 0; i < grp_name_len && name->s[i] != '\0'; ++i)
  {
    if (name->s[i] >= 'a' && name->s[i] <= 'z')
    {
      name->s[i] = (char)(name->s[i] - ('a' - 'A'));
    }
  }
}

static void
grp_write(const char* out, const char* in[], size_t n)
{
  size_t i;
  FILE* out_fp = NULL;

  fprintf(stdout, "Checking in %s already exists.\n", out);
  abort_if_exists(out);

  fprintf(stdout, "Creating %s.\n", out);
  out_fp = safe_fopen(out, "wb+");

  safe_fwrite("KenSilverman", 1, grp_name_len, out_fp);
  safe_fwrite(&n, 1, 4, out_fp);

  for (i = 0; i < n; ++i)
  {
    FILE* in_fp = safe_fopen(in[i], "rb");
    fprintf(stdout, "Adding %s to list.\n", in[i]);

    {
      str12 name = {0};

      strncpy(name.s, in[i], grp_name_len);
      name_upper(&name);
      safe_fwrite(name.s, 1, grp_name_len, out_fp);
      fprintf(stdout, "File name %s", name.s);
    }

    {
      long in_size = 0L;
      safe_fseek(in_fp, 0, SEEK_END);
      in_size = safe_ftell(in_fp);
      safe_fseek(in_fp, 0, SEEK_SET);
      safe_fwrite(&in_size, 1, 4, out_fp);
      fprintf(stdout, " of size %ld.\n", in_size);
    }

    safe_fclose(in_fp);
  }

  for (i = 0; i < n; ++i)
  {
    FILE* in_fp = safe_fopen(in[i], "rb");

    fprintf(stdout, "Adding %s\n", in[i]);
    {
      size_t j = 0;
      char buf[8192] = {0};

      while ((j = fread(buf, 1, sizeof(buf), in_fp)) > 0)
      {
        safe_fwrite(buf, 1, j, out_fp);
      }
    }

    safe_fclose(in_fp);
  }

  safe_fclose(out_fp);
  return;
}

static void
usage(const char* prgname, const char* prgver)
{
  fprintf(stderr, "%s %s : Copyright (c) 2026 Marc-Alexandre Espiaut\n", prgname, prgver);
  fprintf(stderr, "\n");
  fprintf(stderr, "%s is a tool for making group (.grp) files for Build engine games.\n", prgname);
  fprintf(stderr, "\n");
  fprintf(stderr, "Usage: %s [options] [files]\n", prgname);
  fprintf(stderr, "\n");
  fprintf(stderr, "Process input files and creates a GRP output file.\n");
  fprintf(stderr, "\n");

  fprintf(stderr, "Options:\n");
  fprintf(stderr, "  -h, --help             Show this help message and exit\n");
  /*
    fprintf(stderr, "  -v, --verbose          Enable verbose output\n");
    fprintf(stderr, "  -g, --group            Group files by extensions\n");
    fprintf(stderr, "  -l, --list LIST.TXT    Add files as listed in LIST.TXT\n");
    fprintf(stderr, "  -o, --output FILE.GRP  Write output to FILE\n");
    fprintf(stderr, "\n");
  */
  fprintf(stderr, "Examples:\n");
  fprintf(stderr, "  %s output.grp input.map input.dmo input.art\n", prgname);
  exit(EXIT_FAILURE);
}

int
main(int argc, char* argv[])
{
  if (argc < 3)
  {
    usage(argv[0], GRP_VERSION);
  }
  else
  {
    grp_write(argv[1], (const char**)(argv + 2), (size_t)(argc - 2));
  }

  return EXIT_SUCCESS;
}
