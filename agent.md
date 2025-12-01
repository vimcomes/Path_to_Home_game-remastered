## Project Notes (Path_to_Home_game)

**Latest context (this session)**
- Added lives (5 icons in HUD) and shield bonus (20%→35% chance; blue glowing can, ring + red pulsating outline when active). Shield blocks collisions and disables fuel drain; collisions consume life, on zero lives show Game Over screen and wait for key to restart (reset lives, progress marker, shield, respawn bonus).
- Cleaned unused assets, smoothed controls with delta time, hitbox tightening, HUD bars with labels, parallax, smoke, explosions, rotating meteors. Fuel pickups scale with amount.
- Current build succeeds (`cmake --build cmake-build-debug`).
- Resources centralized in code (`Resources` struct), constants in `Config` (speeds, fuel drain, shield duration). Removed `using namespace` in headers. Sounds reconnected, bonus spin restored.

**Next planned refactor (target: closer to mid-level code quality)**
3) Input/render separation (pending): helpers for input polling and HUD/scene rendering; continue minimizing globals.

If resuming: start with step 1 above (GameState + update/render split), then rebuild.
