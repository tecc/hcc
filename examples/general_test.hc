// Hypercode example

namespace Scoreboard {
    native set()
}

/*
 * Join event
 * Argument types:
 * @param player The player that joined
 */
on join(player) {
    player.sendMessage("Hello!")
}
// block equivalent
line([block("PLAYER_EVENT", "JOIN"), block("PLAYER_ACTION", "SEND_MESSAGE", [text("Hello!")])])
/*
 * Function definition
 * function <identifier>([parameter [, parameter]...]) [-> type] { <code> }
 */
function updateScoreboard() {
    Scoreboard.set()
}