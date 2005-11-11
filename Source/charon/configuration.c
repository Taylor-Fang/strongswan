/**
 * @file configuration.c
 * 
 * @brief Configuration class used to store IKE_SA-configurations.
 * 
 * Object of this type represents a configuration for an IKE_SA and its child_sa's.
 * 
 */

/*
 * Copyright (C) 2005 Jan Hutter, Martin Willi
 * Hochschule fuer Technik Rapperswil
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

#include <stdlib.h>

#include "configuration.h"

#include "types.h"
#include "utils/allocator.h"

/**
 * Private data of an configuration_t object
 */
typedef struct private_configuration_s private_configuration_t;

struct private_configuration_s {

	/**
	 * Public part of a configuration_t object
	 */
	configuration_t public;


	/* Private values */
};

/**
 * Implements function destroy of configuration_t.
 * See #configuration_s.destroy for description.
 */
static status_t destroy (private_configuration_t *this)
{
	if (this == NULL)
	{
		return FAILED;
	}
	allocator_free(this);
	return SUCCESS;
}

/*
 * Described in header-file
 */
configuration_t * configuration_create()
{
	private_configuration_t *this = allocator_alloc_thing(private_configuration_t);
	if (this == NULL)
	{
		return NULL;
	}

	/* public functions */
	this->public.destroy = (status_t(*)(configuration_t*))destroy;


	return (&this->public);
}
