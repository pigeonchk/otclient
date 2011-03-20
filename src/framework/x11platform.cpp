/* The MIT License
 *
 * Copyright (c) 2010 OTClient, https://github.com/edubart/otclient
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include "platform.h"
#include "engine.h"
#include "input.h"
#include "logger.h"

#include <sys/time.h>
#include <sys/stat.h>
#include <errno.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <GL/glx.h>
#include <physfs.h>

struct X11PlatformPrivate {
    Display *display;
    XVisualInfo *visual;
    GLXContext glxContext;
    XIM xim;
    XIC xic;
    Colormap colormap;
    Window window;
    Cursor cursor;
    Atom atomDeleteWindow;
    Atom atomClipboard;
    Atom atomTargets;
    Atom atomText;
    Atom atomCompoundText;
    Atom atomUTF8String;
    bool visible;
    bool focused;
    int width;
    int height;
    std::string clipboardText;
    std::map<int, unsigned char> keyMap;
} x11;

void Platform::init()
{
    x11.display = NULL;
    x11.visual = NULL;
    x11.glxContext = NULL;
    x11.xim = NULL;
    x11.xic = NULL;
    x11.colormap = None;
    x11.window = None;
    x11.cursor = None;
    x11.visible = false;
    x11.focused = false;
    x11.width = 0;
    x11.height = 0;

    // setup keymap
    x11.keyMap[XK_1] = KC_1;
    x11.keyMap[XK_2] = KC_2;
    x11.keyMap[XK_3] = KC_3;
    x11.keyMap[XK_4] = KC_4;
    x11.keyMap[XK_5] = KC_5;
    x11.keyMap[XK_6] = KC_6;
    x11.keyMap[XK_7] = KC_7;
    x11.keyMap[XK_8] = KC_8;
    x11.keyMap[XK_9] = KC_9;
    x11.keyMap[XK_0] = KC_0;

    x11.keyMap[XK_BackSpace] = KC_BACK;

    x11.keyMap[XK_minus] = KC_MINUS;
    x11.keyMap[XK_equal] = KC_EQUALS;
    x11.keyMap[XK_space] = KC_SPACE;
    x11.keyMap[XK_comma] = KC_COMMA;
    x11.keyMap[XK_period] = KC_PERIOD;

    x11.keyMap[XK_backslash] = KC_BACKSLASH;
    x11.keyMap[XK_slash] = KC_SLASH;
    x11.keyMap[XK_bracketleft] = KC_LBRACKET;
    x11.keyMap[XK_bracketright] = KC_RBRACKET;

    x11.keyMap[XK_Escape] = KC_ESCAPE;
    x11.keyMap[XK_Caps_Lock] = KC_CAPITAL;

    x11.keyMap[XK_Tab] = KC_TAB;
    x11.keyMap[XK_Return] = KC_RETURN;
    x11.keyMap[XK_Control_L] = KC_LCONTROL;
    x11.keyMap[XK_Control_R] = KC_RCONTROL;

    x11.keyMap[XK_colon] = KC_COLON;
    x11.keyMap[XK_semicolon] = KC_SEMICOLON;
    x11.keyMap[XK_apostrophe] = KC_APOSTROPHE;
    x11.keyMap[XK_grave] = KC_GRAVE;

    x11.keyMap[XK_b] = KC_B;
    x11.keyMap[XK_a] = KC_A;
    x11.keyMap[XK_c] = KC_C;
    x11.keyMap[XK_d] = KC_D;
    x11.keyMap[XK_e] = KC_E;
    x11.keyMap[XK_f] = KC_F;
    x11.keyMap[XK_g] = KC_G;
    x11.keyMap[XK_h] = KC_H;
    x11.keyMap[XK_i] = KC_I;
    x11.keyMap[XK_j] = KC_J;
    x11.keyMap[XK_k] = KC_K;
    x11.keyMap[XK_l] = KC_L;
    x11.keyMap[XK_m] = KC_M;
    x11.keyMap[XK_n] = KC_N;
    x11.keyMap[XK_o] = KC_O;
    x11.keyMap[XK_p] = KC_P;
    x11.keyMap[XK_q] = KC_Q;
    x11.keyMap[XK_r] = KC_R;
    x11.keyMap[XK_s] = KC_S;
    x11.keyMap[XK_t] = KC_T;
    x11.keyMap[XK_u] = KC_U;
    x11.keyMap[XK_v] = KC_V;
    x11.keyMap[XK_w] = KC_W;
    x11.keyMap[XK_x] = KC_X;
    x11.keyMap[XK_y] = KC_Y;
    x11.keyMap[XK_z] = KC_Z;

    x11.keyMap[XK_F1] = KC_F1;
    x11.keyMap[XK_F2] = KC_F2;
    x11.keyMap[XK_F3] = KC_F3;
    x11.keyMap[XK_F4] = KC_F4;
    x11.keyMap[XK_F5] = KC_F5;
    x11.keyMap[XK_F6] = KC_F6;
    x11.keyMap[XK_F7] = KC_F7;
    x11.keyMap[XK_F8] = KC_F8;
    x11.keyMap[XK_F9] = KC_F9;
    x11.keyMap[XK_F10] = KC_F10;
    x11.keyMap[XK_F11] = KC_F11;
    x11.keyMap[XK_F12] = KC_F12;
    x11.keyMap[XK_F13] = KC_F13;
    x11.keyMap[XK_F14] = KC_F14;
    x11.keyMap[XK_F15] = KC_F15;

    // keypad
    x11.keyMap[XK_KP_0] = KC_NUMPAD0;
    x11.keyMap[XK_KP_1] = KC_NUMPAD1;
    x11.keyMap[XK_KP_2] = KC_NUMPAD2;
    x11.keyMap[XK_KP_3] = KC_NUMPAD3;
    x11.keyMap[XK_KP_4] = KC_NUMPAD4;
    x11.keyMap[XK_KP_5] = KC_NUMPAD5;
    x11.keyMap[XK_KP_6] = KC_NUMPAD6;
    x11.keyMap[XK_KP_7] = KC_NUMPAD7;
    x11.keyMap[XK_KP_8] = KC_NUMPAD8;
    x11.keyMap[XK_KP_9] = KC_NUMPAD9;
    x11.keyMap[XK_KP_Add] = KC_ADD;
    x11.keyMap[XK_KP_Subtract] = KC_SUBTRACT;
    x11.keyMap[XK_KP_Decimal] = KC_DECIMAL;
    x11.keyMap[XK_KP_Equal] = KC_NUMPADEQUALS;
    x11.keyMap[XK_KP_Divide] = KC_DIVIDE;
    x11.keyMap[XK_KP_Multiply] = KC_MULTIPLY;
    x11.keyMap[XK_KP_Enter] = KC_NUMPADENTER;

    // keypad with numlock off
    x11.keyMap[XK_KP_Home] = KC_NUMPAD7;
    x11.keyMap[XK_KP_Up] = KC_NUMPAD8;
    x11.keyMap[XK_KP_Page_Up] = KC_NUMPAD9;
    x11.keyMap[XK_KP_Left] = KC_NUMPAD4;
    x11.keyMap[XK_KP_Begin] = KC_NUMPAD5;
    x11.keyMap[XK_KP_Right] = KC_NUMPAD6;
    x11.keyMap[XK_KP_End] = KC_NUMPAD1;
    x11.keyMap[XK_KP_Down] = KC_NUMPAD2;
    x11.keyMap[XK_KP_Page_Down] = KC_NUMPAD3;
    x11.keyMap[XK_KP_Insert] = KC_NUMPAD0;
    x11.keyMap[XK_KP_Delete] = KC_DECIMAL;

    x11.keyMap[XK_Up] = KC_UP;
    x11.keyMap[XK_Down] = KC_DOWN;
    x11.keyMap[XK_Left] = KC_LEFT;
    x11.keyMap[XK_Right] = KC_RIGHT;

    x11.keyMap[XK_Page_Up] = KC_PGUP;
    x11.keyMap[XK_Page_Down] = KC_PGDOWN;
    x11.keyMap[XK_Home] = KC_HOME;
    x11.keyMap[XK_End] = KC_END;

    x11.keyMap[XK_Num_Lock] = KC_NUMLOCK;
    x11.keyMap[XK_Print] = KC_SYSRQ;
    x11.keyMap[XK_Scroll_Lock] = KC_SCROLL;
    x11.keyMap[XK_Pause] = KC_PAUSE;

    x11.keyMap[XK_Shift_R] = KC_RSHIFT;
    x11.keyMap[XK_Shift_L] = KC_LSHIFT;
    x11.keyMap[XK_Alt_R] = KC_RALT;
    x11.keyMap[XK_Alt_L] = KC_LALT;

    x11.keyMap[XK_Insert] = KC_INSERT;
    x11.keyMap[XK_Delete] = KC_DELETE;

    x11.keyMap[XK_Super_L] = KC_LWIN;
    x11.keyMap[XK_Super_R] = KC_RWIN;
    x11.keyMap[XK_Menu] = KC_APPS;

    // try to set a latin1 locales otherwise fallback to standard C locale
    static char locales[][32] = { "en_US.iso88591", "iso88591", "en_US", "C" };
    for(int i=0;i<4;++i) {
        if(setlocale(LC_ALL, locales[i]))
            break;
    }

    // open display
    x11.display = XOpenDisplay(0);
    if(!x11.display)
        fatal("Failed to open X display");

    // check if GLX is supported on this display
    if(!glXQueryExtension(x11.display, 0, 0))
        fatal("GLX not supported");

    // retrieve GLX version
    int glxMajor;
    int glxMinor;
    if(!glXQueryVersion(x11.display, &glxMajor, &glxMinor))
        fatal("Unable to query GLX version");
    notice("GLX version %d.%d", glxMajor, glxMinor);

    // clipboard related atoms
    x11.atomClipboard = XInternAtom(x11.display, "CLIPBOARD", False);
    x11.atomTargets = XInternAtom(x11.display, "TARGETS", False);
    x11.atomUTF8String = XInternAtom(x11.display, "UTF8_STRING", False);
    x11.atomText = XInternAtom(x11.display, "TEXT", False);
    x11.atomCompoundText = XInternAtom(x11.display, "COMPOUND_TEXT", False);
}

void Platform::terminate()
{
    if(x11.window) {
        destroyWindow();
        x11.window = None;
    }

    // close display
    if(x11.display) {
        XCloseDisplay(x11.display);
        x11.display = NULL;
    }
}

void Platform::poll()
{
    XEvent event, peekevent;
    static InputEvent inputEvent;
    while(XPending(x11.display) > 0) {
        XNextEvent(x11.display, &event);

        // call filter because xim will discard KeyPress events when keys still composing
        if(XFilterEvent(&event, x11.window))
            continue;

        // discard events of repeated key releases
        if(event.type == KeyRelease && XPending(x11.display)) {
            XPeekEvent(x11.display, &peekevent);
            if((peekevent.type == KeyPress) &&
              (peekevent.xkey.keycode == event.xkey.keycode) &&
              ((peekevent.xkey.time-event.xkey.time) < 2))
                continue;
        }

        switch(event.type) {
            case ConfigureNotify:
                // window resize
                if(x11.width != event.xconfigure.width || x11.height != event.xconfigure.height) {
                    x11.width = event.xconfigure.width;
                    x11.height = event.xconfigure.height;
                    g_engine.onResize(x11.width, x11.height);
                }
                break;

            case KeyPress:
            case KeyRelease: {
                KeySym keysym;
                char buf[32];
                int len;

                inputEvent.key.ctrl = (event.xkey.state & ControlMask);
                inputEvent.key.shift = (event.xkey.state & ShiftMask);
                inputEvent.key.alt = (event.xkey.state & Mod1Mask);

                // fire enter text event
                if(event.type == KeyPress && !inputEvent.key.ctrl && !inputEvent.key.alt) {
                    if(x11.xic) { // with xim we can get latin1 input correctly
                        Status status;
                        len = XmbLookupString(x11.xic, &event.xkey, buf, sizeof(buf), &keysym, &status);
                    } else { // otherwise use XLookupString, but it doesn't work right with dead keys often
                        static XComposeStatus compose = {NULL, 0};
                        len = XLookupString(&event.xkey, buf, sizeof(buf), &keysym, &compose);
                    }
                    if(len > 0 &&
                       // for some reason these keys produces characters and we don't want that
                       keysym != XK_BackSpace &&
                       keysym != XK_Return &&
                       keysym != XK_Delete &&
                       keysym != XK_Escape
                    ) {
                        //debug("char: %c code: %d", buf[0], (unsigned char)buf[0]);
                        inputEvent.type = EV_TEXT_ENTER;
                        inputEvent.key.keychar = buf[0];
                        inputEvent.key.keycode = KC_UNKNOWN;
                        g_engine.onInputEvent(&inputEvent);
                    }
                }

                // unmask Shift/Lock to get expected results
                event.xkey.state &= ~(ShiftMask | LockMask);
                len = XLookupString(&event.xkey, buf, sizeof(buf), &keysym, 0);

                // fire key up/down event
                if(x11.keyMap.find(keysym) != x11.keyMap.end()) {
                    inputEvent.key.keycode = x11.keyMap[keysym];
                    inputEvent.type = (event.type == KeyPress) ? EV_KEY_DOWN : EV_KEY_UP;
                    inputEvent.key.keychar = (len > 0) ? buf[0] : 0;
                    g_engine.onInputEvent(&inputEvent);
                }
                break;
            }
            case ButtonPress:
            case ButtonRelease:
                switch(event.xbutton.button) {
                    case Button1:
                        inputEvent.type = (event.type == ButtonPress) ? EV_MOUSE_LDOWN : EV_MOUSE_LUP;
                        break;
                    case Button3:
                        inputEvent.type = (event.type == ButtonPress) ? EV_MOUSE_RDOWN : EV_MOUSE_RUP;
                        break;
                    case Button2:
                        inputEvent.type = (event.type == ButtonPress) ? EV_MOUSE_MDOWN : EV_MOUSE_MUP;
                        break;
                    case Button4:
                        inputEvent.type = EV_MOUSE_WHEEL_UP;
                        break;
                    case Button5:
                        inputEvent.type = EV_MOUSE_WHEEL_DOWN;
                        break;
                }
                g_engine.onInputEvent(&inputEvent);
                break;

            case MotionNotify:
                inputEvent.type = EV_MOUSE_MOVE;
                inputEvent.mouse.x = event.xbutton.x;
                inputEvent.mouse.y = event.xbutton.y;
                g_engine.onInputEvent(&inputEvent);
                break;

            case MapNotify:
                x11.visible = true;
                break;

            case UnmapNotify:
                x11.visible = false;
                break;

            case FocusIn:
                x11.focused = true;
                break;

            case FocusOut:
                x11.focused = false;
                break;

            // clipboard data request
            case SelectionRequest:
            {
                XEvent respond;
                XSelectionRequestEvent *req = &(event.xselectionrequest);

                if(req->target == x11.atomTargets ) {
                    Atom typeList[] = {x11.atomText, x11.atomCompoundText, x11.atomUTF8String, XA_STRING};

                    XChangeProperty(x11.display, req->requestor,
                                    req->property, req->target,
                                    8, PropModeReplace,
                                    (unsigned char *) &typeList,
                                    sizeof(typeList));
                    respond.xselection.property = req->property;
                } else {
                    XChangeProperty(x11.display,
                                    req->requestor,
                                    req->property, req->target,
                                    8,
                                    PropModeReplace,
                                    (unsigned char*) x11.clipboardText.c_str(),
                                    x11.clipboardText.size());
                    respond.xselection.property = req->property;
                }

                respond.xselection.type = SelectionNotify;
                respond.xselection.display = req->display;
                respond.xselection.requestor = req->requestor;
                respond.xselection.selection = req->selection;
                respond.xselection.target = req->target;
                respond.xselection.time = req->time;
                XSendEvent(x11.display, req->requestor, 0, 0, &respond);
                XFlush(x11.display);
                break;
            }

            case ClientMessage:
            {
                if((Atom)event.xclient.data.l[0] == x11.atomDeleteWindow)
                    g_engine.onClose();
                break;
            }
        }
    }
}

unsigned long Platform::getTicks()
{
    static timeval tv;
    static unsigned long firstTick = 0;

    gettimeofday(&tv, 0);
    if(!firstTick)
        firstTick = tv.tv_sec;

    return ((tv.tv_sec - firstTick) * 1000) + (tv.tv_usec / 1000);
}

void Platform::sleep(unsigned long miliseconds)
{
    timespec tv;
    tv.tv_sec  = miliseconds / 1000;
    tv.tv_nsec = (miliseconds % 1000) * 1000000;
    nanosleep(&tv, NULL);
}

bool Platform::createWindow(int x, int y, int width, int height, int minWidth, int minHeight, bool maximized)
{
    static int attrList[] = {
        GLX_USE_GL,
        GLX_RGBA,
        GLX_DOUBLEBUFFER,
        None
    };

    // choose OpenGL, RGBA, double buffered, visual
    x11.visual = glXChooseVisual(x11.display, DefaultScreen(x11.display), attrList);
    if(!x11.visual)
        fatal("RGBA/Double buffered visual not supported");

    // create GLX context
    x11.glxContext = glXCreateContext(x11.display, x11.visual, 0, GL_TRUE);
    if(!x11.glxContext)
        fatal("Unable to create GLX context");

    // color map
    x11.colormap  = XCreateColormap(x11.display,
                                  RootWindow(x11.display, x11.visual->screen),
                                  x11.visual->visual,
                                  AllocNone);

    // setup window attributes
    XSetWindowAttributes wa;
    wa.colormap = x11.colormap;
    wa.border_pixel = 0;
    wa.event_mask = KeyPressMask | KeyReleaseMask |
                         ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
                         ExposureMask | VisibilityChangeMask |
                         StructureNotifyMask | FocusChangeMask;

    // calculate center position
    //x = (XDisplayWidth(x11.display, DefaultScreen(x11.display)) - width) / 2;
    //y = (XDisplayHeight(x11.display, DefaultScreen(x11.display)) - height) / 2;

    // create the window
    x11.window = XCreateWindow(x11.display,
                             RootWindow(x11.display, x11.visual->screen),
                             x, y,
                             width, height,
                             0,
                             x11.visual->depth,
                             InputOutput,
                             x11.visual->visual,
                             CWBorderPixel | CWColormap | CWEventMask,
                             &wa);

    if(!x11.window)
        fatal("Unable to create X window");

    //  create input context (to have better key input handling)
    if(XSupportsLocale()) {
        XSetLocaleModifiers("");
        x11.xim = XOpenIM(x11.display, NULL, NULL, NULL);
        if(x11.xim) {
            x11.xic = XCreateIC(x11.xim,
                                    XNInputStyle,
                                    XIMPreeditNothing | XIMStatusNothing,
                                    XNClientWindow, x11.window, NULL);
            if(!x11.xic)
                error("Unable to create the input context");
        } else
            error("Failed to open an input method");
    } else
        error("X11 does not support the current locale");

    if(!x11.xic)
        warning("Input of special keys maybe messed up because we couldn't create an input context");


    // set window minimum size
    XSizeHints xsizehints;
    xsizehints.flags = PMinSize;
    xsizehints.min_width = minWidth;
    xsizehints.min_height= minHeight;
    XSetWMSizeHints(x11.display, x11.window, &xsizehints, XA_WM_NORMAL_HINTS);

    // handle delete window event
    x11.atomDeleteWindow = XInternAtom(x11.display, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(x11.display, x11.window, &x11.atomDeleteWindow , 1);

    // connect the GLX-context to the window
    glXMakeCurrent(x11.display, x11.window, x11.glxContext);

    x11.width = width;
    x11.height = height;
    return true;
}

void Platform::destroyWindow()
{
    if(x11.glxContext) {
        glXMakeCurrent(x11.display, None, NULL);
        glXDestroyContext(x11.display, x11.glxContext);
        x11.glxContext = NULL;
    }

    if(x11.visual) {
        XFree(x11.visual);
        x11.visual = NULL;
    }

    if(x11.colormap != None) {
        XFreeColormap(x11.display, x11.colormap);
        x11.colormap = 0;
    }

    if(x11.window != None) {
        XUnmapWindow(x11.display, x11.window);
        XDestroyWindow(x11.display, x11.window);
        x11.window = None;
    }

    if(x11.xic) {
        XDestroyIC(x11.xic);
        x11.xic = NULL;
    }

    if(x11.xim) {
        XCloseIM(x11.xim);
        x11.xim = NULL;
    }
}

void Platform::showWindow()
{
    XMapWindow(x11.display, x11.window);
}

void Platform::setWindowTitle(const char *title)
{
    XStoreName(x11.display, x11.window, title);
    XSetIconName(x11.display, x11.window, title);
}

void *Platform::getExtensionProcAddress(const char *ext)
{
    return (void*)glXGetProcAddressARB((const GLubyte*)ext);
}

bool Platform::isExtensionSupported(const char *ext)
{
    const char *exts = glXQueryExtensionsString(x11.display, DefaultScreen(x11.display));
    if(strstr(exts, ext))
        return true;
    return true;
}

const char *Platform::getTextFromClipboard()
{
    Window ownerWindow = XGetSelectionOwner(x11.display, x11.atomClipboard);
    if(ownerWindow ==  x11.window)
        return x11.clipboardText.c_str();

    std::string clipboard = "";
    if(ownerWindow != None) {
        XConvertSelection(x11.display, x11.atomClipboard, XA_STRING, 0, ownerWindow, CurrentTime);
        XFlush(x11.display);

        // hack to wait SelectioNotify event, otherwise we will get wrong clipboard pastes
        sleep(100);

        // check for data
        Atom type;
        int format;
        unsigned long numItems, bytesLeft, dummy;
        unsigned char *data;
        XGetWindowProperty(x11.display, ownerWindow,
                            XA_STRING,
                            0, 0, 0,
                            AnyPropertyType,
                            &type,
                            &format,
                            &numItems,
                            &bytesLeft,
                            &data);
        if(bytesLeft > 0)  {
            // get the data get
            int result = XGetWindowProperty(x11.display, ownerWindow,
                                            XA_STRING,
                                            0,
                                            bytesLeft,
                                            0,
                                            AnyPropertyType,
                                            &type,
                                            &format,
                                            &numItems,
                                            &dummy,
                                            &data);
            if(result == Success)
                clipboard = (const char*)data;
            XFree(data);
        }
    }
    return clipboard.c_str();
}

void Platform::copyToClipboard(const char *text)
{
    x11.clipboardText = text;
    XSetSelectionOwner(x11.display, x11.atomClipboard, x11.window, CurrentTime);
    XFlush(x11.display);
}

void Platform::hideMouseCursor()
{
    if(x11.cursor == None) {
        char bm[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
        Pixmap pix = XCreateBitmapFromData(x11.display, x11.window, bm, 8, 8);
        XColor black;
        memset(&black, 0, sizeof(XColor));
        black.flags = DoRed | DoGreen | DoBlue;
        x11.cursor = XCreatePixmapCursor(x11.display, pix, pix, &black, &black, 0, 0);
        XFreePixmap(x11.display, pix);
    }
    XDefineCursor(x11.display, x11.window, x11.cursor);
}

void Platform::showMouseCursor()
{
    XUndefineCursor(x11.display, x11.window);
    if(x11.cursor != None) {
        XFreeCursor(x11.display, x11.cursor);
        x11.cursor = None;
    }
}

void Platform::setVsync(bool enable)
{
    typedef GLint (*glSwapIntervalProc)(GLint);
    glSwapIntervalProc glSwapInterval = NULL;

    if(isExtensionSupported("GLX_MESA_swap_control"))
        glSwapInterval = (glSwapIntervalProc)getExtensionProcAddress("glXSwapIntervalMESA");
    else if(isExtensionSupported("GLX_SGI_swap_control"))
        glSwapInterval = (glSwapIntervalProc)getExtensionProcAddress("glXSwapIntervalSGI");

    if(glSwapInterval)
        glSwapInterval(enable ? 1 : 0);
}

void Platform::swapBuffers()
{
    glXSwapBuffers(x11.display, x11.window);
}

bool Platform::isWindowFocused()
{
    return x11.focused;
}

bool Platform::isWindowVisible()
{
    return x11.visible;
}

int Platform::getWindowWidth()
{
    return x11.width;
}

int Platform::getWindowHeight()
{
    return x11.height;
}

const char *Platform::getAppUserDir(const char *appName)
{
    std::stringstream sdir;
    sdir << PHYSFS_getUserDir() << "/." << appName << "/";
    if((mkdir(sdir.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) && (errno != EEXIST))
        error("Couldn't create directory for saving configuration file. (%s)", sdir.str().c_str());
    return sdir.str().c_str();
}
