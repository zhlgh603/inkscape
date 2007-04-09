	#define __SP_FEIMAGE_CPP__

/** \file
 * SVG <feImage> implementation.
 *
 */
/*
 * Authors:
 *   hugo Rodrigues <haa.rodrigues@gmail.com>
 *
 * Copyright (C) 2006 Hugo Rodrigues
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "attributes.h"
#include "svg/svg.h"
#include "sp-feimage.h"
#include "xml/repr.h"


/* FeImage base class */

static void sp_feImage_class_init(SPFeImageClass *klass);
static void sp_feImage_init(SPFeImage *feImage);

static void sp_feImage_build(SPObject *object, SPDocument *document, Inkscape::XML::Node *repr);
static void sp_feImage_release(SPObject *object);
static void sp_feImage_set(SPObject *object, unsigned int key, gchar const *value);
static void sp_feImage_update(SPObject *object, SPCtx *ctx, guint flags);
static Inkscape::XML::Node *sp_feImage_write(SPObject *object, Inkscape::XML::Node *repr, guint flags);

static SPFilterPrimitiveClass *feImage_parent_class;

GType
sp_feImage_get_type()
{
    static GType feImage_type = 0;

    if (!feImage_type) {
        GTypeInfo feImage_info = {
            sizeof(SPFeImageClass),
            NULL, NULL,
            (GClassInitFunc) sp_feImage_class_init,
            NULL, NULL,
            sizeof(SPFeImage),
            16,
            (GInstanceInitFunc) sp_feImage_init,
            NULL,    /* value_table */
        };
        feImage_type = g_type_register_static(SP_TYPE_FILTER_PRIMITIVE, "SPFeImage", &feImage_info, (GTypeFlags)0);
    }
    return feImage_type;
}

static void
sp_feImage_class_init(SPFeImageClass *klass)
{
    SPObjectClass *sp_object_class = (SPObjectClass *)klass;

    feImage_parent_class = (SPFilterPrimitiveClass*)g_type_class_peek_parent(klass);

    sp_object_class->build = sp_feImage_build;
    sp_object_class->release = sp_feImage_release;
    sp_object_class->write = sp_feImage_write;
    sp_object_class->set = sp_feImage_set;
    sp_object_class->update = sp_feImage_update;
}

static void
sp_feImage_init(SPFeImage *feImage)
{
}

/**
 * Reads the Inkscape::XML::Node, and initializes SPFeImage variables.  For this to get called,
 * our name must be associated with a repr via "sp_object_type_register".  Best done through
 * sp-object-repr.cpp's repr_name_entries array.
 */
static void
sp_feImage_build(SPObject *object, SPDocument *document, Inkscape::XML::Node *repr)
{
    if (((SPObjectClass *) feImage_parent_class)->build) {
        ((SPObjectClass *) feImage_parent_class)->build(object, document, repr);
    }

    /*LOAD ATTRIBUTES FROM REPR HERE*/
}

/**
 * Drops any allocated memory.
 */
static void
sp_feImage_release(SPObject *object)
{

    if (((SPObjectClass *) feImage_parent_class)->release)
        ((SPObjectClass *) feImage_parent_class)->release(object);
}

/**
 * Sets a specific value in the SPFeImage.
 */
static void
sp_feImage_set(SPObject *object, unsigned int key, gchar const *value)
{
    SPFeImage *feImage = SP_FEIMAGE(object);
    (void)feImage;

    switch(key) {
	/*DEAL WITH SETTING ATTRIBUTES HERE*/
        default:
            if (((SPObjectClass *) feImage_parent_class)->set)
                ((SPObjectClass *) feImage_parent_class)->set(object, key, value);
            break;
    }

}

/**
 * Receives update notifications.
 */
static void
sp_feImage_update(SPObject *object, SPCtx *ctx, guint flags)
{
    if (flags & (SP_OBJECT_MODIFIED_FLAG | SP_OBJECT_STYLE_MODIFIED_FLAG |
                 SP_OBJECT_VIEWPORT_MODIFIED_FLAG)) {

        /* do something to trigger redisplay, updates? */

    }

    if (((SPObjectClass *) feImage_parent_class)->update) {
        ((SPObjectClass *) feImage_parent_class)->update(object, ctx, flags);
    }
}

/**
 * Writes its settings to an incoming repr object, if any.
 */
static Inkscape::XML::Node *
sp_feImage_write(SPObject *object, Inkscape::XML::Node *repr, guint flags)
{
    // Inkscape-only object, not copied during an "plain SVG" dump:
    if (flags & SP_OBJECT_WRITE_EXT) {
        if (repr) {
            // is this sane?
            repr->mergeFrom(SP_OBJECT_REPR(object), "id");
        } else {
            repr = SP_OBJECT_REPR(object)->duplicate(NULL); // FIXME
        }
    }

    if (((SPObjectClass *) feImage_parent_class)->write) {
        ((SPObjectClass *) feImage_parent_class)->write(object, repr, flags);
    }

    return repr;
}


/*
  Local Variables:
  mode:c++
  c-file-style:"stroustrup"
  c-file-offsets:((innamespace . 0)(inline-open . 0)(case-label . +))
  indent-tabs-mode:nil
  fill-column:99
  End:
*/
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=8:softtabstop=4:encoding=utf-8:textwidth=99 :
