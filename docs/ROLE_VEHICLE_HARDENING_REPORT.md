# Role-Based GCS and Vehicle Configuration Hardening Report

Date: 2026-05-31

## Implemented

- Added GCS access managers:
  - `RoleAccessManager`
  - `PermissionManager`
  - `ModuleAccessManager`
- Extended Control Center bootstrap consumption so the GCS reads:
  - `roles`
  - `permissions`
  - `allowed_modules`
  - `allowed_gcs_modules`
  - `vehicle_profiles`
- Added role/module/permission-gated navigation:
  - Mission workspace
  - Pilot/manual flight workspace
  - Vehicle configuration workspace
- Added client-side hard stops for high-risk actions:
  - Mission save/validation requires `can_plan_mission`
  - Mission upload requires `can_upload_mission`
  - Autonomous mission start requires `can_start_mission`
  - Manual flight and vehicle action commands require `can_fly_manual`
  - Telemetry streaming requires `can_stream_telemetry`
  - Vehicle connection/bind requires `can_bind_flight_controller`
  - RC mapping requires `can_configure_rc`
  - Parameter snapshot read/save requires `can_read_vehicle_parameters` / `can_write_vehicle_parameters`
  - Vehicle release requires `can_release_vehicle_to_organization`
- Added the GCS vehicle configuration workspace:
  - `qml/vehicle/VehicleConfigurationScreen.qml`
  - `qml/vehicle/VehicleConnectionPanel.qml`
  - `qml/vehicle/VehicleProfilePanel.qml`
  - `qml/vehicle/ControllerMappingPanel.qml`
  - `qml/vehicle/ParameterSnapshotPanel.qml`
  - `qml/vehicle/VehicleConfigAuditPanel.qml`
- Added C++ vehicle configuration managers:
  - `VehicleConfigManager`
  - `VehicleProfileManager`
- Kept vehicle configuration safe by default:
  - No auto-arm
  - No auto-takeoff
  - No motor-test command path
  - Connection uses MAVSDK discovery only
  - Parameter snapshot is read-only vehicle state capture unless explicitly saved to SGG_CC
- Added SGG_CC role/scope source of truth:
  - `apps/accounts/access.py`
  - Unified role normalization and permission-scope mapping
  - Bootstrap now returns roles, permissions, allowed modules, and vehicle profiles
- Added SGG_CC vehicle/manufacturer backend:
  - `Manufacturer`
  - `VehicleProfile`
  - `VehicleConfigurationAudit`
  - DRF endpoints under `/api/vehicle/profiles/`, `/api/vehicle/audit/`, and `/api/manufacturers/`
  - Actions for bind flight controller, save RC mapping, save parameter snapshot, release to organization, and audit listing
- Added backend enforcement for mission, telemetry, flight-session, and vehicle configuration scopes.
- Added migrations:
  - `apps/accounts/migrations/0002_alter_user_role.py`
  - `apps/vehicles/migrations/0001_initial.py`

## Validation

- `python manage.py check`: passed.
- `python manage.py makemigrations --check --dry-run`: passed after adding the account role migration.
- `python -m compileall apps config`: passed.
- Temporary GCS CMake configure outside the workspace: passed.
- Temporary GCS Debug build compiled the new QML vehicle panels and most changed C++ files to object files before the generated QML build exceeded the tool timeout.
- Direct MSVC syntax check over the new/changed C++ source files: passed.
- `qmllint` over the new vehicle QML and router: no syntax failures; warnings are mostly expected context-property warnings for C++ objects exposed from `main.cpp`.

## Cybersecurity Checklist

- Backend is the source of truth for roles, permission scopes, and allowed modules.
- Trusted-device enforcement remains required on GCS-facing API endpoints.
- Client-side checks were added for UX and safety, but backend scope checks are the authoritative enforcement.
- Permission denial events in vehicle configuration are recorded locally and through the GCS event sync path.
- Vehicle profile configuration actions create Control Center audit rows and audit events.
- No privileged vehicle configuration action sends flight-control commands.
- Device revocation/session-block behavior was preserved.

## Incomplete / Needed Next

- Finish a full GCS link/package pass. The temporary MSBuild validation exceeded the tool timeout while compiling/linking generated QML output, so the final executable link was stopped.
- Run a logged-in UI smoke test against SGG_CC with users in each role.
- Add API tests for every permission scope and endpoint action.
- Add seed/management command support for creating the new role groups and assigning users.
- Replace the current read-only telemetry-style parameter snapshot with true MAVSDK parameter enumeration if full flight-controller parameter capture is required.
- Decide manufacturer tenancy rules. Current manufacturer roles can see vehicle profiles broadly; production should tie users to manufacturer records.
- Add organization lookup UI for release-to-organization instead of the current numeric organization id entry point.

## Concerns

- Role names now support both legacy roles and new roles, but existing users will keep their old stored role values until migrated or edited.
- Backend enforcement now blocks actions that older clients may have assumed were always allowed.
- The GCS still depends on successful bootstrap to unlock role-specific modules; offline cached bootstrap is supported, but first-run offline users will be locked out of restricted actions.
- Full package validation still needs to be run before release distribution.
