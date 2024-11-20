#include <glib.h>
#include <gtk/gtk.h>
#include <wintc/comgtk.h>
#include <wintc/shell.h>

#include "../sidebar.h"
#include "../window.h"
#include "fldrside.h"

//
// FORWARD DECLARATIONS
//
static void wintc_exp_folders_sidebar_constructed(
    GObject* object
);
static void wintc_exp_folders_sidebar_dispose(
    GObject* object
);

static void on_explorer_window_mode_changed(
    WinTCExplorerWindow*    self,
    gpointer                user_data
);

//
// GTK OOP CLASS/INSTANCE DEFINITIONS
//
struct _WinTCExpFoldersSidebarClass
{
    WinTCExplorerSidebarClass __parent__;
};

struct _WinTCExpFoldersSidebar
{
    WinTCExplorerSidebar __parent__;

    // Logic stuff
    //
    WinTCShTreeViewBehaviour* behaviour_tree;

    // UI
    //
    GtkWidget* tree_view;
};

//
// GTK TYPE DEFINITION & CTORS
//
G_DEFINE_TYPE(
    WinTCExpFoldersSidebar,
    wintc_exp_folders_sidebar,
    WINTC_TYPE_EXPLORER_SIDEBAR
)

static void wintc_exp_folders_sidebar_class_init(
    WinTCExpFoldersSidebarClass* klass
)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);

    object_class->constructed = wintc_exp_folders_sidebar_constructed;
    object_class->dispose     = wintc_exp_folders_sidebar_dispose;
}

static void wintc_exp_folders_sidebar_init(
    WinTCExpFoldersSidebar* self
)
{
    WinTCExplorerSidebar* sidebar = WINTC_EXPLORER_SIDEBAR(self);

    GtkBuilder* builder =
        gtk_builder_new_from_resource(
            "/uk/oddmatics/wintc/explorer/fldrside.ui"
        );

    sidebar->root_widget =
        GTK_WIDGET(
            g_object_ref(
                gtk_builder_get_object(builder, "main-box")
            )
        );

    self->tree_view =
        GTK_WIDGET(
            g_object_ref(
                gtk_builder_get_object(builder, "tree-view")
            )
        );

    g_object_unref(builder);
}

//
// CLASS VIRTUAL METHODS
//
static void wintc_exp_folders_sidebar_constructed(
    GObject* object
)
{
    WinTCExplorerSidebar* sidebar =
        WINTC_EXPLORER_SIDEBAR(object);
    WinTCExpFoldersSidebar* sidebar_folders =
        WINTC_EXP_FOLDERS_SIDEBAR(object);

    // Potentially may need to delay loading the tree view behaviour depending
    // on the state of the explorer window
    //
    if (
        wintc_explorer_window_get_mode(
            WINTC_EXPLORER_WINDOW(sidebar->owner_explorer_wnd)
        ) == WINTC_EXPLORER_WINDOW_MODE_LOCAL
    )
    {
        sidebar_folders->behaviour_tree =
            wintc_sh_tree_view_behaviour_new(
                GTK_TREE_VIEW(sidebar_folders->tree_view),
                wintc_explorer_window_get_browser(
                    WINTC_EXPLORER_WINDOW(sidebar->owner_explorer_wnd)
                )
            );
    }
    else
    {
        g_signal_connect(
            sidebar->owner_explorer_wnd,
            "mode-changed",
            G_CALLBACK(on_explorer_window_mode_changed),
            object
        );
    }

    (G_OBJECT_CLASS(wintc_exp_folders_sidebar_parent_class))
        ->constructed(object);
}

static void wintc_exp_folders_sidebar_dispose(
    GObject* object
)
{
    WinTCExpFoldersSidebar* sidebar_folders =
        WINTC_EXP_FOLDERS_SIDEBAR(object);

    g_clear_object(&(sidebar_folders->behaviour_tree));

    (G_OBJECT_CLASS(wintc_exp_folders_sidebar_parent_class))
        ->dispose(object);
}

//
// PUBLIC FUNCTIONS
//
WinTCExplorerSidebar* wintc_exp_folders_sidebar_new(
    WinTCExplorerWindow* wnd
)
{
    return WINTC_EXPLORER_SIDEBAR(
        g_object_new(
            WINTC_TYPE_EXP_FOLDERS_SIDEBAR,
            "owner-explorer", wnd,
            NULL
        )
    );
}

//
// CALLBACKS
//
static void on_explorer_window_mode_changed(
    WinTCExplorerWindow*    self,
    gpointer                user_data
)
{
    WinTCExpFoldersSidebar* sidebar_folders =
        WINTC_EXP_FOLDERS_SIDEBAR(user_data);

    if (sidebar_folders->behaviour_tree)
    {
        return;
    }

    if (
        wintc_explorer_window_get_mode(self) !=
            WINTC_EXPLORER_WINDOW_MODE_LOCAL
    )
    {
        return;
    }

    sidebar_folders->behaviour_tree =
        wintc_sh_tree_view_behaviour_new(
            GTK_TREE_VIEW(sidebar_folders->tree_view),
            wintc_explorer_window_get_browser(self)
        );
}
