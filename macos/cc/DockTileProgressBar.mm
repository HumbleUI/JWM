#include "DockTileProgressBar.hh"

@implementation DockTileProgressBar

- (void)drawRect:(NSRect)dirtyRect {
  if (self.style != NSProgressIndicatorBarStyle)
    return;
  // Draw edges of rounded rect.
  NSRect rect = NSInsetRect([self bounds], 1.0, 1.0);
  CGFloat radius = rect.size.height / 2;
  NSBezierPath* bezierPath =
    [NSBezierPath bezierPathWithRoundedRect:rect xRadius:radius yRadius:radius];
  [bezierPath setLineWidth:2.0];
  [[NSColor grayColor] set];
  [bezierPath stroke];

  // Fill the rounded rect.
  rect = NSInsetRect(rect, 2.0, 2.0);
  radius = rect.size.height / 2;
  bezierPath =
    [NSBezierPath bezierPathWithRoundedRect:rect xRadius:radius yRadius:radius];
  [bezierPath setLineWidth:1.0];
  [bezierPath addClip];

  // Calculate the progress width.
  rect.size.width =
      floor(rect.size.width * ([self doubleValue] / [self maxValue]));

  // Fill the progress bar with color blue.
  [[NSColor colorWithSRGBRed:0.2 green:0.6 blue:1 alpha:1] set];
  NSRectFill(rect);
}

@end
