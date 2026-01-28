#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
  char s[12];
} str12;

static void
name_upper(str12* name)
{
  size_t i = 0;

  for (; i < 12; ++i)
  {
    if (name->s[i] >= 'a' && name->s[i] <= 'z')
    {
      name->s[i] -= 32;
    }
  }
}

static void
grp_write(const char* out, const char* in[], size_t n)
{
  size_t i = 0;
  FILE* out_fp = NULL;

  fprintf(stdout, "Checking in %s already exists.\n", out);
  out_fp = fopen(out, "r");
  if (out_fp)
  {
    fclose(out_fp);
    fprintf(stderr, "ERROR: %s already exists! Quitting!\n", out);
    exit(EXIT_FAILURE);
  }

  fprintf(stdout, "Creating %s.\n", out);
  out_fp = fopen(out, "w+");

  fwrite("KenSilverman", 1, 12, out_fp);
  fwrite(&n, 1, 4, out_fp);

  for (i = 0; i < n; ++i)
  {
    FILE* in_fp = fopen(in[i], "r");
    if (!in_fp)
    {
      fprintf(stderr, "ERROR: Couldn't open %s! Quitting!\n", out);
      fclose(out_fp);
      exit(EXIT_FAILURE);
    }
    fprintf(stdout, "Adding %s to list.\n", in[i]);

    {
      str12 name = {0};

      strncpy(name.s, in[i], 12);
      name_upper(&name);
      fwrite(name.s, 1, 12, out_fp);
      fprintf(stdout, "File name %s", name.s);
    }

    {
      long in_size = 0L;
      fseek(in_fp, 0, SEEK_END);
      in_size = ftell(in_fp);
      fseek(in_fp, 0, SEEK_SET);
      fwrite(&in_size, 1, 4, out_fp);
      fprintf(stdout, " of size %ld.\n", in_size);
    }
    fclose(in_fp);
  }

  for (i = 0; i < n; ++i)
  {
    FILE* in_fp = fopen(in[i], "r");
    if (!in_fp)
    {
      fprintf(stderr, "ERROR: Couldn't open %s! Quitting!\n", out);
      fclose(out_fp);
      exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Adding %s\n", in[i]);
    {
      size_t j = 0;
      char buf[8192] = {0};

      while ((j = fread(buf, 1, sizeof(buf), in_fp)) > 0)
      {
        if (fwrite(buf, 1, j, out_fp) != j)
        {
          fprintf(stderr, "ERROR: Couldn't write to %s! Quitting!\n", out);
          fclose(in_fp);
          fclose(out_fp);
          exit(EXIT_FAILURE);
        }
      }
    }
    fclose(in_fp);
  }

  fclose(out_fp);
  return;
}

static void
usage(const char* prgname)
{
  fprintf(stderr, "Usage: %s [output file] [input files]\n", prgname);
  fprintf(stderr, "\n");
  fprintf(stderr, "Process input files and creates a GRP output file.\n");
  fprintf(stderr, "\n");

  fprintf(stderr, "Options:\n");
  fprintf(stderr, "  -h, --help       Show this help message and exit\n");
  /*
    fprintf(stderr, "  -v, --verbose    Enable verbose output\n");
    fprintf(stderr, "  -n NUM           Process NUM items (default: 10)\n");
    fprintf(stderr, "  -o FILE          Write output to FILE\n");
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
    usage(argv[0]);
  }
  else
  {
    grp_write(argv[1], (const char**)(argv + 2), (size_t)(argc - 2));
  }

  return EXIT_SUCCESS;
}
