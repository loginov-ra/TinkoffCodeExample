
#include "../../Libs/SmallLib.h"
#include "../../Libs/TXLib.h"
#include "../../Libs/Vector.h"
#include "Plugins/DIMASDK/GEPSB1.h"
#include "Plugins/FEDORSDK/main.h"
#include "FillFile.h"

#define Pure = 0;

using namespace std;

class wnd_tool;

UINT ___COLOR___ = TX_WHITE; //really bad global but in future IT WILL BE DEFEATED!!!

wnd_tool* ___CURR_TOOL___ = NULL;

char* CURRENT_PLUGIN_PATH = "Plugins/Negative.dll";

const int MAX_WINDOWS        = 100;
const int   N_COLORS         = 6;
const int   N_TOOLS          = 4;
const int CLEAR_BUT          = 'C';
const int PAINT_WND_THICK    = 6;

struct tool_pictures
{
    HDC     circle_tool;
    HDC       rect_tool;
    HDC       fill_tool;
    HDC    ellipse_tool;
    HDC       text_tool;
    HDC    close_button;
    HDC     undo_button;
    HDC  window_example;

    void create_all_images ();
    void delete_all_images ();
};

class abstract_window_t
{
    protected:
      vector_t  pos_;
      vector_t size_;

      bool deleted_;

    public:
                   abstract_window_t (vector_t pos, vector_t size)           ;
      virtual void on_paint          (                           ) const     ;
      virtual void on_mouse          (                           )       Pure;
      virtual bool on_click_test     (                           ) const     ;

              void set_deleted       (bool new_status            ) { deleted_ = new_status; }
              bool get_deleted       (                           ) { return deleted_; }

              vector_t get_pos  () { return pos_;  }
              vector_t get_size () { return size_; }

};

class abstract_manager_t : public abstract_window_t
{
    public:
        abstract_window_t** windows_;

        int filled_;

        abstract_manager_t ( vector_t pos, vector_t size );
       ~abstract_manager_t ();

        virtual bool  on_click_test () const;
        virtual void       on_paint () const;
        virtual void       on_mouse ()      ;
                void remove_deleted ()      ;
                void remove_all     ()      ;
                 int get_filled     ()        { return filled_; }

        void add    (abstract_window_t* new_wnd);
        void remove (int pos);
};

class client_area_t : public abstract_window_t
{
    private:
        HDC copy_;
        HDC last_;

        const char* header_;

    public:

                      client_area_t (vector_t pos, vector_t size, tool_pictures p,
                                     const char* header );

                     ~client_area_t ()                     ;
        virtual void  on_paint      ()                const;
                void  on_draw       (tool_pictures p) const;
        virtual void  on_mouse      ()                     ;
                void      undo      ()                     ;

                void  fill_array (GEPSB1Colour dest [][MAX_IMAGE_SIZE]);
                void  draw_array (GEPSB1Colour from [][MAX_IMAGE_SIZE]);

                void  use_gepsb1 ();
                void  use_fyodor ();

                void  use_plugin ();

                HDC get_copy () { return copy_; }
                HDC get_last () { return last_; }
};

class paint_window_t : public abstract_manager_t
{
    private:
        const char* header_;

    public:
                void fill_array     (tool_pictures p);
                     paint_window_t (vector_t pos, vector_t size, tool_pictures p, const char* header);
        virtual void on_mouse       ();
                void on_move        (tool_pictures p);

        void fill_color_array (GEPSB1Colour image [][MAX_IMAGE_SIZE]);

};

class close_button_t : public abstract_window_t
{
    private:
        abstract_manager_t* parent_;
        HDC                 pict_;

    public:
                     close_button_t (vector_t pos, abstract_manager_t* parent, tool_pictures pic);
        virtual void on_mouse       ()      ;
        virtual void on_paint       () const;
};

class undo_button_t : public abstract_window_t
{
    private:
        client_area_t* to_undo_;
        HDC pict_;

    public:
                     undo_button_t (vector_t pos, client_area_t* to_undo, tool_pictures p)         ;
        virtual void on_paint      ()                                                         const;
        virtual void on_mouse      ()                                                              ;
};

class paint_mngr_t : public abstract_manager_t
{
    public:
      paint_mngr_t ();
};

class wnd_color_but : public abstract_window_t
{
    private:
        UINT  color_;
        bool chosen_;

    public:
                     wnd_color_but (vector_t pos, UINT color, bool chosen);
        virtual void on_paint      () const;
        virtual void on_mouse      ()      ;
                void set_chosen    (bool new_chosen) { chosen_ = new_chosen; }
                UINT get_color     ()                { return color_; }
};

class wnd_tool : public abstract_window_t
{
    public:                                                //correct it
        HDC  tool_pic_;
        bool   chosen_;

    //public:
                      wnd_tool (vector_t pos, HDC tool_pic, bool chosen)           ;
        virtual void on_paint  ()                                        const     ;
        virtual void on_mouse  ()                                                  ;
        virtual void use       (vector_t draw_pos, vector_t draw_size)   const Pure;

                void set_chosen (bool new_chosen) { chosen_ = new_chosen; }
};

class circle_tool : public wnd_tool
{
    public:
                     circle_tool (vector_t pos, tool_pictures p, bool chosen)      ;
        virtual void use         (vector_t draw_pos, vector_t draw_size)      const;
};

/*class text_tool : public wnd_tool
{
    public:
    virtual void text_tool (vector_t pos, tool_pictures p, bool chosen);
};*/

class rect_tool : public wnd_tool
{
    public:
                     rect_tool (vector_t pos, tool_pictures p, bool chosen)      ;
        virtual void use       (vector_t draw_pos, vector_t draw_size)      const;
};

class fill_tool : public wnd_tool
{
    public:
                     fill_tool (vector_t pos, tool_pictures p, bool chosen)      ;
        virtual void use       (vector_t draw_pos, vector_t draw_size)      const;
};

class ellipse_tool : public wnd_tool
{
    public:
                     ellipse_tool (vector_t pos, tool_pictures p, bool chosen)      ;
        virtual void use          (vector_t draw_pos, vector_t draw_size)      const;
};

class wnd_palett : public abstract_manager_t
{
    public:
                void fill_array   (UINT colors [], tool_pictures p)                     ;
                     wnd_palett   (vector_t pos,  UINT colors [], tool_pictures p)      ;
        virtual void on_mouse     ()                                                    ;
};

class tool_bar : public abstract_manager_t
{
    public:
                void fill_array (tool_pictures p)                    ;
                     tool_bar   (vector_t pos, tool_pictures p)      ;
        virtual void on_mouse   ()                                   ;
};

class abstract_plugin_t : public abstract_window_t
{
    private:
        char* path_to_plugin_;

        HDC plugin_pic_;
        bool chosen_;

    public:
                     abstract_plugin_t (vector_t pos, vector_t size,
                                        char* path, HDC plugin_pic, bool chosen);

        virtual void on_paint          () const;
        virtual void on_mouse          ()      ;

        void set_chosen (bool new_status) { chosen_ = new_status; }

        char* get_path ()                 { return path_to_plugin_; }


};

class plugin_manager_t : public abstract_manager_t
{
    public:
                void fill_array       ();
                     plugin_manager_t (vector_t pos);
        virtual void on_mouse         ();
};

enum WINDOW_BOARDERS
{
    WINDOW_X = 1000,
    WINDOW_Y = 700
};

enum COLOR_SIZE
{
    COL_WIDTH  = 30,
    COL_HEIGHT = 30
};

enum TOOL_SIZE
{
    TOOL_WIDTH  = 30,
    TOOL_HEIGHT = 30
};

enum CLOSE_SIZE
{
    CLOSE_WIDTH  = 30,
    CLOSE_HEIGHT = 15
};

enum UNDO_SIZE
{
    UNDO_WIDTH  = 30,
    UNDO_HEIGHT = 15
};

enum EXAMPLE_BORDERS
{
     LEFT_BORD = 6,
    RIGHT_BORD = 6,
       UP_BORD = 26,
     DOWN_BORD = 7,

    UP_BORD_COORD = 228,

    EXAMPLE_HEIGHT = 522,
    EXAMPLE_WIDTH  = 1022,

    BUTTONS_SIZE =  73
};

enum CLOSE_POS
{
    CLOSE_X = 984,
    CLOSE_Y = 236
};

enum UNDO_POS
{
    UNDO_X = 950,
    UNDO_Y = 236
};

enum BUTTONS_DIST
{
     UNDO_DIST = 72,
    CLOSE_DIST = 38,

    Y_DIST = 8
};

enum MODES
{
    DRAW_MODE = 0,
    PLUG_MODE = 1
};

enum SDK_VARIANTS
{
    GEPSB1 = 0,
    FYODOR = 1
};

const int N_FILTERS = 5;

const int SDK = FYODOR;

int CURR_MODE = DRAW_MODE;

bool mouse_on (vector_t pos, vector_t size)
{
    vector_t click ( txMouseX (),
                     txMouseY ()  );

    return ( click.x_ >= pos.x_           &&
             click.x_ <= pos.x_ + size.x_ &&
             click.y_ >= pos.y_           &&
             click.y_ <= pos.y_ + size.y_    );
}

class move_button_t : public abstract_window_t
{
    private:
        paint_window_t* to_move_;

        tool_pictures p_;

    public:
                     move_button_t (paint_window_t* to_move, tool_pictures p)      ;
        virtual bool on_click_test ()                                         const;
        virtual void on_paint      ()                            const      {return;}
        virtual void on_mouse      ()                                              ;

};

double doubleInputBox (const char* text, const char* header, const char* start)
{
    char* char_res = (char*)(txInputBox (text, header, start));

    double res = atof (char_res);

    return res;
}

int main ()
{
    txCreateWindow (WINDOW_X, WINDOW_Y);

    tool_pictures p = {};     p.create_all_images ();

    abstract_manager_t main_mngr (vector_t (0, 0), vector_t (WINDOW_X, WINDOW_Y));

    paint_mngr_t paint_mngr;

    UINT colors [N_COLORS] = {TX_GREEN, TX_BLACK, TX_RED, TX_BLUE, TX_YELLOW, TX_ORANGE}; 

    paint_mngr.add (new paint_window_t   ( vector_t (50, 50),   vector_t (500, 400), p, "Paint Window1" ));

    main_mngr.add (new wnd_palett       (vector_t (0, WINDOW_Y - COL_HEIGHT -
                                         COL_HEIGHT * (N_COLORS / 2)), colors, p));

    main_mngr.add (new tool_bar         (vector_t (WINDOW_X - 2 * TOOL_WIDTH, 0), p));
    main_mngr.add (new plugin_manager_t (vector_t (0,
                                                   WINDOW_Y - TOOL_HEIGHT)));

    main_mngr.add (new paint_mngr_t (paint_mngr));

    while (!GetAsyncKeyState (VK_SPACE))
    {
        txBegin ();

        Clear (TX_WHITE);

        main_mngr.on_paint  ();
        main_mngr.on_mouse  ();

        /*if (GetAsyncKeyState (VK_LCONTROL) && GetAsyncKeyState ('N'))
        {
            double size_x = doubleInputBox ("Введите длину:",  "Новое окно", "0");
            double size_y = doubleInputBox ("Введите ширину:", "Новое окно", "0");

            const char* header = txInputBox ("Введите заголовок:", "Новое окно",
                                             "Новое окно");

            main_mngr.add (new paint_window_t (vector_t (50, 50),
                                               vector_t (size_x, size_y), p, header));
        }*/

        txEnd ();
    }

    p.delete_all_images ();

    return 0;
}

void tool_pictures::create_all_images ()
{
        circle_tool = txLoadImage ("Resourses/circle_tool.bmp");
          rect_tool = txLoadImage ("Resourses/rect_tool.bmp");
          fill_tool = txLoadImage ("Resourses/fill_tool.bmp");
       ellipse_tool = txLoadImage ("Resourses/ellipse_tool.bmp");
          text_tool = txLoadImage ("Resourses/text_tool.bmp");
       close_button = txLoadImage ("Resourses/close_but.bmp");
        undo_button = txLoadImage ("Resourses/undo_button.bmp");
     window_example = txLoadImage ("Resourses/window_example.bmp");
}

void tool_pictures::delete_all_images ()
{
    txDeleteDC (    circle_tool);
    txDeleteDC (      rect_tool);
    txDeleteDC (      fill_tool);
    txDeleteDC (   ellipse_tool);
    txDeleteDC (      text_tool);
    txDeleteDC (   close_button);
    txDeleteDC (    undo_button);
    txDeleteDC ( window_example);
}

abstract_manager_t::abstract_manager_t ( vector_t pos, vector_t size ):

    abstract_window_t (pos, size),

    windows_ (new abstract_window_t* [MAX_WINDOWS]),
     filled_ (0)

    {}

void abstract_manager_t::add (abstract_window_t* new_wnd)
{
    if (filled_ == MAX_WINDOWS) return;

    windows_ [filled_] = new_wnd; windows_ [filled_] -> set_deleted (false);
              filled_++;
}

void abstract_manager_t::remove (int pos)
{
    if (pos < 0      ) return;
    if (pos > filled_) return;

    delete windows_ [pos];

    for (int i = pos; i < filled_ - 1; i++) windows_ [i] = windows_ [i + 1];

    filled_--;
}

void abstract_manager_t::remove_deleted ()
{
    for (int i = 0; i < filled_; i++)
    {
        if (windows_ [i] -> get_deleted () == true) remove (i);
    }
}

void abstract_manager_t::remove_all ()
{
    int num = filled_;

    for (int i = 0; i < num; i++)
    {
        remove (0);

        //$r //printf ("%d items deleted\n", i + 1);
    }
}

void abstract_manager_t::on_paint () const
{
    for (int i = 0; i < filled_; i++) windows_ [i] -> on_paint ();
}

void abstract_manager_t::on_mouse ()
{
    if (on_click_test ())
    {
        for (int i = 0; i < filled_; i++) windows_ [i] -> on_mouse ();
    }
}

abstract_manager_t::~abstract_manager_t ()
{
    delete [] windows_;  // AND INSTALL LINUX.
}

paint_mngr_t::paint_mngr_t () :

    abstract_manager_t ( vector_t (0, 0), vector_t (WINDOW_X, WINDOW_Y) )

    {}

abstract_window_t::abstract_window_t (vector_t pos, vector_t size):

           pos_ (pos),
          size_ (size),
       deleted_ (false)

    {}

void abstract_window_t::on_paint () const
{
    txSetColor     (TX_ORANGE, PAINT_WND_THICK);
    txSetFillColor (TX_BLUE);

    txRectangle (pos_.x_           , pos_.y_,
                 pos_.x_ + size_.x_, pos_.y_ + size_.y_);
}

bool abstract_window_t::on_click_test () const
{
    vector_t click ( txMouseX (),
                     txMouseY ()  );

    return ( txMouseButtons () == 1         &&
             click.x_ >= pos_.x_            &&
             click.x_ <= pos_.x_ + size_.x_ &&
             click.y_ >= pos_.y_            &&
             click.y_ <= pos_.y_ + size_.y_    );
}

client_area_t::client_area_t (vector_t pos, vector_t size, tool_pictures p,
                              const char* header ):

    abstract_window_t (pos, size),

    copy_ (txCreateCompatibleDC (size.x_ + LEFT_BORD + RIGHT_BORD, size.y_ + UP_BORD + DOWN_BORD)),
    last_ (txCreateCompatibleDC (size.x_ + LEFT_BORD + RIGHT_BORD, size.y_ + UP_BORD + DOWN_BORD)),

    header_ (header)

    {
        on_draw (p);

        txBitBlt (copy_, 0, 0, size_.x_ + LEFT_BORD + RIGHT_BORD, size_.y_ + UP_BORD + DOWN_BORD,
                  txDC (), pos_.x_ - LEFT_BORD, pos_.y_ - UP_BORD);

        txBitBlt (copy_, 0, 0, size_.x_ + LEFT_BORD + RIGHT_BORD, size_.y_ + UP_BORD + DOWN_BORD,
                  txDC (), pos_.x_ - LEFT_BORD, pos_.y_ - UP_BORD);
    }

void client_area_t::on_draw (tool_pictures p) const
{
    txRectangle (pos_.x_           , pos_.y_,
                 pos_.x_ + size_.x_, pos_.y_ + size_.y_);

    txBitBlt (txDC (), pos_.x_ - LEFT_BORD, pos_.y_ - UP_BORD, LEFT_BORD, UP_BORD, //draw left-up corner
              p.window_example, 0, UP_BORD_COORD);

    txBitBlt (txDC (), pos_.x_ - LEFT_BORD, pos_.y_ + size_.y_, LEFT_BORD, DOWN_BORD, // draw left_down corner
              p.window_example, 0, UP_BORD_COORD + EXAMPLE_HEIGHT - DOWN_BORD);

    txBitBlt (txDC (), pos_.x_ + size_.x_, pos_.y_ + size_.y_, RIGHT_BORD, DOWN_BORD,
              p.window_example, EXAMPLE_WIDTH - RIGHT_BORD, EXAMPLE_HEIGHT - DOWN_BORD);

    for (int i = 0; i < (int)(size_.y_ / 10); i++)
    {
        txBitBlt (txDC (), pos_.x_ - LEFT_BORD, pos_.y_ + i * 10, LEFT_BORD, 10,
                  p.window_example, 0, UP_BORD_COORD + 2 * UP_BORD);

        txBitBlt (txDC (), pos_.x_ + size_.x_, pos_.y_ + i * 10, RIGHT_BORD, 10,
                  p.window_example, EXAMPLE_WIDTH - RIGHT_BORD, UP_BORD_COORD + 2 * UP_BORD);
    }

    for (int j = 0; j < (int)(size_.x_ / 10); j++)
    {
        txBitBlt (txDC (), pos_.x_ + j * 10, pos_.y_ - UP_BORD, 10, UP_BORD,
                  p.window_example, 2 * LEFT_BORD, UP_BORD_COORD);
    }

    txBitBlt (txDC (), pos_.x_ + size_.x_ + RIGHT_BORD - BUTTONS_SIZE, pos_.y_ - UP_BORD,
              BUTTONS_SIZE, UP_BORD,
              p.window_example, EXAMPLE_WIDTH - BUTTONS_SIZE, UP_BORD_COORD); // draw right-up corner with buttons

    for (int k = 0; k < (int)(size_.x_ / 10); k++)
    {
        txBitBlt (txDC (), pos_.x_ + k * 10, pos_.y_ + size_.y_, 10, DOWN_BORD,
                  p.window_example, 2 * LEFT_BORD, UP_BORD_COORD + EXAMPLE_HEIGHT - DOWN_BORD);
    }

    SetColors (TX_WHITE, TX_WHITE);

    txRectangle (pos_.x_           , pos_.y_,
                 pos_.x_ + size_.x_, pos_.y_ + size_.y_);

    txSelectFont ("Garamond", 20);
    SetColors    (TX_BLACK, TX_BLACK);

    txDrawText (pos_.x_ + 5, pos_.y_ - UP_BORD + 5,
                pos_.x_ + size_.x_ + RIGHT_BORD - BUTTONS_SIZE - 5, pos_.y_ - 5,
                header_);
}

void client_area_t::fill_array (GEPSB1Colour dest [][MAX_IMAGE_SIZE])
{
    for (int iy = 0; iy < size_.y_; iy++)
    {
        for (int ix = 0; ix < size_.x_; ix++)
        {
            unsigned colour = txGetPixel (pos_.x_ + ix, pos_.y_ + iy);

            GEPSB1Colour to_add ( txExtractColor (colour, TX_RED),
                                  txExtractColor (colour, TX_GREEN),
                                  txExtractColor (colour, TX_BLUE)  );

            dest [ix][iy] = to_add;
        }
    }
}

void client_area_t::draw_array (GEPSB1Colour from [][MAX_IMAGE_SIZE])
{
    for (int iy = 0; iy < size_.y_; iy++)
    {
        for (int ix = 0; ix < size_.x_; ix++)
        {
            txSetPixel ( pos_.x_ + ix, pos_.y_ + iy, RGB ( from [ix][iy].R,
                                                           from [ix][iy].G,
                                                           from [ix][iy].B ) );
        }
    }
}

client_area_t::~client_area_t ()
{
    txDeleteDC (copy_);
    txDeleteDC (last_);
}

close_button_t::close_button_t (vector_t pos, abstract_manager_t* parent, tool_pictures pic):

    abstract_window_t (pos, vector_t (CLOSE_WIDTH, CLOSE_HEIGHT)),
    parent_           (parent),
      pict_           (pic.window_example)

    {}

void close_button_t::on_paint () const
{
    txBitBlt (txDC (), pos_.x_, pos_.y_, size_.x_, size_.y_, pict_, CLOSE_X, CLOSE_Y);
}

void close_button_t::on_mouse ()
{
    if (on_click_test ())
    {
        //parent_.set_deleted (true);

        //printf ("%d items filled\n", parent_ -> get_filled ());
        parent_ -> remove_all ();

        ////printf ("I have set deleted\n");
    }
}

undo_button_t::undo_button_t (vector_t pos, client_area_t* to_undo, tool_pictures p):

    abstract_window_t ( pos, vector_t (UNDO_WIDTH, UNDO_HEIGHT) ),
             to_undo_ (to_undo),
                pict_ (p.window_example)

    {}

void undo_button_t::on_paint () const
{
    txBitBlt (txDC (), pos_.x_, pos_.y_, size_.x_, size_.y_, pict_, UNDO_X, UNDO_Y);
}

void undo_button_t::on_mouse ()
{
    if (on_click_test ())
    {
        to_undo_ -> undo ();
    }
}

bool abstract_manager_t::on_click_test () const
{
    for (int i = 0; i < filled_; i++)
    {
        if (windows_ [i] -> on_click_test () == true)
        {
            return true;
        }
    }

    return false;
}

void GEPSB1FunctionsForFilter::setPixel (int x, int y, GEPSB1Colour colour)
{
    txSetPixel (x, y, RGB (colour.R, colour.G, colour.B));
}

void client_area_t::use_gepsb1 ()
{
    typedef GEPSB1FilterObject* GEPSB1GetPluginObject_t ();

    //cout << "Path is " << CURRENT_PLUGIN_PATH << endl << endl;

    HMODULE dll = LoadLibrary (CURRENT_PLUGIN_PATH); assert (dll);

    GEPSB1FunctionsForFilter func;

    GEPSB1GetPluginObject_t* GetPlugin = (GEPSB1GetPluginObject_t*)(GetProcAddress (dll, "GEPSB1GetPluginObject"));
    assert                  (GetPlugin);

    GEPSB1FilterObject* filter = GetPlugin ();
    assert             (filter);

    static GEPSB1Colour image [MAX_IMAGE_SIZE][MAX_IMAGE_SIZE] = {};

    fill_array (image);

    assert (image);

    GEPSB1FilterDescriptor descr = filter -> GetPluginDescriptor ();

    //cout << "I have a descriptor" << endl;

    int N_PARAMETERS = descr.nParams_;

    txEnd ();

    //printf ("nParams is %d\n", nParams);

    double params [N_PARAMETERS]; // = {};

    for (int i = 0; i < N_PARAMETERS; i++)
    {
        assert (i < N_PARAMETERS);

        //printf ("i is %d and n_params is %d so get another box\n", i, N_PARAMETERS);

        params [i] = doubleInputBox ("Введите коэффициэнт", descr.name_, 0);
    }

    assert (filter);

    //printf ("I am going to go to %s plugin and to \" %s \" with parameter %g\n", descr.name_,
    //                                                                            descr.filterDescriptor_,
    //                                                                            params [0]);

    filter -> ActivatePlugin (image, (int)size_.x_, (int)size_.y_,
                                     (int)pos_.x_,  (int)pos_.y_, &func, params);

    //draw_array (image);

    FreeLibrary (dll);

    txBegin ();
}

void txMessageBox (const char* text_, const char* header_, int flags_)
{
    HMODULE dll = LoadLibrary ("user32.dll");
    assert (dll);

    typedef int WINAPI MessageBox_t (HWND wnd, const char* text, const char* title,
                                     int flags);

    MessageBox_t* MessageBoxFromDLL = (MessageBox_t*)(GetProcAddress (dll, "MessageBoxA"));
    assert       (MessageBoxFromDLL);

    MessageBoxFromDLL (NULL, text_, header_, 0);

    FreeLibrary (dll); dll = NULL;
}

void client_area_t::use_fyodor ()
{
    $g cout << "I'm going to take dll from " << CURRENT_PLUGIN_PATH << endl; 
     
    HMODULE dll = LoadLibrary (CURRENT_PLUGIN_PATH); assert (dll);
    
    typedef CAbsFilter* GetPluginObject_t ();
    
    GetPluginObject_t* GetPlugin = (GetPluginObject_t*)(GetProcAddress (dll, "GetPluginObject")); 
    assert            (GetPlugin);
    
    CAbsFilter* filter = GetPlugin ();
    assert     (filter);
    
    filter -> x      =  pos_.x_;
    filter -> y      =  pos_.y_;
    filter -> width  = size_.x_;
    filter -> height = size_.y_;
    
    filter -> Filter (); txClearConsole ();
    
    FreeLibrary (dll); dll = NULL;
}

void client_area_t::use_plugin ()
{
    if (SDK == GEPSB1)
    {
        //cout << "I'm going to use gepsb1" << endl;

        use_gepsb1 ();
    }

    if (SDK == FYODOR) use_fyodor ();
}

void client_area_t::on_mouse ()
{
    //printf ("    I was in paint_on_mouse\n");

    if (on_click_test ())
    {
         txBitBlt (last_, 0, 0, size_.x_ + LEFT_BORD + RIGHT_BORD, size_.y_ + UP_BORD + DOWN_BORD,
                   txDC (), pos_.x_ - LEFT_BORD, pos_.y_ - UP_BORD); //printf ("last is changed\n");

        if (CURR_MODE == DRAW_MODE)
        {
            ___CURR_TOOL___ -> use (pos_, size_);
        }

        if (CURR_MODE == PLUG_MODE)
        {
            use_plugin ();
        }

        txBitBlt (copy_, 0, 0, size_.x_ + LEFT_BORD + RIGHT_BORD, size_.y_ + UP_BORD + DOWN_BORD,
                  txDC (), pos_.x_ - LEFT_BORD, pos_.y_ - UP_BORD);

        //printf ("I have just copied to CompatDC\n");

        //txCircle (txMouseX (), txMouseY (), 50);
    }

    //printf ("    I was in paint_on_mouse \n");
}

void client_area_t::on_paint () const
{
    txBitBlt (txDC (), pos_.x_ - LEFT_BORD, pos_.y_ - UP_BORD,
              size_.x_ + LEFT_BORD + RIGHT_BORD, size_.y_ + UP_BORD + DOWN_BORD, copy_, 0, 0);
}

void client_area_t::undo ()
{
    //printf ("pictures are going to be changed!\n");

    txBitBlt (copy_, 0, 0, size_.x_ + LEFT_BORD + RIGHT_BORD, size_.y_ + UP_BORD + DOWN_BORD,
              last_, 0, 0);

    txBitBlt (txDC (), pos_.x_ - LEFT_BORD, pos_.y_ - UP_BORD,
              size_.x_ + LEFT_BORD + RIGHT_BORD, size_.y_ + UP_BORD + DOWN_BORD, last_, 0, 0);
}

void paint_window_t::fill_array (tool_pictures p)
{
    add ( new client_area_t (pos_, size_, p, header_) ); const int client_i = 0;

    client_area_t* client_area = static_cast <client_area_t*> (windows_ [client_i]);

    add ( new close_button_t ( vector_t (pos_.x_ + size_.x_ + RIGHT_BORD - CLOSE_DIST,
                                         pos_.y_ - UP_BORD + Y_DIST                    ), this, p ) );

    add ( new undo_button_t ( vector_t (pos_.x_ + size_.x_ + RIGHT_BORD - UNDO_DIST,
                                        pos_.y_            -    UP_BORD +    Y_DIST), client_area, p) );

    add ( new move_button_t (this, p) );
}

paint_window_t::paint_window_t (vector_t pos, vector_t size, tool_pictures p, const char* header):

    abstract_manager_t (pos, size),

    header_ (header)

    {
        fill_array (p);
    }

void paint_window_t::on_mouse ()
{
    for (int i = 0; i < filled_; i++) windows_ [i] -> on_mouse ();
}

void paint_window_t::fill_color_array (GEPSB1Colour image [][MAX_IMAGE_SIZE])
{
    for (int iy = 0; iy < size_.y_; iy++)
    {
        for (int ix = 0; ix < size_.x_; ix++)
        {
            COLORREF colour = txGetPixel (pos_.x_ + ix, pos_.y_ + iy);

            GEPSB1Colour new_col ( txExtractColor (colour, TX_RED),
                                   txExtractColor (colour, TX_GREEN),
                                   txExtractColor (colour, TX_BLUE) );

            image [ix][iy] = new_col;
        }
    }
}

void paint_window_t::on_move (tool_pictures p)
{
    int start_x = txMouseX ();
    int start_y = txMouseY ();

    //$b printf ("I have counted start. It is (%d, %d)\n", start_x, start_y);

    HDC background = txCreateCompatibleDC (WINDOW_X, WINDOW_Y);

    txBitBlt (background, 0, 0, WINDOW_X, WINDOW_Y, txDC (), 0, 0);

    while (txMouseButtons () == 1)
    {
       txEnd ();

       txBitBlt (txDC (), 0, 0, WINDOW_X, WINDOW_Y, background, 0, 0);

       txSetColor     (TX_BLACK, 3);
       txSetFillColor (TX_NULL);

       txRectangle ( txMouseX () - start_x + pos_.x_, txMouseY () - start_y + pos_.y_,
                     txMouseX () - start_x + pos_.x_ + size_.x_,
                     txMouseY () - start_y + pos_.y_ + size_.y_ );

       txBegin ();

    }

    //printf ("I came in on_move\n");

    int end_x = txMouseX ();
    int end_y = txMouseY ();

    //printf ("I have counted end. It is (%d, %d)\n", end_x, end_y);

    int dist_x = end_x - start_x;
    int dist_y = end_y - start_y;

    //printf ("I have counted dist. It is (%d, %d)\n\n", dist_x, dist_y);

    //$g printf ("OLD_POS (%g, %g)\n", pos_.x_, pos_.y_);

    pos_.x_ += dist_x;
    pos_.y_ += dist_y;

    //$r printf ("NEW_POS (%g, %g)\n\n", pos_.x_, pos_.y_);
}

wnd_color_but::wnd_color_but (vector_t pos = vector_t (0, 0), UINT color = TX_WHITE, bool chosen = false) :

    abstract_window_t (pos, vector_t (COL_WIDTH, COL_HEIGHT)),
     color_           (color),
    chosen_           (chosen)

    {}

void wnd_color_but::on_paint () const
{
         if (color_ == TX_WHITE  && chosen_ == true) txSetColor (TX_BLACK);
    else if (color_ == TX_YELLOW && chosen_ == true) txSetColor (TX_BLACK);
    else if (                       chosen_ == true) txSetColor (TX_WHITE);
    else
        txSetColor (color_);

    txSetFillColor (color_);

    txRectangle (pos_.x_, pos_.y_, pos_.x_ + size_.x_, pos_.y_ + size_.y_);
}

void wnd_color_but::on_mouse ()
{
    if (on_click_test () == true)
    {
            chosen_ = true;
        ___COLOR___ = color_;
    }
}

wnd_tool::wnd_tool (vector_t pos = vector_t (0, 0),  HDC tool_pic = txLoadImage ("Resourses/circle_tool.bmp"), bool chosen = false):

    abstract_window_t ( pos, vector_t (TOOL_WIDTH, TOOL_HEIGHT) ),

    tool_pic_ ( tool_pic ),
      chosen_ ( chosen )

    {}

void wnd_tool::on_paint () const
{
    txBitBlt (txDC (), pos_.x_, pos_.y_, size_.x_, size_.y_, tool_pic_, 0, 0);

    if (chosen_ == true)
    {
        SetColors   (TX_BLACK, TX_TRANSPARENT);

        txRectangle (pos_.x_,            pos_.y_,
                     pos_.x_ + size_.x_, pos_.y_ + size_.y_);
    }
}

void wnd_tool::on_mouse ()
{
    if (on_click_test ())
    {
        chosen_ = true;

        ___CURR_TOOL___ = this;

        CURR_MODE = DRAW_MODE;
        //printf ("currtool is changed.\n");
    }
}

circle_tool::circle_tool (vector_t pos, tool_pictures p, bool chosen):

    wnd_tool (pos, p.circle_tool, chosen)

    {
        assert (tool_pic_);
    }

void circle_tool::use (vector_t draw_pos, vector_t draw_size) const
{
    const int RAD = 3;

    int half_thick = PAINT_WND_THICK / 2;

    vector_t circle_pos (txMouseX (), txMouseY ());

    if (circle_pos.x_ + RAD >= draw_pos.x_ + draw_size.x_ - half_thick)
    {
        circle_pos.x_ = draw_pos.x_ + draw_size.x_ - half_thick - RAD;
    }

    if (circle_pos.y_ + RAD >= draw_pos.y_ + draw_size.y_ - half_thick)
    {
        circle_pos.y_ = draw_pos.y_ + draw_size.y_ - half_thick - RAD;
    }

    if (circle_pos.x_ - RAD <= draw_pos.x_ + half_thick)
    {
        circle_pos.x_ = draw_pos.x_ + half_thick + RAD;
    }

    if (circle_pos.y_ - RAD <= draw_pos.y_ + half_thick)
    {
        circle_pos.y_ = draw_pos.y_ + half_thick + RAD;
    }

    SetColors (___COLOR___, ___COLOR___);
    txCircle  (circle_pos.x_, circle_pos.y_, RAD);

    txEnd (); txBegin ();

    while (circle_pos.x_ == txMouseX () && circle_pos.y_ == txMouseY () &&
           txMouseButtons () == 1);

    //txEnd (); txBegin (); // something wonderful that makes prog more comfortable
    //while (txMouseButtons () == 1);
}

rect_tool::rect_tool (vector_t pos, tool_pictures p, bool chosen):

    wnd_tool (pos, p.rect_tool, chosen)

    {
        assert (tool_pic_);
    }

void rect_tool::use (vector_t draw_pos, vector_t draw_size) const
{
    int half_thick = PAINT_WND_THICK / 2;

    vector_t start (txMouseX (), txMouseY ());
    
    double end_x = start.x_;
    double end_y = start.y_;
    
    HDC background = txCreateCompatibleDC (WINDOW_X, WINDOW_Y);
    
    txBitBlt (background, 0, 0, WINDOW_X, WINDOW_Y, txDC (), 0, 0);
    
    txSetColor     (TX_BLACK, 3);
    txSetFillColor (___COLOR___);
    
    while (txMouseButtons () == 1)
    {
        txEnd ();  
          
        end_x = txMouseX ();
        end_y = txMouseY ();
          
        if (end_x >= draw_pos.x_ + draw_size.x_ - half_thick) end_x = draw_pos.x_ + draw_size.x_ - half_thick;
        if (end_y >= draw_pos.y_ + draw_size.y_ - half_thick) end_y = draw_pos.y_ + draw_size.y_ - half_thick;
        if (end_x <= draw_pos.x_                + half_thick) end_x = draw_pos.x_                + half_thick;
        if (end_y <= draw_pos.y_                + half_thick) end_y = draw_pos.y_                + half_thick;
        
        //cout << "I'm going to BitBlt" << endl;
        
        txBitBlt (txDC (), 0, 0, WINDOW_X, WINDOW_Y, background, 0, 0);
        
        //cout << "I'm going to put rectangle" << endl;
        
        txRectangle (start.x_, start.y_, (int)end_x, (int)end_y);
        
        //cout << "I have drawn" << endl;
        
        txBegin ();             
    }
    
    //txBitBlt (txDC (), 0, 0, WINDOW_X, WINDOW_Y, background, 0, 0);
        
    //txRectangle (start.x_, start.y_, (int)end_x, (int)end_y);
    
    //cout << "Do you see the rectangle?";
    
    txDeleteDC (background);
}

fill_tool::fill_tool (vector_t pos, tool_pictures p, bool chosen):

    wnd_tool (pos, p.fill_tool, chosen)

    {
        assert (tool_pic_);
    }

void fill_tool::use (vector_t draw_pos, vector_t draw_size) const
{
    SetColors (___COLOR___, ___COLOR___);

    txFloodFill (txMouseX (), txMouseY ());

    txEnd (); txBegin ();
    while (txMouseButtons () == 1);
}

ellipse_tool::ellipse_tool (vector_t pos, tool_pictures p, bool chosen):

    wnd_tool (pos, p.ellipse_tool, chosen)

    {
        assert (tool_pic_);
    }

void ellipse_tool::use (vector_t draw_pos, vector_t draw_size) const
{
    int half_thick = PAINT_WND_THICK / 2;

    vector_t start (txMouseX (), txMouseY ());
    vector_t end   (start);
    
    txSetColor     (TX_BLACK, 3);
    txSetFillColor (___COLOR___);
    
    HDC background = txCreateCompatibleDC (WINDOW_X, WINDOW_Y);
    
    txBitBlt (background, 0, 0, WINDOW_X, WINDOW_Y, txDC (), 0, 0);
    
    while (txMouseButtons () == 1)
    {
        txEnd ();  
          
        end = vector_t (txMouseX (), txMouseY ());
        
        txBitBlt (txDC (), 0, 0, WINDOW_X, WINDOW_Y, background, 0, 0);
        
        if (end.x_ >= draw_pos.x_ + draw_size.x_ - half_thick) end.x_ = draw_pos.x_ + draw_size.x_ - half_thick;
        if (end.y_ >= draw_pos.y_ + draw_size.y_ - half_thick) end.y_ = draw_pos.y_ + draw_size.y_ - half_thick;
        if (end.x_ <= draw_pos.x_                + half_thick) end.x_ = draw_pos.x_                + half_thick;
        if (end.y_ <= draw_pos.y_                + half_thick) end.y_ = draw_pos.y_                + half_thick;
        
        txEllipse (start.x_, start.y_, end.x_, end.y_);
        
        txBegin ();         
    }
    
    txDeleteDC (background);
}

void wnd_palett::fill_array (UINT colors [], tool_pictures p)
{
    int pos = 0;

    for (int i = 0; i < N_COLORS / 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            add ( new wnd_color_but ( vector_t (pos_.x_ + j * COL_WIDTH, pos_.y_ + i * COL_HEIGHT),
                                      colors [pos], false ) );

            //printf ("%d items added\n", pos + 1);

            pos++;
        }
    }

    //printf ("%d items added\n", pos + 1);

    //$g printf ("%d is filled\n\n", filled_);

    wnd_color_but* default_col_but = static_cast <wnd_color_but*> (windows_ [0]);

                  default_col_but -> set_chosen (true);
    ___COLOR___ = default_col_but -> get_color  ();

    //$g printf ("%d is filled\n\n", filled_);
}

wnd_palett::wnd_palett (vector_t pos, UINT colors [], tool_pictures p) :

    abstract_manager_t (pos, vector_t ( COL_WIDTH * 2, COL_HEIGHT * (N_COLORS / 2) ))

    { fill_array (colors, p); }

void wnd_palett::on_mouse ()
{
    if (on_click_test ())
    {
        for (int j = 0; j < N_COLORS; j++)
        {
            wnd_color_but* current_button = static_cast <wnd_color_but*> (windows_ [j]);

            current_button -> set_chosen (false);
        }

        //$g printf ("%d is filled\n\n", filled_);

        for (int i = 0; i < N_COLORS; i++) windows_ [i] -> on_mouse ();

        //$g printf ("%d is filled\n\n", filled_);
    }
}

void tool_bar::fill_array (tool_pictures p)
{
    int i = 0;

    add ( new    circle_tool (           pos_                                        , p,  true  ) ); i++;
    add ( new      rect_tool ( vector_t (pos_.x_ + TOOL_WIDTH, pos_.y_              ), p,  false ) ); i++;
    add ( new      fill_tool ( vector_t (pos_.x_,              pos_.y_ + TOOL_HEIGHT), p,  false ) ); i++;
    add ( new   ellipse_tool ( vector_t (pos_.x_ + TOOL_WIDTH, pos_.y_ + TOOL_HEIGHT), p,  false ) ); i++;

    assert (i == N_TOOLS);

    wnd_tool* default_tool = static_cast <wnd_tool*> (windows_ [0]);

    assert (default_tool);

    ___CURR_TOOL___ = default_tool; //printf ("currtool is not NULL\n");
}

tool_bar::tool_bar (vector_t pos, tool_pictures p):

    abstract_manager_t (pos, vector_t (0, 0))

    {
        size_ = vector_t (            2             * TOOL_WIDTH,
                           (int)((N_TOOLS + 1) / 2) * TOOL_HEIGHT );

        fill_array (p);
    }

void tool_bar::on_mouse ()
{
    if (on_click_test ())
    {
        for (int i = 0; i < N_TOOLS; i++)
        {
            wnd_tool* investigated = static_cast <wnd_tool*> (windows_ [i]);

            investigated -> set_chosen (false);
        }

        for (int j = 0; j < N_TOOLS; j++)
        {
            windows_ [j] -> on_mouse ();
        }
    }
}

abstract_plugin_t::abstract_plugin_t (vector_t pos, vector_t size,
                                      char* path, HDC plugin_pic, bool chosen):

    abstract_window_t (pos, size),

    path_to_plugin_ (path),
    plugin_pic_     (plugin_pic),
    chosen_         (chosen)

    {}

void abstract_plugin_t::on_paint () const
{
    txBitBlt (txDC (), pos_.x_, pos_.y_, size_.x_, size_.y_, plugin_pic_, 0, 0);

    txSetColor     (TX_WHITE, 2);
    txSetFillColor (TX_NULL);

    if (chosen_ == true)
    {
        SetColors (TX_BLACK, TX_NULL);

        txRectangle (pos_.x_, pos_.y_, pos_.x_ + size_.x_, pos_.y_ + size_.y_);
    }
}

void abstract_plugin_t::on_mouse ()
{
    if (on_click_test ())
    {
        chosen_ = true;
        
        cout << "I'm going to change CURRENT_PLUGIN_PATH to " << path_to_plugin_ << endl;
        
        CURRENT_PLUGIN_PATH = path_to_plugin_;
        
        cout << "CURRENT_PLUGIN_PATH is changed. Now it is " << CURRENT_PLUGIN_PATH << endl;
        
        //puts (CURRENT_PLUGIN_PATH);

        CURR_MODE = PLUG_MODE;
    }
}

void plugin_manager_t::fill_array ()
{
    vector <string> modules;
    vector <string> hdcs;

    char* path = "GEPSB1Plugins.txt";

    if (SDK == FYODOR) path = "FYODORPlugins.txt";

    FillArrays ((const char*)(path), &modules, &hdcs);

    short pos_x = (short int)(pos_.x_);

    unsigned int n_filters = modules.size ();

    const int DEFAULT_BUT_SIZE_X = 150;

    for (int i = 0; i < n_filters; i++)
    {
        const char* bmp_name = hdcs [i].c_str ();

        //cout << bmp_name << endl;

        //cout << "I'm going to put plugin " << modules [i] << " in pos " << pos_x << endl;

        HDC image = txLoadImage (bmp_name);

        if (image == NULL)
        {
            txDeleteDC (image);

            HDC background = txCreateCompatibleDC (WINDOW_X, WINDOW_Y);

            txBitBlt (background, 0, 0, WINDOW_X, WINDOW_Y, txDC (), 0, 0);

            HDC button = txCreateCompatibleDC (DEFAULT_BUT_SIZE_X, TOOL_HEIGHT);

            Clear (TX_WHITE);

            SetColors   (TX_WHITE, TX_WHITE);
            txRectangle (0, 0, DEFAULT_BUT_SIZE_X, TOOL_HEIGHT);

            string suff (".dll");
            string pref ("Plugins/");

            size_t fullname_size = modules [i].size ();

            //cout << "Size of " << modules [i] << " is " << modules [i].size () << endl;

            string name = modules [i].substr (pref.size (), fullname_size - suff.size ()
                                                                          - pref.size ());

            //cout << name << endl;

            txSetColor   (TX_BLACK);
            txSelectFont ("Times New Roman", 25);

            txDrawText (0, 0, DEFAULT_BUT_SIZE_X, TOOL_HEIGHT, name.c_str ());

            txBitBlt (button, 0, 0, DEFAULT_BUT_SIZE_X, TOOL_HEIGHT, txDC (), 0, 0);

            txBitBlt (txDC (), 0, 0, WINDOW_X, WINDOW_Y, background, 0, 0);

            txDeleteDC (background);

            add ( new abstract_plugin_t ( vector_t ( pos_x, pos_.y_ ),
                                          vector_t (DEFAULT_BUT_SIZE_X, TOOL_HEIGHT),
                                          (char*)(modules [i].c_str ()), button, false ) );

            pos_x += DEFAULT_BUT_SIZE_X;
        }
        else
        {
            $g
            
            const char* path = modules [i].c_str ();
            
            add ( new abstract_plugin_t ( vector_t (pos_x, pos_.y_),
                                          vector_t (TOOL_WIDTH, TOOL_HEIGHT),
                                          (char*)(path), image, false ));
            
            abstract_plugin_t* plugin = static_cast <abstract_plugin_t*> (windows_ [filled_ - 1]);
                                          
            cout << "I have set plugin. It's path is " << plugin -> get_path () << endl;

            pos_x += TOOL_WIDTH;
        }
    }

    abstract_plugin_t* default_plug = static_cast <abstract_plugin_t*> (windows_ [0]);

    assert (default_plug);

    CURRENT_PLUGIN_PATH = default_plug -> get_path   ();
                          default_plug -> set_chosen (true);

    //cout << CURRENT_PLUGIN_PATH << endl;
}

plugin_manager_t::plugin_manager_t (vector_t pos):

    abstract_manager_t (pos, vector_t (TOOL_WIDTH * N_FILTERS, TOOL_HEIGHT))

    {
        fill_array ();
    }

void plugin_manager_t::on_mouse ()
{
    if (on_click_test ())
    {
        for (int j = 0; j < filled_; j++)
        {
            abstract_plugin_t* investigated = static_cast <abstract_plugin_t*> (windows_ [j]);

            investigated -> set_chosen (false);
        }

        for (int i = 0; i < filled_; i++)
        {
            abstract_plugin_t* investigated = static_cast <abstract_plugin_t*> (windows_ [i]);
            
            cout << "I'm going to call on_mouse of plugin whic path is " << investigated -> get_path ();
            
            windows_ [i] -> on_mouse ();
        }
     }
}

move_button_t::move_button_t (paint_window_t* to_move, tool_pictures p):

    abstract_window_t (vector_t (0, 0), vector_t (0, 0)),

    to_move_ (to_move),
          p_ (p)

    {}

bool move_button_t::on_click_test () const
{
    vector_t par_pos  = to_move_ -> get_pos  ();
    vector_t par_size = to_move_ -> get_size ();

    if (txMouseButtons () == 1)
    {
        if (mouse_on (vector_t (par_pos.x_ - LEFT_BORD, par_pos.y_ - UP_BORD),
                      vector_t (par_pos.x_ + par_size.x_ + LEFT_BORD + RIGHT_BORD - 300,
                                par_pos.y_ + par_size.y_ +   UP_BORD +  LEFT_BORD)))
        {
            if (!mouse_on (par_pos, par_size))
            {
                return true;
            }
            else return false;
        }
        else return false;
    }
    else return false;
}

void move_button_t::on_mouse ()
{
    if (on_click_test ())
    {
        const int CLIENT_AREA_I = 0;

        HDC last_copy = txCreateCompatibleDC (to_move_ -> get_size ().x_, to_move_ -> get_size ().y_);

        client_area_t* moving  = static_cast <client_area_t*> (to_move_ -> windows_ [CLIENT_AREA_I]);

        txBitBlt (last_copy, 0, 0, to_move_ -> get_size ().x_, to_move_ -> get_size ().y_,
                  moving -> get_last (),
                  0, 0);

        HDC copy_copy = txCreateCompatibleDC (to_move_ -> get_size ().x_, to_move_ -> get_size ().y_);

        txBitBlt (copy_copy, 0, 0, to_move_ -> get_size ().x_, to_move_ -> get_size ().y_,
                  moving -> get_copy (),
                  0, 0);

        to_move_ -> on_move    (p_);
        to_move_ -> remove_all (  );
        to_move_ -> fill_array (p_);

        moving = static_cast <client_area_t*> (to_move_ -> windows_ [CLIENT_AREA_I]);

        txBitBlt (moving -> get_copy (),
                  0, 0, to_move_ -> get_size ().x_, to_move_ -> get_size ().y_,
                  copy_copy, 0, 0);

        txBitBlt (moving -> get_last (),
                  0, 0, to_move_ -> get_size ().x_, to_move_ -> get_size ().y_,
                  last_copy, 0, 0);

        txDeleteDC (copy_copy);
        txDeleteDC (last_copy);
    }
}

