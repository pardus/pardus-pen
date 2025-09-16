#include "../tools.h"

char* get_icon_by_id(int id){
    switch(id){
        case ICON:
            return (char*)":tr.org.pardus.pen.svg";
        case MARKER:
            return (char*)":images/tool/marker.svg";
        case PEN:
            return (char*)":images/tool/pen.svg";
        case ERASER:
            return (char*)":images/tool/eraser.svg";
        case CIRCLE:
            return (char*)":images/shape/circle.svg";
        case SPLINE:
            return (char*)":images/shape/spline.svg";
        case LINE:
            return (char*)":images/shape/line.svg";
        case TRIANGLE:
            return (char*)":images/shape/triangle.svg";
        case RECTANGLE:
            return (char*)":images/shape/rectangle.svg";
        case VECTOR:
            return (char*)":images/shape/vector.svg";
        case VECTOR2:
            return (char*)":images/shape/vector2.svg";
        case TRANSPARENT:
            return (char*)":images/shape/paper-transparent.svg";
        case BLACK:
            return (char*)":images/shape/paper-black.svg";
        case WHITE:
            return (char*)":images/shape/paper-white.svg";
        case PREVPAGE:
            return (char*)":images/page/go-back.svg";
        case NEXTPAGE:
            return (char*)":images/page/go-next.svg";
        case BLANK:
            return (char*)":images/overlay/none.svg";
        case SQUARES:
            return (char*)":images/overlay/squares.svg";
        case LINES:
            return (char*)":images/overlay/lines.svg";
        case ISOMETRIC:
            return (char*)":images/overlay/isometric.svg";
        case MUSIC:
            return (char*)":images/overlay/music.svg";
        case CUSTOM:
            return (char*)":images/overlay/custom.svg";
        case OVERLAYSCALEUP:
            return (char*)":images/page/zoom-in.svg";
        case OVERLAYSCALEDOWN:
            return (char*)":images/page/zoom-out.svg";
        case OVERLAYROTATEUP:
            return (char*)":images/page/rotate-plus.svg";
        case OVERLAYROTATEDOWN:
            return (char*)":images/page/rotate-minus.svg";
        case CLEAR:
            return (char*)":images/page/clear.svg";
        case SELECTION:
            return (char*)":images/tool/crop.svg";
        case NORMAL:
            return (char*)":images/shape/line-normal.svg";
        case DOTLINE:
            return (char*)":images/shape/line-dots.svg";
        case LINELINE:
            return (char*)":images/shape/line-lines.svg";
        case COLORPICKER:
            return (char*)":images/tool/color-picker.svg";
        case BACK:
            return (char*)":images/tool/go-back.svg";
        case NEXT:
            return (char*)":images/tool/go-next.svg";
        case CLOSE:
            return (char*)":images/page/close.svg";
        case SCREENSHOT:
            return (char*)":images/page/screenshot.svg";
        case CURSOR:
            return (char*)":images/cursor.svg";
        case MINIFY:
            return (char*)":images/tool/minify.svg";
        case FULLSCREEN:
            return (char*)":images/page/fullscreen.svg";
        case FULLSCREEN_EXIT:
            return (char*)":images/page/fullscreen-exit.svg";
        case ROTATE:
            return (char*)":images/tool/rotate.svg";
        case PAGEMENU:
            return (char*)":images/page/settings.svg";
        case UTILMENU:
            return (char*)":images/page/misc-settings.svg";
        case SAVE:
            return (char*)":images/page/file-save.svg";
        case OPEN:
            return (char*)":images/page/file-open.svg";
    }
    return (char*)"";
}

QString get_overlay_by_id(int id){
    switch(id){
        case TRANSPARENT:
            return "transparent";
        case WHITE:
            return "white";
        case BLACK:
            return "black";
        case BLANK:
            return "blank";
        case SQUARES:
            return "squares";
        case LINES:
            return "lines";
        case ISOMETRIC:
            return "isometric";
        case MUSIC:
            return "music";
        case CUSTOM:
            return "custom";
    }
    // Unknown overlay
    return (char*)"blank";
}

int get_id_by_overlay(QString foverlay){
    std::string overlayString = foverlay.toStdString();
    const char* overlay = overlayString.c_str();
    if(strcmp(overlay, "transparent") == 0) {
        return TRANSPARENT;
    } else if(strcmp(overlay, "white") == 0) {
        return WHITE;
    } else if(strcmp(overlay, "black") == 0) {
        return BLACK;
    } else if(strcmp(overlay, "blank") == 0) {
        return BLANK;
    } else if(strcmp(overlay, "squares") == 0) {
        return SQUARES;
    } else if(strcmp(overlay, "lines") == 0) {
        return LINES;
    } else if(strcmp(overlay, "isometric") == 0) {
        return ISOMETRIC;
    } else if(strcmp(overlay, "music") == 0) {
        return MUSIC;
    } else if(strcmp(overlay, "custom") == 0) {
        return CUSTOM;
    }
    // Unknown overlay
    return BLANK;
}
