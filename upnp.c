/*
 * Copyright © 2008 Ross Burton
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ephy-upnp-extension.h"
#include <gmodule.h>

G_MODULE_EXPORT GType register_module (GTypeModule *module);

G_MODULE_EXPORT GType
register_module (GTypeModule *module)
{
  return ephy_upnp_extension_register_type (module);
}
