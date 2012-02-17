#ifndef SEEN_SP_DESKTOP_H
#define SEEN_SP_DESKTOP_H

/*
 * Author:
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *   Frank Felfe <innerspace@iname.com>
 *   bulia byak <buliabyak@users.sf.net>
 *   Ralf Stephan <ralf@ark.in-berlin.de>
 *   John Bintz <jcoswell@coswellproductions.org>
 *   Johan Engelen <j.b.c.engelen@ewi.utwente.nl>
 *   Jon A. Cruz <jon@joncruz.org>get
 *   Abhishek Sharma
 *
 * Copyright (C) 2007 Johan Engelen
 * Copyright (C) 2006 John Bintz
 * Copyright (C) 1999-2005 authors
 * Copyright (C) 2000-2001 Ximian, Inc.
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stddef.h>
#include <sigc++/sigc++.h>

#include <2geom/affine.h>
#include <2geom/rect.h>

#include "ui/view/view.h"
#include "ui/view/edit-widget-interface.h"
#include "display/rendermode.h"

class SPCSSAttr;
struct SPCanvas;
struct SPCanvasItem;
struct SPCanvasGroup;
struct SPEventContext;
class  SPItem;
struct SPNamedView;
class  SPObject;
struct SPStyle;
typedef struct _DocumentInterface DocumentInterface;//struct DocumentInterface;

namespace Gtk
{
  class Window;
}

typedef int sp_verb_t;

struct _GdkEventAny;
typedef struct _GdkEventAny GdkEventAny;
struct _GdkEventWindowState;
typedef struct _GdkEventWindowState GdkEventWindowState;

namespace Inkscape {
  class Application;
  class MessageContext;
  class Selection;
  class ObjectHierarchy;
  class LayerManager;
  class EventLog;
  namespace UI {
      namespace Dialog {
          class DialogManager;
      }

      namespace Widget {
	      class Dock;
      }
  }
  namespace Whiteboard {
      class SessionManager;
  }
  namespace Display {
      class TemporaryItemList;
      class TemporaryItem;
      class SnapIndicator;
  }
}

/**
 * SPDesktop is a subclass of View, implementing an editable document
 * canvas.  It is extensively used by many UI controls that need certain
 * visual representations of their own.
 *
 * SPDesktop provides a certain set of SPCanvasItems, serving as GUI
 * layers of different control objects. The one containing the whole
 * document is the drawing layer. In addition to it, there are grid,
 * guide, sketch and control layers. The sketch layer is used for
 * temporary drawing objects, before the real objects in document are
 * created. The control layer contains editing knots, rubberband and
 * similar non-document UI objects.
 *
 * Each SPDesktop is associated with a SPNamedView node of the document
 * tree.  Currently, all desktops are created from a single main named
 * view, but in the future there may be support for different ones.
 * SPNamedView serves as an in-document container for desktop-related
 * data, like grid and guideline placement, snapping options and so on.
 *
 * Associated with each SPDesktop are the two most important editing
 * related objects - SPSelection and SPEventContext.
 *
 * Sodipodi keeps track of the active desktop and invokes notification
 * signals whenever it changes. UI elements can use these to update their
 * display to the selection of the currently active editing window.
 * (Lauris Kaplinski)
 *
 * @see \ref desktop-handles.h for desktop macros.
 */
class SPDesktop : public Inkscape::UI::View::View
{
public:
    Inkscape::UI::Dialog::DialogManager *_dlg_mgr;
    SPNamedView               *namedview;
    SPCanvas                  *canvas;
    /// current selection; will never generally be NULL
    Inkscape::Selection       *selection;
    SPEventContext            *event_context;
    Inkscape::LayerManager    *layer_manager;
    Inkscape::EventLog        *event_log;
    DocumentInterface *dbus_document_interface;
    Inkscape::Display::TemporaryItemList *temporary_item_list;
    Inkscape::Display::SnapIndicator *snapindicator;

    SPCanvasItem  *acetate;
    SPCanvasGroup *main;
    SPCanvasGroup *gridgroup;
    SPCanvasGroup *guides;
    SPCanvasItem  *drawing;
    SPCanvasGroup *sketch;
    SPCanvasGroup *controls;
    SPCanvasGroup *tempgroup;   ///< contains temporary canvas items
    SPCanvasItem  *table;       ///< outside-of-page background
    SPCanvasItem  *page;        ///< page background
    SPCanvasItem  *page_border; ///< page border
    SPCSSAttr     *current;     ///< current style
    bool           _focusMode;  ///< Whether we're focused working or general working

    std::list<Geom::Rect> zooms_past;
    std::list<Geom::Rect> zooms_future;

    bool _quick_zoom_enabled; ///< Signifies that currently we're in quick zoom mode
    Geom::Rect _quick_zoom_stored_area;  ///< The area of the screen before quick zoom
    unsigned int dkey;
    unsigned int number;
    guint window_state;
    unsigned int interaction_disabled_counter;
    bool waiting_cursor;
    bool showing_dialogs;

    /// \todo fixme: This has to be implemented in different way */
    guint guides_active : 1;

    // storage for selected dragger used by GrDrag as it's
    // created and deleted by tools
    SPItem *gr_item;
    guint  gr_point_type;
    guint  gr_point_i;
    bool   gr_fill_or_stroke;


    Inkscape::ObjectHierarchy *_layer_hierarchy;
    Glib::ustring _reconstruction_old_layer_id;

    sigc::signal<void, sp_verb_t>      _tool_changed;
    sigc::signal<void, SPObject *>     _layer_changed_signal;
    sigc::signal<bool, const SPCSSAttr *>::accumulated<StopOnTrue> _set_style_signal;
    sigc::signal<int, SPStyle *, int>::accumulated<StopOnNonZero> _query_style_signal;
    
    /// Emitted when the zoom factor changes (not emitted when scrolling).
    /// The parameter is the new zoom factor
    sigc::signal<void, double> signal_zoom_changed;
    
    sigc::connection connectDestroy(const sigc::slot<void, SPDesktop*> &slot)
    {
        return _destroy_signal.connect(slot);
    }

    sigc::connection connectDocumentReplaced (const sigc::slot<void,SPDesktop*,SPDocument*> & slot)
    {
        return _document_replaced_signal.connect (slot);
    }

    sigc::connection connectEventContextChanged (const sigc::slot<void,SPDesktop*,SPEventContext*> & slot)
    {
        return _event_context_changed_signal.connect (slot);
    }
    sigc::connection connectSetStyle (const sigc::slot<bool, const SPCSSAttr *> & slot)
    {
        return _set_style_signal.connect (slot);
    }
    sigc::connection connectQueryStyle (const sigc::slot<int, SPStyle *, int> & slot)
    {
        return _query_style_signal.connect (slot);
    }
     // subselection is some sort of selection which is specific to the tool, such as a handle in gradient tool, or a text selection
    sigc::connection connectToolSubselectionChanged(const sigc::slot<void, gpointer> & slot) {
        return _tool_subselection_changed.connect(slot);
    }
    void emitToolSubselectionChanged(gpointer data);
    sigc::connection connectCurrentLayerChanged(const sigc::slot<void, SPObject *> & slot) {
        return _layer_changed_signal.connect(slot);
    }

    // Whiteboard changes

#ifdef WITH_INKBOARD
    Inkscape::Whiteboard::SessionManager* whiteboard_session_manager() {
        return _whiteboard_session_manager;
    }

    Inkscape::Whiteboard::SessionManager* _whiteboard_session_manager;
#endif

    /**
     * Return new desktop object.
     * \pre namedview != NULL.
     * \pre canvas != NULL.
     */
    SPDesktop();

    void init (SPNamedView* nv, SPCanvas* canvas, Inkscape::UI::View::EditWidgetInterface *widget);
    virtual ~SPDesktop();
    void destroy();

    Inkscape::MessageContext *guidesMessageContext() const {
        return _guides_message_context;
    }

    Inkscape::Display::TemporaryItem * add_temporary_canvasitem (SPCanvasItem *item, guint lifetime, bool move_to_bottom = true);
    void remove_temporary_canvasitem (Inkscape::Display::TemporaryItem * tempitem);

    void _setDisplayMode(Inkscape::RenderMode mode);
    void setDisplayModeNormal() {
        _setDisplayMode(Inkscape::RENDERMODE_NORMAL);
    }
    void setDisplayModeNoFilters() {
        _setDisplayMode(Inkscape::RENDERMODE_NO_FILTERS);
    }
    void setDisplayModeOutline() {
        _setDisplayMode(Inkscape::RENDERMODE_OUTLINE);
    }
    void displayModeToggle();
    Inkscape::RenderMode _display_mode;
    Inkscape::RenderMode getMode() const { return _display_mode; }

    void _setDisplayColorMode(Inkscape::ColorMode mode);
    void setDisplayColorModeNormal() {
        _setDisplayColorMode(Inkscape::COLORMODE_NORMAL);
    }
    void setDisplayColorModeGrayscale() {
        _setDisplayColorMode(Inkscape::COLORMODE_GRAYSCALE);
    }
//    void setDisplayColorModePrintColorsPreview() {
//        _setDisplayColorMode(Inkscape::COLORMODE_PRINT_COLORS_PREVIEW);
//    }
    void displayColorModeToggle();
    Inkscape::ColorMode _display_color_mode;
    Inkscape::ColorMode getColorMode() const { return _display_color_mode; }

    Inkscape::UI::Widget::Dock* getDock() { return _widget->getDock(); }

    void set_active (bool new_active);

    // TODO look into making these return a more specific subclass:
    SPObject *currentRoot() const;
    SPObject *currentLayer() const;

    void setCurrentLayer(SPObject *object);
    void toggleLayerSolo(SPObject *object);
    SPObject *layerForObject(SPObject *object);
    bool isLayer(SPObject *object) const;
    bool isWithinViewport(SPItem *item) const;
    bool itemIsHidden(SPItem const *item) const;

    void activate_guides (bool activate);
    void change_document (SPDocument *document);

    void set_event_context (GType type, const gchar *config);
    void push_event_context (GType type, const gchar *config, unsigned int key);

    void set_coordinate_status (Geom::Point p);
    SPItem *getItemFromListAtPointBottom(const GSList *list, Geom::Point const p) const;
    SPItem *getItemAtPoint(Geom::Point const p, bool into_groups, SPItem *upto = NULL) const;
    SPItem *getGroupAtPoint(Geom::Point const p) const;
    Geom::Point point() const;

    Geom::Rect get_display_area() const;
    void set_display_area (double x0, double y0, double x1, double y1, double border, bool log = true);
    void set_display_area(Geom::Rect const &a, Geom::Coord border, bool log = true);
    void zoom_absolute (double cx, double cy, double zoom);
    void zoom_relative (double cx, double cy, double zoom);
    void zoom_absolute_keep_point (double cx, double cy, double px, double py, double zoom);
    void zoom_relative_keep_point (double cx, double cy, double zoom);
    void zoom_relative_keep_point (Geom::Point const &c, double const zoom)
    {
            zoom_relative_keep_point (c[Geom::X], c[Geom::Y], zoom);
    }

    void zoom_page();
    void zoom_page_width();
    void zoom_drawing();
    void zoom_selection();
    void zoom_grab_focus();
    double current_zoom() const  { return _d2w.descrim(); }
    void prev_zoom();
    void next_zoom();
    void zoom_quick(bool enable = true);

    /** \brief  Returns whether the desktop is in quick zoom mode or not */
    bool quick_zoomed(void) { return _quick_zoom_enabled; }

    bool scroll_to_point (Geom::Point const &s_dt, gdouble autoscrollspeed = 0);
    void scroll_world (double dx, double dy, bool is_scrolling = false);
    void scroll_world (Geom::Point const scroll, bool is_scrolling = false)
    {
        scroll_world(scroll[Geom::X], scroll[Geom::Y], is_scrolling);
    }
    void scroll_world_in_svg_coords (double dx, double dy, bool is_scrolling = false);

    void getWindowGeometry (gint &x, gint &y, gint &w, gint &h);
    void setWindowPosition (Geom::Point p);
    void setWindowSize (gint w, gint h);
    void setWindowTransient (void* p, int transient_policy=1);
    Gtk::Window* getToplevel();
    void presentWindow();
    bool showInfoDialog( Glib::ustring const &message );
    bool warnDialog (gchar *text);
    void toggleRulers();
    void toggleScrollbars();
    void layoutWidget();
    void destroyWidget();
    void setToolboxFocusTo (gchar const* label);
    void setToolboxAdjustmentValue (gchar const* id, double val);
    void setToolboxSelectOneValue (gchar const* id, gint val);
    bool isToolboxButtonActive (gchar const *id);
    void updateNow();
    void updateCanvasNow();

    void enableInteraction();
    void disableInteraction();

    void setWaitingCursor();
    void clearWaitingCursor();
    bool isWaitingCursor() const { return waiting_cursor; };

    void toggleColorProfAdjust();

    void toggleGrids();
    void toggleSnapGlobal();
    bool gridsEnabled() const { return grids_visible; };
    void showGrids(bool show, bool dirty_document = true);

    bool is_iconified();
    bool is_maximized();
    bool is_fullscreen();
    bool is_focusMode();

    void iconify();
    void maximize();
    void fullscreen();
    void focusMode(bool mode = true);
    /**
     * Reopen any dialogs that were open when inkscape last shutdown
     */
    void show_dialogs();

    Geom::Affine w2d() const; //transformation from window to desktop coordinates (used for zooming)
    Geom::Point w2d(Geom::Point const &p) const;
    Geom::Point d2w(Geom::Point const &p) const;
    Geom::Affine doc2dt() const;
    Geom::Affine dt2doc() const;
    Geom::Point doc2dt(Geom::Point const &p) const;
    Geom::Point dt2doc(Geom::Point const &p) const;

    virtual void setDocument (SPDocument* doc);
    virtual bool shutdown();
    virtual void mouseover() {}
    virtual void mouseout() {}


    virtual bool onDeleteUI (GdkEventAny*);
    virtual bool onWindowStateEvent (GdkEventWindowState* event);

    void applyCurrentOrToolStyle(SPObject *obj, Glib::ustring const &tool_path, bool with_text);

private:
    Inkscape::UI::View::EditWidgetInterface       *_widget;
    Inkscape::Application     *_inkscape;
    Inkscape::MessageContext  *_guides_message_context;
    bool _active;
    Geom::Affine _w2d;
    Geom::Affine _d2w;
    Geom::Affine _doc2dt;

    bool grids_visible; /* don't set this variable directly, use the method below */
    void set_grids_visible(bool visible);

    void push_current_zoom(std::list<Geom::Rect> &);

    sigc::signal<void, SPDesktop*> _destroy_signal;
    sigc::signal<void,SPDesktop*,SPDocument*>     _document_replaced_signal;
    sigc::signal<void>                 _activate_signal;
    sigc::signal<void>                 _deactivate_signal;
    sigc::signal<void,SPDesktop*,SPEventContext*> _event_context_changed_signal;
    sigc::signal<void, gpointer>       _tool_subselection_changed;

    sigc::connection _activate_connection;
    sigc::connection _deactivate_connection;
    sigc::connection _sel_modified_connection;
    sigc::connection _sel_changed_connection;
    sigc::connection _reconstruction_start_connection;
    sigc::connection _reconstruction_finish_connection;
    sigc::connection _commit_connection;
    sigc::connection _modified_connection;

    virtual void onPositionSet (double, double);
    virtual void onResized (double, double);
    virtual void onRedrawRequested();
    virtual void onStatusMessage (Inkscape::MessageType type, gchar const *message);
    virtual void onDocumentURISet (gchar const* uri);
    virtual void onDocumentResized (double, double);

    static void _onActivate (SPDesktop* dt);
    static void _onDeactivate (SPDesktop* dt);
    static void _onSelectionModified (Inkscape::Selection *selection, guint flags, SPDesktop *dt);
};

#endif // SEEN_SP_DESKTOP_H

/*
  Local Variables:
  mode:c++
  c-file-style:"stroustrup"
  c-file-offsets:((innamespace . 0)(inline-open . 0)(case-label . +))
  indent-tabs-mode:nil
  fill-column:99
  End:
*/
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=8:softtabstop=4 :
