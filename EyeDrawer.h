/***************************************************
Copyright (c) 2023 Alastair Aitchison, Playful Technology, (c) 2020 Luis Llamas
(www.luisllamas.es)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses
****************************************************/

#ifndef _EYEDRAWER_h
#define _EYEDRAWER_h

#include <Arduino.h>
#include "Common.h"
#include "EyeConfig.h"

enum CornerType {T_R, T_L, B_L, B_R};

/**
 * Contains all functions to draw eye based on supplied (expression-based) config
 */
class EyeDrawer {
  public:
    static void Draw(int16_t centerX, int16_t centerY, EyeConfig *config) {
      // Amount by which corners will be shifted up/down based on requested "slope"
      int32_t delta_y_top = config->Height * config->Slope_Top / 2.0;
      int32_t delta_y_bottom = config->Height * config->Slope_Bottom / 2.0;
      // Full extent of the eye, after accounting for slope added at top and bottom
      auto totalHeight = config->Height + delta_y_top - delta_y_bottom;
      // If the requested top/bottom radius would exceed the height of the eye, adjust them downwards 
      if (config->Radius_Bottom > 0 && config->Radius_Top > 0 && totalHeight - 1 < config->Radius_Bottom + config->Radius_Top) {
        int32_t corrected_radius_top = (float)config->Radius_Top * (totalHeight - 1) / (config->Radius_Bottom + config->Radius_Top);
        int32_t corrected_radius_bottom = (float)config->Radius_Bottom * (totalHeight - 1) / (config->Radius_Bottom + config->Radius_Top);
        config->Radius_Top = corrected_radius_top;
        config->Radius_Bottom = corrected_radius_bottom;
      }

      // Calculate _inside_ corners of eye (TL, TR, BL, and BR) before any slope or rounded corners are applied
      int32_t TLc_y = centerY + config->OffsetY - config->Height/2 + config->Radius_Top - delta_y_top;
      int32_t TLc_x = centerX + config->OffsetX - config->Width/2 + config->Radius_Top;
      int32_t TRc_y = centerY + config->OffsetY - config->Height/2 + config->Radius_Top + delta_y_top;
      int32_t TRc_x = centerX + config->OffsetX + config->Width/2 - config->Radius_Top;
      int32_t BLc_y = centerY + config->OffsetY + config->Height/2 - config->Radius_Bottom - delta_y_bottom;
      int32_t BLc_x = centerX + config->OffsetX - config->Width/2 + config->Radius_Bottom;
      int32_t BRc_y = centerY + config->OffsetY + config->Height/2 - config->Radius_Bottom + delta_y_bottom;
      int32_t BRc_x = centerX + config->OffsetX + config->Width/2 - config->Radius_Bottom;
        
      // Calculate interior extents
      int32_t min_c_x = min(TLc_x, BLc_x);
      int32_t max_c_x = max(TRc_x, BRc_x);
      int32_t min_c_y = min(TLc_y, TRc_y);
      int32_t max_c_y = max(BLc_y, BRc_y);

      // Fill eye centre
      EyeDrawer::FillRectangle(min_c_x, min_c_y, max_c_x, max_c_y, 1);

      // Fill eye outwards to meet edges of rounded corners 
      EyeDrawer::FillRectangle(TRc_x, TRc_y, BRc_x + config->Radius_Bottom, BRc_y, 1); // Right
		  EyeDrawer::FillRectangle(TLc_x - config->Radius_Top, TLc_y, BLc_x, BLc_y, 1); // Left
		  EyeDrawer::FillRectangle(TLc_x, TLc_y - config->Radius_Top, TRc_x, TRc_y, 1); // Top
		  EyeDrawer::FillRectangle(BLc_x, BLc_y, BRc_x, BRc_y + config->Radius_Bottom, 1); // Bottom
        
      // Draw slanted edges at top of bottom of eyes 
      // +ve Slope_Top means eyes slope downwards towards middle of face
      if(config->Slope_Top > 0) {
        EyeDrawer::FillRectangularTriangle(TLc_x, TLc_y-config->Radius_Top, TRc_x, TRc_y-config->Radius_Top, 0);
        EyeDrawer::FillRectangularTriangle(TRc_x, TRc_y-config->Radius_Top, TLc_x, TLc_y-config->Radius_Top, 1);
      } 
      else if(config->Slope_Top < 0) {
        EyeDrawer::FillRectangularTriangle(TRc_x, TRc_y-config->Radius_Top, TLc_x, TLc_y-config->Radius_Top, 0);
        EyeDrawer::FillRectangularTriangle(TLc_x, TLc_y-config->Radius_Top, TRc_x, TRc_y-config->Radius_Top, 1);
      }
      // Draw slanted edges at bottom of eyes
      if(config->Slope_Bottom > 0) {
        EyeDrawer::FillRectangularTriangle(BRc_x+config->Radius_Bottom, BRc_y+config->Radius_Bottom, BLc_x-config->Radius_Bottom, BLc_y+config->Radius_Bottom, 0);
        EyeDrawer::FillRectangularTriangle(BLc_x-config->Radius_Bottom, BLc_y+config->Radius_Bottom, BRc_x+config->Radius_Bottom, BRc_y+config->Radius_Bottom, 1);
      }
      else if (config->Slope_Bottom < 0) {
        EyeDrawer::FillRectangularTriangle(BLc_x-config->Radius_Bottom, BLc_y+config->Radius_Bottom, BRc_x+config->Radius_Bottom, BRc_y+config->Radius_Bottom, 0);
        EyeDrawer::FillRectangularTriangle(BRc_x+config->Radius_Bottom, BRc_y+config->Radius_Bottom, BLc_x-config->Radius_Bottom, BLc_y+config->Radius_Bottom, 1);
      }

      // Draw corners (which extend "outwards" towards corner of screen from supplied coordinate values)
      if(config->Radius_Top > 0) {
        EyeDrawer::FillEllipseCorner(T_L, TLc_x, TLc_y, config->Radius_Top, config->Radius_Top, 1);
        EyeDrawer::FillEllipseCorner(T_R, TRc_x, TRc_y, config->Radius_Top, config->Radius_Top, 1);
      }
      if(config->Radius_Bottom > 0) {
        EyeDrawer::FillEllipseCorner(B_L, BLc_x, BLc_y, config->Radius_Bottom, config->Radius_Bottom, 1);
        EyeDrawer::FillEllipseCorner(B_R, BRc_x, BRc_y, config->Radius_Bottom, config->Radius_Bottom, 1);
      }
    }

    // Draw rounded corners
    static void FillEllipseCorner(CornerType corner, int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color) {
      if (rx < 2) return;
      if (ry < 2) return;
      int32_t x, y;
      int32_t rx2 = rx * rx;
      int32_t ry2 = ry * ry;
      int32_t fx2 = 4 * rx2;
      int32_t fy2 = 4 * ry2;
      int32_t s;

      if (corner == T_R) {
        for(x = 0, y = ry, s = 2 * ry2 + rx2 * (1 - 2 * ry); ry2 * x <= rx2 * y; x++) {
          u8g2.drawHLine(x0, y0 - y, x);
          if(s >= 0) {
            s += fx2 * (1 - y);
            y--;
          }
          s += ry2 * ((4 * x) + 6);
        }         
        for(x = rx, y = 0, s = 2 * rx2 + ry2 * (1 - 2 * rx); rx2 * y <= ry2 * x; y++) {
          u8g2.drawHLine(x0, y0 - y, x);
          if (s >= 0) {
            s += fy2 * (1 - x);
            x--;
          }
          s += rx2 * ((4 * y) + 6);
        }
      }

      else if (corner == B_R) {
        for (x = 0, y = ry, s = 2 * ry2 + rx2 * (1 - 2 * ry); ry2 * x <= rx2 * y; x++) {
          u8g2.drawHLine(x0, y0 + y -1, x);
          if (s >= 0) {
            s += fx2 * (1 - y);
            y--;
          }
          s += ry2 * ((4 * x) + 6);
        }
        for (x = rx, y = 0, s = 2 * rx2 + ry2 * (1 - 2 * rx); rx2 * y <= ry2 * x; y++) {
          u8g2.drawHLine(x0, y0 + y -1, x);
          if (s >= 0) {
            s += fy2 * (1 - x);
            x--;
          }
          s += rx2 * ((4 * y) + 6);
        }
      }

      else if (corner == T_L) {
        for (x = 0, y = ry, s = 2 * ry2 + rx2 * (1 - 2 * ry); ry2 * x <= rx2 * y; x++) {
          u8g2.drawHLine(x0-x, y0 - y, x);
          if (s >= 0) {
            s += fx2 * (1 - y);
            y--;
          }
          s += ry2 * ((4 * x) + 6);
        }
        for (x = rx, y = 0, s = 2 * rx2 + ry2 * (1 - 2 * rx); rx2 * y <= ry2 * x; y++) {
          u8g2.drawHLine(x0-x, y0 - y, x);
          if (s >= 0) {
            s += fy2 * (1 - x);
            x--;
          }
          s += rx2 * ((4 * y) + 6);
        }
      }

      else if (corner == B_L) {
        for (x = 0, y = ry, s = 2 * ry2 + rx2 * (1 - 2 * ry); ry2 * x <= rx2 * y; x++) {
          u8g2.drawHLine(x0-x, y0 + y - 1, x);
          if (s >= 0) {
            s += fx2 * (1 - y);
            y--;
          }
          s += ry2 * ((4 * x) + 6);
        }
        for (x = rx, y = 0, s = 2 * rx2 + ry2 * (1 - 2 * rx); rx2 * y <= ry2 * x; y++) {
          u8g2.drawHLine(x0-x, y0 + y , x);
          if (s >= 0) {
            s += fy2 * (1 - x);
            x--;
          }
          s += rx2 * ((4 * y) + 6);
        }
      }
    }

    // Fill a solid rectangle between specified coordinates
    static void FillRectangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t color) {
      // Always draw from TL->BR
      int32_t l = min(x0, x1);
      int32_t r = max(x0, x1);
      int32_t t = min(y0, y1);
      int32_t b = max(y0, y1);
      int32_t w = r-l;
      int32_t h = b-t; 
      u8g2.setDrawColor(color);
      u8g2.drawBox(l, t, w, h);
      u8g2.setDrawColor(1);
    }

    static void FillRectangularTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t color) {
      u8g2.setDrawColor(color);
      u8g2.drawTriangle(x0, y0, x1, y1, x1, y0);
      u8g2.setDrawColor(1);
    }

    static void FillTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color) {
        u8g2.setDrawColor(color);
        u8g2.drawTriangle(x0, y0, x1, y1, x2, y2);
        u8g2.setDrawColor(1);
    }
};

#endif