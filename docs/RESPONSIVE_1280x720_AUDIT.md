# SkyGrid GCS 1280x720 Responsive Audit

Date: 2026-06-21

## Target

The primary test target is 1280x720. The application window now opens at that
size and keeps a smaller minimum of 900x540 so responsive branches can be tested
below the target without forcing a larger desktop footprint.

## Fixes Applied

- Set the main application window to 1280x720 and lowered the minimum window
  size so the GCS can be checked at the target and smaller breakpoints.
- Updated the pilot manual flight view so the 1280x720 breakpoint uses compact
  side panels and drawers instead of keeping fixed side stacks visible.
- Removed a fixed mission elevation profile `y` position that could push content
  outside the mission workspace.
- Made the home dashboard sidebars depend on both width and height, so dense
  dashboard columns are hidden when the 720px height target cannot support them.
- Converted the GCS tools workspace from a rigid two-column row to a responsive
  grid that stacks the navigation rail above tool content on narrow screens.
- Added flexible minimum sizes, clipping, bounded table heights, wrapped text,
  and internal scroll areas to GCS tools parameter and table-heavy sections.
- Converted the manufacturer workspace shell to a responsive grid with a
  scrollable navigation region and a stacked layout below narrow widths.
- Wrapped manufacturer profile, configuration, parameter, RC mapping, manual
  test, test flight, release lock, and binding pages in scrollable or wrapping
  layouts so forms and action rows remain reachable at 1280x720.
- Tightened preflight and post-mission modals so they respect the visible parent
  bounds, reduce margins at compact sizes, wrap action buttons, and stack summary
  sections when needed.

## Role Coverage

- Pilot: covered through `ManualFlightScreen`, pilot top bar/drawers, mission
  modal behavior, and mission workspace sizing.
- Manufacturer: covered through `ManufacturerWorkspaceScreen` and the
  manufacturer tool pages listed above.
- Admin, auditor, fleet operator, and support user: covered through the shared
  home shell, GCS tools workspace, help/dashboard surfaces, and RBAC visibility
  harness scenarios.

## Verification

Commands run:

```powershell
$env:Path = "C:\Qt\6.11.1\msvc2022_64\bin;$env:Path"
cmake --build build-sgg-qt611 --config Release --target SkyGridGCS_qmllint
cmake --build build-sgg-qt611 --config Release --target SkyGridGCS
cmake --build build-sgg-qt611 --config Release --target SkyGridRbacVisibilityHarness
build-sgg-qt611\Release\SkyGridRbacVisibilityHarness.exe
windeployqt --release --compiler-runtime --qmldir qml build-sgg-qt611\bin\Release\SkyGridGCS.exe
rg -n "width:\s*1600|height:\s*980|y:\s*924|minimumHeight:\s*640|minimumWidth:\s*1024" qml -g "*.qml"
```

Results:

- QML lint completed without new parser errors. Existing unqualified-access
  warnings remain.
- Release build completed and produced `build-sgg-qt611\bin\Release\SkyGridGCS.exe`.
- RBAC visibility harness passed 12 scenarios.
- Static regression sweep found no remaining `1600x980`, `1024x640`, or fixed
  `y: 924` layout markers in source QML.
- The immediate-close startup failure was reproduced and fixed. The build output
  was missing the deployed `QtMultimedia` runtime/QML module, and one
  post-mission modal grid used an invalid `spacing` property. After refreshing
  deployment files and rebuilding, both the build-folder exe and portable dist
  exe stayed open during launch smoke tests.

## Remaining Follow-Up

- Full authenticated role-by-role GUI navigation still needs valid role accounts
  or a scripted auth fixture for pilot, manufacturer, admin, auditor, fleet
  operator, and support user.
- The current `ui-smoke` CMake target points to `scripts/smoke_ui.sh`, but that
  script is not present in this checkout.
- The portable packaging folder was refreshed, but zip creation was blocked by
  a locked Qt DLL from an existing SkyGridGCS process. The folder itself is
  launchable.
- Advanced mission command tables and GCS parameter tables still use internal
  horizontal scrolling where the data itself is wider than 1280px. That is
  intentional table behavior, not outer screen overflow.
- Existing QML lint unqualified-access warnings were not part of this responsive
  pass.
