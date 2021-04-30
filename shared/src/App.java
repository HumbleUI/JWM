package org.jetbrains.jwm;

import org.jetbrains.annotations.*;
import org.jetbrains.jwm.impl.*;

public class App {
   static { Library.staticLoad(); }

   public static void runEventLoop() {
      _nRunEventLoop();
   }
   
   @ApiStatus.Internal public static native void _nRunEventLoop();
}