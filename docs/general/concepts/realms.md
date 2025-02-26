# Realms

Scripts in *Experiment Source* run in one of three realms:

- **Client**: Code executed on the player’s game client (e.g., HUD elements).

- **Server**: Code executed on the game server (e.g., game rules).

- **Shared**: Code executed on both realms, but only affect their own execution environment (e.g., constants or utility functions).

Since there is no automatic state-sharing between client and server, communication between them requires explicit networking via the [`Networks` library](../../libraries/Networks/index.md).
