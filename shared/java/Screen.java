package io.github.humbleui.jwm;

import io.github.humbleui.types.*;
import lombok.*;
import org.jetbrains.annotations.*;
import org.jetbrains.annotations.ApiStatus;

@Data
public class Screen {
    /**
     * <p>Application-wide platform-specific screen identifier.</p>
     */
    public final long _id;

    /**
     * <p>Describes, if this screen is primary.</p>
     * <p>Primary screen has top-left corner in general is global screen space origin.</p>
     */
    public final boolean _isPrimary;

    /**
     * <p>Screen bounds in pixels in the global screen space coordinates.</p>
     */
    public final IRect _bounds;

    /**
     * <p>Area in absolute pixels of the screen excluding dock/menubar</p>
     */
    public final IRect _workArea;

    /**
     * <p>UI and text elements scale for display on this screen</p>
     */
    public final float _scale;

    /**
     * <p>Get the ICC color profile data for this screen.</p>
     * <p>Currently supported on macOS and Windows. Returns null on other platforms.</p>
     *
     * @return ICC profile data as byte array, or null if not available
     */
    @Nullable
    public byte[] getICCProfile() {
        if (Platform.CURRENT == Platform.MACOS || Platform.CURRENT == Platform.WINDOWS) {
            return _nGetICCProfile(_id);
        }
        return null;
    }

    @ApiStatus.Internal
    private static native byte[] _nGetICCProfile(long screenId);
}