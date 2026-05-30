SkyGrid GCS Portable Windows Release
===================================

This folder is a self-contained SkyGrid GCS release. The target Windows
machine does not need Qt installed.

How to run
----------
1. Extract the zip file to a normal local folder.
2. Double-click SkyGridGCS.exe.
3. Edit SkyGridGCS.ini beside the exe if the Control Center or MAVSDK URLs are
   different for your simulator or aircraft setup.

Configuration
-------------
Environment variables override values in SkyGridGCS.ini.

Supported SkyGridGCS.ini keys:
- SKYGRID_BACKEND_URL
- SKYGRID_MAVSDK_URLS
- SKYGRID_MAVSDK_ALLOW_MULTIPLE_URLS
- SKYGRID_PERFORMANCE_MODE

Expected packaged content
-------------------------
- SkyGridGCS.exe
- SkyGridGCS.ini
- Qt DLLs, QML modules, and plugins copied by windeployqt
- MAVSDK runtime DLLs
- librsvg/cairo/glib runtime DLLs used by SVG icon rendering

Troubleshooting
---------------
- If Windows reports a missing DLL, rebuild the package with the dependency
  root passed to tools/package_windows.ps1 through -DependencyRoots.
- If the map or planner opens but the aircraft is not found, check
  SKYGRID_MAVSDK_URLS in SkyGridGCS.ini.
- If Control Center is unreachable, check SKYGRID_BACKEND_URL.
