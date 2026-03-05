/* SPDX-License-Identifier: MIT */
/**
 * @file   errorcodes.h
 * @author Marc-Alexandre Espiaut <ma.dev@espiaut.fr>
 * @date   2026-03-03
 * @brief  Error codes.
 *
 * @copyright Copyright (c) 2026 Marc-Alexandre Espiaut
 */

#ifndef ERRORCODES_H
#define ERRORCODES_H

enum
{
  e_error_ms_seek = -1,
  e_noerror = 0,
  e_error_fclose = 2,
  e_error_fopen,
  e_error_fread,
  e_error_fseek,
  e_error_ftell,
  e_error_fwrite,
  e_error_malloc,
  e_error_name_upper
};

#endif /* ERRORCODES_H */
