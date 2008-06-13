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

#include <epiphany/ephy-extension.h>
#include <epiphany/ephy-shell.h>

#include <libgupnp/gupnp-control-point.h>

#include <gmodule.h>

#define GET_PRIVATE(object) (G_TYPE_INSTANCE_GET_PRIVATE ((object), EPHY_TYPE_UPNP_EXTENSION, EphyUpnpExtensionPrivate))

struct _EphyUpnpExtensionPrivate {
  GHashTable *device_hash;
  GUPnPContext *context;
  GUPnPControlPoint *cp;
  EphyBookmarks *bookmarks;
};

static GObjectClass *parent_class = NULL;
static GType type = 0;

static void
device_available_cb (GUPnPControlPoint *cp,
                     GUPnPDeviceProxy *proxy,
                     EphyUpnpExtension *extension)
{
  EphyUpnpExtensionPrivate *priv;
  GUPnPDeviceInfo *info;
  char *url, *name;

  priv = GET_PRIVATE (extension);
  info = GUPNP_DEVICE_INFO (proxy);

  /* Skip devices we've seen already */
  if (g_hash_table_lookup (priv->device_hash, gupnp_device_info_get_udn (info)))
    return;

  url = gupnp_device_info_get_presentation_url (info);
  if (!url)
    return;

  EphyNode *node;

  node = ephy_node_new (ephy_node_get_db (ephy_bookmarks_get_local (extension->priv->bookmarks)));
  ephy_node_set_is_drag_source (node, FALSE);
  ephy_node_set_property_string (node, EPHY_NODE_BMK_PROP_LOCATION, url);
  g_free (url);

  name = gupnp_device_info_get_friendly_name (info);
  ephy_node_set_property_string (node,
                                 EPHY_NODE_BMK_PROP_TITLE,
                                 name);
  g_free (name);
  ephy_node_set_property_boolean (node,
                                  EPHY_NODE_BMK_PROP_IMMUTABLE,
                                  TRUE);

 
  ephy_node_add_child (ephy_bookmarks_get_bookmarks (extension->priv->bookmarks), node);
  ephy_node_add_child (ephy_bookmarks_get_local (extension->priv->bookmarks), node);

  g_hash_table_insert (priv->device_hash, g_strdup (gupnp_device_info_get_udn (info)), node);
}

static void
ephy_upnp_extension_init (EphyUpnpExtension *extension)
{
  EphyUpnpExtensionPrivate *priv;
  EphyShell *shell;
  
  priv = extension->priv = GET_PRIVATE (extension);
  
  priv->device_hash = g_hash_table_new_full
    (g_str_hash, g_str_equal, g_free, (GDestroyNotify)ephy_node_unref);

  priv->context = gupnp_context_new (NULL, NULL, 0, NULL);
  priv->cp = gupnp_control_point_new (priv->context, GSSDP_ALL_RESOURCES);
  g_signal_connect (priv->cp,
                    "device-proxy-available",
                    G_CALLBACK (device_available_cb),
                    extension);
  gssdp_resource_browser_set_active (GSSDP_RESOURCE_BROWSER (priv->cp), TRUE);
  
  shell = ephy_shell_get_default ();
  priv->bookmarks = ephy_shell_get_bookmarks (shell);
}

static void
ephy_upnp_extension_dispose (GObject *object)
{
  EphyUpnpExtension *extension = EPHY_UPNP_EXTENSION (object);
  
  if (extension->priv->cp) {
    g_object_unref (extension->priv->cp);
    extension->priv->cp = NULL;
  }

  if (extension->priv->context) {
    g_object_unref (extension->priv->context);
    extension->priv->context = NULL;
  }

  G_OBJECT_CLASS (parent_class)->dispose (object);
}

static void
ephy_upnp_extension_finalize (GObject *object)
{
  EphyUpnpExtension *extension = EPHY_UPNP_EXTENSION (object);
  
  /* This will free the keys (UDN strings) and unref the values (EphyNodes),
     which will cause them to be removed from the tree. */
  g_hash_table_destroy (extension->priv->device_hash);
  
  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
ephy_upnp_extension_iface_init (EphyExtensionIface *iface)
{
}

static void
ephy_upnp_extension_class_init (EphyUpnpExtensionClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  
  parent_class = g_type_class_peek_parent (klass);
  
  object_class->dispose = ephy_upnp_extension_dispose;
  object_class->finalize = ephy_upnp_extension_finalize;
  
  g_type_class_add_private (object_class, sizeof (EphyUpnpExtensionPrivate));
}

GType
ephy_upnp_extension_get_type (void)
{
  return type;
}

GType
ephy_upnp_extension_register_type (GTypeModule *module)
{
  const GTypeInfo our_info = {
    sizeof (EphyUpnpExtensionClass),
    NULL, /* base_init */
    NULL, /* base_finalize */
    (GClassInitFunc) ephy_upnp_extension_class_init,
    NULL,
    NULL, /* class_data */
    sizeof (EphyUpnpExtension),
    0, /* n_preallocs */
    (GInstanceInitFunc) ephy_upnp_extension_init
  };
  
  const GInterfaceInfo extension_info = {
    (GInterfaceInitFunc) ephy_upnp_extension_iface_init,
    NULL,
    NULL
  };
  
  type = g_type_module_register_type (module,
                                      G_TYPE_OBJECT,
                                      "EphyUpnpExtension",
                                      &our_info, 0);
  
  g_type_module_add_interface (module,
                               type,
                               EPHY_TYPE_EXTENSION,
                               &extension_info);
  
  return type;
}
