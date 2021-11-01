/*
 * Copyright (C) 2015-2021 Kurt Kanzenbach <kurt@kmk-computers.de>
 *
 * This file is part of Get.
 *
 * Get is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Get is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Get.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SSL_INIT_H_
#define _SSL_INIT_H_

#include "get_config.h"

#ifdef HAVE_OPENSSL

#include <openssl/ssl.h>

/**
 * This class is responsible for initializing the OpenSSL
 * library. There should only be exactly one instance of this class.
 */
class SSLInit
{
public:
    inline SSLInit()
    {
        SSL_load_error_strings();
        SSL_library_init();
    }
};

#endif

#endif /* _SSL_INIT_H_ */
